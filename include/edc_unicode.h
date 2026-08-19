/**
 * @file edc_unicode.h
 * @brief Complete Unicode (UTF-8, UTF-16, UTF-32, CodePoint) subsystem for embedded systems.
 *
 * Implements strict RFC 3629 / Unicode 15.0 compliance:
 * - Rejection of overlong sequences
 * - Rejection of UTF-16 surrogates (U+D800..U+DFFF) in UTF-8/UTF-32
 * - Rejection of codepoints > U+10FFFF
 * - Streaming decoder state machine for byte-by-byte UART/SPI reception.
 *
 * @note MISRA-C:2012 Compliant, Zero Dynamic Allocation, Thread-Safe.
 * @author EmbeddedDataConvert Team
 */

#ifndef EDC_UNICODE_H
#define EDC_UNICODE_H

#include "edc_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#if EDC_FEATURE_UNICODE

/* Unicode Constants */
#define EDC_UNICODE_REPLACEMENT_CHAR    0x0000FFFDUL
#define EDC_UNICODE_MAX_CODEPOINT       0x0010FFFFUL
#define EDC_UNICODE_SURROGATE_MIN       0x0000D800UL
#define EDC_UNICODE_SURROGATE_MAX       0x0000DFFFUL
#define EDC_UNICODE_HIGH_SURROGATE_MIN  0x0000D800UL
#define EDC_UNICODE_HIGH_SURROGATE_MAX  0x0000DBFFUL
#define EDC_UNICODE_LOW_SURROGATE_MIN   0x0000DC00UL
#define EDC_UNICODE_LOW_SURROGATE_MAX   0x0000DFFFUL

/* ========================================================================== */
/*                    CODEPOINT VALIDATION & ENCODING                         */
/* ========================================================================== */

EDC_INLINE bool EDC_Unicode_IsValidCodePoint(uint32_t cp) {
    return (cp <= EDC_UNICODE_MAX_CODEPOINT) &&
           !(cp >= EDC_UNICODE_SURROGATE_MIN && cp <= EDC_UNICODE_SURROGATE_MAX);
}

EDC_INLINE bool EDC_Unicode_IsSurrogate(uint32_t cp) {
    return (cp >= EDC_UNICODE_SURROGATE_MIN && cp <= EDC_UNICODE_SURROGATE_MAX);
}

EDC_INLINE bool EDC_Unicode_IsHighSurrogate(uint16_t u16) {
    return (u16 >= 0xD800U && u16 <= 0xDBFFU);
}

EDC_INLINE bool EDC_Unicode_IsLowSurrogate(uint16_t u16) {
    return (u16 >= 0xDC00U && u16 <= 0xDFFFU);
}

/* ========================================================================== */
/*                    UTF-8 SINGLE CHARACTER DECODE / ENCODE                  */
/* ========================================================================== */

/**
 * @brief Decode a single UTF-8 codepoint from byte buffer.
 *
 * @param[in]  utf8_bytes    Pointer to UTF-8 byte sequence
 * @param[in]  max_len       Maximum readable bytes in utf8_bytes
 * @param[out] out_codepoint Pointer to receive decoded 32-bit Unicode Code Point
 * @param[out] consumed_len  Number of bytes consumed (1 to 4)
 * @return EDC_Status        EDC_OK on success, EDC_ERR_INVALID_ENCODING, EDC_ERR_INCOMPLETE
 */
EDC_NODISCARD EDC_Status EDC_UTF8_DecodeChar(
    const uint8_t *utf8_bytes,
    size_t max_len,
    uint32_t *out_codepoint,
    size_t *consumed_len
);

/**
 * @brief Encode a single Unicode Code Point into UTF-8 byte sequence (1 to 4 bytes).
 *
 * @param[in]  codepoint     Unicode code point (U+0000 .. U+10FFFF)
 * @param[out] out_buf       Destination buffer (at least 4 bytes recommended)
 * @param[in]  buf_size      Size of destination buffer
 * @param[out] out_len       Number of bytes written (1 to 4)
 * @return EDC_Status        EDC_OK on success, EDC_ERR_INVALID_PARAM, EDC_ERR_BUFFER_TOO_SMALL
 */
EDC_NODISCARD EDC_Status EDC_UTF8_EncodeChar(
    uint32_t codepoint,
    uint8_t *out_buf,
    size_t buf_size,
    size_t *out_len
);

/* ========================================================================== */
/*                    UTF-8 BUFFER VALIDATION & INSPECTION                    */
/* ========================================================================== */

/**
 * @brief Validate entire UTF-8 buffer according to RFC 3629.
 *
 * @param[in]  utf8_bytes     Pointer to UTF-8 buffer
 * @param[in]  len            Length in bytes
 * @param[out] invalid_offset Optional pointer to receive byte offset where error occurred
 * @return EDC_Status         EDC_OK if 100% valid UTF-8, EDC_ERR_INVALID_ENCODING otherwise
 */
EDC_NODISCARD EDC_Status EDC_UTF8_Validate(
    const uint8_t *utf8_bytes,
    size_t len,
    size_t *invalid_offset
);

/**
 * @brief Count Unicode characters (codepoints) in a valid UTF-8 string.
 */
EDC_NODISCARD EDC_Status EDC_UTF8_CharCount(
    const uint8_t *utf8_bytes,
    size_t len,
    size_t *out_char_count
);

/* ========================================================================== */
/*                    UTF-8 <-> UTF-16 CONVERSIONS                            */
/* ========================================================================== */

#if EDC_FEATURE_UTF16

/**
 * @brief Convert UTF-8 buffer to UTF-16 array.
 *
 * @param[in]  utf8            Input UTF-8 bytes
 * @param[in]  utf8_len        Length of UTF-8 input in bytes
 * @param[out] utf16           Destination UTF-16 buffer (array of uint16_t)
 * @param[in]  utf16_capacity  Capacity of destination buffer in uint16_t elements
 * @param[in]  big_endian      True for UTF-16BE byte order, false for native / LE
 * @param[out] out_utf16_len   Number of uint16_t elements written
 * @return EDC_Status          EDC_OK on success, error code otherwise
 */
EDC_NODISCARD EDC_Status EDC_UTF8_ToUTF16(
    const uint8_t *utf8,
    size_t utf8_len,
    uint16_t *utf16,
    size_t utf16_capacity,
    bool big_endian,
    size_t *out_utf16_len
);

/**
 * @brief Convert UTF-16 array to UTF-8 buffer.
 *
 * @param[in]  utf16           Input UTF-16 buffer
 * @param[in]  utf16_len       Number of uint16_t elements in input
 * @param[in]  big_endian      True if input is UTF-16BE, false for native / LE
 * @param[out] utf8            Destination UTF-8 buffer
 * @param[in]  utf8_capacity   Capacity of destination buffer in bytes
 * @param[out] out_utf8_len    Number of UTF-8 bytes written
 * @return EDC_Status          EDC_OK on success, error code otherwise
 */
EDC_NODISCARD EDC_Status EDC_UTF16_ToUTF8(
    const uint16_t *utf16,
    size_t utf16_len,
    bool big_endian,
    uint8_t *utf8,
    size_t utf8_capacity,
    size_t *out_utf8_len
);

#endif /* EDC_FEATURE_UTF16 */

/* ========================================================================== */
/*                    UTF-8 <-> UTF-32 CONVERSIONS                            */
/* ========================================================================== */

#if EDC_FEATURE_UTF32

/**
 * @brief Convert UTF-8 buffer to UTF-32 (Unicode Codepoint array).
 */
EDC_NODISCARD EDC_Status EDC_UTF8_ToUTF32(
    const uint8_t *utf8,
    size_t utf8_len,
    uint32_t *utf32,
    size_t utf32_capacity,
    size_t *out_utf32_len
);

/**
 * @brief Convert UTF-32 array to UTF-8 buffer.
 */
EDC_NODISCARD EDC_Status EDC_UTF32_ToUTF8(
    const uint32_t *utf32,
    size_t utf32_len,
    uint8_t *utf8,
    size_t utf8_capacity,
    size_t *out_utf8_len
);

#endif /* EDC_FEATURE_UTF32 */

/* ========================================================================== */
/*                    STREAMING UTF-8 DECODER STATE MACHINE                   */
/* ========================================================================== */

typedef struct {
    uint32_t codepoint;     /**< Partially decoded codepoint */
    uint8_t  bytes_needed;  /**< Number of continuation bytes expected (0 = idle/ready) */
    uint8_t  bytes_seen;    /**< Number of bytes consumed for current codepoint */
    uint32_t min_codepoint; /**< Minimum allowed codepoint for overlong detection */
} EDC_UTF8_StreamDecoder;

/**
 * @brief Initialize or reset a streaming UTF-8 decoder state.
 */
void EDC_UTF8_StreamDecoder_Init(EDC_UTF8_StreamDecoder *decoder);

/**
 * @brief Feed a single byte to the streaming UTF-8 decoder.
 *
 * @param[in,out] decoder       Pointer to decoder state
 * @param[in]     byte          Next incoming byte
 * @param[out]    out_codepoint Pointer to receive completed codepoint when ready
 * @return EDC_Status           EDC_OK when a full character is decoded (*out_codepoint is valid),
 *                              EDC_ERR_INCOMPLETE when more bytes are needed,
 *                              EDC_ERR_INVALID_ENCODING on invalid byte.
 */
EDC_NODISCARD EDC_Status EDC_UTF8_StreamDecoder_Feed(
    EDC_UTF8_StreamDecoder *decoder,
    uint8_t byte,
    uint32_t *out_codepoint
);

#endif /* EDC_FEATURE_UNICODE */

#ifdef __cplusplus
}
#endif

#endif /* EDC_UNICODE_H */
