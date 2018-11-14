/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "legalinfocomponent.h"
#include "ui_legalinfocomponent.h"

#include "blackgui/guiapplication.h"
#include "blackcore/data/globalsetup.h"
#include "blackmisc/network/url.h"
#include "blackmisc/crashsettings.h"
#include "blackmisc/statusmessage.h"
#include "blackconfig/buildconfig.h"

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::Settings;
using namespace BlackCore::Data;
using namespace BlackConfig;

namespace BlackGui
{
    namespace Components
    {
        CLegalInfoComponent::CLegalInfoComponent(QWidget *parent) :
            COverlayMessagesFrame(parent),
            ui(new Ui::CLegalInfoComponent)
        {
            ui->setupUi(this);
            this->setChecklistInfo();

            const CCrashSettings settings = m_crashDumpSettings.get();

            ui->cb_CrashDumps->setChecked(settings.isEnabled());
            ui->cb_Agree->setChecked(CBuildConfig::isLocalDeveloperDebugBuild());
            connect(ui->cb_CrashDumps, &QCheckBox::toggled, this, &CLegalInfoComponent::onAllowCrashDumps);
        }

        CLegalInfoComponent::~CLegalInfoComponent()
        { }

        bool CLegalInfoComponent::isAgreedTo() const
        {
            return ui->cb_Agree->isChecked();
        }

        bool CLegalInfoComponent::validateAgreement()
        {
            if (this->isAgreedTo()) { return true; }
            static const CStatusMessage m = CStatusMessage(this).validationError("You need to agree with the swift license");
            this->showOverlayMessage(m);
            return false;
        }

        void CLegalInfoComponent::onAllowCrashDumps(bool checked)
        {
            CCrashSettings settings = m_crashDumpSettings.get();
            settings.setEnabled(checked);
            m_crashDumpSettings.setAndSave(settings);
        }

        void CLegalInfoComponent::setChecklistInfo()
        {
            if (!sGui) { return; }
            const CGlobalSetup gs = sGui->getGlobalSetup();
            const CUrl url = gs.getHelpPageUrl("checklist");
            ui->lbl_Tip->setText(QStringLiteral("Please read the <a href=\"%1\">checklist before your 1st flight</a>").arg(url.getFullUrl()));
            ui->lbl_Tip->setTextFormat(Qt::RichText);
            ui->lbl_Tip->setTextInteractionFlags(Qt::TextBrowserInteraction);
            ui->lbl_Tip->setOpenExternalLinks(true);
        }

        bool CLegalInfoWizardPage::validatePage()
        {
            return m_legalInfo && m_legalInfo->validateAgreement();
        }
    } // ns
} // ns
