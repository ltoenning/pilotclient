/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_AFV_CONNECTION_CLIENTCONNECTION_H
#define BLACKCORE_AFV_CONNECTION_CLIENTCONNECTION_H

#include "blackcore/afv/crypto/cryptodtoserializer.h"
#include "blackcore/afv/connection/clientconnectiondata.h"
#include "blackcore/afv/connection/apiserverconnection.h"
#include "blackcore/afv/dto.h"

#include <QObject>
#include <QString>
#include <QTimer>
#include <QUdpSocket>

namespace BlackCore
{
    namespace Afv
    {
        namespace Connection
        {
            //! Client connection
            class CClientConnection : public QObject
            {
                Q_OBJECT

            public:
                //! Com status
                enum ConnectionStatus
                {
                    Disconnected,   //!< Not connected
                    Connected,      //!< Connection established
                };
                Q_ENUM(ConnectionStatus)

                CClientConnection(const QString &apiServer, QObject *parent = nullptr);

                void connectTo(const QString &userName, const QString &password, const QString &callsign);
                void disconnectFrom(const QString &reason = {});

                bool isConnected() const { return m_connection.m_connected; }

                void setReceiveAudio(bool value) { m_connection.m_receiveAudio = value; }
                bool receiveAudio() const { return m_connection.m_receiveAudio; }

                template<typename T>
                void sendToVoiceServer(T dto)
                {
                    QUrl voiceServerUrl("udp://" + m_connection.m_tokens.VoiceServer.addressIpV4);
                    QByteArray dataBytes = Crypto::CryptoDtoSerializer::serialize(*m_connection.voiceCryptoChannel, CryptoDtoMode::AEAD_ChaCha20Poly1305, dto);
                    m_udpSocket->writeDatagram(dataBytes, QHostAddress(voiceServerUrl.host()), static_cast<quint16>(voiceServerUrl.port()));
                }

                bool receiveAudioDto() const;
                void setReceiveAudioDto(bool receiveAudioDto);

                void updateTransceivers(const QString &callsign, const QVector<TransceiverDto> &transceivers);

                QVector<StationDto> getAllAliasedStations();

            signals:
                void audioReceived(const AudioRxOnTransceiversDto &dto);

            private:
                void connectToVoiceServer();
                void disconnectFromVoiceServer();

                void readPendingDatagrams();
                void processMessage(const QByteArray &messageDdata, bool loopback = false);
                void handleSocketError(QAbstractSocket::SocketError error);

                void voiceServerHeartbeat();

                const QUuid m_networkVersion = QUuid("3a5ddc6d-cf5d-4319-bd0e-d184f772db80");

                //Data
                CClientConnectionData m_connection;

                // Voice server
                QUdpSocket *m_udpSocket = nullptr;
                QTimer *m_voiceServerTimer = nullptr;

                // API server
                CApiServerConnection *m_apiServerConnection = nullptr;

                // Properties
                bool m_receiveAudioDto = true;
            };
        } // ns
    } // ns
} // ns

#endif // guard
