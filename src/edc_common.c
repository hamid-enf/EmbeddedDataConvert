/**
 * @file edc_common.c
 * @brief Common status code descriptions and helper implementations.
 *
 * @note MISRA-C:2012 Compliant.
 * @author EmbeddedDataConvert Team
 */

#include "edc_common.h"

const char* EDC_StatusToString(EDC_Status status) {
    switch (status) {
        case EDC_OK:
            return "EDC_OK: Success";
        case EDC_ERR_NULL_PTR:
            return "EDC_ERR_NULL_PTR: Null pointer passed";
        case EDC_ERR_INVALID_PARAM:
            return "EDC_ERR_INVALID_PARAM: Invalid parameter";
        case EDC_ERR_BUFFER_TOO_SMALL:
            return "EDC_ERR_BUFFER_TOO_SMALL: Destination buffer too small";
        case EDC_ERR_INVALID_FORMAT:
            return "EDC_ERR_INVALID_FORMAT: Invalid data or string format";
        case EDC_ERR_OVERFLOW:
            return "EDC_ERR_OVERFLOW: Numerical overflow";
        case EDC_ERR_UNDERFLOW:
            return "EDC_ERR_UNDERFLOW: Numerical underflow";
        case EDC_ERR_INVALID_ENCODING:
            return "EDC_ERR_INVALID_ENCODING: Invalid character encoding";
        case EDC_ERR_UNSUPPORTED:
            return "EDC_ERR_UNSUPPORTED: Operation or base unsupported";
        case EDC_ERR_INCOMPLETE:
            return "EDC_ERR_INCOMPLETE: Incomplete stream/sequence";
        case EDC_ERR_NOT_FOUND:
            return "EDC_ERR_NOT_FOUND: Key or token not found";
        case EDC_ERR_CHECKSUM_MISMATCH:
            return "EDC_ERR_CHECKSUM_MISMATCH: Checksum verification failed";
        default:
            return "EDC_ERR_UNKNOWN: Unknown error";
    }
}
