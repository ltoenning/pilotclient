// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_PQ_PRESSURE_H
#define BLACKMISC_PQ_PRESSURE_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/pq/physicalquantity.h"

namespace BlackMisc::PhysicalQuantities
{
    /*!
     * Physical unit distance
     */
    class BLACKMISC_EXPORT CPressure : public CPhysicalQuantity<CPressureUnit, CPressure>
    {
    public:
        //! Default constructor
        CPressure() : CPhysicalQuantity(0, CPressureUnit::defaultUnit()) {}

        //! Init by double value
        CPressure(double value, const CPressureUnit &unit) : CPhysicalQuantity(value, unit) {}

        //! \copydoc CPhysicalQuantity(const QString &unitString)
        CPressure(const QString &unitString) : CPhysicalQuantity(unitString) {}
    };
} // ns

Q_DECLARE_METATYPE(BlackMisc::PhysicalQuantities::CPressure)

#endif // guard
