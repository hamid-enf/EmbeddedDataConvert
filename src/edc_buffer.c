/**
 * @file edc_buffer.c
 * @brief Zero-copy buffer slicing, in-place processing, and streaming utilities.
 *
 * @note MISRA-C:2012 Compliant, Zero Dynamic Allocation.
 * @author EmbeddedDataConvert Team
 */

#include "edc_buffer.h"

#if EDC_FEATURE_BUFFER

EDC_Status EDC_Buffer_ApplyInPlace(
    uint8_t *buffer,
    size_t length,
    EDC_ByteTransformFn fn
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (buffer == NULL || fn == NULL) {
        return EDC_ERR_NULL_PTR;
    }
#endif

    for (size_t i = 0; i < length; i++) {
        buffer[i] = fn(buffer[i]);
    }

    return EDC_OK;
}

#endif /* EDC_FEATURE_BUFFER */
