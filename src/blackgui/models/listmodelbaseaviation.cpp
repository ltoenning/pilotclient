/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "listmodelbase.cpp"

namespace BlackGui
{
    namespace Models
    {
        // see here for the reason of thess forward instantiations
        // https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
        template class CListModelBase<BlackMisc::Aviation::CLivery, BlackMisc::Aviation::CLiveryList, true>;
        template class CListModelBase<BlackMisc::Aviation::CAtcStation, BlackMisc::Aviation::CAtcStationList, true>;
        template class CListModelBase<BlackMisc::Aviation::CAirport, BlackMisc::Aviation::CAirportList, true>;
        template class CListModelBase<BlackMisc::Aviation::CAircraftIcaoCode, BlackMisc::Aviation::CAircraftIcaoCodeList, true>;
        template class CListModelBase<BlackMisc::Aviation::CAirlineIcaoCode, BlackMisc::Aviation::CAirlineIcaoCodeList, true>;
        template class CListModelBase<BlackMisc::Aviation::CAircraftParts, BlackMisc::Aviation::CAircraftPartsList, true>;
        template class CListModelBase<BlackMisc::Aviation::CAircraftSituation, BlackMisc::Aviation::CAircraftSituationList, true>;
        template class CListModelBase<BlackMisc::Aviation::CAircraftSituationChange, BlackMisc::Aviation::CAircraftSituationChangeList, true>;
    } // namespace
} // namespace
