// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/fsd/authresponse.h"
#include "blackmisc/logmessage.h"

namespace BlackCore::Fsd
{
    AuthResponse::AuthResponse() : MessageBase()
    {}

    AuthResponse::AuthResponse(const QString &sender, const QString &receiver, const QString &response) : MessageBase(sender, receiver),
                                                                                                          m_response(response)
    {}

    QStringList AuthResponse::toTokens() const
    {
        auto tokens = QStringList {};
        tokens.push_back(m_sender);
        tokens.push_back(m_receiver);
        tokens.push_back(m_response);
        return tokens;
    }

    AuthResponse AuthResponse::fromTokens(const QStringList &tokens)
    {
        if (tokens.size() < 3)
        {
            BlackMisc::CLogMessage(static_cast<AuthResponse *>(nullptr)).warning(u"Wrong number of arguments.");
            return {};
        }
        return AuthResponse(tokens[0], tokens[1], tokens[2]);
    }
}
