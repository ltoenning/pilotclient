/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRLINEICAOCODELIST_H
#define BLACKMISC_AVIATION_AIRLINEICAOCODELIST_H

#include "airlineicaocode.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/datastoreobjectlist.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include <initializer_list>

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object encapsulating a list of ICAO codes.
        class BLACKMISC_EXPORT CAirlineIcaoCodeList :
            public CSequence<CAirlineIcaoCode>,
            public BlackMisc::IDatastoreObjectList<CAirlineIcaoCode, CAirlineIcaoCodeList, int>,

            public BlackMisc::Mixin::MetaType<CAirlineIcaoCodeList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CAirlineIcaoCodeList)

            //! Default constructor.
            CAirlineIcaoCodeList() = default;

            //! Construct from a base class object.
            CAirlineIcaoCodeList(const CSequence<CAirlineIcaoCode> &other);

            //! Find by designator
            //! Not unique because of virtual airlines
            CAirlineIcaoCodeList findByDesignator(const QString &designator) const;

            //! Find by v-designator, this should be unique
            CAirlineIcaoCodeList findByVDesignator(const QString &designator) const;

            //! Find by IATA code
            //! Not unique because of virtual airlines
            CAirlineIcaoCodeList findByIataCode(const QString &iata) const;

            //! Find by designator or IATA code
            CAirlineIcaoCodeList findByDesignatorOrIataCode(const QString &designatorOrIata) const;

            //! Find by v-designator or IATA code
            CAirlineIcaoCodeList findByVDesignatorOrIataCode(const QString &designatorOrIata) const;

            //! Find by country code
            CAirlineIcaoCodeList findByCountryIsoCode(const QString &isoCode) const;

            //! Find by military flag
            CAirlineIcaoCodeList findByMilitary(bool military) const;

            //! Best selection by given pattern
            CAirlineIcaoCode smartAirlineIcaoSelector(const CAirlineIcaoCode &icaoPattern) const;

            //! String list for completion by ICAO designator
            QStringList toIcaoDesignatorCompleterStrings() const;

            //! String list for completion by name
            QStringList toNameCompleterStrings() const;

            //! From our DB JSON
            static CAirlineIcaoCodeList fromDatabaseJson(const QJsonArray &array, bool ignoreIncomplete = true);
        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAirlineIcaoCodeList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAirlineIcaoCode>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Aviation::CAirlineIcaoCode>)

#endif //guard
