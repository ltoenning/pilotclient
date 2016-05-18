/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/components/enablefordockwidgetinfoarea.h"
#include "blackgui/dockwidgetinfoarea.h"
#include "blackgui/enableforframelesswindow.h"
#include "blackgui/guiutility.h"
#include "blackgui/infoarea.h"

#include <QDockWidget>
#include <QMetaObject>
#include <QObject>
#include <QtGlobal>

using namespace BlackGui;

namespace BlackGui
{
    namespace Components
    {
        CEnableForDockWidgetInfoArea::CEnableForDockWidgetInfoArea(CDockWidgetInfoArea *parentInfoArea)
        {
            // it the parent is already an info area at this time, we keep it
            // otherwise we expect the info area to set it later
            this->m_parentDockableInfoArea = parentInfoArea;
        }

        bool CEnableForDockWidgetInfoArea::setParentDockWidgetInfoArea(CDockWidgetInfoArea *parentDockableWidget)
        {
            // sanity check
            if (this->m_parentDockableInfoArea)
            {
                // we already have a value
                // changes should not happen
                Q_ASSERT_X(this->m_parentDockableInfoArea == parentDockableWidget, Q_FUNC_INFO, "Reassigned parent dock widget area");
                return this->m_parentDockableInfoArea == parentDockableWidget;
            }

            m_parentDockableInfoArea = parentDockableWidget;
            QMetaObject::Connection con = QDockWidget::connect(parentDockableWidget, &QDockWidget::destroyed, [this]
            {
                // break dependency to dockable widget
                this->m_parentDockableInfoArea = nullptr;
            });
            Q_ASSERT_X(con, Q_FUNC_INFO, "Connection failed");
            this->m_connections.append(con);
            return true;
        }

        CInfoArea *CEnableForDockWidgetInfoArea::getParentInfoArea() const
        {
            Q_ASSERT(this->m_parentDockableInfoArea);
            if (!this->m_parentDockableInfoArea) return nullptr;
            return this->m_parentDockableInfoArea->getParentInfoArea();
        }

        bool CEnableForDockWidgetInfoArea::isParentDockWidgetFloating() const
        {
            Q_ASSERT(this->m_parentDockableInfoArea);
            if (!this->m_parentDockableInfoArea) { return false; }
            return this->m_parentDockableInfoArea->isFloating();
        }

        bool CEnableForDockWidgetInfoArea::isVisibleWidget() const
        {
            Q_ASSERT(this->m_parentDockableInfoArea);
            if (!this->m_parentDockableInfoArea) { return false; }
            return this->m_parentDockableInfoArea->isVisibleWidget();
        }

        CEnableForFramelessWindow *CEnableForDockWidgetInfoArea::mainApplicationWindow() const
        {
            CEnableForFramelessWindow *mw = CGuiUtility::mainFramelessEnabledApplicationWindow();
            return mw;
        }

        QWidget *CEnableForDockWidgetInfoArea::mainApplicationWindowWidget() const
        {
            CEnableForFramelessWindow *mw = this->mainApplicationWindow();
            return mw ? mw->getWidget() : nullptr;
        }

        void CEnableForDockWidgetInfoArea::displayMyself()
        {
            // if we are already visible, we are done
            if (this->isVisibleWidget()) { return; }

            // select myself
            if (getParentInfoArea())
            {
                getParentInfoArea()->selectArea(this->getDockWidgetInfoArea());
            }
        }

    } // namespace
} // namespace
