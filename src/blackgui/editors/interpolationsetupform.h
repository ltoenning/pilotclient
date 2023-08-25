// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_EDITORS_INTERPOLATIONSETUPFORM_H
#define BLACKGUI_EDITORS_INTERPOLATIONSETUPFORM_H

#include "blackgui/editors/form.h"
#include "blackmisc/simulation/interpolationrenderingsetup.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/statusmessagelist.h"

#include <QScopedPointer>
#include <QCheckBox>

namespace Ui
{
    class CInterpolationSetupForm;
}
namespace BlackGui::Editors
{
    //! Setup of interpolation and rendering
    class CInterpolationSetupForm : public CForm
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CInterpolationSetupForm(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CInterpolationSetupForm() override;

        //! Set value
        void setValue(const BlackMisc::Simulation::CInterpolationAndRenderingSetupBase &setup);

        //! Get value
        BlackMisc::Simulation::CInterpolationAndRenderingSetupPerCallsign getValue() const;

        //! \name Form class implementations
        //! @{
        virtual void setReadOnly(bool readonly) override;
        virtual BlackMisc::CStatusMessageList validate(bool nested = false) const override;
        //! @}

    signals:
        //! Value changed
        void valueChanged();

    private:
        QScopedPointer<Ui::CInterpolationSetupForm> ui;
        QList<QCheckBox *> m_allCheckBoxes;

        //! Checkbox has been changed
        void onCheckboxChanged(int state);

        //! Interpolator mode
        void onInterpolatorModeChanged(bool checked);

        //! Pitch entry
        void onPitchChanged();

        //! Pitch on ground
        BlackMisc::PhysicalQuantities::CAngle getPitchOnGround() const;

        //! Display pitch on ground
        void displayPitchOnGround(const BlackMisc::PhysicalQuantities::CAngle &pitchOnGround);

        //! Mode from UI
        BlackMisc::Simulation::CInterpolationAndRenderingSetupBase::InterpolatorMode getInterpolatorMode() const;

        //! Set mode
        void setInterpolatorMode(BlackMisc::Simulation::CInterpolationAndRenderingSetupBase::InterpolatorMode mode);
    };
} // ns
#endif // guard
