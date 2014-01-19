/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKCORE_NETWORK_VATLIB_H
#define BLACKCORE_NETWORK_VATLIB_H

#include "network.h"
#include "blackmisc/avallclasses.h"
#include <vatlib/vatlib.h>
#include <QScopedPointer>
#include <QTimer>
#include <QTextCodec>
#include <QByteArray>
#include <QMap>

namespace BlackCore
{

    class CNetworkVatlib : public INetwork
    {
        Q_OBJECT

    public:
        CNetworkVatlib(QObject *parent = nullptr);
        virtual ~CNetworkVatlib();

        bool isConnected() const { return m_status == Cvatlib_Network::connStatus_Connected; }

    public: // INetwork slots overrides

        // Network
        virtual void presetServer(const BlackMisc::Network::CServer &server);
        virtual void presetCallsign(const BlackMisc::Aviation::CCallsign &callsign);
        virtual void presetIcaoCodes(const BlackMisc::Aviation::CAircraftIcao &icao);
        virtual void presetLoginMode(LoginMode mode);
        virtual void initiateConnection();
        virtual void terminateConnection();
        virtual void sendPing(const BlackMisc::Aviation::CCallsign &callsign);

        virtual void sendNameQuery(const BlackMisc::Aviation::CCallsign &callsign);
        virtual void sendIpQuery();
        virtual void sendServerQuery(const BlackMisc::Aviation::CCallsign &callsign);

        // Text messages
        virtual void sendTextMessages(const BlackMisc::Network::CTextMessageList &messages);

        // ATC
        virtual void sendAtcQuery(const BlackMisc::Aviation::CCallsign &callsign);
        virtual void sendAtisQuery(const BlackMisc::Aviation::CCallsign &callsign);

        // Aircraft
        virtual void sendCapabilitiesQuery(const BlackMisc::Aviation::CCallsign &callsign);
        virtual void sendAircraftInfoQuery(const BlackMisc::Aviation::CCallsign &callsign);
        virtual void sendFrequencyQuery(const BlackMisc::Aviation::CCallsign &callsign);
        virtual void setOwnAircraft(const BlackMisc::Aviation::CAircraft &aircraft);
        virtual void setOwnAircraftPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude);
        virtual void setOwnAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation);
        virtual void setOwnAircraftAvionics(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2,
            const BlackMisc::Aviation::CTransponder &xpdr);

        // Weather
        virtual void sendMetarQuery(const QString &airportICAO);
        virtual void sendWeatherDataQuery(const QString &airportICAO);

    private slots:
        void replyToFrequencyQuery(const BlackMisc::Aviation::CCallsign &callsign);
        void replyToNameQuery(const BlackMisc::Aviation::CCallsign &callsign);
        void sendAircraftInfo(const BlackMisc::Aviation::CCallsign &callsign);

    private: //shimlib callbacks
        static void onConnectionStatusChanged(Cvatlib_Network *, Cvatlib_Network::connStatus oldStatus, Cvatlib_Network::connStatus newStatus, void *cbvar);
        static void onTextMessageReceived(Cvatlib_Network *, const char *from, const char *to, const char *msg, void *cbvar);
        static void onRadioMessageReceived(Cvatlib_Network *, const char *from, INT numFreq, INT *freqList, const char *msg, void *cbvar);
        static void onControllerDisconnected(Cvatlib_Network *, const char *callsign, void *cbvar);
        static void onInterimPilotPositionUpdate(Cvatlib_Network *, const char *callsign, Cvatlib_Network::PilotPosUpdate pos, void *cbvar);
        static void onAtcPositionUpdate(Cvatlib_Network *, const char *callsign, Cvatlib_Network::ATCPosUpdate pos, void *cbvar);
        static void onKicked(Cvatlib_Network *, const char *reason, void *cbvar);
        static void onPong(Cvatlib_Network *, const char *callsign, INT elapsedTime, void *cbvar);
        static void onMetarReceived(Cvatlib_Network *, const char *data, void *cbvar);
        static void onInfoQueryRequestReceived(Cvatlib_Network *, const char *callsign, Cvatlib_Network::infoQuery type, const char *data, void *cbvar);
        static void onInfoQueryReplyReceived(Cvatlib_Network *, const char *callsign, Cvatlib_Network::infoQuery type, const char *data, const char *data2, void *cbvar);
        static void onCapabilitiesReplyReceived(Cvatlib_Network *, const char *callsign, const char **keysValues, void *cbvar);
        static void onAtisReplyReceived(Cvatlib_Network *, const char *callsign, Cvatlib_Network::atisLineType type, const char *data, void *cbvar);
        static void onTemperatureDataReceived(Cvatlib_Network *, Cvatlib_Network::TempLayer layers[4], INT pressure, void *cbvar);
        static void onErrorReceived(Cvatlib_Network *, Cvatlib_Network::error type, const char *msg, const char *data, void *cbvar);
        static void onWindDataReceived(Cvatlib_Network *, Cvatlib_Network::WindLayer layers[4], void *cbvar);
        static void onCloudDataReceived(Cvatlib_Network *, Cvatlib_Network::CloudLayer layers[2], Cvatlib_Network::StormLayer storm, float vis, void *cbvar);
        static void onPilotDisconnected(Cvatlib_Network *, const char *callsign, void *cbvar);
        static void onPilotInfoRequestReceived(Cvatlib_Network *, const char *callsign, void *cbvar);
        static void onPilotInfoReceived(Cvatlib_Network *, const char *callsign, const char **keysValues, void *cbvar);
        static void onPilotPositionUpdate(Cvatlib_Network *, const char *callsign, Cvatlib_Network::PilotPosUpdate pos, void *cbvar);

    private:
        QByteArray toFSD(QString qstr) const;
        QByteArray toFSD(const BlackMisc::Aviation::CCallsign &callsign) const;
        QString fromFSD(const char *cstr) const;
        void initializeSession();
        bool isDisconnected() const { return m_status != Cvatlib_Network::connStatus_Connecting && m_status != Cvatlib_Network::connStatus_Connected; }

    private slots:
        void process();
        void update();

    signals:
        void terminate();

    public:
        //! Deletion policy for QScopedPointer
        struct VatlibQScopedPointerDeleter
        {
            //! Called by QScopedPointer destructor
            static void cleanup(Cvatlib_Network *net) { if (net) net->Destroy(); }
        };

    private:
        QScopedPointer<Cvatlib_Network, VatlibQScopedPointerDeleter> m_net;
        LoginMode m_loginMode;
        Cvatlib_Network::connStatus m_status;
        BlackMisc::Network::CServer m_server;
        BlackMisc::Aviation::CCallsign m_callsign;
        BlackMisc::Aviation::CAircraftIcao m_icaoCodes;
        BlackMisc::Aviation::CAircraft m_ownAircraft; // not using callsign, user, or icao parts of this member because they can't be changed when connected
        QMap<BlackMisc::Aviation::CCallsign, BlackMisc::Aviation::CInformationMessage> m_atisParts;

        QTimer m_processingTimer;
        QTimer m_updateTimer;
        static int const c_processingIntervalMsec = 100;
        static int const c_updateIntervalMsec = 5000;
        static int const c_logoffTimeoutSec = 5;
        QTextCodec *m_fsdTextCodec;
    };

} //namespace BlackCore

#endif // guard
