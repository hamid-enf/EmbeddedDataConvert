/**
 * @file edc_numeric.h
 * @brief High-performance numeric conversion, formatting, parsing, and Q-fixed-point for embedded systems.
 *
 * @note MISRA-C:2012 Compliant, Zero Dynamic Allocation, Thread-Safe / Stateless.
 * @author EmbeddedDataConvert Team
 */

#ifndef EDC_NUMERIC_H
#define EDC_NUMERIC_H

#include "edc_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#if EDC_FEATURE_NUMERIC

/* ========================================================================== */
/*                          BOOLEAN FORMAT ENUM                               */
/* ========================================================================== */

typedef enum {
    EDC_BOOL_FMT_TRUE_FALSE       = 0, /**< "true" / "false" (lowercase) */
    EDC_BOOL_FMT_TRUE_FALSE_UPPER = 1, /**< "TRUE" / "FALSE" (uppercase) */
    EDC_BOOL_FMT_ONE_ZERO         = 2, /**< "1" / "0" */
    EDC_BOOL_FMT_YES_NO           = 3, /**< "yes" / "no" */
    EDC_BOOL_FMT_YES_NO_UPPER     = 4, /**< "YES" / "NO" */
    EDC_BOOL_FMT_ON_OFF           = 5, /**< "on" / "off" */
    EDC_BOOL_FMT_ON_OFF_UPPER     = 6  /**< "ON" / "OFF" */
} EDC_BoolFormat;

/* ========================================================================== */
/*                    INTEGER TO STRING (LOW-LEVEL & HIGH-LEVEL)              */
/* ========================================================================== */

/**
 * @brief Convert uint64_t to string with specified base (2 to 36).
 *
 * @param[in]  val          Value to convert
 * @param[out] buf          Destination buffer
 * @param[in]  buf_size     Size of buffer in bytes
 * @param[in]  base         Number base (2 to 36)
 * @param[in]  uppercase    True for uppercase hex/alphabetic digits, false for lowercase
 * @param[in]  pad_zeros    Minimum number of digits (padded with leading '0's, 0 for none)
 * @param[out] out_len      Optional pointer to receive length of written string (excluding '\0')
 * @return EDC_Status       EDC_OK on success, error code otherwise
 */
EDC_NODISCARD EDC_Status EDC_U64_ToStringEx(
    uint64_t val,
    char *buf,
    size_t buf_size,
    uint8_t base,
    bool uppercase,
    size_t pad_zeros,
    size_t *out_len
);

/**
 * @brief Convert int64_t to string with specified base (2 to 36).
 *
 * @param[in]  val          Signed value to convert
 * @param[out] buf          Destination buffer
 * @param[in]  buf_size     Size of buffer in bytes
 * @param[in]  base         Number base (2 to 36)
 * @param[in]  uppercase    True for uppercase hex/alphabetic digits
 * @param[in]  pad_zeros    Minimum number of digits (padded with leading '0's)
 * @param[out] out_len      Optional pointer to receive length of written string
 * @return EDC_Status       EDC_OK on success, error code otherwise
 */
EDC_NODISCARD EDC_Status EDC_I64_ToStringEx(
    int64_t val,
    char *buf,
    size_t buf_size,
    uint8_t base,
    bool uppercase,
    size_t pad_zeros,
    size_t *out_len
);

/* High-level wrappers */
EDC_NODISCARD EDC_Status EDC_U64_ToString(uint64_t val, char *buf, size_t buf_size, uint8_t base);
EDC_NODISCARD EDC_Status EDC_I64_ToString(int64_t val, char *buf, size_t buf_size, uint8_t base);
EDC_NODISCARD EDC_Status EDC_U32_ToString(uint32_t val, char *buf, size_t buf_size, uint8_t base);
EDC_NODISCARD EDC_Status EDC_I32_ToString(int32_t val, char *buf, size_t buf_size, uint8_t base);
EDC_NODISCARD EDC_Status EDC_U16_ToString(uint16_t val, char *buf, size_t buf_size, uint8_t base);
EDC_NODISCARD EDC_Status EDC_I16_ToString(int16_t val, char *buf, size_t buf_size, uint8_t base);
EDC_NODISCARD EDC_Status EDC_U8_ToString(uint8_t val, char *buf, size_t buf_size, uint8_t base);
EDC_NODISCARD EDC_Status EDC_I8_ToString(int8_t val, char *buf, size_t buf_size, uint8_t base);

/* Direct Base Formatting Helpers */
EDC_NODISCARD EDC_Status EDC_ToDecimal(int64_t val, char *buf, size_t buf_size);
EDC_NODISCARD EDC_Status EDC_ToHex(uint64_t val, char *buf, size_t buf_size, bool uppercase);
EDC_NODISCARD EDC_Status EDC_ToHexPadded(uint64_t val, char *buf, size_t buf_size, size_t pad_zeros, bool uppercase);
EDC_NODISCARD EDC_Status EDC_ToBinary(uint64_t val, char *buf, size_t buf_size);
EDC_NODISCARD EDC_Status EDC_ToOctal(uint64_t val, char *buf, size_t buf_size);

/* ========================================================================== */
/*                    STRING TO INTEGER (SAFE PARSING)                        */
/* ========================================================================== */

/**
 * @brief Parse string to uint64_t with strict base and overflow detection.
 *
 * Supports base 0 (auto-detects "0x" for hex, "0b" for binary, "0" for octal/dec).
 *
 * @param[in]  str          Input string
 * @param[in]  len          Length of input string (or 0 to parse null-terminated)
 * @param[in]  base         Base (0 for auto-detect, or 2 to 36)
 * @param[out] out_val      Pointer to receive parsed value
 * @param[out] consumed_len Optional pointer to receive number of characters parsed
 * @return EDC_Status       EDC_OK on success, EDC_ERR_OVERFLOW, EDC_ERR_INVALID_FORMAT, etc.
 */
EDC_NODISCARD EDC_Status EDC_StringToU64(
    const char *str,
    size_t len,
    uint8_t base,
    uint64_t *out_val,
    size_t *consumed_len
);

/**
 * @brief Parse string to int64_t with sign, strict base, and overflow/underflow checks.
 *
 * @param[in]  str          Input string
 * @param[in]  len          Length of input string (or 0 to parse null-terminated)
 * @param[in]  base         Base (0 for auto-detect, or 2 to 36)
 * @param[out] out_val      Pointer to receive parsed value
 * @param[out] consumed_len Optional pointer to receive number of characters parsed
 * @return EDC_Status       EDC_OK on success, error code otherwise
 */
EDC_NODISCARD EDC_Status EDC_StringToI64(
    const char *str,
    size_t len,
    uint8_t base,
    int64_t *out_val,
    size_t *consumed_len
);

EDC_NODISCARD EDC_Status EDC_StringToU32(const char *str, size_t len, uint8_t base, uint32_t *out_val, size_t *consumed_len);
EDC_NODISCARD EDC_Status EDC_StringToI32(const char *str, size_t len, uint8_t base, int32_t *out_val, size_t *consumed_len);
EDC_NODISCARD EDC_Status EDC_StringToU16(const char *str, size_t len, uint8_t base, uint16_t *out_val, size_t *consumed_len);
EDC_NODISCARD EDC_Status EDC_StringToI16(const char *str, size_t len, uint8_t base, int16_t *out_val, size_t *consumed_len);
EDC_NODISCARD EDC_Status EDC_StringToU8(const char *str, size_t len, uint8_t base, uint8_t *out_val, size_t *consumed_len);
EDC_NODISCARD EDC_Status EDC_StringToI8(const char *str, size_t len, uint8_t base, int8_t *out_val, size_t *consumed_len);

/* ========================================================================== */
/*                    FLOATING POINT CONVERSIONS                              */
/* ========================================================================== */

#if EDC_FEATURE_FLOAT

/**
 * @brief Convert float to string.
 *
 * @param[in]  val          Float value
 * @param[out] buf          Destination buffer
 * @param[in]  buf_size     Size of destination buffer
 * @param[in]  decimals     Number of decimal places (0 to EDC_CONFIG_MAX_FLOAT_PRECISION)
 * @param[out] out_len      Optional pointer to receive length of written string
 * @return EDC_Status       EDC_OK on success, error code otherwise
 */
EDC_NODISCARD EDC_Status EDC_Float_ToString(
    float val,
    char *buf,
    size_t buf_size,
    uint8_t decimals,
    size_t *out_len
);

/**
 * @brief Convert double to string with fixed precision.
 *
 * @param[in]  val          Double value
 * @param[out] buf          Destination buffer
 * @param[in]  buf_size     Size of destination buffer
 * @param[in]  decimals     Number of decimal places (0 to EDC_CONFIG_MAX_FLOAT_PRECISION)
 * @param[out] out_len      Optional pointer to receive length of written string
 * @return EDC_Status       EDC_OK on success, error code otherwise
 */
EDC_NODISCARD EDC_Status EDC_Double_ToString(
    double val,
    char *buf,
    size_t buf_size,
    uint8_t decimals,
    size_t *out_len
);

/**
 * @brief Convert double to scientific notation string (e.g. "1.234e-05").
 *
 * @param[in]  val          Double value
 * @param[out] buf          Destination buffer
 * @param[in]  buf_size     Size of destination buffer
 * @param[in]  decimals     Number of significant decimal digits
 * @param[out] out_len      Optional pointer to receive length of written string
 * @return EDC_Status       EDC_OK on success, error code otherwise
 */
EDC_NODISCARD EDC_Status EDC_Double_ToScientific(
    double val,
    char *buf,
    size_t buf_size,
    uint8_t decimals,
    size_t *out_len
);

/**
 * @brief Parse string to float (supports standard decimal and scientific e/E notation).
 *
 * @param[in]  str          Input string
 * @param[in]  len          Length of input string (or 0 for null-terminated)
 * @param[out] out_val      Pointer to receive float result
 * @param[out] consumed_len Optional pointer to receive number of parsed characters
 * @return EDC_Status       EDC_OK on success, error code otherwise
 */
EDC_NODISCARD EDC_Status EDC_StringToFloat(
    const char *str,
    size_t len,
    float *out_val,
    size_t *consumed_len
);

/**
 * @brief Parse string to double (supports standard decimal and scientific e/E notation).
 *
 * @param[in]  str          Input string
 * @param[in]  len          Length of input string (or 0 for null-terminated)
 * @param[out] out_val      Pointer to receive double result
 * @param[out] consumed_len Optional pointer to receive number of parsed characters
 * @return EDC_Status       EDC_OK on success, error code otherwise
 */
EDC_NODISCARD EDC_Status EDC_StringToDouble(
    const char *str,
    size_t len,
    double *out_val,
    size_t *consumed_len
);

#endif /* EDC_FEATURE_FLOAT */

/* ========================================================================== */
/*                    FIXED POINT (Q FORMAT)                                  */
/* ========================================================================== */

#if EDC_FEATURE_FIXED_POINT

/**
 * @brief Convert float to Generic Q format (int32_t with `frac_bits` fractional bits).
 *
 * @param[in]  f            Float value
 * @param[in]  frac_bits    Number of fractional bits (e.g. 15 for Q16.15)
 * @param[in]  saturate     True to clamp on overflow, false to return EDC_ERR_OVERFLOW
 * @param[out] out_q        Pointer to receive Q value
 * @return EDC_Status       EDC_OK on success, EDC_ERR_OVERFLOW on out-of-range
 */
EDC_NODISCARD EDC_Status EDC_Float_To_Q(float f, uint8_t frac_bits, bool saturate, int32_t *out_q);

/**
 * @brief Convert Generic Q format (int32_t) to float.
 *
 * @param[in]  q            Q value
 * @param[in]  frac_bits    Number of fractional bits
 * @param[out] out_f        Pointer to receive float value
 * @return EDC_Status       EDC_OK on success
 */
EDC_NODISCARD EDC_Status EDC_Q_To_Float(int32_t q, uint8_t frac_bits, float *out_f);

/**
 * @brief Convert double to Q64 format (int64_t with `frac_bits` fractional bits).
 */
EDC_NODISCARD EDC_Status EDC_Double_To_Q64(double d, uint8_t frac_bits, bool saturate, int64_t *out_q);

/**
 * @brief Convert Q64 format (int64_t) to double.
 */
EDC_NODISCARD EDC_Status EDC_Q64_To_Double(int64_t q, uint8_t frac_bits, double *out_d);

/* Standard Q Formats Helper Shortcuts */
#define EDC_Float_To_Q8(f, sat, out)   EDC_Float_To_Q((f), 8, (sat), (out))
#define EDC_Q8_To_Float(q, out)        EDC_Q_To_Float((q), 8, (out))

#define EDC_Float_To_Q15(f, sat, out)  EDC_Float_To_Q((f), 15, (sat), (out))
#define EDC_Q15_To_Float(q, out)       EDC_Q_To_Float((q), 15, (out))

#define EDC_Float_To_Q31(f, sat, out)  EDC_Double_To_Q64((double)(f), 31, (sat), (out))
#define EDC_Q31_To_Float(q, out)       do { double _d; EDC_Q64_To_Double((q), 31, &_d); *(out) = (float)_d; } while(0)

/**
 * @brief Format a Q-number to string without using floating-point arithmetic!
 *
 * @param[in]  q            Q-format integer value
 * @param[in]  frac_bits    Number of fractional bits
 * @param[out] buf          Destination buffer
 * @param[in]  buf_size     Size of buffer
 * @param[in]  decimals     Number of decimal digits to output
 * @param[out] out_len      Optional pointer to receive written length
 * @return EDC_Status       EDC_OK on success
 */
EDC_NODISCARD EDC_Status EDC_Q_ToString(
    int32_t q,
    uint8_t frac_bits,
    char *buf,
    size_t buf_size,
    uint8_t decimals,
    size_t *out_len
);

/**
 * @brief Parse string to Q-format integer without float conversion.
 *
 * @param[in]  str          Input string
 * @param[in]  len          Length of string (or 0 for null-terminated)
 * @param[in]  frac_bits    Number of fractional bits
 * @param[out] out_q        Pointer to receive parsed Q integer
 * @param[out] consumed_len Optional pointer to receive parsed length
 * @return EDC_Status       EDC_OK on success
 */
EDC_NODISCARD EDC_Status EDC_StringTo_Q(
    const char *str,
    size_t len,
    uint8_t frac_bits,
    int32_t *out_q,
    size_t *consumed_len
);

#endif /* EDC_FEATURE_FIXED_POINT */

/* ========================================================================== */
/*                    BOOLEAN CONVERSIONS                                     */
/* ========================================================================== */

/**
 * @brief Convert boolean to string.
 *
 * @param[in]  val          Boolean value
 * @param[in]  fmt          Formatting style (e.g. true/false, 1/0, yes/no, on/off)
 * @param[out] buf          Destination buffer
 * @param[in]  buf_size     Size of destination buffer
 * @param[out] out_len      Optional pointer to receive length of written string
 * @return EDC_Status       EDC_OK on success
 */
EDC_NODISCARD EDC_Status EDC_Bool_ToString(
    bool val,
    EDC_BoolFormat fmt,
    char *buf,
    size_t buf_size,
    size_t *out_len
);

/**
 * @brief Parse string to boolean.
 *
 * Accepts: "true", "false", "1", "0", "yes", "no", "y", "n", "t", "f", "on", "off" (case-insensitive).
 *
 * @param[in]  str          Input string
 * @param[in]  len          Length of string (or 0 for null-terminated)
 * @param[out] out_val      Pointer to receive boolean value
 * @param[out] consumed_len Optional pointer to receive parsed character count
 * @return EDC_Status       EDC_OK on success, EDC_ERR_INVALID_FORMAT otherwise
 */
EDC_NODISCARD EDC_Status EDC_StringToBool(
    const char *str,
    size_t len,
    bool *out_val,
    size_t *consumed_len
);

#endif /* EDC_FEATURE_NUMERIC */

#ifdef __cplusplus
}
#endif

#endif /* EDC_NUMERIC_H */
