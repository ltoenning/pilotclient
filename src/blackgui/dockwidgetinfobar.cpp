// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/dockwidgetinfobar.h"
#include "blackgui/guiapplication.h"
#include "blackgui/stylesheetutility.h"
#include "blackmisc/icons.h"

#include <QIcon>
#include <QString>

using namespace BlackMisc;
using namespace BlackGui::Components;

namespace BlackGui
{
    CDockWidgetInfoBar::CDockWidgetInfoBar(QWidget *parent) : CDockWidget(false, parent)
    {
        this->setWindowTitle("Info status bar");
        this->setWindowIcon(CIcons::swift24());
        this->onStyleSheetsChanged(); // margins from infobar

        // style changed overridden from CDockWidget
    }

    void CDockWidgetInfoBar::addToContextMenu(QMenu *contextMenu) const
    {
        // Dockable widget's context menu
        CDockWidget::addToContextMenu(contextMenu);
    }

    void CDockWidgetInfoBar::onStyleSheetsChanged()
    {
        // overridden from CDockWidget
        if (!sGui || sGui->isShuttingDown()) { return; }
        const QString st = sGui->getStyleSheetUtility().style(CStyleSheetUtility::fileNameInfoBar());
        this->setStyleSheet("");
        this->setStyleSheet(st);
    }
}
