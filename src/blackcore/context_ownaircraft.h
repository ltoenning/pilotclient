/* Copyright (C) 2013 VATSIM Community / contributors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCORE_CONTEXTOWNAIRCRAFT_H
#define BLACKCORE_CONTEXTOWNAIRCRAFT_H

#include "blackcore/context.h"
#include "blackmisc/avallclasses.h"

#define BLACKCORE_CONTEXTOWNAIRCRAFT_INTERFACENAME "net.vatsim.PilotClient.BlackCore.ContextOwnAircraft"
#define BLACKCORE_CONTEXTOWNAIRCRAFT_OBJECTPATH "/OwnAircraft"

namespace BlackCore
{

    //! \brief Own context proxy
    class IContextOwnAircraft : public CContext
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTOWNAIRCRAFT_INTERFACENAME)

    public:
        //! DBus interface name
        static const QString &InterfaceName()
        {
            static QString s(BLACKCORE_CONTEXTOWNAIRCRAFT_INTERFACENAME);
            return s;
        }

        //! DBus object path
        static const QString &ObjectPath()
        {
            static QString s(BLACKCORE_CONTEXTOWNAIRCRAFT_OBJECTPATH);
            return s;
        }

        //! Destructor
        virtual ~IContextOwnAircraft() {}

    signals:
        //! Aircraft situation update
        void changedAircraftSituation(const BlackMisc::Aviation::CAircraft &aircraft, const QString &originator);

        //! Aircraft cockpit update
        void changedAircraftCockpit(const BlackMisc::Aviation::CAircraft &aircraft, const QString &originator);

        //! Aircraft position update
        void changedAircraftPosition(const BlackMisc::Aviation::CAircraft &aircraft, const QString &originator);

    public slots:

        //! Get own aircraft
        virtual BlackMisc::Aviation::CAircraft getOwnAircraft() const = 0;

        //! Own aircraft
        virtual void updateOwnAircraft(const BlackMisc::Aviation::CAircraft &aircraft, const QString &originator) = 0;

        //! Own position, be aware height is terrain height
        virtual void updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude, const QString &originator) = 0;

        //! Complete situation update
        virtual void updateOwnSituation(const BlackMisc::Aviation::CAircraftSituation &situation, const QString &originator) = 0;

        //! Update own cockpit
        virtual void updateOwnCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder, const QString &originator) = 0;

    protected:
        //! Constructor
        IContextOwnAircraft(CRuntimeConfig::ContextMode mode, CRuntime *runtime) : CContext(mode, runtime) {}
    };
}

#endif // guard
