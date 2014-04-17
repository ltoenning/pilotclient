/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "blackcore/context_network_proxy.h"
#include "blackmisc/voiceroomlist.h"
#include <QObject>
#include <QMetaEnum>
#include <QDBusConnection>

namespace BlackCore
{

    /*
     * Constructor for DBus
     */
    CContextNetworkProxy::CContextNetworkProxy(const QString &serviceName, QDBusConnection &connection, CRuntimeConfig::ContextMode mode, CRuntime *runtime) : IContextNetwork(mode, runtime), m_dBusInterface(nullptr)
    {
        this->m_dBusInterface = new BlackMisc::CGenericDBusInterface(
            serviceName , IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
            connection, this);
        this->relaySignals(serviceName, connection);
    }

    /*
     * Workaround for signals, not working without, but why?
     */
    void CContextNetworkProxy::relaySignals(const QString &serviceName, QDBusConnection &connection)
    {
        bool s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                                    "connectionStatusChanged", this, SIGNAL(connectionStatusChanged(uint, uint,  const QString &)));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                               "changedAtcStationsBooked", this, SIGNAL(changedAtcStationsBooked()));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                               "changedAtcStationsOnline", this, SIGNAL(changedAtcStationsOnline()));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                               "connectionTerminated", this, SIGNAL(connectionTerminated()));
        Q_ASSERT(s);
        s = connection.connect(serviceName, IContextNetwork::ObjectPath(), IContextNetwork::InterfaceName(),
                               "textMessagesReceived", this, SIGNAL(textMessagesReceived(BlackMisc::Network::CTextMessageList)));
        Q_ASSERT(s);
        Q_UNUSED(s);
    }

    /*
     * Logging
     */
    void CContextNetworkProxy::log(const QString &method, const QString &m1, const QString &m2, const QString &m3, const QString &m4) const
    {
        if (m1.isEmpty())
            qDebug() << "   LOG: " << method;
        else if (m2.isEmpty())
            qDebug() << "   LOG: " << method << m1;
        else if (m3.isEmpty())
            qDebug() << "   LOG: " << method << m1 << m2;
        else if (m4.isEmpty())
            qDebug() << "   LOG: " << method << m1 << m2 << m3;
        else
            qDebug() << "   LOG: " << method << m1 << m2 << m3 << m4;
    }

    void CContextNetworkProxy::readAtcBookingsFromSource() const
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("readAtcBookingsFromSource"));
    }

    const BlackMisc::Aviation::CAtcStationList CContextNetworkProxy::getAtcStationsOnline() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAtcStationList>(QLatin1Literal("getAtcStationsOnline"));
    }

    const BlackMisc::Aviation::CAtcStationList CContextNetworkProxy::getAtcStationsBooked() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAtcStationList>(QLatin1Literal("getAtcStationsBooked"));
    }

    const BlackMisc::Aviation::CAircraftList CContextNetworkProxy::getAircraftsInRange() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAircraftList>(QLatin1Literal("getAircraftsInRange"));
    }

    BlackMisc::Network::CUserList CContextNetworkProxy::getUsers() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Network::CUserList>(QLatin1Literal("getUsers"));
    }

    BlackMisc::Network::CUserList CContextNetworkProxy::getUsersForCallsigns(const BlackMisc::Aviation::CCallsignList &callsigns) const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Network::CUserList>(QLatin1Literal("getUsersForCallsigns"), callsigns);
    }

    BlackMisc::Audio::CVoiceRoomList CContextNetworkProxy::getSelectedVoiceRooms() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Audio::CVoiceRoomList>(QLatin1Literal("getSelectedVoiceRooms"));
    }

    BlackMisc::Aviation::CAtcStationList CContextNetworkProxy::getSelectedAtcStations() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAtcStationList>(QLatin1Literal("getSelectedAtcStations"));
    }

    void CContextNetworkProxy::requestDataUpdates()
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("requestDataUpdates"));
    }

    void CContextNetworkProxy::requestAtisUpdates()
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("requestAtisUpdates"));
    }

    BlackMisc::Aviation::CAircraft CContextNetworkProxy::getOwnAircraft() const
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CAircraft>(QLatin1Literal("getOwnAircraft"));
    }

    BlackMisc::CStatusMessageList CContextNetworkProxy::connectToNetwork(uint loginMode)
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::CStatusMessageList>(QLatin1Literal("connectToNetwork"), loginMode);
    }

    BlackMisc::CStatusMessageList CContextNetworkProxy::disconnectFromNetwork()
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::CStatusMessageList>(QLatin1Literal("disconnectFromNetwork"));
    }

    bool CContextNetworkProxy::isConnected() const
    {
        return this->m_dBusInterface->callDBusRet<bool>(QLatin1Literal("isConnected"));
    }

    BlackMisc::CStatusMessageList CContextNetworkProxy::setOwnAircraft(const BlackMisc::Aviation::CAircraft &aircraft)
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::CStatusMessageList>(QLatin1Literal("setOwnAircraft"), aircraft);
    }

    void CContextNetworkProxy::updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("updateOwnPosition"), position, altitude);
    }

    void CContextNetworkProxy::updateOwnSituation(const BlackMisc::Aviation::CAircraftSituation &situation)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("updateOwnSituation"), situation);
    }

    void CContextNetworkProxy::updateOwnCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("updateOwnCockpit"), com1, com2, transponder);
    }

    void CContextNetworkProxy::sendTextMessages(const BlackMisc::Network::CTextMessageList &textMessages)
    {
        this->m_dBusInterface->callDBus(QLatin1Literal("sendTextMessages"), textMessages);
    }

    BlackMisc::Aviation::CInformationMessage CContextNetworkProxy::getMetar(const QString &airportIcaoCode)
    {
        return this->m_dBusInterface->callDBusRet<BlackMisc::Aviation::CInformationMessage>(QLatin1Literal("getMetar"), airportIcaoCode);
    }

} // namespace
