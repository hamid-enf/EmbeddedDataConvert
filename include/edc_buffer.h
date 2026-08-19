/**
 * @file edc_buffer.h
 * @brief Zero-copy buffer slicing, in-place processing, and streaming utilities.
 *
 * @note MISRA-C:2012 Compliant, Zero Dynamic Allocation.
 * @author EmbeddedDataConvert Team
 */

#ifndef EDC_BUFFER_H
#define EDC_BUFFER_H

#include "edc_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#if EDC_FEATURE_BUFFER

/* ========================================================================== */
/*                    BUFFER SLICE STRUCTURES                                 */
/* ========================================================================== */

typedef struct {
    const uint8_t *data;
    size_t         length;
} EDC_ConstSlice;

typedef struct {
    uint8_t *data;
    size_t   length;
    size_t   capacity;
} EDC_MutSlice;

EDC_INLINE EDC_ConstSlice EDC_ConstSlice_Make(const void *data, size_t length) {
    EDC_ConstSlice s;
    s.data = (const uint8_t *)data;
    s.length = length;
    return s;
}

EDC_INLINE EDC_MutSlice EDC_MutSlice_Make(void *data, size_t length, size_t capacity) {
    EDC_MutSlice s;
    s.data = (uint8_t *)data;
    s.length = length;
    s.capacity = capacity;
    return s;
}

/**
 * @brief In-place byte transformation callback.
 */
typedef uint8_t (*EDC_ByteTransformFn)(uint8_t b);

/**
 * @brief Apply an in-place single-byte transform across a buffer.
 */
EDC_NODISCARD EDC_Status EDC_Buffer_ApplyInPlace(
    uint8_t *buffer,
    size_t length,
    EDC_ByteTransformFn fn
);

#endif /* EDC_FEATURE_BUFFER */

#ifdef __cplusplus
}
#endif

#endif /* EDC_BUFFER_H */
