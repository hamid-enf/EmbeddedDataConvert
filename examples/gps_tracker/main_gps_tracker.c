/**
 * @file main_gps_tracker.c
 * @brief Complete GPS Tracking & Telematics Gateway application example using EDC.
 *
 * Demonstrates:
 * 1. NMEA-0183 multi-sentence decoding ($GPGGA, $GPRMC, $GPVTG).
 * 2. Decimal Degree & DMS coordinate conversion and formatting.
 * 3. Knot to km/h speed conversions.
 * 4. Persian SMS / Server packet formatting.
 *
 * @author EmbeddedDataConvert Team
 */

#include "edc.h"
#include <stdio.h>
#include <string.h>

int main(void) {
    printf("===================================================================\n");
    printf(" Vehicle Tracking Unit & GNSS Telemetry Example (EDC)\n");
    printf("===================================================================\n\n");

    const char *nmea_sentences[] = {
        "$GPGGA,092750.000,3541.4025,N,05125.2045,E,1,08,1.03,1185.3,M,0.0,M,,*61",
        "$GPRMC,092750.000,A,3541.4025,N,05125.2045,E,45.2,185.4,190826,,,A*58"
    };

    EDC_GPS_Sentence gps;
    memset(&gps, 0, sizeof(gps));

    for (size_t i = 0; i < 2; i++) {
        EDC_Status st = EDC_GPS_ParseNMEA(nmea_sentences[i], 0, &gps);
        if (st == EDC_OK) {
            printf("[NMEA PARSE OK] %s\n", gps.sentence_type);
        }
    }

    if (gps.is_valid) {
        char lat_dms[48], lon_dms[48];
        char lat_dec[32], lon_dec[32], alt_str[16], speed_str[16];

        if (EDC_GPS_FormatCoordinate(gps.latitude, 'L', lat_dms, sizeof(lat_dms), NULL) == EDC_OK &&
            EDC_GPS_FormatCoordinate(gps.longitude, 'O', lon_dms, sizeof(lon_dms), NULL) == EDC_OK &&
            EDC_Double_ToString(gps.latitude, lat_dec, sizeof(lat_dec), 6, NULL) == EDC_OK &&
            EDC_Double_ToString(gps.longitude, lon_dec, sizeof(lon_dec), 6, NULL) == EDC_OK &&
            EDC_Float_ToString(gps.altitude_m, alt_str, sizeof(alt_str), 1, NULL) == EDC_OK &&
            EDC_Float_ToString(gps.speed_kmh, speed_str, sizeof(speed_str), 1, NULL) == EDC_OK) {

            printf("\n--- Telemetry Summary ---\n");
            printf(" Latitude     : %s (%s)\n", lat_dec, lat_dms);
            printf(" Longitude    : %s (%s)\n", lon_dec, lon_dms);
            printf(" Altitude     : %s meters (%.1f ft)\n", alt_str, EDC_Unit_MeterToFeet(gps.altitude_m));
            printf(" Speed        : %.1f knots -> %s km/h (%.1f mph)\n",
                   gps.speed_knots, speed_str, EDC_Unit_KmhToMph(gps.speed_kmh));
            printf(" Heading      : %.1f degrees\n", gps.course_deg);
            printf(" Fix Quality  : %u | Satellites: %u\n", (unsigned int)gps.fix_quality, (unsigned int)gps.satellites);
            printf(" UTC Time/Date: %02u:%02u:%02u (20%02u-%02u-%02u)\n",
                   (unsigned int)gps.time.hour, (unsigned int)gps.time.minute, (unsigned int)gps.time.second,
                   (unsigned int)(gps.date.year % 100), (unsigned int)gps.date.month, (unsigned int)gps.date.day);

            char speed_persian[32];
            if (EDC_EnglishDigitsToPersian(speed_str, 0, speed_persian, sizeof(speed_persian), NULL) == EDC_OK) {
                printf("\n--- Persian SMS Report ---\n");
                printf(" موقعیت: تهران | سرعت: %s کیلومتر بر ساعت | ماهواره‌ها: %u\n",
                       speed_persian, (unsigned int)gps.satellites);
            }
        }
    }

    printf("\n===================================================================\n");
    return 0;
}
