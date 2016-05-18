/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/webdataservices.h"
#include "blackgui/components/dbairlinenameselectorcomponent.h"
#include "blackgui/guiapplication.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/datastoreutility.h"
#include "ui_dbairlinenameselectorcomponent.h"

#include <QCompleter>
#include <QLineEdit>
#include <QString>
#include <Qt>

using namespace BlackCore;
using namespace BlackGui;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;

namespace BlackGui
{
    namespace Components
    {
        CDbAirlineNameSelectorComponent::CDbAirlineNameSelectorComponent(QWidget *parent) :
            CDbAirlineIcaoSelectorBase(parent),
            ui(new Ui::CDbAirlineNameSelectorComponent)
        {
            ui->setupUi(this);
            connect(this->ui->le_AirlineName, &QLineEdit::returnPressed, this, &CDbAirlineNameSelectorComponent::ps_dataChanged);
        }

        CDbAirlineNameSelectorComponent::~CDbAirlineNameSelectorComponent()
        {
            // no inline destructor, read QScopedPointer Forward Declared Pointers
        }

        bool CDbAirlineNameSelectorComponent::setAirlineIcao(const CAirlineIcaoCode &icao)
        {
            if (!CDbAirlineIcaoSelectorBase::setAirlineIcao(icao)) { return false; }
            QString name(icao.getName());
            this->ui->le_AirlineName->setText(name);
            return true;
        }

        void CDbAirlineNameSelectorComponent::clear()
        {
            this->ui->le_AirlineName->clear();
        }

        void CDbAirlineNameSelectorComponent::setReadOnly(bool readOnly)
        {
            this->ui->le_AirlineName->setReadOnly(readOnly);
        }

        QCompleter *CDbAirlineNameSelectorComponent::createCompleter()
        {
            QCompleter *c = new QCompleter(sGui->getWebDataServices()->getAirlineIcaoCodes().toNameCompleterStrings(), this);
            c->setCaseSensitivity(Qt::CaseInsensitive);
            c->setCompletionMode(QCompleter::PopupCompletion);
            c->setMaxVisibleItems(10);
            this->ui->le_AirlineName->setCompleter(c);
            return c;
        }

        void CDbAirlineNameSelectorComponent::ps_dataChanged()
        {
            if (!sGui) { return; }
            QString s(this->ui->le_AirlineName->text());
            if (s.isEmpty()) { return; }
            int dbKey = CDatastoreUtility::extractIntegerKey(s);
            if (dbKey >= 0)
            {
                CAirlineIcaoCode icao(sGui->getWebDataServices()->getAirlineIcaoCodeForDbKey(dbKey));
                this->setAirlineIcao(icao);
            }
            else
            {
                // second choice, first object found by designator
                // for name

                // CAirlineIcaoCode icao(getAirlineIcao));
                // this->setAirlineIcao(icao);
            }
        }
    } // ns
} // ns
