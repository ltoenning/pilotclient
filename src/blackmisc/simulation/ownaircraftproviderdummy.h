/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_OWNAIRCRAFTPROVIDERDUMMY_H
#define BLACKMISC_SIMULATION_OWNAIRCRAFTPROVIDERDUMMY_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/simulation/ownaircraftprovider.h"

namespace BlackMisc
{
    namespace Simulation
    {
        //! For testing, thread safety not implemented in this class
        class BLACKMISC_EXPORT COwnAircraftProviderDummy :
            public QObject,
            public IOwnAircraftProvider
        {
            Q_OBJECT

        public:
            //! Constructor
            COwnAircraftProviderDummy() = default;

            //! Singleton
            static COwnAircraftProviderDummy *instance();

            //! \copydoc IOwnAircraftProvider::getOwnAircraft
            virtual CSimulatedAircraft getOwnAircraft() const override { return this->m_ownAircraft; }

            //! \copydoc IOwnAircraftProvider::getOwnAircraftPosition
            virtual BlackMisc::Geo::CCoordinateGeodetic getOwnAircraftPosition() const override;

            //! \copydoc IOwnAircraftProvider::getOwnAircraftParts
            virtual BlackMisc::Aviation::CAircraftParts getOwnAircraftParts() const override;

            //! \copydoc IOwnAircraftProvider::getOwnAircraftModel
            virtual BlackMisc::Simulation::CAircraftModel getOwnAircraftModel() const;

            //! \copydoc IOwnAircraftProvider::getDistanceToOwnAircraft
            virtual BlackMisc::PhysicalQuantities::CLength getDistanceToOwnAircraft(const BlackMisc::Geo::ICoordinateGeodetic &position) const override;

        public slots:
            //! \copydoc IOwnAircraftProvider::updateCockpit
            virtual bool updateCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder, const QString &originator) override;

            //! \copydoc IOwnAircraftProvider::updateComFrequency
            virtual bool updateActiveComFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency, int comUnit, const QString &originator) override;

            //! \copydoc IOwnAircraftProvider::updateSelcal
            virtual bool updateSelcal(const BlackMisc::Aviation::CSelcal &selcal, const QString &originator) override;

            //! \copydoc IOwnAircraftProvider::updateOwnCallsign
            virtual bool updateOwnCallsign(const BlackMisc::Aviation::CCallsign &callsign) override;

            //! \copydoc IOwnAircraftProvider::updateOwnModel
            virtual bool updateOwnModel(const BlackMisc::Simulation::CAircraftModel &model) override;

            //! \copydoc IOwnAircraftProvider::updateOwnIcaoData
            virtual bool updateOwnIcaoData(const BlackMisc::Aviation::CAircraftIcao &icaoData) override;

            //! \copydoc IOwnAircraftProvider::updateOwnSituation
            virtual bool updateOwnSituation(const BlackMisc::Aviation::CAircraftSituation &situation) override;

            //! \copydoc IOwnAircraftProvider::updateOwnParts
            virtual bool updateOwnParts(const BlackMisc::Aviation::CAircraftParts &parts) override;

        private:
            BlackMisc::Simulation::CSimulatedAircraft m_ownAircraft;
        };

    } // namespace
} // namespace

#endif // guard
