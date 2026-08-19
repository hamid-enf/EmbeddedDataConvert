#include "unity.h"
#include "edc_units.h"

void test_Units_Distance(void) {
    /* Meter <-> Km */
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 1.5, EDC_Unit_MeterToKm(1500.0));
    TEST_ASSERT_DOUBLE_WITHIN(0.1, 1500.0, EDC_Unit_KmToMeter(1.5));

    /* Meter <-> Feet */
    TEST_ASSERT_DOUBLE_WITHIN(0.01, 3.28084, EDC_Unit_MeterToFeet(1.0));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 1.0, EDC_Unit_FeetToMeter(3.28084));

    /* Nautical Mile */
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 1.0, EDC_Unit_MeterToNauticalMile(1852.0));
    TEST_ASSERT_DOUBLE_WITHIN(0.1, 1852.0, EDC_Unit_NauticalMileToMeter(1.0));
}

void test_Units_Speed(void) {
    /* m/s <-> km/h */
    TEST_ASSERT_DOUBLE_WITHIN(0.01, 36.0, EDC_Unit_MpsToKmh(10.0));
    TEST_ASSERT_DOUBLE_WITHIN(0.01, 10.0, EDC_Unit_KmhToMps(36.0));

    /* Knots <-> km/h */
    TEST_ASSERT_DOUBLE_WITHIN(0.01, 1.852, EDC_Unit_KnotsToKmh(1.0));
}

void test_Units_Temperature_Angle_Pressure(void) {
    /* Celsius <-> Fahrenheit */
    TEST_ASSERT_DOUBLE_WITHIN(0.01, 212.0, EDC_Unit_CelsiusToFahrenheit(100.0));
    TEST_ASSERT_DOUBLE_WITHIN(0.01, 0.0, EDC_Unit_FahrenheitToCelsius(32.0));

    /* Celsius <-> Kelvin */
    TEST_ASSERT_DOUBLE_WITHIN(0.01, 273.15, EDC_Unit_CelsiusToKelvin(0.0));

    /* Angle: Deg <-> Rad */
    TEST_ASSERT_DOUBLE_WITHIN(0.0001, EDC_CONST_PI, EDC_Unit_DegToRad(180.0));
    TEST_ASSERT_DOUBLE_WITHIN(0.0001, 180.0, EDC_Unit_RadToDeg(EDC_CONST_PI));

    /* Pressure: Pa <-> Bar */
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 1.0, EDC_Unit_PaToBar(100000.0));
    TEST_ASSERT_DOUBLE_WITHIN(0.1, 100000.0, EDC_Unit_BarToPa(1.0));
}

void run_units_tests(void) {
    RUN_TEST(test_Units_Distance);
    RUN_TEST(test_Units_Speed);
    RUN_TEST(test_Units_Temperature_Angle_Pressure);
}
