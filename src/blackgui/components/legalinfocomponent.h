/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_LEGALINFOCOMPONENT_H
#define BLACKGUI_COMPONENTS_LEGALINFOCOMPONENT_H

#include "blackgui/overlaymessagesframe.h"
#include <QFrame>
#include <QScopedPointer>
#include <QWizardPage>

namespace Ui { class CLegalInfoComponent; }
namespace BlackGui
{
    namespace Components
    {
        /**
         * swift legal information
         */
        class CLegalInfoComponent : public COverlayMessagesFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CLegalInfoComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CLegalInfoComponent();

            //! Agreed with?
            bool isAgreedTo() const;

            //! Validate that the agreement is accepted
            bool validateAgreement();

        private:
            QScopedPointer<Ui::CLegalInfoComponent> ui;
        };

        /**
         * Wizard page for CLegalInfoComponent
         */
        class CLegalInfoWizardPage : public QWizardPage
        {
        public:
            //! Constructors
            using QWizardPage::QWizardPage;

            //! Set config
            void setConfigComponent(CLegalInfoComponent *config) { m_legalInfo = config; }

            //! \copydoc QWizardPage::validatePage
            virtual bool validatePage() override;

        private:
            CLegalInfoComponent *m_legalInfo = nullptr;
        };
    } // ns
} // ns

#endif // guard
