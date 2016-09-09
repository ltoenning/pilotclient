/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXT_CONTEXTSIMULATOR_IMPL_H
#define BLACKCORE_CONTEXT_CONTEXTSIMULATOR_IMPL_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/context/contextsimulator.h"
#include "blackcore/corefacadeconfig.h"
#include "blackcore/application/applicationsettings.h"
#include "blackcore/weathermanager.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/identifier.h"
#include "blackmisc/network/textmessagelist.h"
#include "blackmisc/pixmap.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/settingscache.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/simulation/simulatorplugininfolist.h"
#include "blackmisc/simulation/simulatorsetup.h"
#include "blackmisc/worker.h"

#include <QObject>
#include <QPair>
#include <QString>

namespace BlackMisc
{
    class CDBusServer;
    namespace Aviation { class CCallsign; }
    namespace Simulation { class CSimulatedAircraft; }
}

namespace BlackCore
{
    class CCoreFacade;
    class CPluginManagerSimulator;
    class ISimulator;

    namespace Context
    {
        //! Network simulator concrete implementation
        class BLACKCORE_EXPORT CContextSimulator :
                public IContextSimulator,
                public BlackMisc::CIdentifiable
        {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTSIMULATOR_INTERFACENAME)
            friend class BlackCore::CCoreFacade;
            friend class IContextSimulator;

        public:
            //! Destructor
            virtual ~CContextSimulator();

            //! Gracefully shut down, e.g. for plugin unloading
            void gracefulShutdown();

        public slots:
            //! \copydoc IContextSimulator::getSimulatorPluginInfo()
            virtual BlackMisc::Simulation::CSimulatorPluginInfo getSimulatorPluginInfo() const override;

            //! \copydoc IContextSimulator::getAvailableSimulatorPlugins()
            virtual BlackMisc::Simulation::CSimulatorPluginInfoList getAvailableSimulatorPlugins() const override;

            //! \copydoc IContextSimulator::startSimulatorPlugin()
            virtual bool startSimulatorPlugin(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorInfo) override;

            //! \copydoc IContextSimulator::stopSimulatorPlugin()
            virtual void stopSimulatorPlugin(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorInfo) override;

            //! \copydoc IContextSimulator::getSimulatorStatus()
            virtual int getSimulatorStatus() const override;

            //! \copydoc IContextSimulator::getSimulatorSetup()
            virtual BlackMisc::Simulation::CSimulatorSetup getSimulatorSetup() const override;

            //! \copydoc IContextSimulator::getAirportsInRange
            virtual BlackMisc::Aviation::CAirportList getAirportsInRange() const override;

            //! \copydoc IContextSimulator::getInstalledModels
            virtual BlackMisc::Simulation::CAircraftModelList getInstalledModels() const override;

            //! \copydoc IContextSimulator::getInstalledModelsCount
            virtual int getInstalledModelsCount() const override;

            //! \copydoc IContextSimulator::getInstalledModelsStartingWith
            virtual BlackMisc::Simulation::CAircraftModelList getInstalledModelsStartingWith(const QString modelString) const override;

            //! \copydoc IContextSimulator::reloadInstalledModels
            virtual void reloadInstalledModels() override;

            //! \copydoc IContextSimulator::setTimeSynchronization
            virtual bool setTimeSynchronization(bool enable, const BlackMisc::PhysicalQuantities::CTime &offset) override;

            //! \copydoc IContextSimulator::isTimeSynchronized
            virtual bool isTimeSynchronized() const override;

            //! \copydoc IContextSimulator::getMaxRenderedAircraft
            virtual int getMaxRenderedAircraft() const override;

            //! \copydoc IContextSimulator::setMaxRenderedAircraft
            virtual void setMaxRenderedAircraft(int number) override;

            //! \copydoc IContextSimulator::getMaxRenderedDistance
            virtual BlackMisc::PhysicalQuantities::CLength getMaxRenderedDistance() const override;

            //! \copydoc IContextSimulator::setMaxRenderedDistance
            virtual void setMaxRenderedDistance(const BlackMisc::PhysicalQuantities::CLength &distance) override;

            //! \copydoc IContextSimulator::getRenderRestrictionText
            virtual QString getRenderRestrictionText() const override;

            //! \copydoc IContextSimulator::getRenderedDistanceBoundary
            virtual BlackMisc::PhysicalQuantities::CLength getRenderedDistanceBoundary() const override;

            //! \copydoc IContextSimulator::setMaxRenderedDistance
            virtual void deleteAllRenderingRestrictions() override;

            //! \copydoc IContextSimulator::isRenderingRestricted
            virtual bool isRenderingRestricted() const override;

            //! \copydoc IContextSimulator::isRenderingEnabled
            virtual bool isRenderingEnabled() const override;

            //! \copydoc IContextSimulator::getTimeSynchronizationOffset
            virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const override;

            //! \copydoc IContextSimulator::iconForModel
            virtual BlackMisc::CPixmap iconForModel(const QString &modelString) const override;

            //! \copydoc ISimulator::enableDebugMessages
            virtual void enableDebugMessages(bool driver, bool interpolator) override;

            //! \copydoc IContextSimulator::highlightAircraft
            virtual void highlightAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const BlackMisc::PhysicalQuantities::CTime &displayTime) override;

            //! \copydoc IContextSimulator::requestWeatherGrid
            virtual void requestWeatherGrid(const BlackMisc::Weather::CWeatherGrid &weatherGrid, const BlackMisc::CIdentifier &identifier) override;

        protected:
            //! Constructor
            CContextSimulator(CCoreFacadeConfig::ContextMode, CCoreFacade *runtime);

            //! Register myself in DBus
            CContextSimulator *registerWithDBus(BlackMisc::CDBusServer *server);

        private slots:
            //! Remote aircraft added
            void ps_addRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft);

            //! Remove remote aircraft
            void ps_removedRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign);

            //! Handle new connection status of simulator
            void ps_onSimulatorStatusChanged(int status);

            //! Text message received
            void ps_textMessagesReceived(const BlackMisc::Network::CTextMessageList &textMessages);

            //! Listener reports the simulator has started
            void ps_simulatorStarted(const BlackMisc::Simulation::CSimulatorPluginInfo &info);

            //! Simulator has changed cockpit
            void ps_cockpitChangedFromSimulator(const BlackMisc::Simulation::CSimulatedAircraft &ownAircraft);

            //! Changed remote aircraft model
            void ps_changedRemoteAircraftModel(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator);

            //! Enable / disable aircraft
            void ps_changedRemoteAircraftEnabled(const BlackMisc::Simulation::CSimulatedAircraft &aircraft);

            //! Update simulator cockpit from context, because someone else has changed cockpit (e.g. GUI, 3rd party)
            //! \remarks set by runtime, only to be used locally (not via DBus)
            void ps_updateSimulatorCockpitFromContext(const BlackMisc::Simulation::CSimulatedAircraft &ownAircraft, const BlackMisc::CIdentifier &originator);

            //! Reads list of enabled simulators, starts listeners
            void restoreSimulatorPlugins();

        private:
            //! Load plugin, if required start listeners before
            bool loadSimulatorPlugin(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorPluginInfo, bool withListeners);

            //! Unload plugin, if desired restart listeners
            void unloadSimulatorPlugin();

            //! Listen for single simulator
            void listenForSimulator(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorInfo);

            //! Listen for all simulators
            void listenForAllSimulators();

            //! Call stop() on all loaded listeners
            void stopSimulatorListeners();

            QPair<BlackMisc::Simulation::CSimulatorPluginInfo, ISimulator *> m_simulatorPlugin; //!< Currently loaded simulator plugin
            CPluginManagerSimulator *m_plugins = nullptr;
            BlackMisc::CRegularThread m_listenersThread;
            BlackCore::CWeatherManager m_weatherManager { this };
            BlackMisc::CSetting<BlackCore::Application::TEnabledSimulators> m_enabledSimulators { this, &CContextSimulator::restoreSimulatorPlugins };
            bool m_initallyAddAircrafts = false;
        };
    } // namespace
} // namespace

#endif // guard
