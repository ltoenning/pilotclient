/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/processinfo.h"
#include <QFile>
#include <QFileInfo>
#include <type_traits>

#if defined(Q_OS_OSX)
#include <libproc.h>
#elif defined(Q_OS_WIN)
#include <windows.h>
#include <psapi.h>
#endif

namespace BlackMisc
{

    QString CProcessInfo::convertToQString(bool) const
    {
        return QString("{ %1, %2 }").arg(QString::number(m_pid), m_name);
    }

#if defined(Q_OS_LINUX)
    QString CProcessInfo::processNameFromId(qint64 pid)
    {
        QString path = QFileInfo(QString("/proc/%1/exe").arg(pid)).symLinkTarget();
        return QFileInfo(path).fileName();
    }
#elif defined(Q_OS_OSX)
    QString CProcessInfo::processNameFromId(qint64 pid)
    {
        char name[1024];
        proc_name(pid, name, std::extent<decltype(name)>::value);
        return name;
    }
#elif defined(Q_OS_WIN)
    QString CProcessInfo::processNameFromId(qint64 pid)
    {
        HANDLE proc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, static_cast<DWORD>(pid));
        if (! proc) { return ""; }
        wchar_t path[1024];
        auto len = GetModuleFileNameEx(proc, nullptr, path, std::extent<decltype(path)>::value);
        CloseHandle(proc);
        if (len <= 0) { return ""; }
        return QFileInfo(QString::fromWCharArray(path)).completeBaseName();
    }
#else
    QString CProcessInfo::processNameFromId(qint64)
    {
        qFatal("Not implemented");
        return "";
    }
#endif

}
