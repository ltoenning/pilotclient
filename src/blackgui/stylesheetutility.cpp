/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "stylesheetutility.h"
#include <QCoreApplication>
#include <QDir>
#include <QTextStream>
#include <QDebug>

namespace BlackGui
{

    CStyleSheetUtility::CStyleSheetUtility(QObject *parent) : QObject(parent)
    {
        this->read();
    }

    const QString CStyleSheetUtility::fontStyleAsString(const QFont &font)
    {
        switch (font.style())
        {
        case QFont::StyleNormal: return "normal";
        case QFont::StyleItalic: return "italic";
        case QFont::StyleOblique: return "oblique";
        default: return "";
        }
    }

    const QString CStyleSheetUtility::fontWeightAsString(const QFont &font)
    {
        if (font.weight() < static_cast<int>(QFont::Normal))
        {
            return "light";
        }
        else if (font.weight() < static_cast<int>(QFont::DemiBold))
        {
            return "normal";
        }
        else if (font.weight() < static_cast<int>(QFont::Bold))
        {
            return "demibold";
        }
        else if (font.weight() < static_cast<int>(QFont::Black))
        {
            return "bold";
        }
        else
        {
            return "black";
        }
    }

    bool CStyleSheetUtility::read()
    {
        QDir directory(qssDirectory());
        if (!directory.exists()) return false;
        directory.setNameFilters({"*.qss"});
        directory.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);

        QFileInfoList fileInfoList = directory.entryInfoList();

        for (int i = 0; i < fileInfoList.size(); ++i)
        {
            QFileInfo fileInfo = fileInfoList.at(i);
            QFile file(fileInfo.absoluteFilePath());
            if (file.open(QFile::QIODevice::ReadOnly | QIODevice::Text))
            {
                QTextStream in(&file);
                QString c = in.readAll();
                QString f = fileInfo.fileName().toLower();

                // keep even empty files as placeholders
                this->m_styleSheets.insert(f, c);
            }
            file.close();
        }
        emit this->styleSheetsChanged();
        return true;
    }

    QString CStyleSheetUtility::style(const QString &fileName) const
    {
        if (!this->containsStyle(fileName)) return QString();
        return this->m_styleSheets[fileName.toLower()];
    }

    QString CStyleSheetUtility::styles(const QStringList &fileNames) const
    {
        QString style;
        foreach(QString fileName, fileNames)
        {
            if (!this->containsStyle(fileName)) continue;
            QString s = this->m_styleSheets[fileName.toLower()];
            if (s.isEmpty()) continue;
            if (!style.isEmpty()) style.append("\n\n");
            style.append("/** file: %1 **/\n").arg(fileName);
            style.append(s);
        }
        return style;
    }

    bool CStyleSheetUtility::containsStyle(const QString &fileName) const
    {
        if (fileName.isEmpty()) return false;
        return this->m_styleSheets.contains(fileName.toLower());
    }

    bool CStyleSheetUtility::updateFonts(const QFont &font)
    {
        const QString indent("     ");
        QString fontStyleSheet;
        fontStyleSheet.append(indent).append("font-family: \"").append(font.family()).append("\";\n");
        fontStyleSheet.append(indent).append("font-size: ");
        if (font.pixelSize() >= 0)
        {
            fontStyleSheet.append(font.pixelSize()).append("px\n");
        }
        else
        {
            fontStyleSheet.append(QString::number(font.pointSizeF())).append("pt;\n");
        }
        fontStyleSheet.append(indent).append("font-style: ").append(fontStyleAsString(font)).append(";\n");
        fontStyleSheet.append(indent).append("font-weight: ").append(fontWeightAsString(font)).append(";\n");
        qDebug() << fontStyleSheet;

        QString qss("QWidget {\n");
        qss.append(indent).append("color: white;\n");
        qss.append(fontStyleSheet);
        qss.append("}\n");

        QFile fontFile(qssDirectory().append("/").append(fileNameFonts()));
        bool ok = fontFile.open(QFile::Text | QFile::WriteOnly);
        if (ok)
        {
            QTextStream out(&fontFile);
            out << qss;
            fontFile.close();
            ok = this->read();
        }
        return ok;
    }

    QString CStyleSheetUtility::qssDirectory()
    {
        QString dirPath = QCoreApplication::applicationDirPath();
        if (!dirPath.endsWith('/')) dirPath.append('/');
        dirPath.append("qss");
        return dirPath;
    }
}
