/**
 * @file edc_config_template.h
 * @brief User Configuration Template for EmbeddedDataConvert.
 *
 * Copy this file to your application include directory, rename to `edc_config.h`,
 * and adjust macro definitions to match your target MCU constraints.
 */

#ifndef EDC_CONFIG_TEMPLATE_H
#define EDC_CONFIG_TEMPLATE_H

#ifdef __cplusplus
extern "C" {
#endif

#define EDC_FEATURE_NUMERIC             1
#define EDC_FEATURE_FLOAT               1
#define EDC_FEATURE_FIXED_POINT         1
#define EDC_FEATURE_STRING              1
#define EDC_FEATURE_UNICODE             1
#define EDC_FEATURE_UTF16               1
#define EDC_FEATURE_UTF32               1
#define EDC_FEATURE_PERSIAN             1
#define EDC_FEATURE_TRANSLITERATION     1
#define EDC_FEATURE_GPS                 1
#define EDC_FEATURE_UNITS               1
#define EDC_FEATURE_PARSER              1
#define EDC_FEATURE_BUFFER              1

#define EDC_CONFIG_MAX_FLOAT_PRECISION  9
#define EDC_CONFIG_STRICT_CHECKS        1
#define EDC_CONFIG_STREAM_BUF_SIZE      128

#ifdef __cplusplus
}
#endif

#endif /* EDC_CONFIG_TEMPLATE_H */
