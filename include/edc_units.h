/**
 * @file edc_units.h
 * @brief Independent physical unit conversion module for embedded sensors and telemetry.
 *
 * Provides fast, zero-overhead conversion functions for distance, speed, temperature,
 * angle, and pressure with compile-time constants.
 *
 * @note MISRA-C:2012 Compliant, Zero Dynamic Allocation, Thread-Safe.
 * @author EmbeddedDataConvert Team
 */

#ifndef EDC_UNITS_H
#define EDC_UNITS_H

#include "edc_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#if EDC_FEATURE_UNITS

/* ========================================================================== */
/*                    CONVERSION CONSTANTS                                    */
/* ========================================================================== */

#define EDC_CONST_PI                 3.14159265358979323846
#define EDC_CONST_DEG_TO_RAD         (EDC_CONST_PI / 180.0)
#define EDC_CONST_RAD_TO_DEG         (180.0 / EDC_CONST_PI)

#define EDC_CONST_FEET_PER_METER     3.280839895013123
#define EDC_CONST_METERS_PER_FOOT    0.3048
#define EDC_CONST_INCH_PER_CM        0.3937007874015748
#define EDC_CONST_CM_PER_INCH        2.54

#define EDC_CONST_KM_PER_MILE        1.609344
#define EDC_CONST_MILES_PER_KM       0.621371192237334

#define EDC_CONST_METERS_PER_NAUTICAL_MILE 1852.0
#define EDC_CONST_NAUTICAL_MILES_PER_METER (1.0 / 1852.0)

#define EDC_CONST_KMH_PER_MPS        3.6
#define EDC_CONST_MPS_PER_KMH        (1.0 / 3.6)

#define EDC_CONST_KNOTS_PER_MPS      1.9438444924406
#define EDC_CONST_MPS_PER_KNOT       0.51444444444444

#define EDC_CONST_KMH_PER_KNOT       1.852
#define EDC_CONST_KNOTS_PER_KMH      (1.0 / 1.852)

#define EDC_CONST_KMH_PER_MPH        1.609344
#define EDC_CONST_MPH_PER_KMH        (1.0 / 1.609344)

#define EDC_CONST_PA_PER_BAR         100000.0
#define EDC_CONST_BAR_PER_PA         0.00001
#define EDC_CONST_PA_PER_PSI         6894.757293168
#define EDC_CONST_PSI_PER_PA         (1.0 / 6894.757293168)
#define EDC_CONST_PA_PER_ATM         101325.0
#define EDC_CONST_ATM_PER_PA         (1.0 / 101325.0)

/* ========================================================================== */
/*                    DISTANCE / LENGTH CONVERSIONS                           */
/* ========================================================================== */

EDC_INLINE double EDC_Unit_MeterToKm(double m)            { return m * 0.001; }
EDC_INLINE double EDC_Unit_KmToMeter(double km)           { return km * 1000.0; }
EDC_INLINE double EDC_Unit_MeterToCm(double m)            { return m * 100.0; }
EDC_INLINE double EDC_Unit_CmToMeter(double cm)           { return cm * 0.01; }
EDC_INLINE double EDC_Unit_MeterToMm(double m)            { return m * 1000.0; }
EDC_INLINE double EDC_Unit_MmToMeter(double mm)           { return mm * 0.001; }

EDC_INLINE double EDC_Unit_MeterToFeet(double m)          { return m * EDC_CONST_FEET_PER_METER; }
EDC_INLINE double EDC_Unit_FeetToMeter(double ft)         { return ft * EDC_CONST_METERS_PER_FOOT; }

EDC_INLINE double EDC_Unit_InchToCm(double in)            { return in * EDC_CONST_CM_PER_INCH; }
EDC_INLINE double EDC_Unit_CmToInch(double cm)            { return cm * EDC_CONST_INCH_PER_CM; }

EDC_INLINE double EDC_Unit_MeterToNauticalMile(double m)  { return m * EDC_CONST_NAUTICAL_MILES_PER_METER; }
EDC_INLINE double EDC_Unit_NauticalMileToMeter(double nm) { return nm * EDC_CONST_METERS_PER_NAUTICAL_MILE; }

EDC_INLINE double EDC_Unit_KmToMiles(double km)           { return km * EDC_CONST_MILES_PER_KM; }
EDC_INLINE double EDC_Unit_MilesToKm(double mi)           { return mi * EDC_CONST_KM_PER_MILE; }

/* ========================================================================== */
/*                    SPEED CONVERSIONS                                       */
/* ========================================================================== */

EDC_INLINE double EDC_Unit_MpsToKmh(double mps)           { return mps * EDC_CONST_KMH_PER_MPS; }
EDC_INLINE double EDC_Unit_KmhToMps(double kmh)           { return kmh * EDC_CONST_MPS_PER_KMH; }

EDC_INLINE double EDC_Unit_MpsToKnots(double mps)         { return mps * EDC_CONST_KNOTS_PER_MPS; }
EDC_INLINE double EDC_Unit_KnotsToMps(double kts)         { return kts * EDC_CONST_MPS_PER_KNOT; }

EDC_INLINE double EDC_Unit_KnotsToKmh(double kts)         { return kts * EDC_CONST_KMH_PER_KNOT; }
EDC_INLINE double EDC_Unit_KmhToKnots(double kmh)         { return kmh * EDC_CONST_KNOTS_PER_KMH; }

EDC_INLINE double EDC_Unit_MphToKmh(double mph)           { return mph * EDC_CONST_KMH_PER_MPH; }
EDC_INLINE double EDC_Unit_KmhToMph(double kmh)           { return kmh * EDC_CONST_MPH_PER_KMH; }

/* ========================================================================== */
/*                    TEMPERATURE CONVERSIONS                                 */
/* ========================================================================== */

EDC_INLINE double EDC_Unit_CelsiusToFahrenheit(double c)  { return (c * 1.8) + 32.0; }
EDC_INLINE double EDC_Unit_FahrenheitToCelsius(double f)  { return (f - 32.0) / 1.8; }

EDC_INLINE double EDC_Unit_CelsiusToKelvin(double c)      { return c + 273.15; }
EDC_INLINE double EDC_Unit_KelvinToCelsius(double k)      { return k - 273.15; }

EDC_INLINE double EDC_Unit_FahrenheitToKelvin(double f)   { return ((f - 32.0) / 1.8) + 273.15; }
EDC_INLINE double EDC_Unit_KelvinToFahrenheit(double k)   { return ((k - 273.15) * 1.8) + 32.0; }

/* ========================================================================== */
/*                    ANGLE CONVERSIONS                                       */
/* ========================================================================== */

EDC_INLINE double EDC_Unit_DegToRad(double deg)           { return deg * EDC_CONST_DEG_TO_RAD; }
EDC_INLINE double EDC_Unit_RadToDeg(double rad)           { return rad * EDC_CONST_RAD_TO_DEG; }

/* ========================================================================== */
/*                    PRESSURE CONVERSIONS                                    */
/* ========================================================================== */

EDC_INLINE double EDC_Unit_PaToBar(double pa)             { return pa * EDC_CONST_BAR_PER_PA; }
EDC_INLINE double EDC_Unit_BarToPa(double bar)           { return bar * EDC_CONST_PA_PER_BAR; }

EDC_INLINE double EDC_Unit_PaToPsi(double pa)             { return pa * EDC_CONST_PSI_PER_PA; }
EDC_INLINE double EDC_Unit_PsiToPa(double psi)           { return psi * EDC_CONST_PA_PER_PSI; }

EDC_INLINE double EDC_Unit_PaToHpa(double pa)             { return pa * 0.01; }
EDC_INLINE double EDC_Unit_HpaToPa(double hpa)           { return hpa * 100.0; }

EDC_INLINE double EDC_Unit_HpaToAtm(double hpa)           { return (hpa * 100.0) * EDC_CONST_ATM_PER_PA; }
EDC_INLINE double EDC_Unit_AtmToHpa(double atm)           { return (atm * EDC_CONST_PA_PER_ATM) * 0.01; }

#endif /* EDC_FEATURE_UNITS */

#ifdef __cplusplus
}
#endif

#endif /* EDC_UNITS_H */
