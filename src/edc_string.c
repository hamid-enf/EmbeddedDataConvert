/**
 * @file edc_string.c
 * @brief Safe embedded string & hexadecimal utilities.
 *
 * @note MISRA-C:2012 Compliant, Zero Dynamic Allocation.
 * @author EmbeddedDataConvert Team
 */

#include "edc_string.h"

#if EDC_FEATURE_STRING

size_t EDC_StrLen(const char *str, size_t max_len) {
    if (str == NULL) {
        return 0U;
    }
    size_t len = 0;
    while (len < max_len && str[len] != '\0') {
        len++;
    }
    return len;
}

EDC_Status EDC_StrCopy(
    char *dst,
    size_t dst_size,
    const char *src,
    size_t max_copy,
    size_t *out_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (dst == NULL || src == NULL || dst_size == 0U) {
        return EDC_ERR_NULL_PTR;
    }
#endif

    size_t i = 0;
    while (i < max_copy && (i + 1U) < dst_size && src[i] != '\0') {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';

    if (out_len != NULL) {
        *out_len = i;
    }

    if (i == max_copy || src[i] == '\0') {
        return EDC_OK;
    }
    return EDC_ERR_BUFFER_TOO_SMALL;
}

EDC_Status EDC_StrCompare(
    const char *s1,
    size_t len1,
    const char *s2,
    size_t len2,
    int *out_cmp
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (s1 == NULL || s2 == NULL || out_cmp == NULL) {
        return EDC_ERR_NULL_PTR;
    }
#endif

    size_t l1 = (len1 == 0U) ? EDC_StrLen(s1, 4096U) : len1;
    size_t l2 = (len2 == 0U) ? EDC_StrLen(s2, 4096U) : len2;
    size_t min_len = (l1 < l2) ? l1 : l2;

    for (size_t i = 0; i < min_len; i++) {
        uint8_t c1 = (uint8_t)s1[i];
        uint8_t c2 = (uint8_t)s2[i];
        if (c1 != c2) {
            *out_cmp = (c1 < c2) ? -1 : 1;
            return EDC_OK;
        }
    }

    if (l1 < l2) {
        *out_cmp = -1;
    } else if (l1 > l2) {
        *out_cmp = 1;
    } else {
        *out_cmp = 0;
    }

    return EDC_OK;
}

EDC_Status EDC_StrCompareCase(
    const char *s1,
    size_t len1,
    const char *s2,
    size_t len2,
    int *out_cmp
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (s1 == NULL || s2 == NULL || out_cmp == NULL) {
        return EDC_ERR_NULL_PTR;
    }
#endif

    size_t l1 = (len1 == 0U) ? EDC_StrLen(s1, 4096U) : len1;
    size_t l2 = (len2 == 0U) ? EDC_StrLen(s2, 4096U) : len2;
    size_t min_len = (l1 < l2) ? l1 : l2;

    for (size_t i = 0; i < min_len; i++) {
        uint8_t c1 = (uint8_t)EDC_CharToLower(s1[i]);
        uint8_t c2 = (uint8_t)EDC_CharToLower(s2[i]);
        if (c1 != c2) {
            *out_cmp = (c1 < c2) ? -1 : 1;
            return EDC_OK;
        }
    }

    if (l1 < l2) {
        *out_cmp = -1;
    } else if (l1 > l2) {
        *out_cmp = 1;
    } else {
        *out_cmp = 0;
    }

    return EDC_OK;
}

EDC_Status EDC_StrTrim(
    const char *src,
    size_t src_len,
    const char **out_trimmed,
    size_t *out_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (src == NULL || out_trimmed == NULL || out_len == NULL) {
        return EDC_ERR_NULL_PTR;
    }
#endif

    size_t len = (src_len == 0U) ? EDC_StrLen(src, 4096U) : src_len;
    size_t start = 0;

    while (start < len && EDC_IsSpace(src[start])) {
        start++;
    }

    size_t end = len;
    while (end > start && EDC_IsSpace(src[end - 1U])) {
        end--;
    }

    *out_trimmed = &src[start];
    *out_len = end - start;
    return EDC_OK;
}

bool EDC_StrStartsWith(const char *str, size_t str_len, const char *prefix, size_t prefix_len) {
    if (str == NULL || prefix == NULL) return false;
    size_t slen = (str_len == 0U) ? EDC_StrLen(str, 4096U) : str_len;
    size_t plen = (prefix_len == 0U) ? EDC_StrLen(prefix, 4096U) : prefix_len;

    if (slen < plen) return false;
    for (size_t i = 0; i < plen; i++) {
        if (str[i] != prefix[i]) return false;
    }
    return true;
}

bool EDC_StrEndsWith(const char *str, size_t str_len, const char *suffix, size_t suffix_len) {
    if (str == NULL || suffix == NULL) return false;
    size_t slen = (str_len == 0U) ? EDC_StrLen(str, 4096U) : str_len;
    size_t elen = (suffix_len == 0U) ? EDC_StrLen(suffix, 4096U) : suffix_len;

    if (slen < elen) return false;
    size_t offset = slen - elen;
    for (size_t i = 0; i < elen; i++) {
        if (str[offset + i] != suffix[i]) return false;
    }
    return true;
}

EDC_Status EDC_StrFind(
    const char *haystack,
    size_t haystack_len,
    const char *needle,
    size_t needle_len,
    size_t *out_index
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (haystack == NULL || needle == NULL || out_index == NULL) {
        return EDC_ERR_NULL_PTR;
    }
#endif

    size_t hlen = (haystack_len == 0U) ? EDC_StrLen(haystack, 4096U) : haystack_len;
    size_t nlen = (needle_len == 0U) ? EDC_StrLen(needle, 4096U) : needle_len;

    if (nlen == 0U) {
        *out_index = 0U;
        return EDC_OK;
    }
    if (hlen < nlen) {
        return EDC_ERR_NOT_FOUND;
    }

    size_t limit = hlen - nlen;
    for (size_t i = 0; i <= limit; i++) {
        bool match = true;
        for (size_t j = 0; j < nlen; j++) {
            if (haystack[i + j] != needle[j]) {
                match = false;
                break;
            }
        }
        if (match) {
            *out_index = i;
            return EDC_OK;
        }
    }

    return EDC_ERR_NOT_FOUND;
}

EDC_Status EDC_StrToUpper(
    const char *src,
    size_t src_len,
    char *dst,
    size_t dst_size,
    size_t *out_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (src == NULL || dst == NULL || dst_size == 0U) return EDC_ERR_NULL_PTR;
#endif

    size_t len = (src_len == 0U) ? EDC_StrLen(src, 4096U) : src_len;
    if (dst_size <= len) return EDC_ERR_BUFFER_TOO_SMALL;

    for (size_t i = 0; i < len; i++) {
        dst[i] = EDC_CharToUpper(src[i]);
    }
    dst[len] = '\0';

    if (out_len != NULL) *out_len = len;
    return EDC_OK;
}

EDC_Status EDC_StrToLower(
    const char *src,
    size_t src_len,
    char *dst,
    size_t dst_size,
    size_t *out_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (src == NULL || dst == NULL || dst_size == 0U) return EDC_ERR_NULL_PTR;
#endif

    size_t len = (src_len == 0U) ? EDC_StrLen(src, 4096U) : src_len;
    if (dst_size <= len) return EDC_ERR_BUFFER_TOO_SMALL;

    for (size_t i = 0; i < len; i++) {
        dst[i] = EDC_CharToLower(src[i]);
    }
    dst[len] = '\0';

    if (out_len != NULL) *out_len = len;
    return EDC_OK;
}

EDC_Status EDC_StrToUpper_InPlace(char *str, size_t len) {
#if EDC_CONFIG_STRICT_CHECKS
    if (str == NULL) return EDC_ERR_NULL_PTR;
#endif
    size_t l = (len == 0U) ? EDC_StrLen(str, 4096U) : len;
    for (size_t i = 0; i < l; i++) {
        str[i] = EDC_CharToUpper(str[i]);
    }
    return EDC_OK;
}

EDC_Status EDC_StrToLower_InPlace(char *str, size_t len) {
#if EDC_CONFIG_STRICT_CHECKS
    if (str == NULL) return EDC_ERR_NULL_PTR;
#endif
    size_t l = (len == 0U) ? EDC_StrLen(str, 4096U) : len;
    for (size_t i = 0; i < l; i++) {
        str[i] = EDC_CharToLower(str[i]);
    }
    return EDC_OK;
}

/* ========================================================================== */
/*                    HEX / BYTE ARRAY IMPLEMENTATION                         */
/* ========================================================================== */

static const char g_hex_upper[] = "0123456789ABCDEF";
static const char g_hex_lower[] = "0123456789abcdef";

EDC_Status EDC_ByteToHex(uint8_t byte, char out_hex[2], bool uppercase) {
#if EDC_CONFIG_STRICT_CHECKS
    if (out_hex == NULL) return EDC_ERR_NULL_PTR;
#endif
    const char *lut = uppercase ? g_hex_upper : g_hex_lower;
    out_hex[0] = lut[(byte >> 4) & 0x0FU];
    out_hex[1] = lut[byte & 0x0FU];
    return EDC_OK;
}

EDC_Status EDC_HexToByte(const char hex[2], uint8_t *out_byte) {
#if EDC_CONFIG_STRICT_CHECKS
    if (hex == NULL || out_byte == NULL) return EDC_ERR_NULL_PTR;
#endif

    uint8_t high = 0;
    uint8_t low = 0;

    char h = hex[0];
    char l = hex[1];

    if (h >= '0' && h <= '9') high = (uint8_t)(h - '0');
    else if (h >= 'a' && h <= 'f') high = (uint8_t)(h - 'a' + 10);
    else if (h >= 'A' && h <= 'F') high = (uint8_t)(h - 'A' + 10);
    else return EDC_ERR_INVALID_FORMAT;

    if (l >= '0' && l <= '9') low = (uint8_t)(l - '0');
    else if (l >= 'a' && l <= 'f') low = (uint8_t)(l - 'a' + 10);
    else if (l >= 'A' && l <= 'F') low = (uint8_t)(l - 'A' + 10);
    else return EDC_ERR_INVALID_FORMAT;

    *out_byte = (uint8_t)((high << 4) | low);
    return EDC_OK;
}

EDC_Status EDC_BytesToHex(
    const uint8_t *bytes,
    size_t byte_count,
    char *hex_buf,
    size_t hex_buf_size,
    bool uppercase,
    bool space_separated,
    size_t *out_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (bytes == NULL || hex_buf == NULL || hex_buf_size == 0U) {
        return EDC_ERR_NULL_PTR;
    }
#endif

    if (byte_count == 0U) {
        hex_buf[0] = '\0';
        if (out_len) *out_len = 0U;
        return EDC_OK;
    }

    size_t multiplier = space_separated ? 3U : 2U;
    size_t needed = (byte_count * multiplier) - (space_separated ? 1U : 0U) + 1U;

    if (hex_buf_size < needed) {
        return EDC_ERR_BUFFER_TOO_SMALL;
    }

    const char *lut = uppercase ? g_hex_upper : g_hex_lower;
    size_t pos = 0;

    for (size_t i = 0; i < byte_count; i++) {
        uint8_t b = bytes[i];
        hex_buf[pos++] = lut[(b >> 4) & 0x0FU];
        hex_buf[pos++] = lut[b & 0x0FU];
        if (space_separated && (i + 1U < byte_count)) {
            hex_buf[pos++] = ' ';
        }
    }
    hex_buf[pos] = '\0';

    if (out_len != NULL) {
        *out_len = pos;
    }

    return EDC_OK;
}

EDC_Status EDC_HexToBytes(
    const char *hex_str,
    size_t hex_len,
    uint8_t *bytes,
    size_t max_bytes,
    size_t *out_byte_count
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (hex_str == NULL || bytes == NULL) {
        return EDC_ERR_NULL_PTR;
    }
#endif

    size_t hlen = (hex_len == 0U) ? EDC_StrLen(hex_str, 8192U) : hex_len;
    size_t byte_pos = 0;
    size_t i = 0;

    while (i < hlen && hex_str[i] != '\0') {
        /* Skip whitespace, colons, or 0x prefixes */
        if (EDC_IsSpace(hex_str[i]) || hex_str[i] == ':' || hex_str[i] == ',') {
            i++;
            continue;
        }
        if (hex_str[i] == '0' && (i + 1U < hlen) && (hex_str[i + 1U] == 'x' || hex_str[i + 1U] == 'X')) {
            i += 2U;
            continue;
        }

        if (i + 1U >= hlen || hex_str[i + 1U] == '\0') {
            return EDC_ERR_INVALID_FORMAT; /* Incomplete hex pair */
        }

        if (byte_pos >= max_bytes) {
            return EDC_ERR_BUFFER_TOO_SMALL;
        }

        uint8_t b = 0;
        EDC_Status s = EDC_HexToByte(&hex_str[i], &b);
        if (s != EDC_OK) {
            return s;
        }

        bytes[byte_pos++] = b;
        i += 2U;
    }

    if (out_byte_count != NULL) {
        *out_byte_count = byte_pos;
    }

    return EDC_OK;
}

#endif /* EDC_FEATURE_STRING */
