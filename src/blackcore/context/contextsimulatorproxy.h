/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXTSIMULATOR_PROXY_H
#define BLACKCORE_CONTEXTSIMULATOR_PROXY_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/context/contextsimulator.h"
#include "blackcore/corefacadeconfig.h"
#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/pixmap.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/simulatorplugininfolist.h"
#include "blackmisc/simulation/simulatorinternals.h"

#include <QObject>
#include <QString>

class QDBusConnection;
namespace BlackMisc
{
    class CGenericDBusInterface;
    namespace Simulation { class CSimulatedAircraft; }
}

namespace BlackCore
{
    class CCoreFacade;
    namespace Context
    {
        //! DBus proxy for Simulator Context
        //! \ingroup dbus
        class BLACKCORE_EXPORT CContextSimulatorProxy : public IContextSimulator
        {
            Q_OBJECT
            friend class IContextSimulator;

        public:
            //! Destructor
            virtual ~CContextSimulatorProxy() override {}

            //! Unit test relay signals
            //! \private
            static void unitTestRelaySignals();

        public slots:
            //! \name Interface overrides
            //! @{
            virtual BlackMisc::Simulation::CSimulatorPluginInfo getSimulatorPluginInfo() const override;
            virtual BlackMisc::Simulation::CSimulatorPluginInfoList getAvailableSimulatorPlugins() const override;
            virtual bool startSimulatorPlugin(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorInfo) override;
            virtual void stopSimulatorPlugin(const BlackMisc::Simulation::CSimulatorPluginInfo &simulatorInfo) override;
            virtual int checkListeners() override;
            virtual int getSimulatorStatus() const override;
            virtual BlackMisc::Aviation::CAirportList getAirportsInRange(bool recalculatePosition) const override;
            virtual BlackMisc::Simulation::CAircraftModelList getModelSet() const override;
            virtual BlackMisc::Simulation::CSimulatorInfo simulatorsWithInitializedModelSet() const override;
            virtual BlackMisc::CStatusMessageList verifyPrerequisites() const override;
            virtual BlackMisc::Simulation::CSimulatorInfo getModelSetLoaderSimulator() const override;
            virtual void setModelSetLoaderSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator) override;
            virtual QStringList getModelSetStrings() const override;
            virtual QStringList getModelSetCompleterStrings(bool sorted) const override;
            virtual bool isKnownModel(const QString &modelString) const override;
            virtual BlackMisc::Simulation::CAircraftModelList getModelSetModelsStartingWith(const QString &modelString) const override;
            virtual int getModelSetCount() const override;
            virtual BlackMisc::Simulation::CSimulatorInternals getSimulatorInternals() const override;
            virtual bool setTimeSynchronization(bool enable, const BlackMisc::PhysicalQuantities::CTime &offset) override;
            virtual bool isTimeSynchronized() const override;
            virtual BlackMisc::Simulation::CInterpolationAndRenderingSetupGlobal getInterpolationAndRenderingSetupGlobal() const override;
            virtual void setInterpolationAndRenderingSetupGlobal(const BlackMisc::Simulation::CInterpolationAndRenderingSetupGlobal &setup) override;
            virtual BlackMisc::Simulation::CInterpolationSetupList getInterpolationAndRenderingSetupsPerCallsign() const override;
            virtual BlackMisc::Simulation::CInterpolationAndRenderingSetupPerCallsign getInterpolationAndRenderingSetupPerCallsignOrDefault(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual bool setInterpolationAndRenderingSetupsPerCallsign(const BlackMisc::Simulation::CInterpolationSetupList &setups, bool ignoreSameAsGlobal) override;
            virtual BlackMisc::CStatusMessageList getInterpolationMessages(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual BlackMisc::PhysicalQuantities::CTime getTimeSynchronizationOffset() const override;
            virtual BlackMisc::CPixmap iconForModel(const QString &modelString) const override;
            virtual void highlightAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const BlackMisc::PhysicalQuantities::CTime &displayTime) override;
            virtual bool followAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;
            virtual void recalculateAllAircraft() override;
            virtual bool resetToModelMatchingAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;
            virtual void setWeatherActivated(bool activated) override;
            virtual void requestWeatherGrid(const BlackMisc::Weather::CWeatherGrid &weatherGrid, const BlackMisc::CIdentifier &identifier) override;
            virtual BlackMisc::CStatusMessageList getMatchingMessages(const BlackMisc::Aviation::CCallsign &callsign) const override;
            virtual bool isMatchingMessagesEnabled() const override;
            virtual void enableMatchingMessages(bool enabled) override;
            virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) override;
            virtual int doMatchingsAgain() override;
            virtual bool doMatchingAgain(const BlackMisc::Aviation::CCallsign &callsign) override;
            virtual BlackMisc::Simulation::CMatchingStatistics getCurrentMatchingStatistics(bool missingOnly) const override;
            virtual void setMatchingSetup(const BlackMisc::Simulation::CAircraftMatcherSetup &setup) override;
            virtual BlackMisc::Simulation::CAircraftMatcherSetup getMatchingSetup() const override;
            virtual BlackMisc::CStatusMessageList copyFsxTerrainProbe(const BlackMisc::Simulation::CSimulatorInfo &simulator) override;
            //! @}

        private:
            BlackMisc::CGenericDBusInterface *m_dBusInterface = nullptr;

            //! Relay connection signals to local signals
            void relaySignals(const QString &serviceName, QDBusConnection &connection);

        protected:
            //! Constructor
            CContextSimulatorProxy(CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime) : IContextSimulator(mode, runtime) {}

            //! DBus version constructor
            CContextSimulatorProxy(const QString &serviceName, QDBusConnection &connection, CCoreFacadeConfig::ContextMode mode, CCoreFacade *runtime);
        };
    } // ns
} // ns
#endif // guard
