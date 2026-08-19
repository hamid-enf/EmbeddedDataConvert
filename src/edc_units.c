/**
 * @file edc_units.c
 * @brief Independent physical unit conversion implementations.
 *
 * @note MISRA-C:2012 Compliant, Zero Dynamic Allocation.
 * @author EmbeddedDataConvert Team
 */

#include "edc_units.h"

#if EDC_FEATURE_UNITS

/* Non-inline exported symbols for compilers without inline or linking as shared library */

double EDC_Units_MeterToKm(double m) {
    return EDC_Unit_MeterToKm(m);
}

double EDC_Units_KmToMeter(double km) {
    return EDC_Unit_KmToMeter(km);
}

double EDC_Units_MpsToKmh(double mps) {
    return EDC_Unit_MpsToKmh(mps);
}

double EDC_Units_KmhToMps(double kmh) {
    return EDC_Unit_KmhToMps(kmh);
}

double EDC_Units_KnotsToKmh(double kts) {
    return EDC_Unit_KnotsToKmh(kts);
}

double EDC_Units_KmhToKnots(double kmh) {
    return EDC_Unit_KmhToKnots(kmh);
}

double EDC_Units_CelsiusToFahrenheit(double c) {
    return EDC_Unit_CelsiusToFahrenheit(c);
}

double EDC_Units_FahrenheitToCelsius(double f) {
    return EDC_Unit_FahrenheitToCelsius(f);
}

double EDC_Units_DegToRad(double deg) {
    return EDC_Unit_DegToRad(deg);
}

double EDC_Units_RadToDeg(double rad) {
    return EDC_Unit_RadToDeg(rad);
}

#endif /* EDC_FEATURE_UNITS */
