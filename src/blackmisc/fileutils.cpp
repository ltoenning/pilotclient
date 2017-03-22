/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackconfig/buildconfig.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/worker.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QFile>
#include <QFlags>
#include <QIODevice>
#include <QList>
#include <QLockFile>
#include <QTextStream>
#include <QtGlobal>
#include <algorithm>

using namespace BlackConfig;

namespace BlackMisc
{
    const QString &CFileUtils::jsonAppendix()
    {
        static const QString j(".json");
        return j;
    }

    const QString &CFileUtils::jsonWildcardAppendix()
    {
        static const QString jw("*" + jsonAppendix());
        return jw;
    }

    bool CFileUtils::writeStringToFile(const QString &content, const QString &fileNameAndPath)
    {
        if (fileNameAndPath.isEmpty()) { return false; }
        QFile file(fileNameAndPath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) { return false; }
        QTextStream stream(&file);
        stream << content;
        file.close();
        return true;
    }

    bool CFileUtils::writeStringToLockedFile(const QString &content, const QString &fileNameAndPath)
    {
        QLockFile lock(fileNameAndPath + ".lock");
        lock.lock();
        return writeStringToFile(content, fileNameAndPath);
    }

    QString CFileUtils::readFileToString(const QString &fileNameAndPath)
    {
        QFile file(fileNameAndPath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) { return ""; }
        QTextStream stream(&file);
        QString content(stream.readAll());
        file.close();
        return content;
    }

    QString CFileUtils::readLockedFileToString(const QString &fileNameAndPath)
    {
        QLockFile lock(fileNameAndPath + ".lock");
        lock.lock();
        return readFileToString(fileNameAndPath);
    }

    QString CFileUtils::readFileToString(const QString &filePath, const QString &fileName)
    {
        return readFileToString(appendFilePaths(filePath, fileName));
    }

    QString CFileUtils::readLockedFileToString(const QString &filePath, const QString &fileName)
    {
        return readLockedFileToString(appendFilePaths(filePath, fileName));
    }

    bool CFileUtils::writeStringToFileInBackground(const QString &content, const QString &fileNameAndPath)
    {
        if (fileNameAndPath.isEmpty()) { return false; }
        CWorker *worker = BlackMisc::CWorker::fromTask(QCoreApplication::instance(), "writeStringToFileInBackground", [content, fileNameAndPath]()
        {
            bool s = CFileUtils::writeStringToFile(content, fileNameAndPath);
            Q_UNUSED(s);
        });
        return worker ? true : false;
    }

    QString CFileUtils::appendFilePaths(const QString &path1, const QString &path2)
    {
        if (path1.isEmpty()) { return QDir::cleanPath(path2); }
        if (path2.isEmpty()) { return QDir::cleanPath(path1); }
        return QDir::cleanPath(path1 + QChar('/') + path2);
    }

    QString CFileUtils::stripFileFromPath(const QString &path)
    {
        if (path.endsWith('/'))  { return path; }
        if (!path.contains('/')) { return path; }
        return path.left(path.lastIndexOf('/'));
    }

    QString CFileUtils::appendFilePaths(const QString &path1, const QString &path2, const QString &path3)
    {
        return CFileUtils::appendFilePaths(CFileUtils::appendFilePaths(path1, path2), path3);
    }

    bool CFileUtils::copyRecursively(const QString &sourceDir, const QString &destinationDir)
    {
        QFileInfo sourceFileInfo(sourceDir);
        if (sourceFileInfo.isDir())
        {
            QDir targetDir(destinationDir);
            if (!targetDir.mkpath("."))
            {
                return false;
            }

            QDir originDir(sourceFileInfo.absoluteFilePath());
            auto fileNames = originDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
            for (const QString &fileName : fileNames)
            {
                if (!copyRecursively(originDir.absoluteFilePath(fileName), targetDir.absoluteFilePath(fileName)))
                {
                    return false;
                }
            }
        }
        else
        {
            if (!QFile::copy(sourceDir, destinationDir))
            {
                return false;
            }
        }

        return true;
    }

    QString CFileUtils::normalizeFilePathToQtStandard(const QString &filePath)
    {
        if (filePath.isEmpty()) { return ""; }
        QString n(filePath);
        n = n.replace('\\', '/').replace("//", "/");
        return n;
    }

    QStringList CFileUtils::makeDirectoriesRelative(const QStringList &directories, const QString &rootDirectory, Qt::CaseSensitivity cs)
    {
        // not using QDir::relativePath because I do not want "../xyz" paths
        if (rootDirectory.isEmpty() || rootDirectory == "/") { return directories; }
        const QString rd(rootDirectory.endsWith('/') ? rootDirectory.left(rootDirectory.length() - 1) : rootDirectory);
        const int p = rd.length();
        QStringList relativeDirectories;
        for (const QString &dir : directories)
        {
            if (dir.startsWith(rd, cs) && dir.length() > p + 1)
            {
                relativeDirectories.append(dir.mid(p + 1));
            }
            else
            {
                relativeDirectories.append(dir); // absolute
            }
        }
        return relativeDirectories;
    }

    Qt::CaseSensitivity CFileUtils::osFileNameCaseSensitivity()
    {
        return CBuildConfig::isRunningOnWindowsNtPlatform() ? Qt::CaseInsensitive : Qt::CaseSensitive;
    }

    bool CFileUtils::matchesExcludeDirectory(const QString &directoryPath, const QString &excludePattern, Qt::CaseSensitivity cs)
    {
        if (directoryPath.isEmpty() || excludePattern.isEmpty()) { return false; }
        const QString normalizedExcludePattern(normalizeFilePathToQtStandard(excludePattern));
        return directoryPath.contains(normalizedExcludePattern, cs);
    }

    bool CFileUtils::isExcludedDirectory(const QDir &directory, const QStringList &excludeDirectories, Qt::CaseSensitivity cs)
    {
        if (excludeDirectories.isEmpty()) { return false; }
        const QString d = directory.absolutePath();
        return isExcludedDirectory(d, excludeDirectories, cs);
    }

    bool CFileUtils::isExcludedDirectory(const QFileInfo &fileInfo, const QStringList &excludeDirectories, Qt::CaseSensitivity cs)
    {
        if (excludeDirectories.isEmpty()) { return false; }
        return isExcludedDirectory(fileInfo.absoluteDir(), excludeDirectories, cs);
    }

    bool CFileUtils::isExcludedDirectory(const QString &directoryPath, const QStringList &excludeDirectories, Qt::CaseSensitivity cs)
    {
        if (excludeDirectories.isEmpty()) { return false; }
        for (const QString &ex : excludeDirectories)
        {
            if (matchesExcludeDirectory(directoryPath, ex, cs))
            {
                return true;
            }
        }
        return false;
    }

    QStringList CFileUtils::removeSubDirectories(const QStringList &directories, Qt::CaseSensitivity cs)
    {
        if (directories.size() < 2) { return directories; }
        QStringList dirs(directories);
        dirs.removeDuplicates();
        dirs.sort(cs);
        if (dirs.size() < 2) { return dirs; }

        QString last;
        QStringList result;
        for (const QString &path : dirs)
        {
            if (path.isEmpty()) { continue; }
            if (last.isEmpty() || !path.startsWith(last, cs))
            {
                result.append(path);
            }
            last = path;
        }
        return result;
    }

    QString CFileUtils::findFirstExisting(const QStringList &filesOrDirectory)
    {
        if (filesOrDirectory.isEmpty()) { return ""; }
        for (const QString &f : filesOrDirectory)
        {
            if (f.isEmpty()) { continue; }
            const QString fn(normalizeFilePathToQtStandard(f));
            const QFileInfo fi(fn);
            if (fi.exists()) { return fi.absoluteFilePath(); }
        }
        return "";
    }

    QString CFileUtils::findFirstFile(const QDir &dir, bool recursive, const QStringList &nameFilters, const QStringList &excludeDirectories, std::function<bool(const QFileInfo &)> predicate)
    {
        if (isExcludedDirectory(dir, excludeDirectories)) { return QString(); }
        const QFileInfoList result = dir.entryInfoList(nameFilters, QDir::Files);
        if (predicate)
        {
            auto it = std::find_if(result.cbegin(), result.cend(), predicate);
            if (it != result.cend()) { return it->filePath(); }
        }
        else
        {
            if (! result.isEmpty()) { return result.first().filePath(); }
        }
        if (recursive)
        {
            for (const auto &subdir : dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot))
            {
                if (isExcludedDirectory(subdir, excludeDirectories)) { continue; }
                const QString first = findFirstFile(subdir.filePath(), true, nameFilters, excludeDirectories, predicate);
                if (! first.isEmpty()) { return first; }
            }
        }
        return {};
    }

    bool CFileUtils::containsFile(const QDir &dir, bool recursive, const QStringList &nameFilters, const QStringList &excludeDirectories, std::function<bool(const QFileInfo &)> predicate)
    {
        return ! findFirstFile(dir, recursive, nameFilters, excludeDirectories, predicate).isEmpty();
    }

    QString CFileUtils::findFirstNewerThan(const QDateTime &time, const QDir &dir, bool recursive, const QStringList &nameFilters, const QStringList &excludeDirectories)
    {
        return findFirstFile(dir, recursive, nameFilters, excludeDirectories, [time](const QFileInfo & fi) { return fi.lastModified() > time; });
    }

    bool CFileUtils::containsFileNewerThan(const QDateTime &time, const QDir &dir, bool recursive, const QStringList &nameFilters, const QStringList &excludeDirectories)
    {
        return ! findFirstNewerThan(time, dir, recursive, nameFilters, excludeDirectories).isEmpty();
    }

    QFileInfoList CFileUtils::enumerateFiles(const QDir &dir, bool recursive, const QStringList &nameFilters, const QStringList &excludeDirectories, std::function<bool(const QFileInfo &)> predicate)
    {
        if (isExcludedDirectory(dir, excludeDirectories)) { return QFileInfoList(); }
        QFileInfoList result = dir.entryInfoList(nameFilters, QDir::Files);
        if (predicate)
        {
            result.erase(std::remove_if(result.begin(), result.end(), std::not1(predicate)), result.end());
        }
        if (recursive)
        {
            for (const auto &subdir : dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot))
            {
                if (isExcludedDirectory(subdir, excludeDirectories)) { continue; }
                result += enumerateFiles(subdir.filePath(), true, nameFilters, excludeDirectories, predicate);
            }
        }
        return result;
    }

    QString CFileUtils::findNewestFile(const QDir &dir, bool recursive, const QStringList &nameFilters, const QStringList &excludeDirectories)
    {
        if (isExcludedDirectory(dir, excludeDirectories)) { return QString(); }
        const QFileInfoList files = enumerateFiles(dir, recursive, nameFilters, excludeDirectories);
        if (files.isEmpty()) { return {}; }

        auto it = std::max_element(files.cbegin(), files.cend(), [](const QFileInfo & a, const QFileInfo & b)
        {
            return a.lastModified() < b.lastModified();
        });
        return it->filePath();
    }

    const QStringList &CFileUtils::getSwiftExecutables()
    {
        static const QStringList executables(
            QFileInfo(QCoreApplication::applicationFilePath())
            .dir()
            .entryList(QDir::Executable | QDir::Files)
        );
        return executables;
    }

    QString CFileUtils::lockFileError(const QLockFile &lockFile)
    {
        switch (lockFile.error())
        {
        case QLockFile::NoError: return QStringLiteral("No error");
        case QLockFile::PermissionError: return QStringLiteral("Insufficient permission");
        case QLockFile::UnknownError: return QStringLiteral("Unknown error");
        case QLockFile::LockFailedError:
            {
                QString hostname, appname;
                qint64 pid = 0;
                lockFile.getLockInfo(&pid, &hostname, &appname);
                return QStringLiteral("Lock open in another process (%1 %2 on %3)").arg(hostname, QString::number(pid), appname);
            }
        default: return QStringLiteral("Bad error number");
        }
    }

    QString CFileUtils::fixWindowsUncPath(const QString &filePath)
    {
        if (!filePath.startsWith('/')) { return filePath; }
        if (filePath.startsWith("//")) { return filePath; }
        static const QString f("/%1");
        return f.arg(filePath);
    }
} // ns
