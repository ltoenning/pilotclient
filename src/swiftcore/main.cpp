/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/corefacadeconfig.h"
#include "blackgui/guiapplication.h"
#include "blackmisc/icons.h"
#include "swiftcore.h"

#include <stdlib.h>
#include <QApplication>
#include <QString>
#include <QSystemTrayIcon>
#include <Qt>

using namespace BlackMisc;
using namespace BlackCore;
using namespace BlackGui;

int main(int argc, char *argv[])
{
    //! [SwiftApplicationDemo]
    CGuiApplication::highDpiScreenSupport();
    QApplication qa(argc, argv);
    CGuiApplication a("swift core", CGuiApplication::PilotClientCore, CIcons::swiftNova24());
    a.addWindowStateOption();
    a.addDBusAddressOption();
    a.addVatlibOptions();
    a.addParserOption({{"r", "start"}, QCoreApplication::translate("main", "Start the server.")});
    a.addParserOption({{"c", "coreaudio"}, QCoreApplication::translate("main", "Audio in core.")});
    if (!a.parse()) { return EXIT_FAILURE; }

    const QString dBusAdress(a.getCmdDBusAddressValue());
    a.useContexts(a.isParserOptionSet("coreaudio") ?
                  CCoreFacadeConfig::forCoreAllLocalInDBus(dBusAdress) :
                  CCoreFacadeConfig::forCoreAllLocalInDBusNoAudio(dBusAdress));
    a.start();
    //! [SwiftApplicationDemo]

    if (!QSystemTrayIcon::isSystemTrayAvailable())
    {
        a.cmdLineErrorMessage("I could not detect any system tray on this system.");
        return EXIT_FAILURE;
    }

    CSwiftCore w;
    if (a.getWindowState() != Qt::WindowMinimized) { w.show(); }
    return a.exec();
}
