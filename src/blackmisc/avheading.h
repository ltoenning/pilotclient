/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVHEADING_H
#define BLACKMISC_AVHEADING_H

#include "blackmisc/pqangle.h"

namespace BlackMisc
{
    namespace Aviation { class CHeading; }

    //! \private
    template <> struct CValueObjectStdTuplePolicy<Aviation::CHeading> : public CValueObjectStdTuplePolicy<>
    {
        using Compare = Policy::Compare::MetaTuple;
        using Hash = Policy::Hash::MetaTuple;
        using DBus = Policy::DBus::MetaTuple;
        using Json = Policy::Json::MetaTuple;
    };

    namespace Aviation
    {
        /*!
         * \brief Heading as used in aviation, can be true or magnetic heading
         * \remarks Intentionally allowing +/- CAngle , and >= / <= CAngle.
         */
        class CHeading : public CValueObjectStdTuple<CHeading, PhysicalQuantities::CAngle>
        {
        public:
            /*!
             * Enum type to distinguish between true north and magnetic north
             */
            enum ReferenceNorth
            {
                Magnetic = 0,   //!< magnetic north
                True = 1        //!< true north
            };

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

        public:
            //! \brief Default constructor: 0 heading true
            CHeading() : CValueObjectStdTuple(0, BlackMisc::PhysicalQuantities::CAngleUnit::rad()), m_north(Magnetic) {}

            //! \brief Constructor
            CHeading(double value, ReferenceNorth north, const BlackMisc::PhysicalQuantities::CAngleUnit &unit) : CValueObjectStdTuple(value, unit), m_north(north) {}

            //! \brief Constructor by CAngle
            CHeading(CAngle heading, ReferenceNorth north) : CValueObjectStdTuple(heading), m_north(north) {}

            //! \brief Magnetic heading?
            bool isMagneticHeading() const { return Magnetic == this->m_north; }

            //! \brief True heading?
            bool isTrueHeading() const { return True == this->m_north; }

            //! \brief Get reference north (magnetic or true)
            ReferenceNorth getReferenceNorth() const { return m_north; }

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CHeading)
            ReferenceNorth m_north; //!< magnetic or true?
        };
    }
}

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CHeading, (o.m_north))
Q_DECLARE_METATYPE(BlackMisc::Aviation::CHeading)

#endif // guard
