// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_TRANSPONDERMODECOMPONENT_H
#define BLACKGUI_COMPONENTS_TRANSPONDERMODECOMPONENT_H

#include "blackmisc/aviation/transponder.h"
#include "blackmisc/identifiable.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CTransponderModeComponent;
}
namespace BlackMisc
{
    class CIdentifier;
    namespace Simulation
    {
        class CSimulatedAircraft;
    }
}
namespace BlackGui::Components
{
    /*!
     * Small component to display XPDR mode
     */
    class CTransponderModeComponent : public QFrame, BlackMisc::CIdentifiable
    {
        Q_OBJECT

    public:
        //! Ctor
        explicit CTransponderModeComponent(QWidget *parent = nullptr);

        //! Dtor
        virtual ~CTransponderModeComponent() override;

    signals:
        //! value has been changed
        void changed();

    private:
        QScopedPointer<Ui::CTransponderModeComponent> ui;
        BlackMisc::Aviation::CTransponder m_transponder;

        //! Init
        void init();

        //! Clicked
        void onClicked();

        //! Changed cockpit data
        void onChangedAircraftCockpit(const BlackMisc::Simulation::CSimulatedAircraft &aircraft, const BlackMisc::CIdentifier &originator);
    };
} // ns

#endif // guard
