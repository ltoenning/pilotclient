/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_VOICECAPABILITIES_H
#define BLACKMISC_NETWORK_VOICECAPABILITIES_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/blackmiscfreefunctions.h"

namespace BlackMisc
{
    namespace Network
    {
        //! Value object encapsulating information for voice capabilities.
        class BLACKMISC_EXPORT CVoiceCapabilities : public CValueObject<CVoiceCapabilities>
        {
        public:
            //! Voice capabilities
            enum VoiceCapabilities
            {
                Unknown,
                Voice,
                VoiceReceivingOnly,
                TextOnly,
            };

            //! Default constructor.
            CVoiceCapabilities() = default;

            //! Constructor by callsign
            CVoiceCapabilities(VoiceCapabilities capabilities) : m_voiceCapabilities(capabilities) {}

            //! Constructor.
            CVoiceCapabilities(const QString &flightPlanRemarks);

            //! Get capabilities
            VoiceCapabilities getCapabilities() const { return m_voiceCapabilities; }

            //! Set capabilites
            void setCapabilities(VoiceCapabilities capabilites) { m_voiceCapabilities = capabilites; }

            //! Is capability known
            bool isUnknown() const { return m_voiceCapabilities == Unknown; }

            //! \copydoc CValueObject::toIcon()
            CIcon toIcon() const;

            //! From enum
            static const CVoiceCapabilities &fromVoiceCapabilities(VoiceCapabilities capabilities);

            //! From flight plan remarks
            static CVoiceCapabilities fromFlightPlanRemarks(const QString &remarks)
            {
                return CVoiceCapabilities(remarks);
            }

            //! All capabilities as list
            static const QList<CVoiceCapabilities> &allCapabilities();

            //! \copydoc CValueObject::convertToQString
            QString convertToQString(bool i18n = false) const;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CVoiceCapabilities)
            VoiceCapabilities m_voiceCapabilities = Unknown;

            //! Capabilites from flight plans remarks such as "/V/"
            void setFromFlightPlanRemarks(const QString &flightPlanRemarks);
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Network::CVoiceCapabilities, (o.m_voiceCapabilities))
Q_DECLARE_METATYPE(BlackMisc::Network::CVoiceCapabilities)

#endif // guard
