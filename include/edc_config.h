/**
 * @file edc_config.h
 * @brief Configuration macros for EmbeddedDataConvert (EDC) library.
 *
 * This file allows fine-grained control over library features, enabling
 * users to minimize Flash and RAM footprint for resource-constrained MCUs.
 *
 * @note MISRA-C:2012 Compliant
 * @author EmbeddedDataConvert Team
 */

#ifndef EDC_CONFIG_H
#define EDC_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                          FEATURE TOGGLES                                   */
/* ========================================================================== */

/**
 * @brief Enable Numeric Conversion Module (Integer, Base 2-36, Q-Format, Bool)
 * 1 = Enabled, 0 = Disabled
 */
#ifndef EDC_FEATURE_NUMERIC
#define EDC_FEATURE_NUMERIC             1
#endif

/**
 * @brief Enable Floating-Point Conversions (Float/Double <-> String/Integer)
 * 1 = Enabled, 0 = Disabled
 */
#ifndef EDC_FEATURE_FLOAT
#define EDC_FEATURE_FLOAT               1
#endif

/**
 * @brief Enable Fixed-Point (Q-format: Q8, Q15, Q31, Generic Qm.n) Arithmetic
 * 1 = Enabled, 0 = Disabled
 */
#ifndef EDC_FEATURE_FIXED_POINT
#define EDC_FEATURE_FIXED_POINT         1
#endif

/**
 * @brief Enable String Utility Module (Safe length, compare, trim, case, hex)
 * 1 = Enabled, 0 = Disabled
 */
#ifndef EDC_FEATURE_STRING
#define EDC_FEATURE_STRING              1
#endif

/**
 * @brief Enable Unicode Module (UTF-8, UTF-16, UTF-32, CodePoint validation)
 * 1 = Enabled, 0 = Disabled
 */
#ifndef EDC_FEATURE_UNICODE
#define EDC_FEATURE_UNICODE             1
#endif

/**
 * @brief Enable UTF-16 conversions within Unicode module
 * 1 = Enabled, 0 = Disabled
 */
#ifndef EDC_FEATURE_UTF16
#define EDC_FEATURE_UTF16               1
#endif

/**
 * @brief Enable UTF-32 conversions within Unicode module
 * 1 = Enabled, 0 = Disabled
 */
#ifndef EDC_FEATURE_UTF32
#define EDC_FEATURE_UTF32               1
#endif

/**
 * @brief Enable Persian & Arabic Support Module (Digit & Char Normalization)
 * 1 = Enabled, 0 = Disabled
 */
#ifndef EDC_FEATURE_PERSIAN
#define EDC_FEATURE_PERSIAN             1
#endif

/**
 * @brief Enable Transliteration Subsystem (Persian/Arabic <-> Latin)
 * 1 = Enabled, 0 = Disabled
 */
#ifndef EDC_FEATURE_TRANSLITERATION
#define EDC_FEATURE_TRANSLITERATION     1
#endif

/**
 * @brief Enable GPS & NMEA Parsing/Formatting Utilities
 * 1 = Enabled, 0 = Disabled
 */
#ifndef EDC_FEATURE_GPS
#define EDC_FEATURE_GPS                 1
#endif

/**
 * @brief Enable Physical Unit Conversions (Distance, Speed, Temp, Angle, Pressure)
 * 1 = Enabled, 0 = Disabled
 */
#ifndef EDC_FEATURE_UNITS
#define EDC_FEATURE_UNITS               1
#endif

/**
 * @brief Enable Parser Module (Key-Value, CSV, Delimiter, Streaming Parser)
 * 1 = Enabled, 0 = Disabled
 */
#ifndef EDC_FEATURE_PARSER
#define EDC_FEATURE_PARSER              1
#endif

/**
 * @brief Enable Buffer Utilities (Slices, In-place Runners, Stream Helpers)
 * 1 = Enabled, 0 = Disabled
 */
#ifndef EDC_FEATURE_BUFFER
#define EDC_FEATURE_BUFFER              1
#endif

/* ========================================================================== */
/*                          NUMERICAL LIMITS & TUNING                         */
/* ========================================================================== */

/**
 * @brief Maximum decimal precision supported for float/double formatting.
 * Default is 9 decimal places (sufficient for single/double precision GPS & sensors).
 */
#ifndef EDC_CONFIG_MAX_FLOAT_PRECISION
#define EDC_CONFIG_MAX_FLOAT_PRECISION  9
#endif

/**
 * @brief Enable strict parameter checks (NULL checks, bounds checks).
 * 1 = Strict defensive checks enabled (Recommended for Safety-Critical & MISRA)
 * 0 = Minimal checks (Slightly faster, only for verified callers)
 */
#ifndef EDC_CONFIG_STRICT_CHECKS
#define EDC_CONFIG_STRICT_CHECKS        1
#endif

/**
 * @brief Buffer size for internal streaming parsers if needed on stack.
 */
#ifndef EDC_CONFIG_STREAM_BUF_SIZE
#define EDC_CONFIG_STREAM_BUF_SIZE      128
#endif

#ifdef __cplusplus
}
#endif

#endif /* EDC_CONFIG_H */
