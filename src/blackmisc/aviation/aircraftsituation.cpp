/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/pq/physicalquantity.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/variant.h"
#include "blackmisc/verify.h"
#include "QStringBuilder"
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;

namespace BlackMisc
{
    namespace Aviation
    {
        CAircraftSituation::CAircraftSituation()
            : m_groundElevation( { 0, nullptr }, CAltitude::MeanSeaLevel) {}

        CAircraftSituation::CAircraftSituation(const CCoordinateGeodetic &position, const CHeading &heading, const CAngle &pitch, const CAngle &bank, const CSpeed &gs, const CAltitude &groundElevation)
            : m_position(position), m_heading(heading), m_pitch(pitch),
              m_bank(bank), m_groundSpeed(gs), m_groundElevation(groundElevation) {}

        CAircraftSituation::CAircraftSituation(const CCallsign &correspondingCallsign, const CCoordinateGeodetic &position, const CHeading &heading, const CAngle &pitch, const CAngle &bank, const CSpeed &gs, const CAltitude &groundElevation)
            : m_correspondingCallsign(correspondingCallsign),
              m_position(position), m_heading(heading), m_pitch(pitch),
              m_bank(bank), m_groundSpeed(gs), m_groundElevation(groundElevation)
        {
            m_correspondingCallsign.setTypeHint(CCallsign::Aircraft);
        }

        QString CAircraftSituation::convertToQString(bool i18n) const
        {
            const QString s = (this->m_position.toQString(i18n)) %
                              QLatin1Literal(" bank: ") % (this->m_bank.toQString(i18n)) %
                              QLatin1Literal(" pitch: ") % (this->m_pitch.toQString(i18n)) %
                              QLatin1Literal(" gs: ") % (this->m_groundSpeed.toQString(i18n)) %
                              QLatin1Literal(" elevation: ") % (this->m_groundElevation.toQString(i18n)) %
                              QLatin1Literal(" heading: ") % (this->m_heading.toQString(i18n)) %
                              QLatin1Literal(" timestamp: ") % (this->getFormattedUtcTimestampDhms());
            return s;
        }

        const QString &CAircraftSituation::isOnGroundToString(CAircraftSituation::IsOnGround onGround)
        {
            static const QString notog("not on ground");
            static const QString og("on ground");
            static const QString unknown("unknown");

            switch (onGround)
            {
            case CAircraftSituation::NotOnGround: return notog;
            case CAircraftSituation::OnGround: return og;
            case CAircraftSituation::OnGroundSituationUnknown:
            default:
                return unknown;
            }
        }

        const QString &CAircraftSituation::onGroundReliabilityToString(CAircraftSituation::OnGroundReliability reliability)
        {
            static const QString elv("by elevation");
            static const QString elvCg("by elevation/CG");
            static const QString inter("by interpolation");
            static const QString guess("guessing");
            static const QString unknown("unknown");

            switch (reliability)
            {
            case CAircraftSituation::OnGroundByElevation: return elv;
            case CAircraftSituation::OnGroundByElevationAndCG: return elvCg;
            case CAircraftSituation::OnGroundByGuessing: return guess;
            case CAircraftSituation::OnGroundByInterpolation: return inter;
            case CAircraftSituation::OnGroundReliabilityNoSet:
            default:
                return unknown;
            }
        }

        CVariant CAircraftSituation::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }
            if (ICoordinateGeodetic::canHandleIndex(index)) { return ICoordinateGeodetic::propertyByIndex(index); }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexPosition:
                return this->m_position.propertyByIndex(index.copyFrontRemoved());
            case IndexLatitude:
                return this->latitude().propertyByIndex(index.copyFrontRemoved());
            case IndexLongitude:
                return this->longitude().propertyByIndex(index.copyFrontRemoved());
            case IndexAltitude:
                return this->getAltitude().propertyByIndex(index.copyFrontRemoved());
            case IndexHeading:
                return this->m_heading.propertyByIndex(index.copyFrontRemoved());
            case IndexPitch:
                return this->m_pitch.propertyByIndex(index.copyFrontRemoved());
            case IndexBank:
                return this->m_bank.propertyByIndex(index.copyFrontRemoved());
            case IndexGroundSpeed:
                return this->m_groundSpeed.propertyByIndex(index.copyFrontRemoved());
            case IndexGroundElevation:
                return this->m_groundElevation.propertyByIndex(index.copyFrontRemoved());
            case IndexCallsign:
                return this->m_correspondingCallsign.propertyByIndex(index.copyFrontRemoved());
            case IndexIsOnGround:
                return CVariant::fromValue(m_isOnGround);
            case IndexIsOnGroundString:
                return CVariant::fromValue(this->isOnGroundAsString());
            case IndexOnGroundReliability:
                return CVariant::fromValue(m_onGroundReliability);
            case IndexOnGroundReliabilityString:
                return CVariant::fromValue(this->getOnGroundReliabilityAsString());
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CAircraftSituation::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CAircraftSituation>(); return; }
            if (ITimestampBased::canHandleIndex(index))
            {
                ITimestampBased::setPropertyByIndex(index, variant);
                return;
            }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexPosition:
                this->m_position.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexPitch:
                this->m_pitch.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexBank:
                this->m_bank.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexGroundSpeed:
                this->m_groundSpeed.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexGroundElevation:
                this->m_groundElevation.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexCallsign:
                this->m_correspondingCallsign.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexIsOnGround:
                this->m_isOnGround = variant.toInt();
                break;
            case IndexOnGroundReliability:
                this->m_onGroundReliability = variant.toInt();
                break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }

        int CAircraftSituation::comparePropertyByIndex(const CPropertyIndex &index, const CAircraftSituation &compareValue) const
        {
            if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::comparePropertyByIndex(index, compareValue); }
            if (ICoordinateGeodetic::canHandleIndex(index)) { return ICoordinateGeodetic::comparePropertyByIndex(index, compareValue); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexPosition:
                return this->m_position.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getPosition());
            case IndexAltitude:
                return this->getAltitude().comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getAltitude());
            case IndexPitch:
                return this->m_pitch.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getPitch());
            case IndexBank:
                return this->m_bank.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getBank());
            case IndexGroundSpeed:
                return this->m_groundSpeed.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getGroundSpeed());
            case IndexGroundElevation:
                return this->m_groundElevation.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getGroundElevation());
            case IndexCallsign:
                return this->m_correspondingCallsign.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getCallsign());
            case IndexIsOnGround:
            case IndexIsOnGroundString:
                return Compare::compare(this->m_isOnGround, compareValue.m_isOnGround);
            case IndexOnGroundReliability:
            case IndexOnGroundReliabilityString:
                return Compare::compare(this->m_onGroundReliability, compareValue.m_onGroundReliability);
            default:
                break;
            }
            const QString assertMsg("No comparison for index " + index.toQString());
            BLACK_VERIFY_X(false, Q_FUNC_INFO, qUtf8Printable(assertMsg));
            return 0;
        }

        const QString &CAircraftSituation::isOnGroundAsString() const
        {
            return CAircraftSituation::isOnGroundToString(this->isOnGround());
        }

        bool CAircraftSituation::isOnGroundInfoAvailable() const
        {
            return this->isOnGround() != CAircraftSituation::OnGroundSituationUnknown &&
                   this->getOnGroundReliability() != CAircraftSituation::OnGroundReliabilityNoSet;
        }

        void CAircraftSituation::setOnGround(CAircraftSituation::IsOnGround onGround, CAircraftSituation::OnGroundReliability reliability)
        {
            this->setOnGround(onGround);
            this->setOnGroundReliabiliy(reliability);
        }

        const QString &CAircraftSituation::getOnGroundReliabilityAsString() const
        {
            return CAircraftSituation::onGroundReliabilityToString(this->getOnGroundReliability());
        }

        QString CAircraftSituation::getOnGroundInfo() const
        {
            return this->isOnGroundAsString() % QLatin1Char(' ') % this->getOnGroundReliabilityAsString();
        }

        bool CAircraftSituation::hasGroundElevation() const
        {
            return !this->getGroundElevation().isNull();
        }

        CLength CAircraftSituation::getHeightAboveGround() const
        {
            if (this->getAltitude().getReferenceDatum() == CAltitude::AboveGround)
            {
                // we have a sure value explicitly set
                return this->getAltitude();
            }
            const CLength gh(this->getGroundElevation());
            if (!gh.isNull() && !getAltitude().isNull())
            {
                return this->getAltitude() - gh;
            }
            return { 0, nullptr };
        }

        CAltitude CAircraftSituation::getCorrectedAltitude() const
        {
            if (this->getGroundElevation().isNull()) { return this->getAltitude(); }
            if (this->getAltitude().getReferenceDatum() != CAltitude::MeanSeaLevel) { return this->getAltitude(); }
            if (this->getGroundElevation() < this->getAltitude()) { return this->getAltitude(); }
            return this->getGroundElevation();
        }

        void CAircraftSituation::setCallsign(const CCallsign &callsign)
        {
            this->m_correspondingCallsign = callsign;
            this->m_correspondingCallsign.setTypeHint(CCallsign::Aircraft);
        }
    } // namespace
} // namespace
