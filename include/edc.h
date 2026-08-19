/**
 * @file edc.h
 * @brief Master Header for EmbeddedDataConvert (EDC) Library.
 *
 * A professional, lightweight, standalone, and ultra-portable C library
 * for Data Conversion, Parsing, Formatting, Encoding, and Normalization
 * specifically engineered for STM32, ESP32, and bare-metal embedded targets.
 *
 * @version 1.0.0
 * @note MISRA-C:2012 Compliant, Zero Dynamic Allocation.
 * @author EmbeddedDataConvert Team
 */

#ifndef EDC_H
#define EDC_H

#define EDC_VERSION_MAJOR   1
#define EDC_VERSION_MINOR   0
#define EDC_VERSION_PATCH   0
#define EDC_VERSION_STRING  "1.0.0"

/* Common Types and Configuration */
#include "edc_config.h"
#include "edc_common.h"

/* Modular Subsystems */
#include "edc_numeric.h"
#include "edc_string.h"
#include "edc_unicode.h"
#include "edc_persian.h"
#include "edc_gps.h"
#include "edc_units.h"
#include "edc_parser.h"
#include "edc_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get EDC library version string.
 */
EDC_INLINE const char* EDC_GetVersion(void) {
    return EDC_VERSION_STRING;
}

#ifdef __cplusplus
}
#endif

#endif /* EDC_H */
