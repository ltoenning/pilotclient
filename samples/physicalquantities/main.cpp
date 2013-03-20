#include <QCoreApplication>
#include <QDebug>
#include "blackcore/pqdistance.h"
#include "blackcore/pqfrequency.h"
#include "blackcore/pqspeed.h"
#include "blackcore/pqangle.h"
#include "blackcore/pqmass.h"
#include "blackcore/pqpressure.h"

using namespace BlackCore;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    const CDistance d1(5.0, CDistanceUnit::ft()); // 5 ft
    CDistance d2(1, CDistanceUnit::NM()); // 1NM
    CDistance d3(1, CDistanceUnit::km());
    qDebug() << d1 << d2 << d3;
    qDebug() << d1.valueRoundedWithUnit(CDistanceUnit::ft(),5)
             << d2.valueRoundedWithUnit(CDistanceUnit::km());
    qDebug() << d3.getUnit();

    d2.switchUnit(CDistanceUnit::ft()); // now in ft
    d3 += d3; // 2km now
    d3 *= 1.5;// 3km now
    qDebug() << d2 << d3;

    CFrequency f1(1E6); // 1MHz
    qDebug() << f1 << f1.valueRoundedWithUnit(CFrequencyUnit::MHz()) << f1.valueRoundedWithUnit(CFrequencyUnit::GHz(), 3);

    CSpeed s1 = CSpeed(100, CSpeedUnit::km_h());
    CSpeed s2 = CSpeed(1000,CSpeedUnit::ft_min());
    CSpeed s3 = CSpeed(s2);
    s3.switchUnit(CSpeedUnit::m_s());
    qDebug() << s1 << s1.convertedSiValueRoundedWithUnit() << s1.valueRoundedWithUnit(CSpeedUnit::NM_h());
    qDebug() << s2 << s3;

    CAngle a1(180, CAngleUnit::deg());
    CAngle a2(1.5 * CAngle::pi());
    a2.switchUnit(CAngleUnit::deg());
    qDebug() << a1.unitValueRoundedWithUnit() << a1.piFactor();
    qDebug() << a2;

    CMass w1(1,CMassUnit::t());
    CMass w2(w1);
    w2.switchUnit(CMassUnit::lb());
    qDebug() << w1 << w1.valueRoundedWithUnit(CMassUnit::kg()) << w2;

    CPressure p1(1013.25, CPressureUnit::hPa());
    qDebug() << p1 << p1.valueRoundedWithUnit(CPressureUnit::psi()) << p1.valueRoundedWithUnit(CPressureUnit::inHg());

    // bye
    return a.exec();
}
