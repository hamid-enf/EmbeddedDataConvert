/**
 * @file edc_gps.c
 * @brief High-performance embedded GPS / NMEA-0183 parser and coordinate engine.
 *
 * @note MISRA-C:2012 Compliant, Zero Dynamic Allocation.
 * @author EmbeddedDataConvert Team
 */

#include "edc_gps.h"
#include "edc_numeric.h"
#include "edc_string.h"
#include "edc_units.h"

#if EDC_FEATURE_GPS

/* ========================================================================== */
/*                    COORDINATE CONVERSIONS                                  */
/* ========================================================================== */

EDC_Status EDC_GPS_NMEA_ToDecimalDegrees(
    double nmea_coord,
    char hemisphere,
    double *out_deg
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (out_deg == NULL) return EDC_ERR_NULL_PTR;
#endif

    if (nmea_coord < 0.0) {
        return EDC_ERR_INVALID_PARAM;
    }

    /* NMEA coordinate: DDMM.MMMM or DDDMM.MMMM */
    double raw_deg = nmea_coord / 100.0;
    int32_t whole_deg = (int32_t)raw_deg;
    double minutes = nmea_coord - ((double)whole_deg * 100.0);

    double deg = (double)whole_deg + (minutes / 60.0);

    if (hemisphere == 'S' || hemisphere == 's' ||
        hemisphere == 'W' || hemisphere == 'w') {
        deg = -deg;
    } else if (hemisphere != 'N' && hemisphere != 'n' &&
               hemisphere != 'E' && hemisphere != 'e') {
        return EDC_ERR_INVALID_PARAM;
    }

    *out_deg = deg;
    return EDC_OK;
}

EDC_Status EDC_GPS_DecimalDegrees_ToNMEA(
    double deg,
    char lat_or_lon,
    double *out_nmea,
    char *out_hemisphere
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (out_nmea == NULL || out_hemisphere == NULL) return EDC_ERR_NULL_PTR;
#endif

    bool is_lat = (lat_or_lon == 'L' || lat_or_lon == 'l');
    bool is_neg = (deg < 0.0);
    double abs_deg = is_neg ? -deg : deg;

    if (is_lat) {
        *out_hemisphere = is_neg ? 'S' : 'N';
    } else {
        *out_hemisphere = is_neg ? 'W' : 'E';
    }

    int32_t whole_deg = (int32_t)abs_deg;
    double minutes = (abs_deg - (double)whole_deg) * 60.0;

    *out_nmea = ((double)whole_deg * 100.0) + minutes;
    return EDC_OK;
}

EDC_Status EDC_GPS_DMSToDecimal(
    int16_t deg,
    uint8_t min,
    double sec,
    char hemisphere,
    double *out_deg
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (out_deg == NULL) return EDC_ERR_NULL_PTR;
    if (min >= 60U || sec < 0.0 || sec >= 60.0) return EDC_ERR_INVALID_PARAM;
#endif

    double d = (double)deg + ((double)min / 60.0) + (sec / 3600.0);
    if (hemisphere == 'S' || hemisphere == 's' ||
        hemisphere == 'W' || hemisphere == 'w') {
        d = -d;
    }
    *out_deg = d;
    return EDC_OK;
}

EDC_Status EDC_GPS_DecimalToDMS(
    double deg,
    char lat_or_lon,
    int16_t *out_deg,
    uint8_t *out_min,
    double *out_sec,
    char *out_hemisphere
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (out_deg == NULL || out_min == NULL || out_sec == NULL || out_hemisphere == NULL) {
        return EDC_ERR_NULL_PTR;
    }
#endif

    bool is_lat = (lat_or_lon == 'L' || lat_or_lon == 'l');
    bool is_neg = (deg < 0.0);
    double abs_deg = is_neg ? -deg : deg;

    if (is_lat) {
        *out_hemisphere = is_neg ? 'S' : 'N';
    } else {
        *out_hemisphere = is_neg ? 'W' : 'E';
    }

    int16_t d = (int16_t)abs_deg;
    double rem_min = (abs_deg - (double)d) * 60.0;
    uint8_t m = (uint8_t)rem_min;
    double s = (rem_min - (double)m) * 60.0;

    *out_deg = d;
    *out_min = m;
    *out_sec = s;
    return EDC_OK;
}

EDC_Status EDC_GPS_FormatCoordinate(
    double deg,
    char lat_or_lon,
    char *buf,
    size_t buf_size,
    size_t *out_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (buf == NULL || buf_size == 0U) return EDC_ERR_NULL_PTR;
#endif

    int16_t d = 0;
    uint8_t m = 0;
    double s = 0.0;
    char hemi = 'N';

    EDC_Status st = EDC_GPS_DecimalToDMS(deg, lat_or_lon, &d, &m, &s, &hemi);
    if (st != EDC_OK) return st;

    char d_buf[8], m_buf[8], s_buf[16];
    EDC_Status s1 = EDC_I16_ToString(d, d_buf, sizeof(d_buf), 10);
    EDC_Status s2 = EDC_U64_ToStringEx((uint64_t)m, m_buf, sizeof(m_buf), 10, true, 2, NULL);
    EDC_Status s3 = EDC_Double_ToString(s, s_buf, sizeof(s_buf), 2, NULL);
    if (s1 != EDC_OK || s2 != EDC_OK || s3 != EDC_OK) {
        return EDC_ERR_INVALID_PARAM;
    }

    size_t pos = 0;
    for (size_t i = 0; d_buf[i] != '\0' && pos + 1U < buf_size; i++) buf[pos++] = d_buf[i];
    if (pos + 2U < buf_size) { buf[pos++] = (char)0xC2; buf[pos++] = (char)0xB0; } /* Degree symbol ° */
    if (pos + 1U < buf_size) buf[pos++] = ' ';
    for (size_t i = 0; m_buf[i] != '\0' && pos + 1U < buf_size; i++) buf[pos++] = m_buf[i];
    if (pos + 1U < buf_size) buf[pos++] = '\'';
    if (pos + 1U < buf_size) buf[pos++] = ' ';
    for (size_t i = 0; s_buf[i] != '\0' && pos + 1U < buf_size; i++) buf[pos++] = s_buf[i];
    if (pos + 1U < buf_size) buf[pos++] = '\"';
    if (pos + 1U < buf_size) buf[pos++] = ' ';
    if (pos + 1U < buf_size) buf[pos++] = hemi;
    buf[pos] = '\0';

    if (out_len != NULL) *out_len = pos;
    return EDC_OK;
}

/* ========================================================================== */
/*                    NMEA CHECKSUM & SENTENCE PARSING                        */
/* ========================================================================== */

bool EDC_GPS_ValidateChecksum(const char *sentence, size_t len) {
    if (sentence == NULL || len < 4U) return false;

    size_t start = 0;
    if (sentence[0] == '$') {
        start = 1;
    }

    uint8_t calculated_xor = 0;
    size_t star_idx = 0;

    for (size_t i = start; i < len && sentence[i] != '\0' && sentence[i] != '\r' && sentence[i] != '\n'; i++) {
        if (sentence[i] == '*') {
            star_idx = i;
            break;
        }
        calculated_xor ^= (uint8_t)sentence[i];
    }

    if (star_idx == 0 || (star_idx + 2U >= len)) {
        return false;
    }

    uint8_t expected_xor = 0;
    if (EDC_HexToByte(&sentence[star_idx + 1U], &expected_xor) != EDC_OK) {
        return false;
    }

    return (calculated_xor == expected_xor);
}

/* Helper to get next comma-separated field in NMEA sentence */
static const char* GetNMEAField(const char *cursor, const char *end, size_t *field_len) {
    if (cursor >= end || *cursor == '\0' || *cursor == '*' || *cursor == '\r' || *cursor == '\n') {
        *field_len = 0;
        return NULL;
    }
    const char *start = cursor;
    while (cursor < end && *cursor != ',' && *cursor != '*' && *cursor != '\r' && *cursor != '\n' && *cursor != '\0') {
        cursor++;
    }
    *field_len = (size_t)(cursor - start);
    return start;
}

static const char* SkipNMEAField(const char *cursor, const char *end) {
    while (cursor < end && *cursor != ',' && *cursor != '*' && *cursor != '\r' && *cursor != '\n' && *cursor != '\0') {
        cursor++;
    }
    if (cursor < end && *cursor == ',') {
        cursor++;
    }
    return cursor;
}

static void ParseUTCTime(const char *field, size_t len, EDC_GPS_Time *time) {
    if (field == NULL || len < 6U) return;
    time->hour   = (uint8_t)((field[0] - '0') * 10 + (field[1] - '0'));
    time->minute = (uint8_t)((field[2] - '0') * 10 + (field[3] - '0'));
    time->second = (uint8_t)((field[4] - '0') * 10 + (field[5] - '0'));
    time->millisecond = 0;
    if (len > 7U && field[6] == '.') {
        double frac = 0.0;
        EDC_Status s = EDC_StringToDouble(&field[6], len - 6U, &frac, NULL);
        (void)s;
        time->millisecond = (uint16_t)(frac * 1000.0);
    }
}

static void ParseUTCDate(const char *field, size_t len, EDC_GPS_Date *date) {
    if (field == NULL || len < 6U) return;
    date->day   = (uint8_t)((field[0] - '0') * 10 + (field[1] - '0'));
    date->month = (uint8_t)((field[2] - '0') * 10 + (field[3] - '0'));
    date->year  = (uint16_t)(2000U + (uint16_t)((field[4] - '0') * 10 + (field[5] - '0')));
}

EDC_Status EDC_GPS_ParseNMEA(
    const char *sentence,
    size_t len,
    EDC_GPS_Sentence *out_gps
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (sentence == NULL || out_gps == NULL) return EDC_ERR_NULL_PTR;
#endif

    size_t slen = (len == 0U) ? EDC_StrLen(sentence, 256U) : len;
    if (slen < 6U) return EDC_ERR_INVALID_FORMAT;

    /* Validate checksum if asterisk is present */
    size_t star_idx = 0;
    if (EDC_StrFind(sentence, slen, "*", 1, &star_idx) == EDC_OK) {
        if (!EDC_GPS_ValidateChecksum(sentence, slen)) {
            return EDC_ERR_CHECKSUM_MISMATCH;
        }
    }

    const char *cursor = sentence;
    const char *end = sentence + slen;

    if (*cursor == '$') {
        cursor++;
    }

    /* Extract 5-character sentence identifier (e.g. GPGGA, GNRMC) */
    size_t talker_len = 0;
    const char *talker = GetNMEAField(cursor, end, &talker_len);
    if (talker == NULL || talker_len < 3U || talker_len > 5U) {
        return EDC_ERR_INVALID_FORMAT;
    }

    for (size_t i = 0; i < talker_len && i < 5U; i++) {
        out_gps->sentence_type[i] = talker[i];
    }
    out_gps->sentence_type[talker_len] = '\0';

    cursor = SkipNMEAField(cursor, end);

    /* Match sentence format */
    bool is_gga = EDC_StrEndsWith(out_gps->sentence_type, talker_len, "GGA", 3);
    bool is_rmc = EDC_StrEndsWith(out_gps->sentence_type, talker_len, "RMC", 3);
    bool is_gll = EDC_StrEndsWith(out_gps->sentence_type, talker_len, "GLL", 3);
    bool is_vtg = EDC_StrEndsWith(out_gps->sentence_type, talker_len, "VTG", 3);

    EDC_Status st;
    (void)st;

    if (is_gga) {
        /* $GPGGA,time,lat,N/S,lon,E/W,fix,sats,hdop,alt,M,... */
        size_t flen = 0;
        const char *f = NULL;

        /* Field 1: Time */
        f = GetNMEAField(cursor, end, &flen);
        if (f && flen > 0) ParseUTCTime(f, flen, &out_gps->time);
        cursor = SkipNMEAField(cursor, end);

        /* Field 2: Latitude */
        double raw_lat = 0.0;
        f = GetNMEAField(cursor, end, &flen);
        if (f && flen > 0) { st = EDC_StringToDouble(f, flen, &raw_lat, NULL); (void)st; }
        cursor = SkipNMEAField(cursor, end);

        /* Field 3: N/S */
        char lat_hemi = 'N';
        f = GetNMEAField(cursor, end, &flen);
        if (f && flen > 0) lat_hemi = f[0];
        cursor = SkipNMEAField(cursor, end);
        if (raw_lat > 0.0) { st = EDC_GPS_NMEA_ToDecimalDegrees(raw_lat, lat_hemi, &out_gps->latitude); (void)st; }

        /* Field 4: Longitude */
        double raw_lon = 0.0;
        f = GetNMEAField(cursor, end, &flen);
        if (f && flen > 0) { st = EDC_StringToDouble(f, flen, &raw_lon, NULL); (void)st; }
        cursor = SkipNMEAField(cursor, end);

        /* Field 5: E/W */
        char lon_hemi = 'E';
        f = GetNMEAField(cursor, end, &flen);
        if (f && flen > 0) lon_hemi = f[0];
        cursor = SkipNMEAField(cursor, end);
        if (raw_lon > 0.0) { st = EDC_GPS_NMEA_ToDecimalDegrees(raw_lon, lon_hemi, &out_gps->longitude); (void)st; }

        /* Field 6: Fix Quality */
        uint32_t fix_q = 0;
        f = GetNMEAField(cursor, end, &flen);
        if (f && flen > 0) { st = EDC_StringToU32(f, flen, 10, &fix_q, NULL); (void)st; }
        out_gps->fix_quality = (uint8_t)fix_q;
        out_gps->is_valid = (fix_q > 0U);
        cursor = SkipNMEAField(cursor, end);

        /* Field 7: Satellites */
        uint32_t sats = 0;
        f = GetNMEAField(cursor, end, &flen);
        if (f && flen > 0) { st = EDC_StringToU32(f, flen, 10, &sats, NULL); (void)st; }
        out_gps->satellites = (uint8_t)sats;
        cursor = SkipNMEAField(cursor, end);

        /* Field 8: HDOP */
        float hdop = 0.0f;
        f = GetNMEAField(cursor, end, &flen);
        if (f && flen > 0) { st = EDC_StringToFloat(f, flen, &hdop, NULL); (void)st; }
        out_gps->hdop = hdop;
        cursor = SkipNMEAField(cursor, end);

        /* Field 9: Altitude */
        float alt = 0.0f;
        f = GetNMEAField(cursor, end, &flen);
        if (f && flen > 0) { st = EDC_StringToFloat(f, flen, &alt, NULL); (void)st; }
        out_gps->altitude_m = alt;

        return EDC_OK;
    }

    if (is_rmc) {
        /* $GPRMC,time,status,lat,N/S,lon,E/W,speed_knots,course,date,... */
        size_t flen = 0;
        const char *f = NULL;

        /* Field 1: Time */
        f = GetNMEAField(cursor, end, &flen);
        if (f && flen > 0) ParseUTCTime(f, flen, &out_gps->time);
        cursor = SkipNMEAField(cursor, end);

        /* Field 2: Status ('A'=Valid, 'V'=Warning) */
        f = GetNMEAField(cursor, end, &flen);
        if (f && flen > 0) out_gps->is_valid = (f[0] == 'A');
        cursor = SkipNMEAField(cursor, end);

        /* Field 3: Latitude */
        double raw_lat = 0.0;
        f = GetNMEAField(cursor, end, &flen);
        if (f && flen > 0) { st = EDC_StringToDouble(f, flen, &raw_lat, NULL); (void)st; }
        cursor = SkipNMEAField(cursor, end);

        /* Field 4: N/S */
        char lat_hemi = 'N';
        f = GetNMEAField(cursor, end, &flen);
        if (f && flen > 0) lat_hemi = f[0];
        cursor = SkipNMEAField(cursor, end);
        if (raw_lat > 0.0) { st = EDC_GPS_NMEA_ToDecimalDegrees(raw_lat, lat_hemi, &out_gps->latitude); (void)st; }

        /* Field 5: Longitude */
        double raw_lon = 0.0;
        f = GetNMEAField(cursor, end, &flen);
        if (f && flen > 0) { st = EDC_StringToDouble(f, flen, &raw_lon, NULL); (void)st; }
        cursor = SkipNMEAField(cursor, end);

        /* Field 6: E/W */
        char lon_hemi = 'E';
        f = GetNMEAField(cursor, end, &flen);
        if (f && flen > 0) lon_hemi = f[0];
        cursor = SkipNMEAField(cursor, end);
        if (raw_lon > 0.0) { st = EDC_GPS_NMEA_ToDecimalDegrees(raw_lon, lon_hemi, &out_gps->longitude); (void)st; }

        /* Field 7: Speed in knots */
        float spd_kts = 0.0f;
        f = GetNMEAField(cursor, end, &flen);
        if (f && flen > 0) { st = EDC_StringToFloat(f, flen, &spd_kts, NULL); (void)st; }
        out_gps->speed_knots = spd_kts;
        out_gps->speed_kmh = (float)EDC_Unit_KnotsToKmh((double)spd_kts);
        cursor = SkipNMEAField(cursor, end);

        /* Field 8: Course / Heading */
        float course = 0.0f;
        f = GetNMEAField(cursor, end, &flen);
        if (f && flen > 0) { st = EDC_StringToFloat(f, flen, &course, NULL); (void)st; }
        out_gps->course_deg = course;
        cursor = SkipNMEAField(cursor, end);

        /* Field 9: Date */
        f = GetNMEAField(cursor, end, &flen);
        if (f && flen > 0) ParseUTCDate(f, flen, &out_gps->date);

        return EDC_OK;
    }

    if (is_vtg) {
        /* $GPVTG,course,T,course_mag,M,speed_kts,N,speed_kmh,K,... */
        size_t flen = 0;
        const char *f = NULL;

        /* Field 1: Track made good (degrees true) */
        float course = 0.0f;
        f = GetNMEAField(cursor, end, &flen);
        if (f && flen > 0) { st = EDC_StringToFloat(f, flen, &course, NULL); (void)st; }
        out_gps->course_deg = course;
        cursor = SkipNMEAField(cursor, end);
        cursor = SkipNMEAField(cursor, end); /* Skip 'T' */
        cursor = SkipNMEAField(cursor, end); /* Skip course mag */
        cursor = SkipNMEAField(cursor, end); /* Skip 'M' */

        /* Field 5: Speed knots */
        float kts = 0.0f;
        f = GetNMEAField(cursor, end, &flen);
        if (f && flen > 0) { st = EDC_StringToFloat(f, flen, &kts, NULL); (void)st; }
        out_gps->speed_knots = kts;
        cursor = SkipNMEAField(cursor, end);
        cursor = SkipNMEAField(cursor, end); /* Skip 'N' */

        /* Field 7: Speed km/h */
        float kmh = 0.0f;
        f = GetNMEAField(cursor, end, &flen);
        if (f && flen > 0) { st = EDC_StringToFloat(f, flen, &kmh, NULL); (void)st; }
        out_gps->speed_kmh = kmh;
        out_gps->is_valid = true;

        return EDC_OK;
    }

    if (is_gll) {
        /* $GPGLL,lat,N/S,lon,E/W,time,status,... */
        size_t flen = 0;
        const char *f = NULL;

        double raw_lat = 0.0;
        f = GetNMEAField(cursor, end, &flen);
        if (f && flen > 0) { st = EDC_StringToDouble(f, flen, &raw_lat, NULL); (void)st; }
        cursor = SkipNMEAField(cursor, end);

        char lat_hemi = 'N';
        f = GetNMEAField(cursor, end, &flen);
        if (f && flen > 0) lat_hemi = f[0];
        cursor = SkipNMEAField(cursor, end);
        if (raw_lat > 0.0) { st = EDC_GPS_NMEA_ToDecimalDegrees(raw_lat, lat_hemi, &out_gps->latitude); (void)st; }

        double raw_lon = 0.0;
        f = GetNMEAField(cursor, end, &flen);
        if (f && flen > 0) { st = EDC_StringToDouble(f, flen, &raw_lon, NULL); (void)st; }
        cursor = SkipNMEAField(cursor, end);

        char lon_hemi = 'E';
        f = GetNMEAField(cursor, end, &flen);
        if (f && flen > 0) lon_hemi = f[0];
        cursor = SkipNMEAField(cursor, end);
        if (raw_lon > 0.0) { st = EDC_GPS_NMEA_ToDecimalDegrees(raw_lon, lon_hemi, &out_gps->longitude); (void)st; }

        f = GetNMEAField(cursor, end, &flen);
        if (f && flen > 0) ParseUTCTime(f, flen, &out_gps->time);
        cursor = SkipNMEAField(cursor, end);

        f = GetNMEAField(cursor, end, &flen);
        if (f && flen > 0) out_gps->is_valid = (f[0] == 'A');

        return EDC_OK;
    }

    return EDC_ERR_UNSUPPORTED;
}

/* ========================================================================== */
/*                    STREAMING NMEA DECODER                                  */
/* ========================================================================== */

void EDC_GPS_StreamInit(EDC_GPS_StreamContext *ctx) {
    if (ctx != NULL) {
        ctx->index = 0;
        ctx->in_sentence = false;
        ctx->buffer[0] = '\0';
    }
}

EDC_Status EDC_GPS_StreamFeed(
    EDC_GPS_StreamContext *ctx,
    uint8_t byte,
    EDC_GPS_Sentence *out_sentence
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (ctx == NULL || out_sentence == NULL) return EDC_ERR_NULL_PTR;
#endif

    char c = (char)byte;

    if (c == '$') {
        ctx->index = 0;
        ctx->buffer[ctx->index++] = c;
        ctx->in_sentence = true;
        return EDC_ERR_INCOMPLETE;
    }

    if (!ctx->in_sentence) {
        return EDC_ERR_INCOMPLETE;
    }

    if (c == '\n' || c == '\r') {
        if (ctx->index > 5U) {
            ctx->buffer[ctx->index] = '\0';
            ctx->in_sentence = false;
            EDC_Status s = EDC_GPS_ParseNMEA(ctx->buffer, ctx->index, out_sentence);
            ctx->index = 0;
            return s;
        }
        ctx->index = 0;
        ctx->in_sentence = false;
        return EDC_ERR_INCOMPLETE;
    }

    if (ctx->index < (sizeof(ctx->buffer) - 1U)) {
        ctx->buffer[ctx->index++] = c;
    } else {
        /* Sentence exceeded buffer capacity */
        ctx->index = 0;
        ctx->in_sentence = false;
        return EDC_ERR_BUFFER_TOO_SMALL;
    }

    return EDC_ERR_INCOMPLETE;
}

#endif /* EDC_FEATURE_GPS */
