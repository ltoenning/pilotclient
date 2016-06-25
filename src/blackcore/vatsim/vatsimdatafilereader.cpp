/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/vatsim/vatsimdatafilereader.h"
#include "blackcore/application.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/altitude.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/compare.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/network/server.h"
#include "blackmisc/network/url.h"
#include "blackmisc/network/urllist.h"
#include "blackmisc/network/user.h"
#include "blackmisc/pq/frequency.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/speed.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/predicates.h"
#include "blackmisc/range.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/verify.h"

#include <QByteArray>
#include <QDateTime>
#include <QMetaObject>
#include <QNetworkReply>
#include <QReadLocker>
#include <QRegExp>
#include <QRegularExpression>
#include <QScopedPointer>
#include <QScopedPointerDeleteLater>
#include <QTimer>
#include <QUrl>
#include <QWriteLocker>
#include <Qt>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackCore::Data;
using namespace BlackCore::Settings;

namespace BlackCore
{
    namespace Vatsim
    {
        CVatsimDataFileReader::CVatsimDataFileReader(QObject *owner) :
            CThreadedReader(owner, "CVatsimDataFileReader")
        {
            this->connect(this->m_updateTimer, &QTimer::timeout, this, &CVatsimDataFileReader::ps_read);
        }

        CSimulatedAircraftList CVatsimDataFileReader::getAircraft() const
        {
            QReadLocker rl(&this->m_lock);
            return this->m_aircraft;
        }

        CAtcStationList CVatsimDataFileReader::getAtcStations() const
        {
            QReadLocker rl(&this->m_lock);
            return this->m_atcStations;
        }

        CAtcStationList CVatsimDataFileReader::getAtcStationsForCallsign(const CCallsign &callsign) const
        {
            CCallsignSet cs({callsign});
            return this->getAtcStationsForCallsigns(cs);
        }

        CAtcStationList CVatsimDataFileReader::getAtcStationsForCallsigns(const CCallsignSet &callsigns) const
        {
            return this->getAtcStations().findByCallsigns(callsigns);
        }

        CServerList CVatsimDataFileReader::getVoiceServers() const
        {
            return this->m_lastGoodSetup.get().getVoiceServers();
        }

        CServerList CVatsimDataFileReader::getFsdServers() const
        {
            return this->m_lastGoodSetup.get().getFsdServers();
        }

        CUserList CVatsimDataFileReader::getPilotsForCallsigns(const CCallsignSet &callsigns)
        {
            return this->getAircraft().findByCallsigns(callsigns).transform(Predicates::MemberTransform(&CSimulatedAircraft::getPilot));
        }

        CUserList CVatsimDataFileReader::getPilotsForCallsign(const CCallsign &callsign)
        {
            CCallsignSet callsigns({callsign});
            return this->getPilotsForCallsigns(callsigns);
        }

        CAirlineIcaoCode CVatsimDataFileReader::getAirlineIcaoCode(const CCallsign &callsign)
        {
            CSimulatedAircraft aircraft = this->getAircraft().findFirstByCallsign(callsign);
            return aircraft.getAirlineIcaoCode();
        }

        CAircraftIcaoCode CVatsimDataFileReader::getAircraftIcaoCode(const CCallsign &callsign)
        {
            CSimulatedAircraft aircraft = this->getAircraft().findFirstByCallsign(callsign);
            return aircraft.getAircraftIcaoCode();
        }

        CVoiceCapabilities CVatsimDataFileReader::getVoiceCapabilityForCallsign(const CCallsign &callsign)
        {
            QReadLocker rl(&this->m_lock);
            if (this->m_voiceCapabilities.contains(callsign))
            {
                return m_voiceCapabilities[callsign];
            }
            else
            {
                return CVoiceCapabilities::fromVoiceCapabilities(CVoiceCapabilities::Unknown);
            }
        }

        void CVatsimDataFileReader::updateWithVatsimDataFileData(CSimulatedAircraft &aircraftToBeUdpated) const
        {
            this->getAircraft().updateWithVatsimDataFileData(aircraftToBeUdpated);
        }

        CUserList CVatsimDataFileReader::getControllersForCallsign(const CCallsign &callsign)
        {
            CCallsignSet cs({callsign});
            return this->getControllersForCallsigns(cs);
        }

        CUserList CVatsimDataFileReader::getControllersForCallsigns(const CCallsignSet &callsigns)
        {
            return this->getAtcStations().findByCallsigns(callsigns).transform(Predicates::MemberTransform(&CAtcStation::getController));
        }

        CUserList CVatsimDataFileReader::getUsersForCallsign(const CCallsign &callsign)
        {
            CCallsignSet callsigns({callsign});
            return this->getUsersForCallsigns(callsigns);
        }

        CUserList CVatsimDataFileReader::getUsersForCallsigns(const CCallsignSet &callsigns)
        {
            CUserList users;
            if (callsigns.isEmpty()) { return users; }
            for (const CCallsign &callsign : callsigns)
            {
                users.push_back(this->getPilotsForCallsign(callsign));
                users.push_back(this->getControllersForCallsign(callsign));
            }
            return users;
        }

        void CVatsimDataFileReader::readInBackgroundThread()
        {
            bool s = QMetaObject::invokeMethod(this, "ps_read");
            Q_ASSERT_X(s, Q_FUNC_INFO, "Invoke failed");
            Q_UNUSED(s);
        }

        void CVatsimDataFileReader::cleanup()
        {
            // void
        }

        CSettingsReader CVatsimDataFileReader::getSettings() const
        {
            return this->m_settings.get();
        }

        void CVatsimDataFileReader::ps_read()
        {
            this->threadAssertCheck();
            this->restartTimer(true); // when timer active, restart so we cause no undesired reads

            // round robin for load balancing
            // remark: Don't use QThread to run network operations in the background
            // see http://qt-project.org/doc/qt-4.7/qnetworkaccessmanager.html
            Q_ASSERT_X(sApp, Q_FUNC_INFO, "Missing application");
            CFailoverUrlList urls(sApp->getVatsimDataFileUrls());
            const QUrl url(urls.obtainNextWorkingUrl(true));
            if (url.isEmpty()) { return; }
            sApp->getFromNetwork(url, { this, &CVatsimDataFileReader::ps_parseVatsimFile});
        }

        void CVatsimDataFileReader::ps_parseVatsimFile(QNetworkReply *nwReplyPtr)
        {
            // wrap pointer, make sure any exit cleans up reply
            // required to use delete later as object is created in a different thread
            QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> nwReply(nwReplyPtr);

            this->threadAssertCheck();

            // Worker thread, make sure to write only synced here!
            if (this->isAbandoned())
            {
                CLogMessage(this).debug() << Q_FUNC_INFO;
                CLogMessage(this).info("Terminated VATSIM file parsing process");
                return; // stop, terminate straight away, ending thread
            }

            QStringList illegalIcaoCodes;
            if (nwReply->error() == QNetworkReply::NoError)
            {
                const QString dataFileData = nwReply->readAll();
                nwReply->close(); // close asap

                if (dataFileData.isEmpty()) { return; }
                this->restartTimer(); // do not consider time for reading
                if (!this->didContentChange(dataFileData)) // Quick check by hash
                {
                    CLogMessage(this).info("VATSIM file has same content, skipped");
                    return;
                }
                const QStringList lines = dataFileData.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
                if (lines.isEmpty()) { return; }

                // build on local vars for thread safety
                CServerList                         voiceServers;
                CServerList                         fsdServers;
                CAtcStationList                     atcStations;
                CSimulatedAircraftList              aircraft;
                QMap<CCallsign, CVoiceCapabilities> voiceCapabilities;
                QDateTime                           updateTimestampFromFile;

                QStringList clientSectionAttributes;
                Section section = SectionNone;
                for (const QString &cl : lines)
                {
                    if (this->isAbandoned())
                    {
                        CLogMessage(this).debug() << Q_FUNC_INFO;
                        CLogMessage(this).info("Terminated VATSIM file parsing process"); // for users
                        return; // stop, terminate straight away, ending thread
                    }

                    // parse lines
                    QString currentLine(cl.trimmed());
                    if (currentLine.isEmpty()) continue;
                    if (currentLine.startsWith(";"))
                    {
                        if (clientSectionAttributes.isEmpty() && currentLine.contains("!CLIENTS SECTION", Qt::CaseInsensitive))
                        {
                            // ; !CLIENTS section
                            int i = currentLine.lastIndexOf(' ');
                            const QString attributes = currentLine.mid(i).trimmed();
                            clientSectionAttributes = attributes.split(':', QString::SkipEmptyParts);
                            section = SectionNone; // reset
                        }
                        continue;
                    }
                    else if (currentLine.startsWith("!"))
                    {
                        section = currentLineToSection(currentLine);
                        continue;
                    }

                    switch (section)
                    {
                    case SectionClients:
                        {
                            const QMap<QString, QString> clientPartsMap = clientPartsToMap(currentLine, clientSectionAttributes);
                            const CCallsign callsign = CCallsign(clientPartsMap["callsign"]);
                            if (callsign.isEmpty()) { break; }
                            const BlackMisc::Network::CUser user(clientPartsMap["cid"], clientPartsMap["realname"], callsign);
                            const QString clientType = clientPartsMap["clienttype"].toLower();
                            if (clientType.isEmpty()) { break; } // sometimes type is empty
                            const double lat = clientPartsMap["latitude"].toDouble();
                            const double lng = clientPartsMap["longitude"].toDouble();
                            const double alt = clientPartsMap["altitude"].toDouble();
                            const CFrequency frequency = CFrequency(clientPartsMap["frequency"].toDouble(), CFrequencyUnit::MHz());
                            CCoordinateGeodetic position(lat, lng, -1);
                            CAltitude altitude(alt, CAltitude::MeanSeaLevel, CLengthUnit::ft());
                            QString flightPlanRemarks = clientPartsMap["planned_remarks"];

                            // Voice capabilities
                            if (!flightPlanRemarks.isEmpty())
                            {
                                CVoiceCapabilities vc(flightPlanRemarks);
                                if (!vc.isUnknown())
                                {
                                    voiceCapabilities.insert(callsign, vc);
                                }
                            }

                            // set as per ATC/pilot
                            if (clientType.startsWith('p'))
                            {
                                // Pilot section
                                const double groundspeed = clientPartsMap["groundspeed"].toDouble();
                                CAircraftSituation situation(position, altitude);
                                situation.setGroundSpeed(CSpeed(groundspeed, CSpeedUnit::kts()));
                                CSimulatedAircraft currentAircraft(user.getCallsign().getStringAsSet(), user, situation);

                                QString aircraftIcaoCode = clientPartsMap["planned_aircraft"];
                                if (!aircraftIcaoCode.isEmpty())
                                {
                                    // http://uk.flightaware.com/about/faq_aircraft_flight_plan_suffix.rvt
                                    // we expect something like H/B772/F B773 B773/F
                                    static const QRegularExpression reg("/.");
                                    aircraftIcaoCode = aircraftIcaoCode.replace(reg, "").trimmed().toUpper();
                                    if (CAircraftIcaoCode::isValidDesignator(aircraftIcaoCode))
                                    {
                                        currentAircraft.setAircraftIcaoDesignator(aircraftIcaoCode);
                                    }
                                    else
                                    {
                                        illegalIcaoCodes.append(aircraftIcaoCode);
                                    }
                                }

                                aircraft.push_back(currentAircraft);
                            }
                            else if (clientType.startsWith('a'))
                            {
                                // ATC section
                                CLength range;
                                position.setGeodeticHeight(altitude); // the altitude is elevation for a station
                                CAtcStation station(user.getCallsign().getStringAsSet(), user, frequency, position, range);
                                station.setOnline(true);
                                atcStations.push_back(station);
                            }
                            else
                            {
                                BLACK_VERIFY_X(false, Q_FUNC_INFO, "Wrong client type");
                                break;
                            }
                        }
                        break;
                    case SectionGeneral:
                        {
                            if (currentLine.contains("UPDATE"))
                            {
                                const QStringList updateParts = currentLine.replace(" ", "").split('=');
                                if (updateParts.length() < 2) break;
                                const QString dts = updateParts.at(1).trimmed();
                                updateTimestampFromFile = QDateTime::fromString(dts, "yyyyMMddHHmmss");
                                updateTimestampFromFile.setOffsetFromUtc(0);
                                bool alreadyRead = (updateTimestampFromFile == this->getUpdateTimestamp());
                                if (alreadyRead)
                                {
                                    CLogMessage(this).info("VATSIM file has same timestamp, skipped");
                                    return;
                                }
                            }
                        }
                        break;
                    case SectionFsdServers:
                        {
                            // ident:hostname_or_IP:location:name:clients_connection_allowed:
                            const QStringList fsdServerParts = currentLine.split(':');
                            if (fsdServerParts.size() < 5) break;
                            if (!fsdServerParts.at(4).trimmed().contains('1')) break; // allowed?
                            QString description(fsdServerParts.at(2)); // part(3) could be added
                            BlackMisc::Network::CServer fsdServer(fsdServerParts.at(0), description, fsdServerParts.at(1), 6809, CUser("id", "real name", "email", "password"));
                            fsdServers.push_back(fsdServer);
                        }
                        break;
                    case SectionVoiceServers:
                        {
                            // hostname_or_IP:location:name:clients_connection_allowed:type_of_voice_server:
                            const QStringList voiceServerParts = currentLine.split(':');
                            if (voiceServerParts.size() < 3) break;
                            if (!voiceServerParts.at(3).trimmed().contains('1')) break; // allowed?
                            BlackMisc::Network::CServer voiceServer(voiceServerParts.at(1), voiceServerParts.at(2), voiceServerParts.at(0), -1, CUser());
                            voiceServers.push_back(voiceServer);
                        }
                        break;
                    case SectionNone:
                    default:
                        break;

                    } // switch section

                } // for each line

                // this part needs to be synchronized
                {
                    QWriteLocker wl(&this->m_lock);
                    this->setUpdateTimestamp(updateTimestampFromFile);
                    this->m_aircraft = aircraft;
                    this->m_atcStations = atcStations;
                    this->m_voiceCapabilities = voiceCapabilities;
                    CVatsimSetup vs(this->m_lastGoodSetup.getThreadLocal());

                    // check if we need to save in cache
                    if (vs.getVoiceServers() != voiceServers || vs.getFsdServers() != fsdServers)
                    {
                        vs.setVoiceServers(voiceServers);
                        vs.setFsdServers(fsdServers);
                        vs.setUtcTimestamp(updateTimestampFromFile);
                        this->m_lastGoodSetup.set(vs);
                    }
                }

                // warnings, if required
                if (!illegalIcaoCodes.isEmpty())
                {
                    CLogMessage(this).info("Illegal / ignored ICAO code(s) in VATSIM data file: %1") << illegalIcaoCodes.join(", ");
                }

                // data read finished
                emit this->dataFileRead(lines.count());
                emit this->dataRead(CEntityFlags::VatsimDataFile, CEntityFlags::ReadFinished, lines.count());
            }
            else
            {
                // network error
                CLogMessage(this).warning("Reading VATSIM data file failed %1 %2") << nwReply->errorString() << nwReply->url().toString();
                nwReply->abort();
                emit this->dataRead(CEntityFlags::VatsimDataFile, CEntityFlags::ReadFailed, 0);
            }
        }

        const QMap<QString, QString> CVatsimDataFileReader::clientPartsToMap(const QString &currentLine, const QStringList &clientSectionAttributes)
        {
            QMap<QString, QString> parts;
            if (currentLine.isEmpty()) { return parts; }
            const QStringList clientParts = currentLine.split(':');
            for (int i = 0; i < clientSectionAttributes.size(); i++)
            {
                BLACK_VERIFY_X(i < clientSectionAttributes.size(), Q_FUNC_INFO, "Wrong section attribute size");
                BLACK_VERIFY_X(i < clientParts.size(), Q_FUNC_INFO, "Wrong parts size");
                if (i < clientSectionAttributes.size() || i < clientParts.size()) { continue; }
                parts.insert(clientSectionAttributes.at(i).toLower(), clientParts.at(i));
            }
            return parts;
        }

        CVatsimDataFileReader::Section CVatsimDataFileReader::currentLineToSection(const QString &currentLine)
        {
            if (currentLine.contains("!GENERAL", Qt::CaseInsensitive)) { return SectionGeneral; }
            if (currentLine.contains("!VOICE SERVERS", Qt::CaseInsensitive)) { return SectionVoiceServers; }
            if (currentLine.contains("!SERVERS", Qt::CaseInsensitive)) { return SectionFsdServers; }
            if (currentLine.contains("!CLIENTS", Qt::CaseInsensitive)) { return SectionClients; }
            return SectionNone;
        }
    } // ns
} // ns
