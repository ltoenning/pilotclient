/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/components/datasettingscomponent.h"
#include "ui_datasettingscomponent.h"

using namespace BlackGui;
using namespace BlackCore::Db;

namespace BlackGui::Components
{
    CDataSettingsComponent::CDataSettingsComponent(QWidget *parent) :
        COverlayMessagesFrame(parent),
        ui(new Ui::CDataSettingsComponent)
    {
        ui->setupUi(this);
        ui->comp_GuiSettings->hideOpacity(true);
    }

    CDataSettingsComponent::~CDataSettingsComponent()
    { }

    void CDataSettingsComponent::setBackgroundUpdater(const CBackgroundDataUpdater *updater)
    {
        ui->comp_ModelSettings->setBackgroundUpdater(updater);
    }
} // ns
