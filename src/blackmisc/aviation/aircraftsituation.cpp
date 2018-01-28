/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/geo/elevationplane.h"
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
              m_bank(bank), m_groundSpeed(gs), m_groundElevation(groundElevation)
        {
            m_pressureAltitude = position.geodeticHeight().toPressureAltitude(CPressure(1013.25, CPressureUnit::mbar()));
        }

        CAircraftSituation::CAircraftSituation(const CCallsign &correspondingCallsign, const CCoordinateGeodetic &position, const CHeading &heading, const CAngle &pitch, const CAngle &bank, const CSpeed &gs, const CAltitude &groundElevation)
            : m_correspondingCallsign(correspondingCallsign),
              m_position(position), m_heading(heading), m_pitch(pitch),
              m_bank(bank), m_groundSpeed(gs), m_groundElevation(groundElevation)
        {
            m_correspondingCallsign.setTypeHint(CCallsign::Aircraft);
            m_pressureAltitude = position.geodeticHeight().toPressureAltitude(CPressure(1013.25, CPressureUnit::mbar()));
        }

        QString CAircraftSituation::convertToQString(bool i18n) const
        {
            return QStringLiteral("ts: ") % this->getFormattedTimestampAndOffset(true) %
                   QStringLiteral(" ") % m_position.toQString(i18n) %
                   QStringLiteral(" bank: ") % (m_bank.toQString(i18n)) %
                   QStringLiteral(" pitch: ") % (m_pitch.toQString(i18n)) %
                   QStringLiteral(" heading: ") % (m_heading.toQString(i18n)) %
                   QStringLiteral(" og: ") % this->getOnGroundInfo() %
                   QStringLiteral(" gs: ") % m_groundSpeed.valueRoundedWithUnit(CSpeedUnit::kts(), 1, true) %
                   QStringLiteral(" ") % m_groundSpeed.valueRoundedWithUnit(CSpeedUnit::m_s(), 1, true) %
                   QStringLiteral(" elevation: ") % (m_groundElevation.toQString(i18n));
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
            default: return unknown;
            }
        }

        const QString &CAircraftSituation::onGroundReliabilityToString(CAircraftSituation::OnGroundReliability reliability)
        {
            static const QString elv("elevation");
            static const QString elvCg("elevation/CG");
            static const QString inter("interpolation");
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
            if (ITimestampWithOffsetBased::canHandleIndex(index)) { return ITimestampWithOffsetBased::propertyByIndex(index); }
            if (ICoordinateGeodetic::canHandleIndex(index)) { return ICoordinateGeodetic::propertyByIndex(index); }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexPosition: return m_position.propertyByIndex(index.copyFrontRemoved());
            case IndexLatitude: return this->latitude().propertyByIndex(index.copyFrontRemoved());
            case IndexLongitude: return this->longitude().propertyByIndex(index.copyFrontRemoved());
            case IndexAltitude: return this->getAltitude().propertyByIndex(index.copyFrontRemoved());
            case IndexHeading: return m_heading.propertyByIndex(index.copyFrontRemoved());
            case IndexPitch: return m_pitch.propertyByIndex(index.copyFrontRemoved());
            case IndexBank: return m_bank.propertyByIndex(index.copyFrontRemoved());
            case IndexGroundSpeed: return m_groundSpeed.propertyByIndex(index.copyFrontRemoved());
            case IndexGroundElevation: return m_groundElevation.propertyByIndex(index.copyFrontRemoved());
            case IndexCallsign: return m_correspondingCallsign.propertyByIndex(index.copyFrontRemoved());
            case IndexIsOnGround: return CVariant::fromValue(m_isOnGround);
            case IndexIsOnGroundString: return CVariant::fromValue(this->isOnGroundAsString());
            case IndexOnGroundReliability: return CVariant::fromValue(m_onGroundReliability);
            case IndexOnGroundReliabilityString: return CVariant::fromValue(this->getOnGroundReliabilityAsString());
            default: return CValueObject::propertyByIndex(index);
            }
        }

        void CAircraftSituation::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CAircraftSituation>(); return; }
            if (ITimestampWithOffsetBased::canHandleIndex(index)) { ITimestampWithOffsetBased::setPropertyByIndex(index, variant); return; }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexPosition: m_position.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexPitch: m_pitch.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexBank: m_bank.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexGroundSpeed: m_groundSpeed.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexGroundElevation: m_groundElevation.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexCallsign: m_correspondingCallsign.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexIsOnGround: m_isOnGround = variant.toInt(); break;
            case IndexOnGroundReliability: m_onGroundReliability = variant.toInt(); break;
            default: CValueObject::setPropertyByIndex(index, variant); break;
            }
        }

        int CAircraftSituation::comparePropertyByIndex(const CPropertyIndex &index, const CAircraftSituation &compareValue) const
        {
            if (ITimestampWithOffsetBased::canHandleIndex(index)) { return ITimestampWithOffsetBased::comparePropertyByIndex(index, compareValue); }
            if (ICoordinateGeodetic::canHandleIndex(index)) { return ICoordinateGeodetic::comparePropertyByIndex(index, compareValue); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexPosition: return m_position.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getPosition());
            case IndexAltitude: return this->getAltitude().comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getAltitude());
            case IndexPitch: return m_pitch.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getPitch());
            case IndexBank: return m_bank.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getBank());
            case IndexGroundSpeed: return m_groundSpeed.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getGroundSpeed());
            case IndexGroundElevation: return m_groundElevation.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getGroundElevation());
            case IndexCallsign: return m_correspondingCallsign.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getCallsign());
            case IndexIsOnGround:
            case IndexIsOnGroundString:
                return Compare::compare(m_isOnGround, compareValue.m_isOnGround);
            case IndexOnGroundReliability:
            case IndexOnGroundReliabilityString:
                return Compare::compare(m_onGroundReliability, compareValue.m_onGroundReliability);
            default: break;
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

        bool CAircraftSituation::setGroundElevationChecked(const CAltitude &elevation, bool ignoreNullValues, bool overrideExisting)
        {
            if (ignoreNullValues && elevation.isNull()) { return false; }
            if (!overrideExisting && this->hasGroundElevation()) { return false; }
            m_groundElevation = elevation;
            return true;
        }

        bool CAircraftSituation::setGroundElevationChecked(const CElevationPlane &elevationPlane, bool ignoreNullValues, bool overrideExisting)
        {
            if (ignoreNullValues && elevationPlane.isNull()) { return false; }
            if (!overrideExisting && this->hasGroundElevation()) { return false; }
            if (!elevationPlane.isWithinRange(*this)) { return false; }
            m_groundElevation = elevationPlane.getAltitude();
            return true;
        }

        CLength CAircraftSituation::getHeightAboveGround() const
        {
            if (this->getAltitude().isNull()) { return { 0, nullptr }; }
            if (this->getAltitude().getReferenceDatum() == CAltitude::AboveGround)
            {
                // we have a sure value explicitly set
                return this->getAltitude();
            }
            const CLength gh(this->getGroundElevation());
            if (gh.isNull()) { return { 0, nullptr }; }
            return this->getAltitude() - gh;
        }

        CAltitude CAircraftSituation::getCorrectedAltitude(const CLength &centerOfGravity, bool *corrected) const
        {
            if (corrected) { *corrected = false; }
            if (this->getGroundElevation().isNull()) { return this->getAltitude(); }

            // above ground
            if (this->getAltitude().getReferenceDatum() == CAltitude::AboveGround)
            {
                const CAltitude aboveGround = this->getAltitude().withOffset(centerOfGravity);
                const bool c = !aboveGround.isPositiveWithEpsilonConsidered();
                if (corrected) { *corrected = c; }
                return c ?
                       CAltitude(0, CAltitude::AboveGround, aboveGround.getUnit()) :
                       aboveGround;
            }
            else
            {
                const CAltitude groundPlusCG = this->getGroundElevation().withOffset(centerOfGravity);
                const bool c = (this->getAltitude() < groundPlusCG);
                if (corrected) { *corrected = c; }
                return c ?
                       groundPlusCG :
                       this->getAltitude();
            }
        }

        void CAircraftSituation::setPressureAltitude(const CAltitude &altitude)
        {
            Q_ASSERT(altitude.getAltitudeType() == CAltitude::PressureAltitude);
            m_pressureAltitude = altitude;
        }

        CLength CAircraftSituation::getDistancePerTime(const CTime &time) const
        {
            if (this->getGroundSpeed().isNull()) { return CLength(0, CLengthUnit::nullUnit()); }
            const int ms = time.valueInteger(CTimeUnit::ms());
            return this->getDistancePerTime(ms);
        }

        CLength CAircraftSituation::getDistancePerTime(int milliseconds) const
        {
            if (this->getGroundSpeed().isNull()) { return CLength(0, CLengthUnit::nullUnit()); }
            const double seconds = milliseconds / 1000;
            const double gsMeterSecond = this->getGroundSpeed().value(CSpeedUnit::m_s());
            const CLength d(seconds * gsMeterSecond, CLengthUnit::m());
            return d;
        }

        void CAircraftSituation::setCallsign(const CCallsign &callsign)
        {
            m_correspondingCallsign = callsign;
            m_correspondingCallsign.setTypeHint(CCallsign::Aircraft);
        }
    } // namespace
} // namespace
