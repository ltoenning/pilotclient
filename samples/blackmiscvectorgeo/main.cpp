/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "samplesvectormatrix.h"
#include "samplesgeo.h"
#include "samplesgeodetictoecef.h"
#include <QCoreApplication>

/*!
 * \brief Main entry
 * \param argc
 * \param argv
 * \return
 */
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    BlackMiscTest::CSamplesVectorMatrix::samples();
    BlackMiscTest::CSamplesGeo::samples();
    BlackMiscTest::CSamplesGeodeticToEcef::samples();
    return a.exec();
}
