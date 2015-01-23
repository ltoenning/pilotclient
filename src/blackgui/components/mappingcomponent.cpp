/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/context_simulator.h"
#include "blackcore/context_network.h"
#include "blackcore/network.h"
#include "blackgui/views/aircraftmodelview.h"
#include "blackgui/views/aircraftmodelfilterform.h"
#include "blackgui/models/aircraftmodellistmodel.h"
#include "blackgui/guiutility.h"
#include "blackmisc/propertyindexlist.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/pixmap.h"
#include "mappingcomponent.h"
#include "ui_mappingcomponent.h"

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackCore;
using namespace BlackGui;
using namespace BlackGui::Views;
using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Components
    {

        CMappingComponent::CMappingComponent(QWidget *parent) :
            QFrame(parent), ui(new Ui::CMappingComponent)
        {
            ui->setupUi(this);
            this->ui->tvp_AircraftModels->setAircraftModelMode(CAircraftModelListModel::ModelOnly);
            this->ui->tvp_CurrentMappings->setAircraftMode(CSimulatedAircraftListModel::ModelMode);
            this->ui->tvp_CurrentMappings->setResizeMode(CAircraftModelView::ResizingOnce);
            this->ui->tvp_AircraftModels->setResizeMode(CAircraftModelView::ResizingOff);

            connect(this->ui->tvp_AircraftModels,  &CAircraftModelView::requestUpdate, this, &CMappingComponent::ps_onModelsUpdateRequested);
            connect(this->ui->tvp_CurrentMappings, &CAircraftModelView::requestUpdate, this, &CMappingComponent::ps_onMappingsUpdateRequested);
            connect(this->ui->tvp_AircraftModels,  &CAircraftModelView::rowCountChanged, this, &CMappingComponent::ps_onRowCountChanged);
            connect(this->ui->tvp_CurrentMappings, &CAircraftModelView::rowCountChanged, this, &CMappingComponent::ps_onRowCountChanged);
            connect(this->ui->tvp_CurrentMappings, &CAircraftModelView::clicked, this, &CMappingComponent::ps_onAircraftSelectedInView);
            connect(this->ui->tvp_AircraftModels,  &CAircraftModelView::clicked, this, &CMappingComponent::ps_onModelSelectedInView);

            connect(this->ui->pb_ApplyMaxAircraft, &QPushButton::clicked, this, &CMappingComponent::ps_onApplyNewMaxRemoteAircraft);
            connect(this->ui->pb_SaveAircraft, &QPushButton::clicked, this, &CMappingComponent::ps_onSaveAircraft);

            // sync slider and text field
            connect(this->ui->hs_MaxAircraft, &QSlider::valueChanged, [ = ](int newValue)
            {
                this->ui->sb_MaxAircraft->setValue(newValue);
            });
            connect(this->ui->sb_MaxAircraft, static_cast<void (QSpinBox::*)(int)> (&QSpinBox::valueChanged), [ = ](int newValue)
            {
                this->ui->hs_MaxAircraft->setValue(newValue);
            });

            this->m_modelCompleter = new QCompleter(this);
            this->m_currentMappingsViewDelegate = new CCheckBoxDelegate(":/diagona/icons/diagona/icons/tick.png", ":/diagona/icons/diagona/icons/cross.png", this);
            this->ui->tvp_CurrentMappings->setItemDelegateForColumn(0, this->m_currentMappingsViewDelegate);

            //! Aircraft previes
            connect(this->ui->cb_AircraftIconDisplayed, &QCheckBox::stateChanged, this, &CMappingComponent::ps_onModelPreviewChanged);
            this->ui->lbl_AircraftIconDisplayed->setText("Icon displayed here");
        }

        CMappingComponent::~CMappingComponent()
        { }

        int CMappingComponent::countCurrentMappings() const
        {
            Q_ASSERT(this->ui->tvp_CurrentMappings);
            return this->ui->tvp_CurrentMappings->rowCount();
        }

        int CMappingComponent::countAircraftModels() const
        {
            Q_ASSERT(this->ui->tvp_AircraftModels);
            return this->ui->tvp_AircraftModels->rowCount();
        }

        CAircraftModelList CMappingComponent::findModelsStartingWith(const QString modelName, Qt::CaseSensitivity cs)
        {
            Q_ASSERT(this->ui->tvp_AircraftModels);
            return this->ui->tvp_AircraftModels->getContainer().findModelsStartingWith(modelName, cs);
        }

        void CMappingComponent::runtimeHasBeenSet()
        {
            Q_ASSERT(getIContextSimulator());
            Q_ASSERT(getIContextNetwork());
            connect(getIContextSimulator(), &IContextSimulator::installedAircraftModelsChanged, this, &CMappingComponent::ps_onAircraftModelsLoaded);
            connect(getIContextSimulator(), &IContextSimulator::modelMatchingCompleted, this, &CMappingComponent::ps_modelMatched);
            connect(getIContextSimulator(), &IContextSimulator::remoteAircraftChanged, this, &CMappingComponent::ps_modelMatched);
            connect(getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CMappingComponent::ps_onConnectionStatusChanged);

            // requires simulator context
            connect(this->ui->tvp_CurrentMappings, &CAircraftModelView::objectChanged, this, &CMappingComponent::ps_onChangedSimulatedAircraft);

            // data
            this->ui->hs_MaxAircraft->setValue(getIContextSimulator()->getMaxRenderedRemoteAircraft());

            // with external core models might be already available
            this->ps_onAircraftModelsLoaded();
        }

        void CMappingComponent::ps_onAircraftModelsLoaded()
        {
            this->ps_onModelsUpdateRequested();
        }

        void CMappingComponent::ps_modelMatched(const BlackMisc::Simulation::CSimulatedAircraft &aircraft)
        {
            Q_UNUSED(aircraft);
            ps_onMappingsChanged();
        }

        void CMappingComponent::ps_onMappingsChanged()
        {
            this->ps_onMappingsUpdateRequested();
        }

        void CMappingComponent::ps_onRowCountChanged(int count, bool withFilter)
        {
            Q_UNUSED(count);
            Q_UNUSED(withFilter);
            int am = this->ui->tw_ListViews->indexOf(this->ui->tb_AircraftModels);
            int cm = this->ui->tw_ListViews->indexOf(this->ui->tb_CurrentMappings);
            QString amf = this->ui->tvp_AircraftModels->derivedModel()->hasFilter() ? "F" : "";
            QString a = this->ui->tw_ListViews->tabBar()->tabText(am);
            QString c = this->ui->tw_ListViews->tabBar()->tabText(cm);
            a = CGuiUtility::replaceTabCountValue(a, this->countAircraftModels()) + amf;
            c = CGuiUtility::replaceTabCountValue(c, this->countCurrentMappings());
            this->ui->tw_ListViews->tabBar()->setTabText(am, a);
            this->ui->tw_ListViews->tabBar()->setTabText(cm, c);
        }

        void CMappingComponent::ps_onChangedSimulatedAircraft(const BlackMisc::CVariant &object, const BlackMisc::CPropertyIndex &index)
        {
            Q_UNUSED(index);
            const CSimulatedAircraft sa = object.to<CSimulatedAircraft>();
            CPropertyIndexVariantMap vm(index, sa.propertyByIndex(index));
            this->getIContextSimulator()->changeRemoteAircraft(sa, vm);
        }

        void CMappingComponent::ps_onAircraftSelectedInView(const QModelIndex &index)
        {
            const CSimulatedAircraft simAircraft = this->ui->tvp_CurrentMappings->at(index);
            this->ui->cb_AircraftEnabled->setChecked(simAircraft.isEnabled());
            this->ui->le_Callsign->setText(simAircraft.getCallsign().asString());
            this->ui->le_AircraftModel->setText(simAircraft.getModel().getModelString());
        }

        void CMappingComponent::ps_onModelSelectedInView(const QModelIndex &index)
        {
            Q_ASSERT(this->getIContextSimulator());
            const CAircraftModel model = this->ui->tvp_AircraftModels->at(index);
            this->ui->le_AircraftModel->setText(model.getModelString());

            if (this->ui->cb_AircraftIconDisplayed->isChecked())
            {
                const int MaxHeight = 125;
                this->ui->lbl_AircraftIconDisplayed->setText("");
                this->ui->lbl_AircraftIconDisplayed->setToolTip(model.getDescription());
                CPixmap pm =  this->getIContextSimulator()->iconForModel(model.getModelString());
                if (pm.isNull())
                {
                    this->ui->lbl_AircraftIconDisplayed->setPixmap(CIcons::crossWhite16());
                }
                else
                {
                    QPixmap qpm = pm.pixmap();
                    if (qpm.height() > MaxHeight)
                    {
                        qpm = qpm.scaledToWidth(MaxHeight, Qt::SmoothTransformation);
                    }
                    this->ui->lbl_AircraftIconDisplayed->setPixmap(qpm);
                }
            }
            else
            {
                this->ps_onModelPreviewChanged(Qt::Unchecked);
            }
        }

        void CMappingComponent::ps_onApplyNewMaxRemoteAircraft()
        {
            Q_ASSERT(getIContextSimulator());
            int noRequested = this->ui->hs_MaxAircraft->value();
            this->getIContextSimulator()->setMaxRenderedRemoteAircraft(noRequested);

            // real value
            int noRendered = this->getIContextSimulator()->getMaxRenderedRemoteAircraft();
            if (noRequested == noRendered)
            {
                CLogMessage(this).info("Max.rendered aircraft: %1") << noRendered;
            }
            else
            {
                CLogMessage(this).info("Max.rendered aircraft: %1, requested: %2") << noRendered << noRequested;
                this->ui->sb_MaxAircraft->setValue(noRendered);
            }
        }

        void CMappingComponent::ps_onSaveAircraft()
        {
            Q_ASSERT(getIContextSimulator());
            QString cs = ui->le_Callsign->text().trimmed();
            if (!CCallsign::isValidCallsign(cs))
            {
                CLogMessage(this).validationError("Invalid callsign for mapping");
                return;
            }

            const CCallsign callsign(cs);
            bool hasCallsign = this->ui->tvp_CurrentMappings->getContainer().containsCallsign(callsign);
            if (!hasCallsign)
            {
                CLogMessage(this).validationError("Unmapped callsign %1 for mapping") << callsign.asString();
                return;
            }

            QString modelString = this->ui->le_AircraftModel->text().trimmed();
            if (modelString.isEmpty())
            {
                CLogMessage(this).validationError("Missing model for mapping");
                return;
            }

            bool hasModel = this->ui->tvp_AircraftModels->getContainer().containsModelString(modelString);
            if (!hasModel)
            {
                CLogMessage(this).validationError("Invalid model for mapping");
                return;
            }

            bool enabled = this->ui->cb_AircraftEnabled->isChecked();
            CSimulatedAircraft aircraft = this->ui->tvp_CurrentMappings->getContainer().findFirstByCallsign(callsign);
            CAircraftModel model = this->ui->tvp_AircraftModels->getContainer().findFirstByModelString(modelString);
            Q_ASSERT(model.hasModelString());

            CPropertyIndexVariantMap changedValues;
            if (aircraft.getModel().getModelString() != model.getModelString())
            {
                model.updateMissingParts(aircraft.getModel());
                model.setModelType(CAircraftModel::TypeManuallySet);
                changedValues.addValue(CSimulatedAircraft::IndexModel, model.toCVariant());
            }
            if (aircraft.isEnabled() != enabled)
            {
                changedValues.addValue(CSimulatedAircraft::IndexEnabled, CVariant::fromValue(enabled));
            }
            if (changedValues.isEmpty())
            {
                CLogMessage(this).info("Model mapping, nothing to change");
                return;
            }
            getIContextSimulator()->changeRemoteAircraft(aircraft, changedValues);
            CLogMessage(this).info("Requesting model change for %1") << callsign.asString();
        }

        void CMappingComponent::ps_onModelPreviewChanged(int state)
        {
            Qt::CheckState s = static_cast<Qt::CheckState>(state);
            if (s == Qt::Unchecked)
            {
                this->ui->lbl_AircraftIconDisplayed->setPixmap(QPixmap());
                this->ui->lbl_AircraftIconDisplayed->setText("Icon disabled");
            }
        }

        void CMappingComponent::ps_onMappingsUpdateRequested()
        {
            Q_ASSERT(getIContextSimulator());
            const CSimulatedAircraftList aircraft = getIContextSimulator()->getRemoteAircraft();
            this->ui->tvp_CurrentMappings->updateContainer(aircraft);
        }

        void CMappingComponent::ps_onModelsUpdateRequested()
        {
            Q_ASSERT(getIContextSimulator());
            CAircraftModelList ml = getIContextSimulator()->getInstalledModels();
            this->ui->tvp_AircraftModels->updateContainer(ml);

            // model completer
            this->m_modelCompleter->setModel(new QStringListModel(ml.getSortedModelStrings(), this->m_modelCompleter));
            this->m_modelCompleter->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
            this->m_modelCompleter->setCaseSensitivity(Qt::CaseInsensitive);
            this->m_modelCompleter->setWrapAround(true);
            this->m_modelCompleter->setCompletionMode(QCompleter::InlineCompletion);
            this->ui->le_AircraftModel->setCompleter(this->m_modelCompleter);
        }

        void CMappingComponent::ps_onConnectionStatusChanged(uint from, uint to)
        {
            INetwork::ConnectionStatus fromStatus = static_cast<INetwork::ConnectionStatus>(from);
            INetwork::ConnectionStatus toStatus = static_cast<INetwork::ConnectionStatus>(to);
            Q_UNUSED(fromStatus);
            if (INetwork::isDisconnectedStatus(toStatus))
            {
                this->ui->tvp_CurrentMappings->clear();
            }
        }

    } // namespace
} // namespace
