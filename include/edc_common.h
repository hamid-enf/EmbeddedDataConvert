/**
 * @file edc_common.h
 * @brief Common types, status codes, and utility macros for EDC.
 *
 * @note MISRA-C:2012 Compliant
 * @author EmbeddedDataConvert Team
 */

#ifndef EDC_COMMON_H
#define EDC_COMMON_H

#include "edc_config.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                          COMPILER ATTRIBUTES                               */
/* ========================================================================== */

#if defined(__GNUC__) || defined(__clang__)
  #define EDC_NODISCARD __attribute__((warn_unused_result))
  #define EDC_INLINE    static inline
  #define EDC_RESTRICT  __restrict__
#elif defined(_MSC_VER)
  #define EDC_NODISCARD _Check_return_
  #define EDC_INLINE    static __inline
  #define EDC_RESTRICT  __restrict
#else
  #define EDC_NODISCARD
  #define EDC_INLINE    static inline
  #define EDC_RESTRICT
#endif

/* ========================================================================== */
/*                          STATUS / ERROR CODES                              */
/* ========================================================================== */

/**
 * @brief Return status codes for all EDC operations.
 * Non-zero return codes indicate errors or special conditions.
 */
typedef enum {
    EDC_OK                     =  0,  /**< Operation successful */
    EDC_ERR_NULL_PTR           = -1,  /**< Null pointer passed where non-null expected */
    EDC_ERR_INVALID_PARAM      = -2,  /**< Parameter out of range or invalid */
    EDC_ERR_BUFFER_TOO_SMALL   = -3,  /**< Destination buffer cannot hold output */
    EDC_ERR_INVALID_FORMAT     = -4,  /**< Input string/data does not match format */
    EDC_ERR_OVERFLOW           = -5,  /**< Arithmetic or numerical representation overflow */
    EDC_ERR_UNDERFLOW          = -6,  /**< Numerical representation underflow */
    EDC_ERR_INVALID_ENCODING   = -7,  /**< Malformed UTF sequence, invalid codepoint */
    EDC_ERR_UNSUPPORTED        = -8,  /**< Requested base or operation not supported */
    EDC_ERR_INCOMPLETE         = -9,  /**< Data stream incomplete (need more bytes) */
    EDC_ERR_NOT_FOUND          = -10, /**< Key, token or character not found */
    EDC_ERR_CHECKSUM_MISMATCH  = -11  /**< Checksum verification failed (e.g., NMEA) */
} EDC_Status;

/* ========================================================================== */
/*                          HELPER MACROS                                     */
/* ========================================================================== */

#define EDC_ARRAY_SIZE(arr)    (sizeof(arr) / sizeof((arr)[0]))
#define EDC_MIN(a, b)          (((a) < (b)) ? (a) : (b))
#define EDC_MAX(a, b)          (((a) > (b)) ? (a) : (b))
#define EDC_CLAMP(val, lo, hi) (((val) < (lo)) ? (lo) : (((val) > (hi)) ? (hi) : (val)))

/**
 * @brief Convert status code to human-readable ASCII string description.
 *
 * @param[in] status Status code
 * @return const char* Pointer to constant string representation
 */
const char* EDC_StatusToString(EDC_Status status);

#ifdef __cplusplus
}
#endif

#endif /* EDC_COMMON_H */
