/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file
//! \ingroup sampleweatherdata

#include "reader.h"
#include <QFile>
#include <QRegularExpression>

using namespace BlackMisc::Geo;
using namespace BlackMisc::PhysicalQuantities;

void CLineReader::run()
{
    QFile file;
    file.open(stdin, QIODevice::ReadOnly | QIODevice::Text);
    QRegularExpression re("^(\\d+).([0,5])\\s(\\d+).([0,5])$");
    forever
    {
        QString line = file.readLine().trimmed();

        if (line == "x")
        {
            emit quit();
            continue;
        }

        QRegularExpressionMatch match = re.match(line);
        if (match.hasMatch())
        {
            double latitudeValue = match.captured(1).toDouble();
            latitudeValue += match.captured(2).toDouble() / 10;
            double longitudeValue = match.captured(3).toDouble();
            longitudeValue += match.captured(4).toDouble() / 10;
            const CLatitude latitude(latitudeValue, CAngleUnit::deg());
            const CLongitude longitude(longitudeValue, CAngleUnit::deg());
            const CCoordinateGeodetic position { latitude, longitude, {0} };
            emit weatherDataRequest(position);
        }
        else
        {
            QTextStream qtout(stdout);
            qtout << "Invalid command." << endl;
            qtout << "Usage: <lat> <lon>" << endl;
        }
    }
}
