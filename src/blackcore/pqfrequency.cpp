#include "pqfrequency.h"

namespace BlackCore {

/**
 * Default constructor
 */
CFrequency::CFrequency() : CPhysicalQuantity(0, CFrequencyUnit::Hz(), CFrequencyUnit::Hz())
{
    // void
}

/**
 * Constructor
 */
CFrequency::CFrequency(const CPhysicalQuantity &frequency): CPhysicalQuantity(frequency)
{
    //void
}

/**
 * Constructor
 */
CFrequency::CFrequency(qint32 value, const CFrequencyUnit &unit) : CPhysicalQuantity(value, unit, CFrequencyUnit::Hz())
{
    // void
}

/**
 * Constructor
 */
CFrequency::CFrequency(double value, const CFrequencyUnit &unit) : CPhysicalQuantity(value, unit, CFrequencyUnit::Hz())
{
    // void
}


} // namespace
