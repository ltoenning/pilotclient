/* Copyright (C) 2018
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "xplanempaircraft.h"
#include "blackcore/simulator.h"
#include "blackmisc/simulation/interpolatormulti.h"

using namespace BlackCore;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;

namespace BlackSimPlugin
{
    namespace XPlane
    {
        CXPlaneMPAircraft::CXPlaneMPAircraft()
        { }

        CXPlaneMPAircraft::CXPlaneMPAircraft(
            const CSimulatedAircraft &aircraft, ISimulator *simulator, CInterpolationLogger *logger) :
            m_aircraft(aircraft),
            m_interpolator(QSharedPointer<CInterpolatorMulti>::create(aircraft.getCallsign(), simulator, simulator, simulator->getRemoteAircraftProvider(), logger))
        {
            m_interpolator->attachLogger(logger);
            m_interpolator->initCorrespondingModel(aircraft.getModel());
        }

        QString CXPlaneMPAircraft::getInterpolatorInfo(CInterpolationAndRenderingSetupBase::InterpolatorMode mode) const
        {
            Q_ASSERT(m_interpolator);
            return m_interpolator->getInterpolatorInfo(mode);
        }

        void CXPlaneMPAircraft::attachInterpolatorLogger(CInterpolationLogger *logger) const
        {
            Q_ASSERT(m_interpolator);
            return m_interpolator->attachLogger(logger);
        }

        CInterpolationResult CXPlaneMPAircraft::getInterpolation(qint64 currentTimeSinceEpoc, const CInterpolationAndRenderingSetupPerCallsign &setup, int aircraftNumber) const
        {
            Q_ASSERT(m_interpolator);
            return m_interpolator->getInterpolation(currentTimeSinceEpoc, setup, aircraftNumber);
        }

        CCallsignSet CXPlaneMPAircraftObjects::getAllCallsigns() const
        {
            return CCallsignSet(this->keys());
        }

        QStringList CXPlaneMPAircraftObjects::getAllCallsignStrings(bool sorted) const
        {
            return this->getAllCallsigns().getCallsignStrings(sorted);
        }
    } // namespace
} // namespace
