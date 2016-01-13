/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "reader.h"
#include "weatherdataprinter.h"
#include "blackmisc/registermetadata.h"
#include "blackmisc/loghandler.h"
#include <QCoreApplication>
#include <QTextStream>
#include <QFile>

using namespace BlackMisc;
using namespace BlackMisc::PhysicalQuantities;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    BlackMisc::registerMetadata();
    CLogHandler::instance()->install(true);
//    CLogHandler::instance()->enableConsoleOutput(false); // default disable
    BlackMisc::registerMetadata();

    CLineReader lineReader(&a);
    CWeatherDataPrinter printer(&a);
    QObject::connect(&lineReader, &CLineReader::weatherDataRequest, &printer, &CWeatherDataPrinter::fetchAndPrintWetherData);
    QObject::connect(&lineReader, &CLineReader::quit, &lineReader, &CLineReader::terminate);
    QObject::connect(&lineReader, &CLineReader::finished, &a, &QCoreApplication::quit);

    QTextStream qtout(stdout);
    qtout << "Usage: <lat> <lon>" << endl;
    qtout << "Example: 48.5 11.5" << endl;
    qtout << "Type x to quit" << endl;

    lineReader.start();
    return a.exec();
}
