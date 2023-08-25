// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackmisc/crashsettings.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/comparefunctions.h"

#include <QStringBuilder>

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackMisc::Settings, CCrashSettings)

namespace BlackMisc::Settings
{
    CCrashSettings::CCrashSettings() {}

    QString CCrashSettings::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return QStringLiteral("{ %1, %2 }").arg(boolToYesNo(this->isEnabled()), boolToYesNo(this->withPrivacyInfo()));
    }

    QVariant CCrashSettings::propertyByIndex(CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexEnabled: return QVariant::fromValue(this->isEnabled());
        case IndexPrivateInfo: return QVariant::fromValue(this->withPrivacyInfo());
        default: break;
        }
        return CValueObject::propertyByIndex(index);
    }

    void CCrashSettings::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CCrashSettings>();
            return;
        }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexEnabled: this->setEnabled(variant.toBool()); break;
        case IndexPrivateInfo: this->setPrivacyInfo(variant.toBool()); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    int CCrashSettings::comparePropertyByIndex(CPropertyIndexRef index, const CCrashSettings &compareValue) const
    {
        if (index.isMyself()) { return this->convertToQString().compare(compareValue.convertToQString()); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexEnabled: return Compare::compare(this->isEnabled(), compareValue.isEnabled());
        case IndexPrivateInfo: return Compare::compare(this->withPrivacyInfo(), compareValue.withPrivacyInfo());
        default: return CValueObject::comparePropertyByIndex(index.copyFrontRemoved(), compareValue);
        }
    }
} // ns
