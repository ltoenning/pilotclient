/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "remotefile.h"
#include "blackmisc/stringutils.h"
#include <QJsonValue>
#include <QtGlobal>

namespace BlackMisc
{
    namespace Network
    {
        CRemoteFile::CRemoteFile(const QString &name, const QString &description)
            : m_name(name), m_description(description)
        { }

        CRemoteFile::CRemoteFile(const QString &name, qint64 size)
            : m_name(name), m_size(size)
        { }

        CRemoteFile::CRemoteFile(const QString &name, qint64 size, const QString &url)
            : m_name(name), m_url(url), m_size(size)
        { }

        QString CRemoteFile::getNameAndSize() const
        {
            if (!this->hasName()) { return {}; }
            return QStringLiteral("%1 (%2)").arg(this->getName(), this->getSizeHumanReadable());
        }

        bool CRemoteFile::matchesName(const QString &name) const
        {
            if (name.isEmpty()) { return false; }
            if (caseInsensitiveStringCompare(name, this->getName())) { return true; }
            if (name.startsWith(this->getName(), Qt::CaseInsensitive)) { return true; }
            if (this->getName().startsWith(name, Qt::CaseInsensitive)) { return true; }
            return false;
        }

        CUrl CRemoteFile::getSmartUrl() const
        {
            if (!this->hasName()) { return this->getUrl(); }
            if (!this->hasUrl())  { return this->getUrl(); }
            return this->getUrl().withAppendedPath(this->getName());
        }

        bool CRemoteFile::isExecutableFile() const
        {
            return CFileUtils::isExecutableFile(this->getName());
        }

        bool CRemoteFile::isSwiftInstaller() const
        {
            return CFileUtils::isSwiftInstaller(this->getName());
        }

        void CRemoteFile::setUrl(const QString &url)
        {
            this->setUrl(CUrl(url));
        }

        QString CRemoteFile::getSizeHumanReadable() const
        {
            return CFileUtils::humanReadableFileSize(this->getSize());
        }

        QString CRemoteFile::getFormattedCreatedYmdhms() const
        {
            if (m_created < 1) { return ""; }
            const QDateTime dt = QDateTime::fromMSecsSinceEpoch(m_created);
            return dt.toString("yyyy-MM-dd HH:mm:ss");
        }

        QString CRemoteFile::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            return "Name: " + m_name +
                   " description: " + m_description +
                   " size: " + QString::number(m_size) +
                   " modified: " + this->getFormattedUtcTimestampYmdhms() +
                   " created: " + this->getFormattedUtcTimestampYmdhms() +
                   " URL: " + m_url.getFullUrl();
        }

        CVariant CRemoteFile::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexName: return CVariant::fromValue(m_name);
            case IndexDescription: return CVariant::fromValue(m_description);
            case IndexUrl: return CVariant::fromValue(m_url);
            case IndexSize: return CVariant::fromValue(m_size);
            default: return CValueObject::propertyByIndex(index);
            }
        }

        void CRemoteFile::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CRemoteFile>(); return; }
            if (ITimestampBased::canHandleIndex(index)) { ITimestampBased::setPropertyByIndex(index, variant); return; }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexName: this->setName(variant.value<QString>()); break;
            case IndexDescription: this->setDescription(variant.value<QString>()); break;
            case IndexUrl: m_url.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexSize: this->setSize(variant.toInt()); break;
            default: CValueObject::setPropertyByIndex(index, variant); break;
            }
        }

        CRemoteFile CRemoteFile::fromDatabaseJson(const QJsonObject &json)
        {
            CRemoteFile file;
            file.setName(json.value("name").toString());
            file.setDescription(json.value("description").toString());
            file.setUrl(json.value("url").toString());
            const qint64 size = json.value("size").toInt();
            file.setSize(size);
            const qint64 created = json.value("tsCreated").toInt();
            const qint64 lastUpdated = json.value("tsLastUpdated").toInt();
            file.m_created = created;
            file.setMSecsSinceEpoch(lastUpdated);
            return file;
        }
    } // ns
} // ns
