/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_SIMULATORINFO_H
#define BLACKMISC_SIMULATION_SIMULATORINFO_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/valueobject.h"

namespace BlackMisc
{
    namespace Simulation
    {
        //! Simple hardcoded info about the corresponding simulator.
        //! * in an ideal world this class would not exist, all would depend on flexible plugins \sa CSimulatorPluginInfo
        //! * in a real world the info is needed in a couple of places to specify the simulator
        //! ** when data from the swift data store a read, the corresponding simulator is specified
        //! ** when model metadata are written to the swift data store the DB simulator info needs to be provided
        //! ** when models are indexed from disk it does not know the corresponding driver
        //! ** also there is no strict dependency of some functions to the driver. I might not have the XP plugin installed,
        //!    but need to handle XP data from the swift data store
        //! If someone manages to remove this hardocded simulator information and makes it entirely flexible
        //! based upon the plugin metadata feel free.
        class BLACKMISC_EXPORT CSimulatorInfo : public BlackMisc::CValueObject<CSimulatorInfo>
        {
        public:

            //! Simulator
            enum SimulatorFlags
            {
                None   = 0,
                FSX    = 1 << 0,
                FS9    = 1 << 1,
                XP     = 1 << 2
            };
            Q_DECLARE_FLAGS(Simulator, SimulatorFlags)

            //! Default constructor
            CSimulatorInfo();

            //! Constructor
            CSimulatorInfo(Simulator s);

            //! Constructor
            CSimulatorInfo(bool fsx, bool fs9, bool xp);

            //! Unspecified simulator
            bool isUnspecified() const;

            //! Simulator
            Simulator getSimulator() const { return static_cast<Simulator>(m_simulator); }

            //! Simulator
            void setSimulator(Simulator s) { m_simulator = static_cast<int>(s); }

            //! \copydoc CValueObject::convertToQString
            QString convertToQString(bool i18n = false) const;

            //! Bool flags to enum
            static Simulator boolToFlag(bool fsx, bool fs9, bool xp);

            //! Identifer, as provided by plugin
            static Simulator identifierToFlag(const QString &identifier);

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CSimulatorInfo)
            int m_simulator = static_cast<int>(None);
        };
    } // ns
} // ns

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Simulation::CSimulatorInfo, (
                                   attr(o.m_simulator)
                               ))
Q_DECLARE_METATYPE(BlackMisc::Simulation::CSimulatorInfo)
Q_DECLARE_OPERATORS_FOR_FLAGS(BlackMisc::Simulation::CSimulatorInfo::Simulator)

#endif // guard
