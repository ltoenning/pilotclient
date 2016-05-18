/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_EDITORS_DISTRIBUTORFORM_H
#define BLACKGUI_EDITORS_DISTRIBUTORFORM_H

#include "blackgui/blackguiexport.h"
#include "blackgui/editors/form.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/variant.h"

#include <QObject>
#include <QScopedPointer>

class QWidget;
namespace Ui { class CDistributorForm; }

namespace BlackGui
{
    namespace Editors
    {
        /*!
         * Distributor form
         */
        class BLACKGUI_EXPORT CDistributorForm :
            public CForm
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDistributorForm(QWidget *parent = nullptr);

            //! Destructor
            ~CDistributorForm();

            //! Get value
            BlackMisc::Simulation::CDistributor getValue() const;

            //! Validate, empty list means OK
            BlackMisc::CStatusMessageList validate() const;

            //! Allow to drop
            void allowDrop(bool allowDrop);

            //! Is drop allowed?
            bool isDropAllowed() const;

            //! \copydoc CForm::setReadOnly
            virtual void setReadOnly(bool readOnly) override;

            //! \copydoc CForm::setSelectOnly
            virtual void setSelectOnly() override;

            //! Clear
            void clear();

        public slots:
            //! Set value
            bool setValue(const BlackMisc::Simulation::CDistributor &distributor = BlackMisc::Simulation::CDistributor());

        private slots:
            //! Variant has been dropped
            void ps_droppedCode(const BlackMisc::CVariant &variantDropped);

        private:
            QScopedPointer<Ui::CDistributorForm> ui;
            bool m_readOnly = false;
        };

    } // ns
} //ns

#endif // guard
