// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_FSD_PONG_H
#define BLACKCORE_FSD_PONG_H

#include "blackcore/fsd/messagebase.h"

namespace BlackCore::Fsd
{
    //! Sent or received as reply to a ping.
    class BLACKCORE_EXPORT Pong : public MessageBase
    {
    public:
        //! Constructor
        Pong(const QString &sender, const QString &receiver, const QString &timestamp);

        //! Message converted to tokens
        QStringList toTokens() const;

        //! Construct from tokens
        static Pong fromTokens(const QStringList &tokens);

        //! PDU identifier
        static QString pdu() { return "$PO"; }

        QString m_timestamp; //!< timestamp

    private:
        Pong();
    };

    //! Equal to operator
    inline bool operator==(const Pong &lhs, const Pong &rhs)
    {
        return lhs.sender() == rhs.sender() &&
               lhs.receiver() == rhs.receiver() &&
               lhs.m_timestamp == rhs.m_timestamp;
    }

    //! Not equal to operator
    inline bool operator!=(const Pong &lhs, const Pong &rhs)
    {
        return !(lhs == rhs);
    }
}

#endif // guard
