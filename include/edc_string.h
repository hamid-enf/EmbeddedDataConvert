/**
 * @file edc_string.h
 * @brief Embedded-safe, buffer-overflow protected string & hex utilities.
 *
 * @note MISRA-C:2012 Compliant, Zero Dynamic Allocation, Thread-Safe.
 * @author EmbeddedDataConvert Team
 */

#ifndef EDC_STRING_H
#define EDC_STRING_H

#include "edc_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#if EDC_FEATURE_STRING

/* ========================================================================== */
/*                    CHARACTER CLASSIFICATION (MISRA SAFE)                   */
/* ========================================================================== */

EDC_INLINE bool EDC_IsAscii(char c) {
    return ((uint8_t)c <= 0x7FU);
}

EDC_INLINE bool EDC_IsDigit(char c) {
    return (c >= '0' && c <= '9');
}

EDC_INLINE bool EDC_IsHexDigit(char c) {
    return ((c >= '0' && c <= '9') ||
            (c >= 'a' && c <= 'f') ||
            (c >= 'A' && c <= 'F'));
}

EDC_INLINE bool EDC_IsAlpha(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

EDC_INLINE bool EDC_IsAlphaNumeric(char c) {
    return (EDC_IsAlpha(c) || EDC_IsDigit(c));
}

EDC_INLINE bool EDC_IsSpace(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f');
}

EDC_INLINE bool EDC_IsPrintable(char c) {
    return ((uint8_t)c >= 0x20U && (uint8_t)c <= 0x7EU);
}

EDC_INLINE char EDC_CharToUpper(char c) {
    return (c >= 'a' && c <= 'z') ? (char)(c - ('a' - 'A')) : c;
}

EDC_INLINE char EDC_CharToLower(char c) {
    return (c >= 'A' && c <= 'Z') ? (char)(c + ('a' - 'A')) : c;
}

/* ========================================================================== */
/*                    STRING OPERATIONS (BOUNDED & SAFE)                      */
/* ========================================================================== */

/**
 * @brief Calculate string length up to max_len (safe against missing null terminator).
 */
size_t EDC_StrLen(const char *str, size_t max_len);

/**
 * @brief Copy string safely with bounds checking, guaranteeing null termination if size > 0.
 */
EDC_NODISCARD EDC_Status EDC_StrCopy(
    char *dst,
    size_t dst_size,
    const char *src,
    size_t max_copy,
    size_t *out_len
);

/**
 * @brief Compare two bounded strings (case-sensitive).
 *
 * @param[out] out_cmp Pointer to receive comparison result (<0, 0, >0)
 */
EDC_NODISCARD EDC_Status EDC_StrCompare(
    const char *s1,
    size_t len1,
    const char *s2,
    size_t len2,
    int *out_cmp
);

/**
 * @brief Compare two bounded strings (case-insensitive).
 */
EDC_NODISCARD EDC_Status EDC_StrCompareCase(
    const char *s1,
    size_t len1,
    const char *s2,
    size_t len2,
    int *out_cmp
);

/**
 * @brief Zero-copy trim whitespace from start and end of string.
 *
 * @param[in]  src         Input string
 * @param[in]  src_len     Length of input (or 0 for null-terminated)
 * @param[out] out_trimmed Pointer to start of trimmed slice inside src
 * @param[out] out_len     Length of trimmed slice
 */
EDC_NODISCARD EDC_Status EDC_StrTrim(
    const char *src,
    size_t src_len,
    const char **out_trimmed,
    size_t *out_len
);

/**
 * @brief Check if string starts with prefix.
 */
bool EDC_StrStartsWith(const char *str, size_t str_len, const char *prefix, size_t prefix_len);

/**
 * @brief Check if string ends with suffix.
 */
bool EDC_StrEndsWith(const char *str, size_t str_len, const char *suffix, size_t suffix_len);

/**
 * @brief Find substring within string.
 *
 * @param[out] out_index Pointer to receive 0-based offset of needle inside haystack
 */
EDC_NODISCARD EDC_Status EDC_StrFind(
    const char *haystack,
    size_t haystack_len,
    const char *needle,
    size_t needle_len,
    size_t *out_index
);

/**
 * @brief Convert string to uppercase (in-place or buffer-to-buffer).
 */
EDC_NODISCARD EDC_Status EDC_StrToUpper(
    const char *src,
    size_t src_len,
    char *dst,
    size_t dst_size,
    size_t *out_len
);

/**
 * @brief Convert string to lowercase (in-place or buffer-to-buffer).
 */
EDC_NODISCARD EDC_Status EDC_StrToLower(
    const char *src,
    size_t src_len,
    char *dst,
    size_t dst_size,
    size_t *out_len
);

/**
 * @brief In-place conversion to uppercase.
 */
EDC_NODISCARD EDC_Status EDC_StrToUpper_InPlace(char *str, size_t len);

/**
 * @brief In-place conversion to lowercase.
 */
EDC_NODISCARD EDC_Status EDC_StrToLower_InPlace(char *str, size_t len);

/* ========================================================================== */
/*                    HEXADECIMAL / BYTE ARRAY CONVERSIONS                    */
/* ========================================================================== */

/**
 * @brief Convert single byte to 2-character hex representation.
 */
EDC_NODISCARD EDC_Status EDC_ByteToHex(uint8_t byte, char out_hex[2], bool uppercase);

/**
 * @brief Convert 2-character hex string to single byte.
 */
EDC_NODISCARD EDC_Status EDC_HexToByte(const char hex[2], uint8_t *out_byte);

/**
 * @brief Convert raw byte buffer to Hex string (e.g. {0xDE, 0xAD} -> "DEAD" or "DE AD").
 *
 * @param[in]  bytes            Input byte array
 * @param[in]  byte_count       Number of bytes
 * @param[out] hex_buf          Destination string buffer
 * @param[in]  hex_buf_size     Size of destination buffer
 * @param[in]  uppercase        True for "A-F", false for "a-f"
 * @param[in]  space_separated  If true, puts space between bytes (e.g. "DE AD BE EF")
 * @param[out] out_len          Optional pointer to receive length of hex string
 */
EDC_NODISCARD EDC_Status EDC_BytesToHex(
    const uint8_t *bytes,
    size_t byte_count,
    char *hex_buf,
    size_t hex_buf_size,
    bool uppercase,
    bool space_separated,
    size_t *out_len
);

/**
 * @brief Convert Hex string to raw byte buffer (ignores spaces, colons, "0x" prefixes).
 *
 * @param[in]  hex_str          Input hex string
 * @param[in]  hex_len          Length of hex string (or 0 for null-terminated)
 * @param[out] bytes            Destination byte array
 * @param[in]  max_bytes        Capacity of destination byte array
 * @param[out] out_byte_count   Pointer to receive written byte count
 */
EDC_NODISCARD EDC_Status EDC_HexToBytes(
    const char *hex_str,
    size_t hex_len,
    uint8_t *bytes,
    size_t max_bytes,
    size_t *out_byte_count
);

#endif /* EDC_FEATURE_STRING */

#ifdef __cplusplus
}
#endif

#endif /* EDC_STRING_H */
