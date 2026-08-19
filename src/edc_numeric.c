/**
 * @file edc_numeric.c
 * @brief High-performance numeric conversion, formatting, parsing, and Q-fixed-point.
 *
 * @note MISRA-C:2012 Compliant, Zero Dynamic Allocation.
 * @author EmbeddedDataConvert Team
 */

#include "edc_numeric.h"
#include "edc_string.h"

#if EDC_FEATURE_NUMERIC

/* Lookup table for bases up to 36 */
static const char g_digits_upper[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const char g_digits_lower[] = "0123456789abcdefghijklmnopqrstuvwxyz";

/* ========================================================================== */
/*                    INTEGER TO STRING IMPLEMENTATION                        */
/* ========================================================================== */

EDC_Status EDC_U64_ToStringEx(
    uint64_t val,
    char *buf,
    size_t buf_size,
    uint8_t base,
    bool uppercase,
    size_t pad_zeros,
    size_t *out_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (buf == NULL || buf_size == 0U) {
        return EDC_ERR_NULL_PTR;
    }
    if (base < 2U || base > 36U) {
        return EDC_ERR_UNSUPPORTED;
    }
#endif

    const char *digits = uppercase ? g_digits_upper : g_digits_lower;
    char temp[66]; /* Max 64 binary digits + padding safety */
    size_t pos = 0;

    /* Generate digits in reverse order */
    if (val == 0U) {
        temp[pos] = '0';
        pos++;
    } else {
        uint64_t v = val;
        while (v > 0U) {
            uint64_t rem = v % (uint64_t)base;
            temp[pos] = digits[rem];
            pos++;
            v /= (uint64_t)base;
        }
    }

    /* Apply zero padding if requested */
    while (pos < pad_zeros && pos < (sizeof(temp) - 1U)) {
        temp[pos] = '0';
        pos++;
    }

    /* Check if destination buffer has enough room for string + null terminator */
    if (buf_size <= pos) {
        return EDC_ERR_BUFFER_TOO_SMALL;
    }

    /* Reverse into destination buffer */
    for (size_t i = 0; i < pos; i++) {
        buf[i] = temp[pos - 1U - i];
    }
    buf[pos] = '\0';

    if (out_len != NULL) {
        *out_len = pos;
    }

    return EDC_OK;
}

EDC_Status EDC_I64_ToStringEx(
    int64_t val,
    char *buf,
    size_t buf_size,
    uint8_t base,
    bool uppercase,
    size_t pad_zeros,
    size_t *out_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (buf == NULL || buf_size == 0U) {
        return EDC_ERR_NULL_PTR;
    }
    if (base < 2U || base > 36U) {
        return EDC_ERR_UNSUPPORTED;
    }
#endif

    bool is_negative = (val < 0);
    uint64_t uval;

    if (is_negative) {
        /* Safe negation handling INT64_MIN without overflow */
        uval = (uint64_t)(-(val + 1)) + 1U;
    } else {
        uval = (uint64_t)val;
    }

    const char *digits = uppercase ? g_digits_upper : g_digits_lower;
    char temp[66];
    size_t pos = 0;

    if (uval == 0U) {
        temp[pos] = '0';
        pos++;
    } else {
        uint64_t v = uval;
        while (v > 0U) {
            uint64_t rem = v % (uint64_t)base;
            temp[pos] = digits[rem];
            pos++;
            v /= (uint64_t)base;
        }
    }

    while (pos < pad_zeros && pos < (sizeof(temp) - 1U)) {
        temp[pos] = '0';
        pos++;
    }

    size_t total_needed = pos + (is_negative ? 1U : 0U);
    if (buf_size <= total_needed) {
        return EDC_ERR_BUFFER_TOO_SMALL;
    }

    size_t dest_idx = 0;
    if (is_negative) {
        buf[dest_idx] = '-';
        dest_idx++;
    }

    for (size_t i = 0; i < pos; i++) {
        buf[dest_idx + i] = temp[pos - 1U - i];
    }
    buf[total_needed] = '\0';

    if (out_len != NULL) {
        *out_len = total_needed;
    }

    return EDC_OK;
}

EDC_Status EDC_U64_ToString(uint64_t val, char *buf, size_t buf_size, uint8_t base) {
    return EDC_U64_ToStringEx(val, buf, buf_size, base, true, 0, NULL);
}

EDC_Status EDC_I64_ToString(int64_t val, char *buf, size_t buf_size, uint8_t base) {
    return EDC_I64_ToStringEx(val, buf, buf_size, base, true, 0, NULL);
}

EDC_Status EDC_U32_ToString(uint32_t val, char *buf, size_t buf_size, uint8_t base) {
    return EDC_U64_ToStringEx((uint64_t)val, buf, buf_size, base, true, 0, NULL);
}

EDC_Status EDC_I32_ToString(int32_t val, char *buf, size_t buf_size, uint8_t base) {
    return EDC_I64_ToStringEx((int64_t)val, buf, buf_size, base, true, 0, NULL);
}

EDC_Status EDC_U16_ToString(uint16_t val, char *buf, size_t buf_size, uint8_t base) {
    return EDC_U64_ToStringEx((uint64_t)val, buf, buf_size, base, true, 0, NULL);
}

EDC_Status EDC_I16_ToString(int16_t val, char *buf, size_t buf_size, uint8_t base) {
    return EDC_I64_ToStringEx((int64_t)val, buf, buf_size, base, true, 0, NULL);
}

EDC_Status EDC_U8_ToString(uint8_t val, char *buf, size_t buf_size, uint8_t base) {
    return EDC_U64_ToStringEx((uint64_t)val, buf, buf_size, base, true, 0, NULL);
}

EDC_Status EDC_I8_ToString(int8_t val, char *buf, size_t buf_size, uint8_t base) {
    return EDC_I64_ToStringEx((int64_t)val, buf, buf_size, base, true, 0, NULL);
}

EDC_Status EDC_ToDecimal(int64_t val, char *buf, size_t buf_size) {
    return EDC_I64_ToStringEx(val, buf, buf_size, 10, true, 0, NULL);
}

EDC_Status EDC_ToHex(uint64_t val, char *buf, size_t buf_size, bool uppercase) {
    return EDC_U64_ToStringEx(val, buf, buf_size, 16, uppercase, 0, NULL);
}

EDC_Status EDC_ToHexPadded(uint64_t val, char *buf, size_t buf_size, size_t pad_zeros, bool uppercase) {
    return EDC_U64_ToStringEx(val, buf, buf_size, 16, uppercase, pad_zeros, NULL);
}

EDC_Status EDC_ToBinary(uint64_t val, char *buf, size_t buf_size) {
    return EDC_U64_ToStringEx(val, buf, buf_size, 2, true, 0, NULL);
}

EDC_Status EDC_ToOctal(uint64_t val, char *buf, size_t buf_size) {
    return EDC_U64_ToStringEx(val, buf, buf_size, 8, true, 0, NULL);
}

/* ========================================================================== */
/*                    STRING TO INTEGER IMPLEMENTATION                        */
/* ========================================================================== */

static int8_t CharToDigit(char c, uint8_t base) {
    uint8_t d;
    if (c >= '0' && c <= '9') {
        d = (uint8_t)(c - '0');
    } else if (c >= 'a' && c <= 'z') {
        d = (uint8_t)(c - 'a' + 10);
    } else if (c >= 'A' && c <= 'Z') {
        d = (uint8_t)(c - 'A' + 10);
    } else {
        return -1;
    }
    return (d < base) ? (int8_t)d : (int8_t)-1;
}

EDC_Status EDC_StringToU64(
    const char *str,
    size_t len,
    uint8_t base,
    uint64_t *out_val,
    size_t *consumed_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (str == NULL || out_val == NULL) {
        return EDC_ERR_NULL_PTR;
    }
#endif

    size_t max_len = (len == 0U) ? 64U : len;
    size_t idx = 0;

    /* Skip leading whitespace */
    while (idx < max_len && str[idx] != '\0' && (str[idx] == ' ' || str[idx] == '\t' || str[idx] == '\r' || str[idx] == '\n')) {
        idx++;
    }

    if (idx >= max_len || str[idx] == '\0') {
        return EDC_ERR_INVALID_FORMAT;
    }

    /* Optional positive sign */
    if (str[idx] == '+') {
        idx++;
    }

    /* Auto-detect base if 0 */
    uint8_t effective_base = base;
    if (effective_base == 0U) {
        if (idx + 1U < max_len && str[idx] == '0') {
            char next = str[idx + 1U];
            if (next == 'x' || next == 'X') {
                effective_base = 16;
                idx += 2U;
            } else if (next == 'b' || next == 'B') {
                effective_base = 2;
                idx += 2U;
            } else {
                effective_base = 10;
            }
        } else {
            effective_base = 10;
        }
    } else if (effective_base == 16U) {
        if (idx + 1U < max_len && str[idx] == '0' && (str[idx + 1U] == 'x' || str[idx + 1U] == 'X')) {
            idx += 2U;
        }
    } else if (effective_base == 2U) {
        if (idx + 1U < max_len && str[idx] == '0' && (str[idx + 1U] == 'b' || str[idx + 1U] == 'B')) {
            idx += 2U;
        }
    }

    if (effective_base < 2U || effective_base > 36U) {
        return EDC_ERR_UNSUPPORTED;
    }

    uint64_t result = 0U;
    bool digits_found = false;
    uint64_t max_before_mult = UINT64_MAX / (uint64_t)effective_base;

    while (idx < max_len && str[idx] != '\0') {
        int8_t digit = CharToDigit(str[idx], effective_base);
        if (digit < 0) {
            break; /* Non-digit terminates integer parsing */
        }
        digits_found = true;

        /* Check for overflow before multiply */
        if (result > max_before_mult) {
            return EDC_ERR_OVERFLOW;
        }
        result *= (uint64_t)effective_base;

        /* Check for overflow before addition */
        if (result > (UINT64_MAX - (uint64_t)digit)) {
            return EDC_ERR_OVERFLOW;
        }
        result += (uint64_t)digit;
        idx++;
    }

    if (!digits_found) {
        return EDC_ERR_INVALID_FORMAT;
    }

    *out_val = result;
    if (consumed_len != NULL) {
        *consumed_len = idx;
    }

    return EDC_OK;
}

EDC_Status EDC_StringToI64(
    const char *str,
    size_t len,
    uint8_t base,
    int64_t *out_val,
    size_t *consumed_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (str == NULL || out_val == NULL) {
        return EDC_ERR_NULL_PTR;
    }
#endif

    size_t max_len = (len == 0U) ? 64U : len;
    size_t idx = 0;

    /* Skip leading whitespace */
    while (idx < max_len && str[idx] != '\0' && (str[idx] == ' ' || str[idx] == '\t' || str[idx] == '\r' || str[idx] == '\n')) {
        idx++;
    }

    if (idx >= max_len || str[idx] == '\0') {
        return EDC_ERR_INVALID_FORMAT;
    }

    bool is_negative = false;
    if (str[idx] == '-') {
        is_negative = true;
        idx++;
    } else if (str[idx] == '+') {
        idx++;
    }

    uint8_t effective_base = base;
    if (effective_base == 0U) {
        if (idx + 1U < max_len && str[idx] == '0') {
            char next = str[idx + 1U];
            if (next == 'x' || next == 'X') {
                effective_base = 16;
                idx += 2U;
            } else if (next == 'b' || next == 'B') {
                effective_base = 2;
                idx += 2U;
            } else {
                effective_base = 10;
            }
        } else {
            effective_base = 10;
        }
    } else if (effective_base == 16U) {
        if (idx + 1U < max_len && str[idx] == '0' && (str[idx + 1U] == 'x' || str[idx + 1U] == 'X')) {
            idx += 2U;
        }
    } else if (effective_base == 2U) {
        if (idx + 1U < max_len && str[idx] == '0' && (str[idx + 1U] == 'b' || str[idx + 1U] == 'B')) {
            idx += 2U;
        }
    }

    if (effective_base < 2U || effective_base > 36U) {
        return EDC_ERR_UNSUPPORTED;
    }

    uint64_t limit = is_negative ? (0x7FFFFFFFFFFFFFFFULL + 1U) : 0x7FFFFFFFFFFFFFFFULL;
    uint64_t max_before_mult = limit / (uint64_t)effective_base;
    uint64_t result = 0U;
    bool digits_found = false;

    while (idx < max_len && str[idx] != '\0') {
        int8_t digit = CharToDigit(str[idx], effective_base);
        if (digit < 0) {
            break;
        }
        digits_found = true;

        if (result > max_before_mult) {
            return is_negative ? EDC_ERR_UNDERFLOW : EDC_ERR_OVERFLOW;
        }
        result *= (uint64_t)effective_base;

        if (result > (limit - (uint64_t)digit)) {
            return is_negative ? EDC_ERR_UNDERFLOW : EDC_ERR_OVERFLOW;
        }
        result += (uint64_t)digit;
        idx++;
    }

    if (!digits_found) {
        return EDC_ERR_INVALID_FORMAT;
    }

    if (is_negative) {
        if (result == 0x8000000000000000ULL) {
            *out_val = (int64_t)(-9223372036854775807LL - 1LL);
        } else {
            *out_val = -(int64_t)result;
        }
    } else {
        *out_val = (int64_t)result;
    }

    if (consumed_len != NULL) {
        *consumed_len = idx;
    }

    return EDC_OK;
}

EDC_Status EDC_StringToU32(const char *str, size_t len, uint8_t base, uint32_t *out_val, size_t *consumed_len) {
#if EDC_CONFIG_STRICT_CHECKS
    if (out_val == NULL) return EDC_ERR_NULL_PTR;
#endif
    uint64_t v;
    EDC_Status s = EDC_StringToU64(str, len, base, &v, consumed_len);
    if (s != EDC_OK) return s;
    if (v > UINT32_MAX) return EDC_ERR_OVERFLOW;
    *out_val = (uint32_t)v;
    return EDC_OK;
}

EDC_Status EDC_StringToI32(const char *str, size_t len, uint8_t base, int32_t *out_val, size_t *consumed_len) {
#if EDC_CONFIG_STRICT_CHECKS
    if (out_val == NULL) return EDC_ERR_NULL_PTR;
#endif
    int64_t v;
    EDC_Status s = EDC_StringToI64(str, len, base, &v, consumed_len);
    if (s != EDC_OK) return s;
    if (v > INT32_MAX) return EDC_ERR_OVERFLOW;
    if (v < INT32_MIN) return EDC_ERR_UNDERFLOW;
    *out_val = (int32_t)v;
    return EDC_OK;
}

EDC_Status EDC_StringToU16(const char *str, size_t len, uint8_t base, uint16_t *out_val, size_t *consumed_len) {
#if EDC_CONFIG_STRICT_CHECKS
    if (out_val == NULL) return EDC_ERR_NULL_PTR;
#endif
    uint64_t v;
    EDC_Status s = EDC_StringToU64(str, len, base, &v, consumed_len);
    if (s != EDC_OK) return s;
    if (v > UINT16_MAX) return EDC_ERR_OVERFLOW;
    *out_val = (uint16_t)v;
    return EDC_OK;
}

EDC_Status EDC_StringToI16(const char *str, size_t len, uint8_t base, int16_t *out_val, size_t *consumed_len) {
#if EDC_CONFIG_STRICT_CHECKS
    if (out_val == NULL) return EDC_ERR_NULL_PTR;
#endif
    int64_t v;
    EDC_Status s = EDC_StringToI64(str, len, base, &v, consumed_len);
    if (s != EDC_OK) return s;
    if (v > INT16_MAX) return EDC_ERR_OVERFLOW;
    if (v < INT16_MIN) return EDC_ERR_UNDERFLOW;
    *out_val = (int16_t)v;
    return EDC_OK;
}

EDC_Status EDC_StringToU8(const char *str, size_t len, uint8_t base, uint8_t *out_val, size_t *consumed_len) {
#if EDC_CONFIG_STRICT_CHECKS
    if (out_val == NULL) return EDC_ERR_NULL_PTR;
#endif
    uint64_t v;
    EDC_Status s = EDC_StringToU64(str, len, base, &v, consumed_len);
    if (s != EDC_OK) return s;
    if (v > UINT8_MAX) return EDC_ERR_OVERFLOW;
    *out_val = (uint8_t)v;
    return EDC_OK;
}

EDC_Status EDC_StringToI8(const char *str, size_t len, uint8_t base, int8_t *out_val, size_t *consumed_len) {
#if EDC_CONFIG_STRICT_CHECKS
    if (out_val == NULL) return EDC_ERR_NULL_PTR;
#endif
    int64_t v;
    EDC_Status s = EDC_StringToI64(str, len, base, &v, consumed_len);
    if (s != EDC_OK) return s;
    if (v > INT8_MAX) return EDC_ERR_OVERFLOW;
    if (v < INT8_MIN) return EDC_ERR_UNDERFLOW;
    *out_val = (int8_t)v;
    return EDC_OK;
}

/* ========================================================================== */
/*                    FLOATING POINT IMPLEMENTATION                           */
/* ========================================================================== */

#if EDC_FEATURE_FLOAT

EDC_Status EDC_Double_ToString(
    double val,
    char *buf,
    size_t buf_size,
    uint8_t decimals,
    size_t *out_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (buf == NULL || buf_size == 0U) {
        return EDC_ERR_NULL_PTR;
    }
#endif

    /* Check for NaN */
    if (val != val) {
        if (buf_size < 4U) return EDC_ERR_BUFFER_TOO_SMALL;
        buf[0] = 'n'; buf[1] = 'a'; buf[2] = 'n'; buf[3] = '\0';
        if (out_len) *out_len = 3U;
        return EDC_OK;
    }

    /* Check for Infinity */
    if (val > 1.7976931348623157e+308) {
        if (buf_size < 4U) return EDC_ERR_BUFFER_TOO_SMALL;
        buf[0] = 'i'; buf[1] = 'n'; buf[2] = 'f'; buf[3] = '\0';
        if (out_len) *out_len = 3U;
        return EDC_OK;
    }
    if (val < -1.7976931348623157e+308) {
        if (buf_size < 5U) return EDC_ERR_BUFFER_TOO_SMALL;
        buf[0] = '-'; buf[1] = 'i'; buf[2] = 'n'; buf[3] = 'f'; buf[4] = '\0';
        if (out_len) *out_len = 4U;
        return EDC_OK;
    }

    uint8_t prec = (decimals > EDC_CONFIG_MAX_FLOAT_PRECISION) ?
                   EDC_CONFIG_MAX_FLOAT_PRECISION : decimals;

    /* Handle sign bit */
    bool is_negative = false;
    double d = val;
    if (d < 0.0) {
        is_negative = true;
        d = -d;
    }

    /* Rounding */
    double rounder = 0.5;
    for (uint8_t i = 0; i < prec; i++) {
        rounder /= 10.0;
    }
    d += rounder;

    uint64_t int_part = (uint64_t)d;
    double frac_part = d - (double)int_part;

    size_t pos = 0;

    /* Integer part conversion */
    char int_buf[32];
    size_t int_len = 0;
    EDC_Status s = EDC_U64_ToStringEx(int_part, int_buf, sizeof(int_buf), 10, true, 0, &int_len);
    if (s != EDC_OK) return s;

    size_t total_needed = (is_negative ? 1U : 0U) + int_len + (prec > 0U ? (1U + prec) : 0U) + 1U;
    if (buf_size < total_needed) {
        return EDC_ERR_BUFFER_TOO_SMALL;
    }

    if (is_negative) {
        buf[pos++] = '-';
    }

    for (size_t i = 0; i < int_len; i++) {
        buf[pos++] = int_buf[i];
    }

    if (prec > 0U) {
        buf[pos++] = '.';
        for (uint8_t i = 0; i < prec; i++) {
            frac_part *= 10.0;
            uint8_t digit = (uint8_t)frac_part;
            if (digit > 9U) digit = 9U;
            buf[pos++] = (char)('0' + digit);
            frac_part -= (double)digit;
        }
    }

    buf[pos] = '\0';
    if (out_len != NULL) {
        *out_len = pos;
    }

    return EDC_OK;
}

EDC_Status EDC_Float_ToString(
    float val,
    char *buf,
    size_t buf_size,
    uint8_t decimals,
    size_t *out_len
) {
    return EDC_Double_ToString((double)val, buf, buf_size, decimals, out_len);
}

EDC_Status EDC_Double_ToScientific(
    double val,
    char *buf,
    size_t buf_size,
    uint8_t decimals,
    size_t *out_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (buf == NULL || buf_size == 0U) {
        return EDC_ERR_NULL_PTR;
    }
#endif

    if (val != val) {
        if (buf_size < 4U) return EDC_ERR_BUFFER_TOO_SMALL;
        buf[0] = 'n'; buf[1] = 'a'; buf[2] = 'n'; buf[3] = '\0';
        if (out_len) *out_len = 3U;
        return EDC_OK;
    }

    bool is_negative = false;
    double d = val;
    if (d < 0.0) {
        is_negative = true;
        d = -d;
    }

    int16_t exponent = 0;
    if (d > 0.0) {
        while (d >= 10.0) {
            d /= 10.0;
            exponent++;
        }
        while (d < 1.0 && d > 0.0) {
            d *= 10.0;
            exponent--;
        }
    }

    char mantissa_buf[32];
    size_t mantissa_len = 0;
    EDC_Status s = EDC_Double_ToString(d, mantissa_buf, sizeof(mantissa_buf), decimals, &mantissa_len);
    if (s != EDC_OK) return s;

    char exp_buf[16];
    size_t exp_len = 0;
    int16_t abs_exp = (exponent < 0) ? -exponent : exponent;
    s = EDC_I64_ToStringEx((int64_t)abs_exp, exp_buf, sizeof(exp_buf), 10, true, 2, &exp_len);
    if (s != EDC_OK) return s;

    size_t total_needed = (is_negative ? 1U : 0U) + mantissa_len + 1U + 1U + exp_len + 1U;
    if (buf_size < total_needed) {
        return EDC_ERR_BUFFER_TOO_SMALL;
    }

    size_t pos = 0;
    if (is_negative) {
        buf[pos++] = '-';
    }
    for (size_t i = 0; i < mantissa_len; i++) {
        buf[pos++] = mantissa_buf[i];
    }
    buf[pos++] = 'e';
    buf[pos++] = (exponent >= 0) ? '+' : '-';
    for (size_t i = 0; i < exp_len; i++) {
        buf[pos++] = exp_buf[i];
    }
    buf[pos] = '\0';

    if (out_len != NULL) {
        *out_len = pos;
    }

    return EDC_OK;
}

EDC_Status EDC_StringToDouble(
    const char *str,
    size_t len,
    double *out_val,
    size_t *consumed_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (str == NULL || out_val == NULL) {
        return EDC_ERR_NULL_PTR;
    }
#endif

    size_t max_len = (len == 0U) ? 64U : len;
    size_t idx = 0;

    /* Skip leading whitespace */
    while (idx < max_len && str[idx] != '\0' && (str[idx] == ' ' || str[idx] == '\t' || str[idx] == '\r' || str[idx] == '\n')) {
        idx++;
    }

    if (idx >= max_len || str[idx] == '\0') {
        return EDC_ERR_INVALID_FORMAT;
    }

    /* Check for NaN */
    if ((idx + 2U < max_len) &&
        (str[idx] == 'n' || str[idx] == 'N') &&
        (str[idx+1] == 'a' || str[idx+1] == 'A') &&
        (str[idx+2] == 'n' || str[idx+2] == 'N')) {
        *out_val = 0.0 / 0.0;
        if (consumed_len) *consumed_len = idx + 3U;
        return EDC_OK;
    }

    bool is_negative = false;
    if (str[idx] == '-') {
        is_negative = true;
        idx++;
    } else if (str[idx] == '+') {
        idx++;
    }

    /* Check for Inf / Infinity */
    if ((idx + 2U < max_len) &&
        (str[idx] == 'i' || str[idx] == 'I') &&
        (str[idx+1] == 'n' || str[idx+1] == 'N') &&
        (str[idx+2] == 'f' || str[idx+2] == 'F')) {
        idx += 3U;
        if (idx + 4U < max_len &&
            (str[idx] == 'i' || str[idx] == 'I') &&
            (str[idx+1] == 'n' || str[idx+1] == 'N') &&
            (str[idx+2] == 'i' || str[idx+2] == 'I') &&
            (str[idx+3] == 't' || str[idx+3] == 'T') &&
            (str[idx+4] == 'y' || str[idx+4] == 'Y')) {
            idx += 5U;
        }
        *out_val = is_negative ? (-1.0 / 0.0) : (1.0 / 0.0);
        if (consumed_len) *consumed_len = idx;
        return EDC_OK;
    }

    double result = 0.0;
    bool digits_found = false;

    /* Integer part */
    while (idx < max_len && str[idx] >= '0' && str[idx] <= '9') {
        digits_found = true;
        result = (result * 10.0) + (double)(str[idx] - '0');
        idx++;
    }

    /* Fractional part */
    if (idx < max_len && str[idx] == '.') {
        idx++;
        double frac_divisor = 10.0;
        while (idx < max_len && str[idx] >= '0' && str[idx] <= '9') {
            digits_found = true;
            result += (double)(str[idx] - '0') / frac_divisor;
            frac_divisor *= 10.0;
            idx++;
        }
    }

    if (!digits_found) {
        return EDC_ERR_INVALID_FORMAT;
    }

    /* Exponent part (e.g. 1.23e-4 or 4.5E+2) */
    if (idx < max_len && (str[idx] == 'e' || str[idx] == 'E')) {
        idx++;
        bool exp_negative = false;
        if (idx < max_len && str[idx] == '-') {
            exp_negative = true;
            idx++;
        } else if (idx < max_len && str[idx] == '+') {
            idx++;
        }

        int32_t exponent = 0;
        bool exp_digits = false;
        while (idx < max_len && str[idx] >= '0' && str[idx] <= '9') {
            exp_digits = true;
            exponent = (exponent * 10) + (str[idx] - '0');
            idx++;
        }

        if (exp_digits) {
            double scale = 1.0;
            for (int32_t i = 0; i < exponent; i++) {
                scale *= 10.0;
            }
            if (exp_negative) {
                result /= scale;
            } else {
                result *= scale;
            }
        }
    }

    *out_val = is_negative ? -result : result;
    if (consumed_len != NULL) {
        *consumed_len = idx;
    }

    return EDC_OK;
}

EDC_Status EDC_StringToFloat(
    const char *str,
    size_t len,
    float *out_val,
    size_t *consumed_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (out_val == NULL) return EDC_ERR_NULL_PTR;
#endif
    double d;
    EDC_Status s = EDC_StringToDouble(str, len, &d, consumed_len);
    if (s != EDC_OK) return s;
    *out_val = (float)d;
    return EDC_OK;
}

#endif /* EDC_FEATURE_FLOAT */

/* ========================================================================== */
/*                    FIXED POINT IMPLEMENTATION                              */
/* ========================================================================== */

#if EDC_FEATURE_FIXED_POINT

EDC_Status EDC_Float_To_Q(float f, uint8_t frac_bits, bool saturate, int32_t *out_q) {
#if EDC_CONFIG_STRICT_CHECKS
    if (out_q == NULL) return EDC_ERR_NULL_PTR;
    if (frac_bits >= 31U) return EDC_ERR_INVALID_PARAM;
#endif

    double scale = (double)(1UL << frac_bits);
    double scaled_val = (double)f * scale;

    double max_q = (double)(0x7FFFFFFFL);
    double min_q = (double)(-0x7FFFFFFFL - 1L);

    if (scaled_val > max_q) {
        if (saturate) {
            *out_q = 0x7FFFFFFFL;
            return EDC_OK;
        }
        return EDC_ERR_OVERFLOW;
    }
    if (scaled_val < min_q) {
        if (saturate) {
            *out_q = (int32_t)(-0x7FFFFFFFL - 1L);
            return EDC_OK;
        }
        return EDC_ERR_OVERFLOW;
    }

    *out_q = (int32_t)(scaled_val + (scaled_val >= 0.0 ? 0.5 : -0.5));
    return EDC_OK;
}

EDC_Status EDC_Q_To_Float(int32_t q, uint8_t frac_bits, float *out_f) {
#if EDC_CONFIG_STRICT_CHECKS
    if (out_f == NULL) return EDC_ERR_NULL_PTR;
    if (frac_bits >= 31U) return EDC_ERR_INVALID_PARAM;
#endif

    float scale = (float)(1UL << frac_bits);
    *out_f = (float)q / scale;
    return EDC_OK;
}

EDC_Status EDC_Double_To_Q64(double d, uint8_t frac_bits, bool saturate, int64_t *out_q) {
#if EDC_CONFIG_STRICT_CHECKS
    if (out_q == NULL) return EDC_ERR_NULL_PTR;
    if (frac_bits >= 63U) return EDC_ERR_INVALID_PARAM;
#endif

    double scale = (double)(1ULL << frac_bits);
    double scaled_val = d * scale;

    double max_q = (double)(0x7FFFFFFFFFFFFFFFLL);
    double min_q = -(double)(0x7FFFFFFFFFFFFFFFLL) - 1.0;

    if (scaled_val > max_q) {
        if (saturate) {
            *out_q = 0x7FFFFFFFFFFFFFFFLL;
            return EDC_OK;
        }
        return EDC_ERR_OVERFLOW;
    }
    if (scaled_val < min_q) {
        if (saturate) {
            *out_q = -0x7FFFFFFFFFFFFFFFLL - 1LL;
            return EDC_OK;
        }
        return EDC_ERR_OVERFLOW;
    }

    *out_q = (int64_t)(scaled_val + (scaled_val >= 0.0 ? 0.5 : -0.5));
    return EDC_OK;
}

EDC_Status EDC_Q64_To_Double(int64_t q, uint8_t frac_bits, double *out_d) {
#if EDC_CONFIG_STRICT_CHECKS
    if (out_d == NULL) return EDC_ERR_NULL_PTR;
    if (frac_bits >= 63U) return EDC_ERR_INVALID_PARAM;
#endif

    double scale = (double)(1ULL << frac_bits);
    *out_d = (double)q / scale;
    return EDC_OK;
}

EDC_Status EDC_Q_ToString(
    int32_t q,
    uint8_t frac_bits,
    char *buf,
    size_t buf_size,
    uint8_t decimals,
    size_t *out_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (buf == NULL || buf_size == 0U) return EDC_ERR_NULL_PTR;
    if (frac_bits >= 31U) return EDC_ERR_INVALID_PARAM;
#endif

    bool is_neg = (q < 0);
    uint32_t uq = is_neg ? (uint32_t)(-q) : (uint32_t)q;

    uint32_t int_part = uq >> frac_bits;
    uint32_t frac_mask = (1UL << frac_bits) - 1U;
    uint32_t frac_part = uq & frac_mask;

    uint8_t prec = (decimals > 9U) ? 9U : decimals;

    char int_buf[16];
    size_t int_len = 0;
    EDC_Status s = EDC_U64_ToStringEx((uint64_t)int_part, int_buf, sizeof(int_buf), 10, true, 0, &int_len);
    if (s != EDC_OK) return s;

    size_t total_needed = (is_neg ? 1U : 0U) + int_len + (prec > 0U ? (1U + prec) : 0U) + 1U;
    if (buf_size < total_needed) return EDC_ERR_BUFFER_TOO_SMALL;

    size_t pos = 0;
    if (is_neg) {
        buf[pos++] = '-';
    }
    for (size_t i = 0; i < int_len; i++) {
        buf[pos++] = int_buf[i];
    }

    if (prec > 0U) {
        buf[pos++] = '.';
        uint64_t scaled_frac = (uint64_t)frac_part;
        for (uint8_t i = 0; i < prec; i++) {
            scaled_frac *= 10U;
            uint8_t digit = (uint8_t)(scaled_frac >> frac_bits);
            if (digit > 9U) digit = 9U;
            buf[pos++] = (char)('0' + digit);
            scaled_frac &= frac_mask;
        }
    }

    buf[pos] = '\0';
    if (out_len != NULL) {
        *out_len = pos;
    }

    return EDC_OK;
}

EDC_Status EDC_StringTo_Q(
    const char *str,
    size_t len,
    uint8_t frac_bits,
    int32_t *out_q,
    size_t *consumed_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (str == NULL || out_q == NULL) return EDC_ERR_NULL_PTR;
    if (frac_bits >= 31U) return EDC_ERR_INVALID_PARAM;
#endif

    size_t max_len = (len == 0U) ? 64U : len;
    size_t idx = 0;

    while (idx < max_len && str[idx] != '\0' && (str[idx] == ' ' || str[idx] == '\t' || str[idx] == '\r' || str[idx] == '\n')) {
        idx++;
    }

    if (idx >= max_len || str[idx] == '\0') {
        return EDC_ERR_INVALID_FORMAT;
    }

    bool is_neg = false;
    if (str[idx] == '-') {
        is_neg = true;
        idx++;
    } else if (str[idx] == '+') {
        idx++;
    }

    uint32_t int_part = 0;
    bool digits_found = false;

    while (idx < max_len && str[idx] >= '0' && str[idx] <= '9') {
        digits_found = true;
        int_part = (int_part * 10U) + (uint32_t)(str[idx] - '0');
        idx++;
    }

    uint64_t q_val = ((uint64_t)int_part) << frac_bits;

    if (idx < max_len && str[idx] == '.') {
        idx++;
        uint64_t frac_accum = 0;
        uint64_t frac_div = 1;
        while (idx < max_len && str[idx] >= '0' && str[idx] <= '9' && frac_div <= 1000000000ULL) {
            digits_found = true;
            frac_accum = (frac_accum * 10ULL) + (uint64_t)(str[idx] - '0');
            frac_div *= 10ULL;
            idx++;
        }
        if (frac_div > 1ULL) {
            uint64_t frac_q = (frac_accum * (1ULL << frac_bits) + (frac_div / 2ULL)) / frac_div;
            q_val += frac_q;
        }
    }

    if (!digits_found) {
        return EDC_ERR_INVALID_FORMAT;
    }

    if (q_val > 0x7FFFFFFFULL) {
        return is_neg ? EDC_ERR_UNDERFLOW : EDC_ERR_OVERFLOW;
    }

    *out_q = is_neg ? -(int32_t)q_val : (int32_t)q_val;
    if (consumed_len != NULL) {
        *consumed_len = idx;
    }

    return EDC_OK;
}

#endif /* EDC_FEATURE_FIXED_POINT */

/* ========================================================================== */
/*                    BOOLEAN IMPLEMENTATION                                  */
/* ========================================================================== */

EDC_Status EDC_Bool_ToString(
    bool val,
    EDC_BoolFormat fmt,
    char *buf,
    size_t buf_size,
    size_t *out_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (buf == NULL || buf_size == 0U) return EDC_ERR_NULL_PTR;
#endif

    const char *str;
    switch (fmt) {
        case EDC_BOOL_FMT_TRUE_FALSE:
            str = val ? "true" : "false";
            break;
        case EDC_BOOL_FMT_TRUE_FALSE_UPPER:
            str = val ? "TRUE" : "FALSE";
            break;
        case EDC_BOOL_FMT_ONE_ZERO:
            str = val ? "1" : "0";
            break;
        case EDC_BOOL_FMT_YES_NO:
            str = val ? "yes" : "no";
            break;
        case EDC_BOOL_FMT_YES_NO_UPPER:
            str = val ? "YES" : "NO";
            break;
        case EDC_BOOL_FMT_ON_OFF:
            str = val ? "on" : "off";
            break;
        case EDC_BOOL_FMT_ON_OFF_UPPER:
            str = val ? "ON" : "OFF";
            break;
        default:
            return EDC_ERR_INVALID_PARAM;
    }

    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }

    if (buf_size <= len) {
        return EDC_ERR_BUFFER_TOO_SMALL;
    }

    for (size_t i = 0; i < len; i++) {
        buf[i] = str[i];
    }
    buf[len] = '\0';

    if (out_len != NULL) {
        *out_len = len;
    }

    return EDC_OK;
}

EDC_Status EDC_StringToBool(
    const char *str,
    size_t len,
    bool *out_val,
    size_t *consumed_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (str == NULL || out_val == NULL) return EDC_ERR_NULL_PTR;
#endif

    size_t max_len = (len == 0U) ? 32U : len;
    size_t idx = 0;

    /* Skip leading whitespace */
    while (idx < max_len && str[idx] != '\0' && (str[idx] == ' ' || str[idx] == '\t' || str[idx] == '\r' || str[idx] == '\n')) {
        idx++;
    }

    if (idx >= max_len || str[idx] == '\0') {
        return EDC_ERR_INVALID_FORMAT;
    }

    char word[8];
    size_t w_len = 0;

    while (idx < max_len && str[idx] != '\0' && EDC_IsAlphaNumeric(str[idx]) && w_len < sizeof(word) - 1U) {
        word[w_len++] = EDC_CharToLower(str[idx++]);
    }
    word[w_len] = '\0';

    if (w_len == 0U) {
        return EDC_ERR_INVALID_FORMAT;
    }

    if ((w_len == 4U && word[0]=='t' && word[1]=='r' && word[2]=='u' && word[3]=='e') ||
        (w_len == 1U && word[0]=='1') ||
        (w_len == 3U && word[0]=='y' && word[1]=='e' && word[2]=='s') ||
        (w_len == 1U && word[0]=='y') ||
        (w_len == 1U && word[0]=='t') ||
        (w_len == 2U && word[0]=='o' && word[1]=='n')) {
        *out_val = true;
    } else if ((w_len == 5U && word[0]=='f' && word[1]=='a' && word[2]=='l' && word[3]=='s' && word[4]=='e') ||
               (w_len == 1U && word[0]=='0') ||
               (w_len == 2U && word[0]=='n' && word[1]=='o') ||
               (w_len == 1U && word[0]=='n') ||
               (w_len == 1U && word[0]=='f') ||
               (w_len == 3U && word[0]=='o' && word[1]=='f' && word[2]=='f')) {
        *out_val = false;
    } else {
        return EDC_ERR_INVALID_FORMAT;
    }

    if (consumed_len != NULL) {
        *consumed_len = idx;
    }

    return EDC_OK;
}

#endif /* EDC_FEATURE_NUMERIC */
