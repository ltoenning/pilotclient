/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/input/keyboardkey.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/variant.h"

namespace BlackMisc
{
    namespace Input
    {
        CKeyboardKey::CKeyboardKey() :
            m_keyCode(Key_Unknown)
        {}

        CKeyboardKey::CKeyboardKey(KeyCode keyCode) :
            m_keyCode(keyCode)
        {}

        QString CKeyboardKey::convertToQString(bool /* i18n */) const
        {
            return getKeyAsString();
        }

        void CKeyboardKey::setKeyObject(const CKeyboardKey &key)
        {
            this->m_keyCode = key.m_keyCode;
        }

        QString CKeyboardKey::getKeyAsString() const
        {
            if (m_keyCode == Key_Unknown) return QString();

            static const QHash<KeyCode, QString> keyStrings =
            {
                { Key_ShiftLeft, QStringLiteral("ShiftLeft") },
                { Key_ShiftRight, QStringLiteral("ShiftRight") },
                { Key_ControlLeft, QStringLiteral("CtrlLeft") },
                { Key_ControlRight, QStringLiteral("CtrlRight") },
                { Key_AltLeft, QStringLiteral("AltLeft") },
                { Key_AltRight, QStringLiteral("AltRight") }
            };

            if (isModifier()) { return keyStrings.value(m_keyCode); }

            QChar key = QChar::fromLatin1(m_keyCode);
            return key;
        }

        const QList<KeyCode> &CKeyboardKey::allModifiers()
        {
            static const QList<KeyCode> allModifiers =
            {
                Key_ShiftLeft,
                Key_ShiftRight,
                Key_ControlLeft,
                Key_ControlRight,
                Key_AltLeft,
                Key_AltRight,
            };
            return allModifiers;
        }

        CVariant CKeyboardKey::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexKey:
                return CVariant::from(this->m_keyCode);
            case IndexKeyAsString:
                return CVariant::from(getKeyAsString());
            default:
                break;
            }

            Q_ASSERT_X(false, "CKeyboardKey", "index unknown");
            QString m = QString("no property, index ").append(index.toQString());
            return CVariant::fromValue(m);
        }

        void CKeyboardKey::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself()) { (*this) = variant.to<CKeyboardKey>(); return; }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexKey:
            case IndexKeyAsString:
                qFatal("Not implemented");
                break;
            case IndexKeyObject:
                this->setKeyObject(variant.value<CKeyboardKey>());
                break;
            default:
                Q_ASSERT_X(false, "CKeyboardKey", "index unknown (setter)");
                break;
            }
        }
    } // namespace Hardware
} // BlackMisc
