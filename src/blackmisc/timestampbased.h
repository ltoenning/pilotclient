/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_TIMESTAMPBASED_H
#define BLACKMISC_TIMESTAMPBASED_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/variant.h"

#include <QDateTime>
#include <QString>
#include <QtGlobal>

namespace BlackMisc
{
    //! Entity with timestamp
    class BLACKMISC_EXPORT ITimestampBased
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexUtcTimestamp = CPropertyIndex::GlobalIndexITimestampBased,
            IndexUtcTimestampFormattedYmdhms,
            IndexUtcTimestampFormattedYmdhmsz,
            IndexUtcTimestampFormattedMdhms,
            IndexUtcTimestampFormattedMdhmsz,
            IndexUtcTimestampFormattedDhms,
            IndexUtcTimestampFormattedHms,
            IndexUtcTimestampFormattedHm,
            IndexMSecsSinceEpoch // keep this as last item
        };

        //! Get timestamp
        QDateTime getUtcTimestamp() const;

        //! Timestamp as ms value
        qint64 getMSecsSinceEpoch() const { return m_timestampMSecsSinceEpoch; }

        //! Set to null
        void setTimestampToNull();

        //! Timestamp as ms value
        void setMSecsSinceEpoch(qint64 mSecsSinceEpoch) { m_timestampMSecsSinceEpoch = mSecsSinceEpoch; }

        //! Set by value such as "20141003231045"
        void setByYearMonthDayHourMinute(const QString &yyyyMMddhhmmss);

        //! Set timestamp
        void setUtcTimestamp(const QDateTime &timestamp);

        //! Is this newer than other?
        bool isNewerThan(const ITimestampBased &otherTimestampObj) const;

        //! Is newer than epoch value?
        bool isNewerThan(qint64 mSecsSinceEpoch) const;

        //! Is this older than other?
        bool isOlderThan(const ITimestampBased &otherTimestampObj) const;

        //! Is this older than other?
        bool isOlderThan(qint64 mSecsSinceEpoch) const;

        //! Older than now-offset
        bool isOlderThanNowMinusOffset(int offsetMs) const;

        //! Same timestamp
        bool isSame(const ITimestampBased &otherTimestampObj) const;

        //! Time difference
        qint64 msecsTo(const ITimestampBased &otherTimestampObj) const;

        //! Time difference
        qint64 absMsecsTo(const ITimestampBased &otherTimestampObj) const;

        //! Milliseconds to now
        qint64 msecsToNow() const;

        //! Set the current time as timestamp
        void setCurrentUtcTime();

        //! Add the given number of milliseconds to the timestamp.
        void addMsecs(qint64 ms);

        //! As hh:mm:ss
        QString getFormattedUtcTimestampHms() const;

        //! As hh:mm:ss.zzz
        QString getFormattedUtcTimestampHmsz() const;

        //! As hh:mm
        QString getFormattedUtcTimestampHm() const;

        //! As dd HH mm ss
        QString getFormattedUtcTimestampDhms() const;

        //! As MM dd HH mm ss
        QString getFormattedUtcTimestampMdhms() const;

        //! As MM dd HH mm ss.zzz
        QString getFormattedUtcTimestampMdhmsz() const;

        //! As yyyy MM dd HH mm ss
        QString getFormattedUtcTimestampYmdhms() const;

        //! As yyyy MM dd HH mm ss.zzz
        QString getFormattedUtcTimestampYmdhmsz() const;

        //! Valid timestamp?
        bool hasValidTimestamp() const;

    protected:
        //! Can given index be handled
        static bool canHandleIndex(const CPropertyIndex &index);

        //! Constructor
        ITimestampBased();

        //! Constructor
        ITimestampBased(qint64 msSincePoch);

        //! Constructor
        ITimestampBased(const QDateTime &timestamp);

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        CVariant propertyByIndex(const CPropertyIndex &index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant);

        //! Compare for index
        int comparePropertyByIndex(const CPropertyIndex &index, const ITimestampBased &compareValue) const;

        qint64 m_timestampMSecsSinceEpoch = -1; //!< timestamp value
    };

    //! Entity with timestamp
    class BLACKMISC_EXPORT ITimestampWithOffsetBased : public ITimestampBased
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexOffsetMs = CPropertyIndex::GlobalIndexITimestampBased + ITimestampBased::IndexMSecsSinceEpoch + 1,
            IndexAdjustedMisWithOffset,
            IndexOffsetWithUnit // keep this as last item
        };

        //! Milliseconds to add to timestamp for interpolation
        void setTimeOffsetMs(qint64 offset) { m_timeOffsetMs = offset; }

        //! Milliseconds to add to timestamp for interpolation
        qint64 getTimeOffsetMs() const { return m_timeOffsetMs; }

        //! Offset with unit
        QString getTimeOffsetWithUnit() const;

        //! Timestamp with offset added for interpolation
        qint64 getAdjustedMSecsSinceEpoch() const { return this->getMSecsSinceEpoch() + this->getTimeOffsetMs(); }

        //! Timestamp and offset
        QString getTimestampAndOffset(bool formatted) const;

        //! Timestamp and offset
        QString getFormattedTimestampAndOffset(bool includeRawTimestamp) const;

    protected:
        //! Can given index be handled
        static bool canHandleIndex(const CPropertyIndex &index);

        //! Constructor
        ITimestampWithOffsetBased() : ITimestampBased() {}

        //! Constructor
        ITimestampWithOffsetBased(qint64 msSincePoch) : ITimestampBased(msSincePoch) {}

        //! Constructor
        ITimestampWithOffsetBased(const QDateTime &timestamp) : ITimestampBased(timestamp) {}

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const CVariant &variant);

        //! Compare for index
        int comparePropertyByIndex(const CPropertyIndex &index, const ITimestampWithOffsetBased &compareValue) const;

        qint64 m_timeOffsetMs = 0; //!< offset time in ms
    };
} // namespace

#endif // guard
