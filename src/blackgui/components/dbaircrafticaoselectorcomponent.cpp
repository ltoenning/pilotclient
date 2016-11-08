/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "ui_dbaircrafticaoselectorcomponent.h"
#include "blackcore/application.h"
#include "blackcore/webdataservices.h"
#include "blackgui/components/dbaircrafticaoselectorcomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/uppercasevalidator.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/db/datastoreutility.h"
#include "blackmisc/compare.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/variant.h"

#include <QCompleter>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMetaType>
#include <QPalette>
#include <Qt>

using namespace BlackGui;
using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Db;
using namespace BlackMisc::Network;

namespace BlackGui
{
    namespace Components
    {
        CDbAircraftIcaoSelectorComponent::CDbAircraftIcaoSelectorComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CDbAircraftIcaoSelectorComponent)
        {
            ui->setupUi(this);
            this->setFocusProxy(ui->le_Aircraft);
            this->setAcceptDrops(true);
            this->setAcceptedMetaTypeIds({qMetaTypeId<CAircraftIcaoCode>(), qMetaTypeId<CAircraftIcaoCodeList>()});
            ui->le_Aircraft->setValidator(new CUpperCaseValidator(this));
            connect(ui->le_Aircraft, &QLineEdit::editingFinished, this, &CDbAircraftIcaoSelectorComponent::ps_dataChanged);
            connect(sApp->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbAircraftIcaoSelectorComponent::ps_codesRead);
            this->ps_codesRead(CEntityFlags::AircraftIcaoEntity, CEntityFlags::ReadFinished, sApp->getWebDataServices()->getAircraftIcaoCodesCount());
        }

        CDbAircraftIcaoSelectorComponent::~CDbAircraftIcaoSelectorComponent()
        { }

        void CDbAircraftIcaoSelectorComponent::setAircraftIcao(const CAircraftIcaoCode &icao)
        {
            CAircraftIcaoCode setIcao(icao);
            if (!icao.isLoadedFromDb())
            {
                // resolve against DB
                setIcao = sGui->getWebDataServices()->smartAircraftIcaoSelector(icao);
            }

            const QString icaoStr(this->m_display == DisplayIcaoAndId ? setIcao.getDesignatorDbKey() : setIcao.getCombinedIcaoStringWithKey());
            ui->le_Aircraft->setText(icaoStr);
            ui->lbl_Description->setText(setIcao.getManufacturer());
            if (setIcao != m_currentIcao)
            {
                m_currentIcao = setIcao;
                emit changedAircraftIcao(setIcao);
            }
        }

        void CDbAircraftIcaoSelectorComponent::setAircraftIcao(int key)
        {
            const CAircraftIcaoCode icao(sGui->getWebDataServices()->getAircraftIcaoCodeForDbKey(key));
            ui->lbl_Description->setText("");
            if (icao.hasCompleteData())
            {
                this->setAircraftIcao(icao);
            }
        }

        CAircraftIcaoCode CDbAircraftIcaoSelectorComponent::getAircraftIcao() const
        {
            QString text(ui->le_Aircraft->text().trimmed().toUpper());
            int key = CDatastoreUtility::extractIntegerKey(text);
            if (key < 0)
            {
                if (this->m_currentIcao.getDesignator() == text) { return this->m_currentIcao; }
                return CAircraftIcaoCode(text);
            }
            CAircraftIcaoCode icao(sGui->getWebDataServices()->getAircraftIcaoCodeForDbKey(key));
            return icao;
        }

        void CDbAircraftIcaoSelectorComponent::setReadOnly(bool readOnly)
        {
            ui->le_Aircraft->setReadOnly(readOnly);
        }

        QString CDbAircraftIcaoSelectorComponent::getRawDesignator() const
        {
            return stripDesignatorFromCompleterString(ui->le_Aircraft->text());
        }

        void CDbAircraftIcaoSelectorComponent::displayWithIcaoDescription(bool description)
        {
            ui->lbl_Description->setVisible(description);
        }

        bool CDbAircraftIcaoSelectorComponent::isSet() const
        {
            const CAircraftIcaoCode icao(this->getAircraftIcao());
            return icao.isLoadedFromDb() || icao.hasCompleteData();
        }

        void CDbAircraftIcaoSelectorComponent::clear()
        {
            ui->le_Aircraft->clear();
        }

        void CDbAircraftIcaoSelectorComponent::dragEnterEvent(QDragEnterEvent *event)
        {
            if (!event || !acceptDrop(event->mimeData())) { return; }
            setBackgroundRole(QPalette::Highlight);
            event->acceptProposedAction();
        }

        void CDbAircraftIcaoSelectorComponent::dragMoveEvent(QDragMoveEvent *event)
        {
            if (!event || !acceptDrop(event->mimeData())) { return; }
            event->acceptProposedAction();
        }

        void CDbAircraftIcaoSelectorComponent::dragLeaveEvent(QDragLeaveEvent *event)
        {
            if (!event) { return; }
            event->accept();
        }

        void CDbAircraftIcaoSelectorComponent::dropEvent(QDropEvent *event)
        {
            if (!event || !acceptDrop(event->mimeData())) { return; }
            CVariant valueVariant(toCVariant(event->mimeData()));
            if (valueVariant.isValid())
            {
                if (valueVariant.canConvert<CAircraftIcaoCode>())
                {
                    CAircraftIcaoCode icao(valueVariant.value<CAircraftIcaoCode>());
                    if (!icao.hasValidDbKey()) { return; }
                    this->setAircraftIcao(icao);
                }
                else if (valueVariant.canConvert<CAircraftIcaoCodeList>())
                {
                    CAircraftIcaoCodeList icaos(valueVariant.value<CAircraftIcaoCodeList>());
                    if (icaos.isEmpty()) { return; }
                    this->setAircraftIcao(icaos.front());
                }
            }
        }

        const QStringList &CDbAircraftIcaoSelectorComponent::completerStrings()
        {
            // done for performance reasons
            // init only once, future instance can share thte list
            // only to be called when data are read!
            static const QStringList cs(sGui->getWebDataServices()->getAircraftIcaoCodes().toCompleterStrings(true, true, true));
            return cs;
        }

        void CDbAircraftIcaoSelectorComponent::ps_codesRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count)
        {
            if (!sGui || !sGui->hasWebDataServices()) { return; }
            if (entity.testFlag(CEntityFlags::AircraftIcaoEntity) && readState == CEntityFlags::ReadFinished)
            {
                if (count > 0)
                {
                    QCompleter *c = new QCompleter(completerStrings(), this);
                    c->setCaseSensitivity(Qt::CaseInsensitive);
                    c->setCompletionMode(QCompleter::PopupCompletion);
                    c->setMaxVisibleItems(10);
                    c->popup()->setMinimumWidth(175);
                    this->connect(c, static_cast<void (QCompleter::*)(const QString &)>(&QCompleter::activated), this, &CDbAircraftIcaoSelectorComponent::ps_completerActivated);

                    ui->le_Aircraft->setCompleter(c);
                    m_completerIcaoDescription.reset(c); // deletes any old completer
                    this->setReadOnly(false);
                }
                else
                {
                    this->m_completerIcaoDescription.reset(nullptr);
                    this->setReadOnly(true);
                }
            }
        }

        void CDbAircraftIcaoSelectorComponent::ps_dataChanged()
        {
            if (!sGui || !sGui->hasWebDataServices()) { return; }
            const int key = CDatastoreUtility::extractIntegerKey(ui->le_Aircraft->text());
            CAircraftIcaoCode icao;
            if (key >= 0)
            {
                // with valid key
                icao = sGui->getWebDataServices()->getAircraftIcaoCodeForDbKey(key);
            }
            else
            {
                const QString designator(this->getRawDesignator());
                icao = sGui->getWebDataServices()->smartAircraftIcaoSelector(CAircraftIcaoCode(designator));
            }
            this->setAircraftIcao(icao);
        }

        void CDbAircraftIcaoSelectorComponent::ps_completerActivated(const QString &icaoString)
        {
            const int dbKey = CDatastoreUtility::extractIntegerKey(icaoString);
            if (dbKey < 0) { return; }
            this->setAircraftIcao(dbKey);
        }
    }// class
} // ns
