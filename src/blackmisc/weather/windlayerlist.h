// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_WEATHER_WINDLAYERLIST_H
#define BLACKMISC_WEATHER_WINDLAYERLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/sequence.h"
#include "blackmisc/weather/windlayer.h"

#include <QMetaType>
#include <initializer_list>
#include <tuple>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Weather, CWindLayer, CWindLayerList)

namespace BlackMisc
{
    namespace Aviation
    {
        class CAltitude;
    }

    namespace Weather
    {
        /*!
         * Value object encapsulating a set of wind layers
         */
        class BLACKMISC_EXPORT CWindLayerList :
            public CSequence<CWindLayer>,
            public BlackMisc::Mixin::MetaType<CWindLayerList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CWindLayerList)
            using CSequence::CSequence;

            //! Default constructor.
            CWindLayerList() = default;

            //! Construct from a base class object.
            CWindLayerList(const CSequence<CWindLayer> &other);

            //! Contains cloud layer with level?
            bool containsLevel(const BlackMisc::Aviation::CAltitude &level) const;

            //! Find cloud layer by level
            CWindLayer findByLevel(const BlackMisc::Aviation::CAltitude &level) const;
        };

    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Weather::CWindLayerList)

#endif // guard
