/**
 * @file edc_unicode.c
 * @brief Complete Unicode (UTF-8, UTF-16, UTF-32) validation, transcoding, and streaming engine.
 *
 * Implements strict RFC 3629 / Unicode Standard validation:
 * - Overlong sequences rejected
 * - Surrogate codepoints (U+D800..U+DFFF) rejected
 * - Codepoints > U+10FFFF rejected
 *
 * @note MISRA-C:2012 Compliant, Zero Dynamic Allocation.
 * @author EmbeddedDataConvert Team
 */

#include "edc_unicode.h"

#if EDC_FEATURE_UNICODE

/* ========================================================================== */
/*                    UTF-8 SINGLE CHAR DECODE / ENCODE                       */
/* ========================================================================== */

EDC_Status EDC_UTF8_DecodeChar(
    const uint8_t *utf8_bytes,
    size_t max_len,
    uint32_t *out_codepoint,
    size_t *consumed_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (utf8_bytes == NULL || out_codepoint == NULL || consumed_len == NULL) {
        return EDC_ERR_NULL_PTR;
    }
    if (max_len == 0U) {
        return EDC_ERR_INCOMPLETE;
    }
#endif

    uint8_t b0 = utf8_bytes[0];

    /* 1-byte sequence (0x00 .. 0x7F) */
    if (b0 <= 0x7FU) {
        *out_codepoint = (uint32_t)b0;
        *consumed_len = 1U;
        return EDC_OK;
    }

    /* 2-byte sequence (0xC2 .. 0xDF) */
    if (b0 >= 0xC2U && b0 <= 0xDFU) {
        if (max_len < 2U) return EDC_ERR_INCOMPLETE;
        uint8_t b1 = utf8_bytes[1];
        if ((b1 & 0xC0U) != 0x80U) return EDC_ERR_INVALID_ENCODING;

        *out_codepoint = (((uint32_t)(b0 & 0x1FU)) << 6) |
                          ((uint32_t)(b1 & 0x3FU));
        *consumed_len = 2U;
        return EDC_OK;
    }

    /* 3-byte sequence (0xE0 .. 0xEF) */
    if (b0 >= 0xE0U && b0 <= 0xEFU) {
        if (max_len < 3U) return EDC_ERR_INCOMPLETE;
        uint8_t b1 = utf8_bytes[1];
        uint8_t b2 = utf8_bytes[2];

        /* Overlong and surrogate checks */
        if (b0 == 0xE0U && (b1 < 0xA0U || b1 > 0xBFU)) return EDC_ERR_INVALID_ENCODING;
        if (b0 == 0xEDU && (b1 < 0x80U || b1 > 0x9FU)) return EDC_ERR_INVALID_ENCODING; /* Surrogates U+D800..U+DFFF */
        if ((b1 & 0xC0U) != 0x80U || (b2 & 0xC0U) != 0x80U) return EDC_ERR_INVALID_ENCODING;

        *out_codepoint = (((uint32_t)(b0 & 0x0FU)) << 12) |
                         (((uint32_t)(b1 & 0x3FU)) << 6)  |
                          ((uint32_t)(b2 & 0x3FU));
        *consumed_len = 3U;
        return EDC_OK;
    }

    /* 4-byte sequence (0xF0 .. 0xF4) */
    if (b0 >= 0xF0U && b0 <= 0xF4U) {
        if (max_len < 4U) return EDC_ERR_INCOMPLETE;
        uint8_t b1 = utf8_bytes[1];
        uint8_t b2 = utf8_bytes[2];
        uint8_t b3 = utf8_bytes[3];

        if (b0 == 0xF0U && (b1 < 0x90U || b1 > 0xBFU)) return EDC_ERR_INVALID_ENCODING; /* Overlong */
        if (b0 == 0xF4U && (b1 < 0x80U || b1 > 0x8FU)) return EDC_ERR_INVALID_ENCODING; /* > U+10FFFF */
        if ((b1 & 0xC0U) != 0x80U || (b2 & 0xC0U) != 0x80U || (b3 & 0xC0U) != 0x80U) return EDC_ERR_INVALID_ENCODING;

        uint32_t cp = (((uint32_t)(b0 & 0x07U)) << 18) |
                      (((uint32_t)(b1 & 0x3FU)) << 12) |
                      (((uint32_t)(b2 & 0x3FU)) << 6)  |
                       ((uint32_t)(b3 & 0x3FU));

        if (cp > EDC_UNICODE_MAX_CODEPOINT) return EDC_ERR_INVALID_ENCODING;

        *out_codepoint = cp;
        *consumed_len = 4U;
        return EDC_OK;
    }

    /* Invalid lead byte (0x80..0xBF, 0xC0..0xC1, 0xF5..0xFF) */
    return EDC_ERR_INVALID_ENCODING;
}

EDC_Status EDC_UTF8_EncodeChar(
    uint32_t codepoint,
    uint8_t *out_buf,
    size_t buf_size,
    size_t *out_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (out_buf == NULL || out_len == NULL) {
        return EDC_ERR_NULL_PTR;
    }
    if (!EDC_Unicode_IsValidCodePoint(codepoint)) {
        return EDC_ERR_INVALID_PARAM;
    }
#endif

    if (codepoint <= 0x7FU) {
        if (buf_size < 1U) return EDC_ERR_BUFFER_TOO_SMALL;
        out_buf[0] = (uint8_t)codepoint;
        *out_len = 1U;
        return EDC_OK;
    }

    if (codepoint <= 0x7FFU) {
        if (buf_size < 2U) return EDC_ERR_BUFFER_TOO_SMALL;
        out_buf[0] = (uint8_t)(0xC0U | ((codepoint >> 6) & 0x1FU));
        out_buf[1] = (uint8_t)(0x80U | (codepoint & 0x3FU));
        *out_len = 2U;
        return EDC_OK;
    }

    if (codepoint <= 0xFFFFU) {
        if (buf_size < 3U) return EDC_ERR_BUFFER_TOO_SMALL;
        out_buf[0] = (uint8_t)(0xE0U | ((codepoint >> 12) & 0x0FU));
        out_buf[1] = (uint8_t)(0x80U | ((codepoint >> 6) & 0x3FU));
        out_buf[2] = (uint8_t)(0x80U | (codepoint & 0x3FU));
        *out_len = 3U;
        return EDC_OK;
    }

    if (codepoint <= EDC_UNICODE_MAX_CODEPOINT) {
        if (buf_size < 4U) return EDC_ERR_BUFFER_TOO_SMALL;
        out_buf[0] = (uint8_t)(0xF0U | ((codepoint >> 18) & 0x07U));
        out_buf[1] = (uint8_t)(0x80U | ((codepoint >> 12) & 0x3FU));
        out_buf[2] = (uint8_t)(0x80U | ((codepoint >> 6) & 0x3FU));
        out_buf[3] = (uint8_t)(0x80U | (codepoint & 0x3FU));
        *out_len = 4U;
        return EDC_OK;
    }

    return EDC_ERR_INVALID_PARAM;
}

/* ========================================================================== */
/*                    UTF-8 BUFFER VALIDATION & INSPECTION                    */
/* ========================================================================== */

EDC_Status EDC_UTF8_Validate(
    const uint8_t *utf8_bytes,
    size_t len,
    size_t *invalid_offset
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (utf8_bytes == NULL) return EDC_ERR_NULL_PTR;
#endif

    size_t i = 0;
    while (i < len) {
        uint32_t cp = 0;
        size_t consumed = 0;
        EDC_Status s = EDC_UTF8_DecodeChar(&utf8_bytes[i], len - i, &cp, &consumed);
        if (s != EDC_OK) {
            if (invalid_offset != NULL) {
                *invalid_offset = i;
            }
            return s;
        }
        i += consumed;
    }

    return EDC_OK;
}

EDC_Status EDC_UTF8_CharCount(
    const uint8_t *utf8_bytes,
    size_t len,
    size_t *out_char_count
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (utf8_bytes == NULL || out_char_count == NULL) return EDC_ERR_NULL_PTR;
#endif

    size_t count = 0;
    size_t i = 0;

    while (i < len) {
        uint32_t cp = 0;
        size_t consumed = 0;
        EDC_Status s = EDC_UTF8_DecodeChar(&utf8_bytes[i], len - i, &cp, &consumed);
        if (s != EDC_OK) {
            return s;
        }
        count++;
        i += consumed;
    }

    *out_char_count = count;
    return EDC_OK;
}

/* ========================================================================== */
/*                    UTF-8 <-> UTF-16 CONVERSIONS                            */
/* ========================================================================== */

#if EDC_FEATURE_UTF16

EDC_Status EDC_UTF8_ToUTF16(
    const uint8_t *utf8,
    size_t utf8_len,
    uint16_t *utf16,
    size_t utf16_capacity,
    bool big_endian,
    size_t *out_utf16_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (utf8 == NULL || utf16 == NULL) return EDC_ERR_NULL_PTR;
#endif

    size_t in_pos = 0;
    size_t out_pos = 0;

    while (in_pos < utf8_len) {
        uint32_t cp = 0;
        size_t consumed = 0;
        EDC_Status s = EDC_UTF8_DecodeChar(&utf8[in_pos], utf8_len - in_pos, &cp, &consumed);
        if (s != EDC_OK) {
            return s;
        }
        in_pos += consumed;

        if (cp <= 0xFFFFU) {
            if (out_pos >= utf16_capacity) return EDC_ERR_BUFFER_TOO_SMALL;
            uint16_t w = (uint16_t)cp;
            if (big_endian) {
                w = (uint16_t)(((w & 0x00FFU) << 8) | ((w & 0xFF00U) >> 8));
            }
            utf16[out_pos++] = w;
        } else {
            /* Surrogate pair */
            if (out_pos + 2U > utf16_capacity) return EDC_ERR_BUFFER_TOO_SMALL;
            uint32_t v = cp - 0x10000U;
            uint16_t high = (uint16_t)(0xD800U + (v >> 10));
            uint16_t low  = (uint16_t)(0xDC00U + (v & 0x3FFU));

            if (big_endian) {
                high = (uint16_t)(((high & 0x00FFU) << 8) | ((high & 0xFF00U) >> 8));
                low  = (uint16_t)(((low  & 0x00FFU) << 8) | ((low  & 0xFF00U) >> 8));
            }
            utf16[out_pos++] = high;
            utf16[out_pos++] = low;
        }
    }

    if (out_utf16_len != NULL) {
        *out_utf16_len = out_pos;
    }

    return EDC_OK;
}

EDC_Status EDC_UTF16_ToUTF8(
    const uint16_t *utf16,
    size_t utf16_len,
    bool big_endian,
    uint8_t *utf8,
    size_t utf8_capacity,
    size_t *out_utf8_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (utf16 == NULL || utf8 == NULL) return EDC_ERR_NULL_PTR;
#endif

    size_t in_pos = 0;
    size_t out_pos = 0;

    while (in_pos < utf16_len) {
        uint16_t w1 = utf16[in_pos++];
        if (big_endian) {
            w1 = (uint16_t)(((w1 & 0x00FFU) << 8) | ((w1 & 0xFF00U) >> 8));
        }

        uint32_t cp;
        if (EDC_Unicode_IsHighSurrogate(w1)) {
            if (in_pos >= utf16_len) return EDC_ERR_INCOMPLETE;
            uint16_t w2 = utf16[in_pos++];
            if (big_endian) {
                w2 = (uint16_t)(((w2 & 0x00FFU) << 8) | ((w2 & 0xFF00U) >> 8));
            }
            if (!EDC_Unicode_IsLowSurrogate(w2)) {
                return EDC_ERR_INVALID_ENCODING;
            }
            cp = 0x10000U + ((((uint32_t)w1 & 0x3FFU) << 10) | ((uint32_t)w2 & 0x3FFU));
        } else if (EDC_Unicode_IsLowSurrogate(w1)) {
            return EDC_ERR_INVALID_ENCODING; /* Unpaired low surrogate */
        } else {
            cp = (uint32_t)w1;
        }

        uint8_t encoded[4];
        size_t enc_len = 0;
        EDC_Status s = EDC_UTF8_EncodeChar(cp, encoded, sizeof(encoded), &enc_len);
        if (s != EDC_OK) return s;

        if (out_pos + enc_len > utf8_capacity) {
            return EDC_ERR_BUFFER_TOO_SMALL;
        }

        for (size_t i = 0; i < enc_len; i++) {
            utf8[out_pos++] = encoded[i];
        }
    }

    if (out_utf8_len != NULL) {
        *out_utf8_len = out_pos;
    }

    return EDC_OK;
}

#endif /* EDC_FEATURE_UTF16 */

/* ========================================================================== */
/*                    UTF-8 <-> UTF-32 CONVERSIONS                            */
/* ========================================================================== */

#if EDC_FEATURE_UTF32

EDC_Status EDC_UTF8_ToUTF32(
    const uint8_t *utf8,
    size_t utf8_len,
    uint32_t *utf32,
    size_t utf32_capacity,
    size_t *out_utf32_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (utf8 == NULL || utf32 == NULL) return EDC_ERR_NULL_PTR;
#endif

    size_t in_pos = 0;
    size_t out_pos = 0;

    while (in_pos < utf8_len) {
        uint32_t cp = 0;
        size_t consumed = 0;
        EDC_Status s = EDC_UTF8_DecodeChar(&utf8[in_pos], utf8_len - in_pos, &cp, &consumed);
        if (s != EDC_OK) return s;
        in_pos += consumed;

        if (out_pos >= utf32_capacity) return EDC_ERR_BUFFER_TOO_SMALL;
        utf32[out_pos++] = cp;
    }

    if (out_utf32_len != NULL) {
        *out_utf32_len = out_pos;
    }

    return EDC_OK;
}

EDC_Status EDC_UTF32_ToUTF8(
    const uint32_t *utf32,
    size_t utf32_len,
    uint8_t *utf8,
    size_t utf8_capacity,
    size_t *out_utf8_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (utf32 == NULL || utf8 == NULL) return EDC_ERR_NULL_PTR;
#endif

    size_t out_pos = 0;

    for (size_t i = 0; i < utf32_len; i++) {
        uint32_t cp = utf32[i];
        uint8_t encoded[4];
        size_t enc_len = 0;
        EDC_Status s = EDC_UTF8_EncodeChar(cp, encoded, sizeof(encoded), &enc_len);
        if (s != EDC_OK) return s;

        if (out_pos + enc_len > utf8_capacity) {
            return EDC_ERR_BUFFER_TOO_SMALL;
        }

        for (size_t j = 0; j < enc_len; j++) {
            utf8[out_pos++] = encoded[j];
        }
    }

    if (out_utf8_len != NULL) {
        *out_utf8_len = out_pos;
    }

    return EDC_OK;
}

#endif /* EDC_FEATURE_UTF32 */

/* ========================================================================== */
/*                    STREAMING UTF-8 DECODER                                 */
/* ========================================================================== */

void EDC_UTF8_StreamDecoder_Init(EDC_UTF8_StreamDecoder *decoder) {
    if (decoder != NULL) {
        decoder->codepoint = 0;
        decoder->bytes_needed = 0;
        decoder->bytes_seen = 0;
        decoder->min_codepoint = 0;
    }
}

EDC_Status EDC_UTF8_StreamDecoder_Feed(
    EDC_UTF8_StreamDecoder *decoder,
    uint8_t byte,
    uint32_t *out_codepoint
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (decoder == NULL || out_codepoint == NULL) return EDC_ERR_NULL_PTR;
#endif

    /* If we are waiting for continuation bytes */
    if (decoder->bytes_needed > 0U) {
        if ((byte & 0xC0U) != 0x80U) {
            EDC_UTF8_StreamDecoder_Init(decoder);
            return EDC_ERR_INVALID_ENCODING;
        }

        decoder->codepoint = (decoder->codepoint << 6) | (uint32_t)(byte & 0x3FU);
        decoder->bytes_needed--;
        decoder->bytes_seen++;

        if (decoder->bytes_needed == 0U) {
            /* Check for overlong encodings and surrogates */
            if (decoder->codepoint < decoder->min_codepoint ||
                EDC_Unicode_IsSurrogate(decoder->codepoint) ||
                decoder->codepoint > EDC_UNICODE_MAX_CODEPOINT) {
                EDC_UTF8_StreamDecoder_Init(decoder);
                return EDC_ERR_INVALID_ENCODING;
            }

            *out_codepoint = decoder->codepoint;
            EDC_UTF8_StreamDecoder_Init(decoder);
            return EDC_OK;
        }

        return EDC_ERR_INCOMPLETE;
    }

    /* Start of a new character */
    if (byte <= 0x7FU) {
        *out_codepoint = (uint32_t)byte;
        return EDC_OK;
    }

    if (byte >= 0xC2U && byte <= 0xDFU) {
        decoder->codepoint = (uint32_t)(byte & 0x1FU);
        decoder->bytes_needed = 1U;
        decoder->bytes_seen = 1U;
        decoder->min_codepoint = 0x80U;
        return EDC_ERR_INCOMPLETE;
    }

    if (byte >= 0xE0U && byte <= 0xEFU) {
        decoder->codepoint = (uint32_t)(byte & 0x0FU);
        decoder->bytes_needed = 2U;
        decoder->bytes_seen = 1U;
        decoder->min_codepoint = 0x800U;
        return EDC_ERR_INCOMPLETE;
    }

    if (byte >= 0xF0U && byte <= 0xF4U) {
        decoder->codepoint = (uint32_t)(byte & 0x07U);
        decoder->bytes_needed = 3U;
        decoder->bytes_seen = 1U;
        decoder->min_codepoint = 0x10000U;
        return EDC_ERR_INCOMPLETE;
    }

    return EDC_ERR_INVALID_ENCODING;
}

#endif /* EDC_FEATURE_UNICODE */
