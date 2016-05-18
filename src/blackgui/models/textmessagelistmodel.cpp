/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/models/columnformatters.h"
#include "blackgui/models/columns.h"
#include "blackgui/models/textmessagelistmodel.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/timestampbased.h"

#include <Qt>
#include <QtDebug>
#include <QtGlobal>

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;

namespace BlackGui
{
    namespace Models
    {
        CTextMessageListModel::CTextMessageListModel(TextMessageMode mode, QObject *parent) :
            CListModelBase("ModelTextMessageList", parent), m_textMessageMode(NotSet)
        {
            this->setTextMessageMode(mode);

            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("ModelTextMessageList", "time");
            (void)QT_TRANSLATE_NOOP("ModelTextMessageList", "from");
            (void)QT_TRANSLATE_NOOP("ModelTextMessageList", "to");
            (void)QT_TRANSLATE_NOOP("ModelTextMessageList", "message");
        }

        void CTextMessageListModel::setTextMessageMode(CTextMessageListModel::TextMessageMode mode)
        {
            if (this->m_textMessageMode == mode) return;
            this->m_textMessageMode = mode;
            this->m_columns.clear();
            switch (mode)
            {
            case NotSet:
            case FromTo:
                {
                    CColumn col = CColumn("type", CTextMessage::IndexIcon);
                    col.setSortPropertyIndex({ CTextMessage::IndexSenderCallsign, CCallsign::IndexSuffix });
                    this->m_columns.addColumn(col);
                    this->m_columns.addColumn(CColumn("time", "received", CTextMessage::IndexUtcTimestamp, new CDateTimeFormatter(CDateTimeFormatter::formatHms())));
                    this->m_columns.addColumn(CColumn::standardString("from", { CTextMessage::IndexSenderCallsign, CCallsign::IndexCallsignString }));
                    this->m_columns.addColumn(CColumn::standardString("to", CTextMessage::IndexRecipientCallsignOrFrequency));
                    this->m_columns.addColumn(CColumn::standardString("message", CTextMessage::IndexMessage));

                    // default sort order
                    this->setSortColumnByPropertyIndex(CTextMessage::IndexUtcTimestamp);
                    this->m_sortOrder = Qt::DescendingOrder;
                }
                break;

            case From:
                {
                    this->m_columns.addColumn(CColumn("time", "received", CTextMessage::IndexUtcTimestamp, new CDateTimeFormatter(CDateTimeFormatter::formatHms())));
                    this->m_columns.addColumn(CColumn::standardString("from", { CTextMessage::IndexSenderCallsign, CCallsign::IndexCallsignString }));
                    this->m_columns.addColumn(CColumn::standardString("message", CTextMessage::IndexMessage));

                    // default sort order
                    this->setSortColumnByPropertyIndex(CTextMessage::IndexUtcTimestamp);
                    this->m_sortOrder = Qt::DescendingOrder;
                }
                break;

            default:
                qFatal("Wrong mode");
                break;
            }
        }

    }  // namespace
} // namespace
