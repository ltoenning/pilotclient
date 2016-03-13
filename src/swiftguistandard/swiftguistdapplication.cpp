/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "swiftguistdapplication.h"
#include "blackcore/coremodeenums.h"
#include "blackmisc/dbusserver.h"

using namespace BlackMisc;
using namespace BlackCore;

CSwiftGuiStdApplication::CSwiftGuiStdApplication() : CGuiApplication("swift pilot client GUI", CIcons::swift24())
{
    this->addParserOption(this->m_cmdFacadeMode);
    this->addWindowModeOption();
    this->addDBusAddressOption();
    this->useWebDataServices(CWebReaderFlags::AllReaders, CWebReaderFlags::FromCache);
}

bool CSwiftGuiStdApplication::startHookIn()
{
    Q_ASSERT_X(m_parsed, Q_FUNC_INFO, "Not yet parsed cmd line arguments");
    CoreModes::CoreMode coreMode = CoreModes::CoreInGuiProcess;
    const QString dBusAddress(this->getCmdDBusAddressValue());
    if (this->isParserOptionSet(this->m_cmdFacadeMode))
    {
        const QString v(this->getParserOptionValue(this->m_cmdFacadeMode));
        coreMode = CoreModes::stringToCoreMode(v);
    }

    CCoreFacadeConfig runtimeConfig;
    switch (coreMode)
    {
    case CoreModes::CoreExternalCoreAudio:
        runtimeConfig = CCoreFacadeConfig::remote(dBusAddress);
        break;
    default:
    case CoreModes::CoreInGuiProcess:
        runtimeConfig = CCoreFacadeConfig::local(dBusAddress);
        break;
    case CoreModes::CoreExternalAudioGui:
        runtimeConfig = CCoreFacadeConfig::remoteLocalAudio(dBusAddress);
        break;
    }
    return this->useContexts(runtimeConfig);
}

bool CSwiftGuiStdApplication::parsingHookIn()
{
    // Parse core relevant arguments
    const QString dBusAddress(this->getCmdDBusAddressValue());
    if (!dBusAddress.isEmpty())
    {
        // check if rechable
        if (!CDBusServer::isDBusAvailable(dBusAddress))
        {
            this->cmdLineErrorMessage("DBus server at " + dBusAddress + " can not be reached");
            return false;
        }
    }
    return CGuiApplication::parsingHookIn();
}

CSwiftGuiStdApplication *instance()
{
    return qobject_cast<CSwiftGuiStdApplication *>(CApplication::instance());
}
