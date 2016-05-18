/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/components/dbmappingcomponent.h"
#include "blackgui/components/dbmappingcomponentaware.h"
#include "dbmappingcomponentaware.h"

#include <QObject>

namespace BlackGui
{
    namespace Components
    {
        void CDbMappingComponentAware::setMappingComponent(CDbMappingComponent *component)
        {
            m_mappingComponent = component;
        }

        CDbMappingComponentAware::CDbMappingComponentAware(QObject *parent)
        {
            // if we get a mapping component we use it
            if (!parent) { return; }
            if (!parent->isWidgetType()) { return; }
            CDbMappingComponent *m = qobject_cast<CDbMappingComponent *>(parent);
            if (!m) { return; }
            m_mappingComponent = m;
        }

    } // ns
} // ns
