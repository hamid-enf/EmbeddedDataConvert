#include "unity.h"
#include "edc_gps.h"
#include <string.h>

void test_GPS_Coordinate_Conversions(void) {
    double deg = 0.0;
    double nmea = 0.0;
    char hemi = ' ';

    /* NMEA 3156.1234, 'N' -> 31.93539 */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_GPS_NMEA_ToDecimalDegrees(3156.1234, 'N', &deg));
    TEST_ASSERT_DOUBLE_WITHIN(0.0001, 31.93539, deg);

    /* Decimal back to NMEA */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_GPS_DecimalDegrees_ToNMEA(31.93539, 'L', &nmea, &hemi));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 3156.1234, nmea);
    TEST_ASSERT_EQUAL_HEX8('N', hemi);

    /* South hemisphere (negative) */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_GPS_NMEA_ToDecimalDegrees(3156.1234, 'S', &deg));
    TEST_ASSERT_DOUBLE_WITHIN(0.0001, -31.93539, deg);
}

void test_GPS_NMEA_Parsing(void) {
    EDC_GPS_Sentence gps;
    memset(&gps, 0, sizeof(gps));

    /* GGA Sentence: $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47 */
    const char *gga = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_GPS_ParseNMEA(gga, 0, &gps));
    TEST_ASSERT_TRUE(gps.is_valid);
    TEST_ASSERT_EQUAL_STRING("GPGGA", gps.sentence_type);
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 48.1173, gps.latitude);
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 11.5166, gps.longitude);
    TEST_ASSERT_EQUAL_UINT(8, gps.satellites);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 545.4f, gps.altitude_m);
    TEST_ASSERT_EQUAL_UINT(12, gps.time.hour);
    TEST_ASSERT_EQUAL_UINT(35, gps.time.minute);
    TEST_ASSERT_EQUAL_UINT(19, gps.time.second);

    /* RMC Sentence */
    const char *rmc = "$GPRMC,083559,A,4717.11437,N,00833.91522,E,10.5,77.52,091204,,,A*4F";
    memset(&gps, 0, sizeof(gps));
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_GPS_ParseNMEA(rmc, 0, &gps));
    TEST_ASSERT_TRUE(gps.is_valid);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 10.5f, gps.speed_knots);
    TEST_ASSERT_FLOAT_WITHIN(0.2f, 19.446f, gps.speed_kmh); /* 10.5 * 1.852 = 19.446 */
    TEST_ASSERT_EQUAL_UINT(9, gps.date.day);
    TEST_ASSERT_EQUAL_UINT(12, gps.date.month);
    TEST_ASSERT_EQUAL_UINT(2004, gps.date.year);
}

void test_GPS_Streaming_Feed(void) {
    EDC_GPS_StreamContext ctx;
    EDC_GPS_StreamInit(&ctx);
    EDC_GPS_Sentence gps;

    const char *stream_data = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n";
    size_t len = strlen(stream_data);

    bool received_sentence = false;
    for (size_t i = 0; i < len; i++) {
        EDC_Status st = EDC_GPS_StreamFeed(&ctx, (uint8_t)stream_data[i], &gps);
        if (st == EDC_OK) {
            received_sentence = true;
            break;
        }
    }
    TEST_ASSERT_TRUE(received_sentence);
    TEST_ASSERT_TRUE(gps.is_valid);
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 545.4f, gps.altitude_m);
}

void run_gps_tests(void) {
    RUN_TEST(test_GPS_Coordinate_Conversions);
    RUN_TEST(test_GPS_NMEA_Parsing);
    RUN_TEST(test_GPS_Streaming_Feed);
}
