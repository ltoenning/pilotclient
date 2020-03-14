/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/webdataservices.h"
#include "blackgui/components/dbautostashingcomponent.h"
#include "blackgui/components/dbmappingcomponent.h"
#include "blackgui/components/dbstashcomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/views/aircraftmodelview.h"
#include "blackgui/views/statusmessageview.h"
#include "blackgui/views/viewbase.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/logcategory.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/sequence.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/statusmessagelist.h"
#include "dbautostashingcomponent.h"
#include "ui_dbautostashingcomponent.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFlags>
#include <QIntValidator>
#include <QLineEdit>
#include <QProgressBar>
#include <QRadioButton>
#include <QStringBuilder>
#include <QString>
#include <QWidget>
#include <Qt>
#include <QtGlobal>

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackGui::Views;

namespace BlackGui
{
    namespace Components
    {
        const CLogCategoryList &CDbAutoStashingComponent::getLogCategories()
        {
            static const BlackMisc::CLogCategoryList cats { CLogCategory::mapping(), CLogCategory::guiComponent() };
            return cats;
        }

        CDbAutoStashingComponent::CDbAutoStashingComponent(QWidget *parent) :
            QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
            CDbMappingComponentAware(qobject_cast<CDbMappingComponent * >(parent)),
            ui(new Ui::CDbAutoStashingComponent)
        {
            ui->setupUi(this);
            ui->tvp_StatusMessages->setResizeMode(CAircraftModelView::ResizingAuto);
            ui->tvp_StatusMessages->menuAddItems(CAircraftModelView::MenuSave);
            ui->le_MaxModelsStashed->setValidator(new QIntValidator(10, CDbStashComponent::MaxModelPublished, this));
            Q_ASSERT_X(this->getMappingComponent(), Q_FUNC_INFO, "Expect mapping componet");

            connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CDbAutoStashingComponent::onEntitiesRead, Qt::QueuedConnection);
            connect(ui->tb_ResetDescription, &QToolButton::clicked, this, &CDbAutoStashingComponent::resetDescription);

            this->resetDescription();
        }

        CDbAutoStashingComponent::~CDbAutoStashingComponent()
        { }

        void CDbAutoStashingComponent::accept()
        {
            switch (m_state)
            {
            case Running: return;
            case Completed:
                {
                    if (!m_modelsToStash.isEmpty())
                    {
                        // this removes previously stashed models
                        this->getMappingComponent()->replaceStashedModelsUnvalidated(m_modelsToStash);
                        if (ui->cb_RemovedChecked->isChecked())
                        {
                            this->currentModelView()->removeModelsWithModelString(m_modelsToStash);
                        }
                        const CStatusMessage stashedMsg(this, CStatusMessage::SeverityInfo, QStringLiteral("Auto stashed %1 models").arg(m_modelsToStash.size()));
                        this->addStatusMessage(stashedMsg);
                        m_modelsToStash.clear();
                    }
                    QDialog::accept();
                    break;
                }
            default:
                {
                    if (this->getSelectedOrAllCount() < 1)
                    {
                        const CStatusMessage m(this, CStatusMessage::SeverityError, u"No data, nothing to do");
                        this->addStatusMessage(m);
                        QDialog::accept();
                    }
                    this->tryToStashModels();
                }
            }
        }

        int CDbAutoStashingComponent::exec()
        {
            this->initGui();
            return QDialog::exec();
        }

        void CDbAutoStashingComponent::showLastResults()
        {
            ui->bb_AutoStashing->setStandardButtons(QDialogButtonBox::Close);
            this->setVisible(true);
        }

        void CDbAutoStashingComponent::onEntitiesRead(CEntityFlags::Entity entity, CEntityFlags::ReadState readState, int count, const QUrl &url)
        {
            if (readState != CEntityFlags::ReadFinished) { return; }
            Q_UNUSED(count)
            Q_UNUSED(entity)
            Q_UNUSED(url)
        }

        void CDbAutoStashingComponent::resetDescription()
        {
            ui->rb_DescriptionEmptyOnly->setChecked(true);
            ui->le_Description->setText(CAircraftModel::autoGenerated());
        }

        void CDbAutoStashingComponent::initGui()
        {
            m_state = Idle;
            ui->bb_AutoStashing->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
            ui->tvp_StatusMessages->clear();
            m_noData = 0;
            m_noValidationFailed = 0;
            m_noStashed = 0;
            this->updateProgressIndicator(0);

            if (!this->currentModelView())
            {
                const CStatusMessage m(this, CStatusMessage::SeverityError, u"No data for auto stashing");
                this->addStatusMessage(m);
            }
            else
            {
                int selected = this->currentModelView()->selectedRowCount();
                int all = this->currentModelView()->rowCount();
                ui->le_Selected->setText(QString::number(selected));
                QString allStr(QString::number(all));
                if (all > CDbStashComponent::MaxModelPublished)
                {
                    allStr += u" (Max." % QString::number(CDbStashComponent::MaxModelPublished) % u")";
                }
                ui->le_All->setText(allStr);
                if (ui->le_MaxModelsStashed->text().isEmpty())
                {
                    ui->le_MaxModelsStashed->setText(all > 100 ? QStringLiteral("100") : QString());
                }
                if (selected > 0)
                {
                    ui->rb_Selected->setChecked(true);
                    ui->rb_Selected->setEnabled(true);
                }
                else
                {
                    ui->rb_All->setChecked(true);
                    ui->rb_Selected->setEnabled(false);
                }
            }
        }

        void CDbAutoStashingComponent::updateProgressIndicator(int percent)
        {
            if (percent > 100) { percent = 100; }
            if (percent < 0) { percent = 0; }
            ui->pb_StashingProgress->setValue(percent);
            ui->le_Stashed->setText(QString::number(m_noStashed));
            ui->le_NoData->setText(QString::number(m_noData));
            ui->le_ValidationFailed->setText(QString::number(m_noValidationFailed));
        }

        int CDbAutoStashingComponent::getSelectedOrAllCount() const
        {
            if (!this->currentModelView()) { return 0; }
            if (ui->rb_Selected->isChecked())
            {
                return this->currentModelView()->selectedRowCount();
            }
            else
            {
                return this->currentModelView()->rowCount();
            }
        }

        CAircraftModelView *CDbAutoStashingComponent::currentModelView() const
        {
            return this->getMappingComponent()->currentModelView();
        }

        void CDbAutoStashingComponent::addStatusMessage(const CStatusMessage &msg)
        {
            if (msg.isEmpty()) { return; }
            ui->tvp_StatusMessages->insert(msg);
        }

        void CDbAutoStashingComponent::addStatusMessage(const CStatusMessage &msg, const CAircraftModel &model)
        {
            if (msg.isEmpty()) { return; }
            if (model.hasModelString())
            {
                CStatusMessage prefixMessage(msg);
                prefixMessage.prependMessage(QString(model.getModelString() + ", " + model.getMembersDbStatus() + ": "));
                ui->tvp_StatusMessages->insert(prefixMessage);
            }
            else
            {
                ui->tvp_StatusMessages->insert(msg);
            }
        }

        void CDbAutoStashingComponent::tryToStashModels()
        {
            Q_ASSERT_X(this->currentModelView(), Q_FUNC_INFO, "No view");
            const CAircraftModelList models(ui->rb_Selected->isChecked() ? this->currentModelView()->selectedObjects() : this->currentModelView()->containerOrFilteredContainer());
            if (models.isEmpty()) { return; }
            if (!sGui || sGui->isShuttingDown()) { return; }

            // we have data and are good to go
            m_state = Running;
            const int all = models.size();

            // maximum
            int max = CDbStashComponent::MaxModelPublished;
            const QString maxStr(ui->le_MaxModelsStashed->text());
            bool okMaxStr = true;
            if (!maxStr.isEmpty()) { max = maxStr.toInt(&okMaxStr); }
            if (!okMaxStr || max > all) { max = all; }

            // override description
            const QString description(ui->le_Description->text().trimmed());

            // temp livery if applicable
            const CLivery tempLivery(ui->cb_UseTempLivery->isChecked() ? getTempLivery() : CLivery());

            int c = 0;
            CAircraftModelList autoStashed;
            for (const CAircraftModel &model : models)
            {
                CAircraftModel stashModel(model);
                const bool stashed = this->tryToStashModel(stashModel, tempLivery);
                if (stashed)
                {
                    if (!description.isEmpty())
                    {
                        this->setModelDescription(stashModel, description);
                    }
                    autoStashed.push_back(stashModel);
                }

                c++;
                if (c % 25 == 0)
                {
                    Q_ASSERT_X(c <= all, Q_FUNC_INFO, "illegal numbers");
                    sGui->processEventsToRefreshGui();

                    int percent = c * 100 / all;
                    if (max < all)
                    {
                        int maxPercent = autoStashed.size() * 100 / max;
                        if (maxPercent > percent) { percent = maxPercent; }
                    }
                    this->updateProgressIndicator(percent);
                }
                if (autoStashed.size() >= max) { break; }
            }

            this->updateProgressIndicator(100);
            sGui->processEventsToRefreshGui();

            const CStatusMessage stashedMsg(this, CStatusMessage::SeverityInfo, QStringLiteral("Ready to auto stash %1 models").arg(autoStashed.size()));
            this->addStatusMessage(stashedMsg);
            m_modelsToStash = autoStashed;
            m_state = Completed;
        }

        bool CDbAutoStashingComponent::tryToStashModel(CAircraftModel &model, const CLivery &tempLivery)
        {
            const bool useTempLivery = tempLivery.isLoadedFromDb();

            // no airline and no livery, here replaced by temp livery
            if (useTempLivery && !model.hasAirlineDesignator() && !model.getLivery().hasValidDbKey())
            {
                model.setLivery(tempLivery);
            }

            //! Some upfront tests
            if (!model.hasModelString())
            {
                this->addStatusMessage(CStatusMessage(this, CStatusMessage::SeverityError, u"No model string"));
                m_noData++;
                return false;
            }

            if (!model.hasAircraftDesignator())
            {
                this->addStatusMessage(CStatusMessage(this, CStatusMessage::SeverityError, u"No aircraft designator"), model);
                m_noData++;
                return false;
            }

            if (!model.hasAirlineDesignator() && !model.getLivery().hasValidDbKey())
            {
                // no valid airline and NO DB livery
                // we try one fallback
                const QString liveryCombinedCode = model.getLivery().getCombinedCode();
                bool fallback = false;
                if (liveryCombinedCode.length() == 3 || liveryCombinedCode.length() == 4)
                {
                    // could we use the combined code as airline
                    if (CAirlineIcaoCode::isValidAirlineDesignator(liveryCombinedCode))
                    {
                        model.setAirlineIcaoDesignator(liveryCombinedCode);
                        fallback = true;
                    }
                }

                // if there is no livery (normal) we need an airline
                if (!fallback)
                {
                    this->addStatusMessage(CStatusMessage(this, CStatusMessage::SeverityError, u"No airline designator"), model);
                    m_noData++;
                    return false;
                }
            }

            // stash here consolidates with DB data and validates
            bool stashed = false;
            const CAircraftModel stashModel(this->getMappingComponent()->consolidateModel(model));
            CStatusMessageList validationMsgs(stashModel.validate(true));
            validationMsgs.removeWarningsAndBelow();
            CStatusMessage msg = validationMsgs.toSingleMessage();
            if (msg.getSeverity() == CStatusMessage::SeverityError)
            {
                m_noValidationFailed++;
            }
            else
            {
                msg = CStatusMessage(this, CStatusMessage::SeverityInfo, u"Stashed succesfully");
                stashed = true;
                m_noStashed++;
                model = stashModel;
            }
            this->addStatusMessage(msg, stashModel);
            return stashed;
        }

        void CDbAutoStashingComponent::setModelDescription(CAircraftModel &model, const QString &description) const
        {
            if (description.isEmpty()) { return; }
            if (ui->rb_All->isChecked())
            {
                model.setDescription(description);
            }
            else
            {
                // only for "empty" ones
                if (model.hasDescription(true)) { return; }
                model.setDescription(description);
            }
        }

        BlackMisc::Aviation::CLivery CDbAutoStashingComponent::getTempLivery()
        {
            if (!sGui || !sGui->hasWebDataServices()) { return CLivery(); }
            return sGui->getWebDataServices()->getTempLiveryOrDefault();
        }
    } // ns
} // ns
