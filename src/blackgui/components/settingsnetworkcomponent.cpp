/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "settingsnetworkcomponent.h"
#include "ui_settingsnetworkcomponent.h"
#include "blackgui/guiapplication.h"
#include "blackcore/contextnetwork.h"

using namespace BlackCore;

namespace BlackGui
{
    namespace Components
    {
        CSettingsNetworkComponent::CSettingsNetworkComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CSettingsNetworkComponent)
        {
            ui->setupUi(this);
            connect(this->ui->cb_FastPositionUpdates, &QCheckBox::released, this, &CSettingsNetworkComponent::ps_guiValuesChanged);
            bool enabled =  sGui->getIContextNetwork()->isFastPositionSendingEnabled();
            this->ui->cb_FastPositionUpdates->setChecked(enabled);
        }

        CSettingsNetworkComponent::~CSettingsNetworkComponent() { }

        void CSettingsNetworkComponent::ps_guiValuesChanged()
        {
            QObject *sender = QObject::sender();
            if (sender == ui->cb_FastPositionUpdates)
            {
                bool enabled = this->ui->cb_FastPositionUpdates->isChecked();
                sGui->getIContextNetwork()->enableFastPositionSending(enabled);
            }
        }
    } // ns
} // ns
