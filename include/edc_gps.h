/**
 * @file edc_gps.h
 * @brief Embedded GPS / NMEA-0183 parser, coordinate converter, and formatting utilities.
 *
 * Designed for real-time STM32/ESP32 GPS telemetry, vehicle tracking, and GNSS receivers.
 *
 * @note MISRA-C:2012 Compliant, Zero Dynamic Allocation, Thread-Safe.
 * @author EmbeddedDataConvert Team
 */

#ifndef EDC_GPS_H
#define EDC_GPS_H

#include "edc_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#if EDC_FEATURE_GPS

/* ========================================================================== */
/*                          GPS DATA STRUCTURES                               */
/* ========================================================================== */

typedef struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t millisecond;
} EDC_GPS_Time;

typedef struct {
    uint8_t day;
    uint8_t month;
    uint16_t year;
} EDC_GPS_Date;

typedef struct {
    double       latitude;        /**< Decimal degrees (-90.0 to +90.0, South is negative) */
    double       longitude;       /**< Decimal degrees (-180.0 to +180.0, West is negative) */
    float        altitude_m;      /**< Altitude in meters above mean sea level */
    float        speed_knots;     /**< Speed over ground in knots */
    float        speed_kmh;       /**< Speed over ground in km/h */
    float        course_deg;      /**< Course over ground (track angle) in degrees (0.0 - 359.9) */
    float        hdop;            /**< Horizontal Dilution of Precision */
    uint8_t      satellites;      /**< Number of satellites in view / used */
    uint8_t      fix_quality;     /**< Fix quality: 0=invalid, 1=GPS fix, 2=DGPS, 4=RTK fixed, 5=RTK float */
    bool         is_valid;        /**< True if fix data is valid (RMC 'A' or GGA fix > 0) */
    char         sentence_type[6];/**< e.g., "GPGGA", "GNRMC", "GPGLL", "GPVTG" */
    EDC_GPS_Time time;            /**< UTC Time */
    EDC_GPS_Date date;            /**< UTC Date */
} EDC_GPS_Sentence;

/* ========================================================================== */
/*                    COORDINATE CONVERSIONS                                  */
/* ========================================================================== */

/**
 * @brief Convert NMEA DDMM.MMMM format to Decimal Degrees (e.g. 3156.1234, 'N' -> 31.93539).
 *
 * @param[in]  nmea_coord   Coordinate in NMEA DDMM.MMMM (or DDDMM.MMMM) format
 * @param[in]  hemisphere   'N', 'S', 'E', or 'W'
 * @param[out] out_deg      Pointer to receive decimal degrees
 * @return EDC_Status       EDC_OK on success, error code otherwise
 */
EDC_NODISCARD EDC_Status EDC_GPS_NMEA_ToDecimalDegrees(
    double nmea_coord,
    char hemisphere,
    double *out_deg
);

/**
 * @brief Convert Decimal Degrees to NMEA format (DDMM.MMMM or DDDMM.MMMM).
 *
 * @param[in]  deg            Decimal degrees (-180.0 to +180.0)
 * @param[in]  lat_or_lon     'L' for Latitude (DDMM), 'O' for Longitude (DDDMM)
 * @param[out] out_nmea       Pointer to receive NMEA coordinate
 * @param[out] out_hemisphere Pointer to receive 'N'/'S' or 'E'/'W'
 * @return EDC_Status         EDC_OK on success
 */
EDC_NODISCARD EDC_Status EDC_GPS_DecimalDegrees_ToNMEA(
    double deg,
    char lat_or_lon,
    double *out_nmea,
    char *out_hemisphere
);

/**
 * @brief Convert Degrees, Minutes, Seconds (DMS) to Decimal Degrees.
 */
EDC_NODISCARD EDC_Status EDC_GPS_DMSToDecimal(
    int16_t deg,
    uint8_t min,
    double sec,
    char hemisphere,
    double *out_deg
);

/**
 * @brief Convert Decimal Degrees to Degrees, Minutes, Seconds (DMS).
 */
EDC_NODISCARD EDC_Status EDC_GPS_DecimalToDMS(
    double deg,
    char lat_or_lon,
    int16_t *out_deg,
    uint8_t *out_min,
    double *out_sec,
    char *out_hemisphere
);

/**
 * @brief Format coordinate into human-readable string (e.g. "31° 56' 7.4\" N").
 */
EDC_NODISCARD EDC_Status EDC_GPS_FormatCoordinate(
    double deg,
    char lat_or_lon,
    char *buf,
    size_t buf_size,
    size_t *out_len
);

/* ========================================================================== */
/*                    NMEA SENTENCE PARSING                                   */
/* ========================================================================== */

/**
 * @brief Verify NMEA-0183 XOR checksum (*hh).
 *
 * @param[in] sentence  NMEA sentence (with or without '$', must have '*hh')
 * @param[in] len       Length in bytes
 * @return true if checksum is valid, false otherwise
 */
bool EDC_GPS_ValidateChecksum(const char *sentence, size_t len);

/**
 * @brief Parse a single complete NMEA-0183 sentence (e.g. $GPGGA, $GPRMC, $GNRMC, $GPVTG).
 *
 * @param[in]  sentence   NMEA sentence string
 * @param[in]  len        Length of string (or 0 for null-terminated)
 * @param[out] out_gps    Pointer to receive parsed data
 * @return EDC_Status     EDC_OK on success, EDC_ERR_CHECKSUM_MISMATCH, EDC_ERR_INVALID_FORMAT
 */
EDC_NODISCARD EDC_Status EDC_GPS_ParseNMEA(
    const char *sentence,
    size_t len,
    EDC_GPS_Sentence *out_gps
);

/* ========================================================================== */
/*                    STREAMING NMEA BYTE-BY-BYTE PARSER                      */
/* ========================================================================== */

#define EDC_GPS_MAX_SENTENCE_LEN  120

typedef struct {
    char    buffer[EDC_GPS_MAX_SENTENCE_LEN];
    uint8_t index;
    bool    in_sentence;
} EDC_GPS_StreamContext;

/**
 * @brief Initialize GPS streaming context.
 */
void EDC_GPS_StreamInit(EDC_GPS_StreamContext *ctx);

/**
 * @brief Feed a single incoming byte (from UART/DMA) into GPS stream decoder.
 *
 * @param[in,out] ctx          Stream context
 * @param[in]     byte         Incoming byte
 * @param[out]    out_sentence Pointer to receive parsed sentence when complete
 * @return EDC_Status          EDC_OK when a complete valid sentence was received and parsed,
 *                             EDC_ERR_INCOMPLETE when waiting for more bytes,
 *                             EDC_ERR_CHECKSUM_MISMATCH on corrupt packet.
 */
EDC_NODISCARD EDC_Status EDC_GPS_StreamFeed(
    EDC_GPS_StreamContext *ctx,
    uint8_t byte,
    EDC_GPS_Sentence *out_sentence
);

#endif /* EDC_FEATURE_GPS */

#ifdef __cplusplus
}
#endif

#endif /* EDC_GPS_H */
