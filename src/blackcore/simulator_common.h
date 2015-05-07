/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_SIMULATOR_COMMON_H
#define BLACKCORE_SIMULATOR_COMMON_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/interpolator.h"
#include "blackcore/simulator.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/simulation/simulatorsetup.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/ownaircraftprovider.h"
#include "blackmisc/simulation/remoteaircraftprovider.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/network/client.h"
#include "blackmisc/pixmap.h"
#include <QObject>

namespace BlackCore
{

    //! Common base class with providers, interface and some base functionality
    class BLACKCORE_EXPORT CSimulatorCommon :
        public BlackCore::ISimulator,
        public BlackMisc::Simulation::COwnAircraftAware,   // gain access to in memor own aircraft data
        public BlackMisc::Simulation::CRemoteAircraftAware // gain access to in memory remote aircraft data
    {

        Q_OBJECT

    public:
        //! \copydoc ISimulator::getMaxRenderedAircraft
        virtual int getMaxRenderedAircraft() const override;

        //! \copydoc ISimulator::setMaxRenderedAircraft
        virtual void setMaxRenderedAircraft(int maxRenderedAircraft) override;

        //! \copydoc ISimulator::setMaxRenderedDistance
        virtual void setMaxRenderedDistance(BlackMisc::PhysicalQuantities::CLength &distance) override;

        //! \copydoc ISimulator::getMaxRenderedDistance
        virtual BlackMisc::PhysicalQuantities::CLength getMaxRenderedDistance() const override;

        //! \copydoc ISimulator::getRenderedDistanceBoundary
        virtual BlackMisc::PhysicalQuantities::CLength getRenderedDistanceBoundary() const override;

        //! \copydoc ISimulator::isMaxAircraftRestricted
        virtual bool isMaxAircraftRestricted() const override;

        //! \copydoc ISimulator::isMaxDistanceRestricted
        virtual bool isMaxDistanceRestricted() const override;

        //! \copydoc ISimulator::enableDebuggingMessages
        virtual void enableDebugMessages(bool driverMessages, bool interpolatorMessages) override;

        //! \copydoc ISimulator::getInstalledModelsCount
        virtual int getInstalledModelsCount() const override;

        //! \copydoc IContextSimulator::highlightAircraft
        virtual void highlightAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const BlackMisc::PhysicalQuantities::CTime &displayTime) override;

        //! \copydoc IContextSimulator::isRenderingEnabled
        virtual bool isRenderingEnabled() const override;

        //! \copydoc IContextSimulator::isRenderingRestricted
        virtual bool isRenderingRestricted() const override;

        //! \copydoc IContextSimulator::getSimulatorPluginInfo
        virtual const BlackMisc::Simulation::CSimulatorPluginInfo &getSimulatorPluginInfo() const override;

        //! \copydoc IContextSimulator::getSimulatorSetup
        virtual const BlackMisc::Simulation::CSimulatorSetup &getSimulatorSetup() const override;

        //! \copydoc IContextSimulator::deleteAllRenderingRestrictions
        virtual void deleteAllRenderingRestrictions() override;

        //! \copydoc IContextSimulator::physicallyRemoveRemoteAircraft
        virtual bool physicallyRemoveMultipleRemoteAircraft(const BlackMisc::Aviation::CCallsignSet &callsigns) override;

    protected slots:
        //! Slow timer used to highlight aircraft, can be used for other things too
        virtual void ps_oneSecondTimer();

        //! Recalculate the rendered aircraft
        virtual void ps_recalculateRenderedAircraft();

        //! Recalculate the rendered aircraft
        virtual void ps_recalculateRenderedAircraft(const BlackMisc::Simulation::CAirspaceAircraftSnapshot &snapshot);

        //! Provider added situation
        virtual void ps_remoteProviderAddAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

        //! Provider added parts
        virtual void ps_remoteProviderAddAircraftParts(const BlackMisc::Aviation::CAircraftParts &parts);

        //! Provider removed aircraft
        virtual void ps_remoteProviderRemovedAircraft(const BlackMisc::Aviation::CCallsign &callsign);

    protected:
        //! Constructor
        CSimulatorCommon(const BlackMisc::Simulation::CSimulatorPluginInfo &info,
                         BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
                         BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                         QObject *parent = nullptr);

        //! \copydoc IContextSimulator::logicallyAddRemoteAircraft
        virtual bool logicallyAddRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft) override;

        //! \copydoc IContextSimulator::logicallyRemoveRemoteAircraft
        virtual bool logicallyRemoveRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;

        //! Blink the highlighted aircraft
        void blinkHighlightedAircraft();

        //! Restore aircraft from backedn data
        void resetAircraftFromBacked(const BlackMisc::Aviation::CCallsign &callsign);

        //! Override parts and situation from current interpolator values, if any!
        void setInitialAircraftSituationAndParts(BlackMisc::Simulation::CSimulatedAircraft &aircraft) const;

        bool m_debugMessages = false;             //!< Display debug messages
        bool m_blinkCycle = false;                //!< use for highlighting
        IInterpolator *m_interpolator = nullptr;  //!< interpolator instance
        qint64 m_highlightEndTimeMsEpoch = 0;     //!< end highlighting
        int m_timerCounter = 0;                   //!< allows to calculate n seconds
        QTimer *m_oneSecondTimer = nullptr;       //!< timer
        BlackMisc::Simulation::CSimulatorPluginInfo m_simulatorPluginInfo;   //!< info object
        BlackMisc::Simulation::CSimulatorSetup m_simulatorSetup;             //!< setup object
        BlackMisc::Simulation::CSimulatedAircraftList m_highlightedAircraft; //!< all other aircraft are to be ignored
        BlackMisc::Aviation::CCallsignSet m_callsignsToBeRendered;           //!< callsigns which will be rendered
        int m_maxRenderedAircraft = MaxAircraftInfinite;                     //!< max.rendered aircraft
        BlackMisc::PhysicalQuantities::CLength m_maxRenderedDistance { 0.0, BlackMisc::PhysicalQuantities::CLengthUnit::nullUnit()}; //!< max.distance for rendering
    };

} // namespace

#endif // guard
