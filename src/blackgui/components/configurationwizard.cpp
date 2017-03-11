/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "configurationwizard.h"
#include "ui_configurationwizard.h"

namespace BlackGui
{
    namespace Components
    {
        CConfigurationWizard::CConfigurationWizard(QWidget *parent) :
            QWizard(parent),
            ui(new Ui::CConfigurationWizard)
        {
            ui->setupUi(this);
            ui->wp_CopyCaches->setConfigComponent(ui->comp_CopyCaches);
            ui->wp_CopySettings->setConfigComponent(ui->comp_CopySettings);
            ui->wp_Simulator->setConfigComponent(ui->comp_Simulator);
            ui->wp_DataLoad->setConfigComponent(ui->comp_DataLoad);
            this->setButtonText(CustomButton1, "skip");

            // no other versions, skip copy pages
            if (!ui->comp_CopySettings->hasOtherVersionData())
            {
                this->setStartId(ConfigSimulator);
            }

            const QList<int> ids = this->pageIds();
            auto mm = std::minmax_element(ids.begin(), ids.end());
            m_maxId = *mm.second;
            m_minId = *mm.first;

            connect(this, &QWizard::currentIdChanged, this, &CConfigurationWizard::wizardCurrentIdChanged);
            connect(this, &QWizard::customButtonClicked, this, &CConfigurationWizard::clickedCustomButton);
        }

        CConfigurationWizard::~CConfigurationWizard()
        { }

        bool CConfigurationWizard::lastStepSkipped() const
        {
            return m_skipped;
        }

        void CConfigurationWizard::wizardCurrentIdChanged(int id)
        {
            const int previousId = m_previousId;
            const bool backward = id < previousId;
            const bool skipped = m_skipped;
            m_previousId = id; // update
            m_skipped = false; // reset
            Q_UNUSED(backward);
            Q_UNUSED(skipped);

            const QWizardPage *page = this->currentPage();
            if (page == ui->wp_CopyCaches)
            {
                ui->comp_CopyCaches->setCacheMode();
                ui->comp_CopyCaches->initCurrentDirectories(true);
            }
            else if (page == ui->wp_CopySettings)
            {
                ui->comp_CopySettings->setSettingsMode();
                ui->comp_CopySettings->initCurrentDirectories(true);
            }

            this->setOption(HaveCustomButton1, id != m_maxId);
        }

        void CConfigurationWizard::clickedCustomButton(int which)
        {
            if (which == static_cast<int>(CustomButton1))
            {
                this->m_skipped = true;
                this->next();
            }
            else
            {
                this->m_skipped = false;
            }
        }
    } // ns
} // ns
