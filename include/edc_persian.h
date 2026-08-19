/**
 * @file edc_persian.h
 * @brief Persian & Arabic digit conversion, character normalization, and transliteration for embedded devices.
 *
 * @note MISRA-C:2012 Compliant, Zero Dynamic Allocation, Thread-Safe.
 * @author EmbeddedDataConvert Team
 */

#ifndef EDC_PERSIAN_H
#define EDC_PERSIAN_H

#include "edc_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#if EDC_FEATURE_PERSIAN

/* ========================================================================== */
/*                    NORMALIZATION FLAGS                                     */
/* ========================================================================== */

#define EDC_NORM_YEH_TO_PERSIAN        (1U << 0) /**< Convert Arabic Yeh (ي, ى) to Persian (ی) */
#define EDC_NORM_KAF_TO_PERSIAN        (1U << 1) /**< Convert Arabic Kaf (ك) to Persian (ک) */
#define EDC_NORM_TEH_MARBUTA_TO_HEH    (1U << 2) /**< Convert Arabic Teh Marbuta (ة) to Persian Heh (ه) */
#define EDC_NORM_REMOVE_TATWEEL        (1U << 3) /**< Remove Kashida / Tatweel (ـ) */
#define EDC_NORM_REMOVE_HARAKAT        (1U << 4) /**< Remove Arabic diacritics (Fatha, Damma, Kasra, Tanween, Sukun, Shadda) */
#define EDC_NORM_HAMZA_TO_BARE_ALEF    (1U << 5) /**< Convert أ, إ to bare Alef (ا) */
#define EDC_NORM_DIGITS_TO_ENGLISH     (1U << 6) /**< Convert Persian and Arabic digits to English (0-9) */
#define EDC_NORM_DIGITS_TO_PERSIAN     (1U << 7) /**< Convert English and Arabic digits to Persian (۰-۹) */
#define EDC_NORM_REMOVE_ZWNJ           (1U << 8) /**< Remove Zero-Width Non-Joiner (ZWNJ / نیم‌فاصله) */

/** Default complete Persian normalization preset */
#define EDC_NORM_DEFAULT_PERSIAN \
    (EDC_NORM_YEH_TO_PERSIAN | \
     EDC_NORM_KAF_TO_PERSIAN | \
     EDC_NORM_TEH_MARBUTA_TO_HEH | \
     EDC_NORM_REMOVE_TATWEEL | \
     EDC_NORM_REMOVE_HARAKAT)

/* ========================================================================== */
/*                    DIGIT CONVERSIONS                                       */
/* ========================================================================== */

/**
 * @brief Convert Persian (۰-۹) and Arabic (٠-٩) digits and separators to English (0-9, .).
 *
 * @param[in]  in_utf8       Input UTF-8 string containing Persian/Arabic digits
 * @param[in]  in_len        Length in bytes (or 0 for null-terminated)
 * @param[out] out_buf       Destination buffer for English ASCII string
 * @param[in]  out_capacity  Size of destination buffer
 * @param[out] out_len       Optional pointer to receive length of written string
 * @return EDC_Status        EDC_OK on success, error code otherwise
 */
EDC_NODISCARD EDC_Status EDC_PersianDigitsToEnglish(
    const char *in_utf8,
    size_t in_len,
    char *out_buf,
    size_t out_capacity,
    size_t *out_len
);

/**
 * @brief In-place conversion of Persian/Arabic digits to English digits.
 *
 * Since Persian/Arabic digits are 2 bytes in UTF-8 and English ASCII digits are 1 byte,
 * in-place conversion is 100% memory-safe and requires ZERO buffer allocation.
 *
 * @param[in,out] str_buf    Null-terminated buffer to convert in-place
 * @param[in,out] len_in_out Pointer to string length (updated to new shorter length on return)
 * @return EDC_Status        EDC_OK on success
 */
EDC_NODISCARD EDC_Status EDC_PersianDigitsToEnglish_InPlace(
    char *str_buf,
    size_t *len_in_out
);

/**
 * @brief Convert English ASCII digits (0-9) to Persian UTF-8 digits (۰-۹).
 *
 * Note: Persian digits require 2 bytes per digit in UTF-8, so out_capacity
 * should be at least (2 * digit_count + 1).
 */
EDC_NODISCARD EDC_Status EDC_EnglishDigitsToPersian(
    const char *in_ascii,
    size_t in_len,
    char *out_utf8,
    size_t out_capacity,
    size_t *out_len
);

/**
 * @brief Convert English ASCII digits (0-9) to Arabic-Indic UTF-8 digits (٠-٩).
 */
EDC_NODISCARD EDC_Status EDC_EnglishDigitsToArabic(
    const char *in_ascii,
    size_t in_len,
    char *out_utf8,
    size_t out_capacity,
    size_t *out_len
);

/* ========================================================================== */
/*                    PERSIAN / ARABIC NORMALIZATION                          */
/* ========================================================================== */

/**
 * @brief Normalize Persian and Arabic UTF-8 text according to specified flags.
 *
 * @param[in]  in_utf8      Input UTF-8 string
 * @param[in]  in_len       Input length in bytes (or 0 for null-terminated)
 * @param[out] out_utf8     Destination UTF-8 buffer
 * @param[in]  out_capacity Capacity of destination buffer
 * @param[in]  flags        Bitwise OR of EDC_NORM_* flags
 * @param[out] out_len      Optional pointer to receive length of written string
 * @return EDC_Status       EDC_OK on success
 */
EDC_NODISCARD EDC_Status EDC_Persian_Normalize(
    const char *in_utf8,
    size_t in_len,
    char *out_utf8,
    size_t out_capacity,
    uint32_t flags,
    size_t *out_len
);

/**
 * @brief In-place normalization for operations that do not expand string length.
 *
 * Supported in-place with flags that only replace 2-byte UTF-8 chars with 2-byte/1-byte
 * chars or remove chars (Yeh, Kaf, Teh Marbuta, Tatweel, Harakat, ZWNJ, English digits).
 */
EDC_NODISCARD EDC_Status EDC_Persian_Normalize_InPlace(
    char *str_buf,
    size_t *len_in_out,
    uint32_t flags
);

/* ========================================================================== */
/*                    PERSIAN TRANSLITERATION (FINGILISH / LATIN)             */
/* ========================================================================== */

#if EDC_FEATURE_TRANSLITERATION

typedef struct {
    bool uppercase_first; /**< Capitalize first letter of transliterated word */
    bool map_vav_to_v;    /**< True for 'v', false for 'u' / 'o' */
    bool map_yeh_to_y;    /**< True for 'y', false for 'i' */
    bool map_khe_to_kh;   /**< True for "kh", false for "x" */
    bool map_ghaf_to_gh;  /**< True for "gh", false for "q" */
} EDC_TranslitOptions;

/**
 * @brief Default transliteration options.
 */
void EDC_Translit_GetDefaultOptions(EDC_TranslitOptions *opts);

/**
 * @brief Transliterate Persian UTF-8 text to Latin (Pinglish / Fingilish).
 *
 * @important This is Transliteration (phonetic character mapping), NOT Language Translation!
 *
 * @param[in]  in_utf8      Input Persian UTF-8 string
 * @param[in]  in_len       Input length in bytes (or 0 for null-terminated)
 * @param[out] out_latin    Destination Latin ASCII buffer
 * @param[in]  out_capacity Capacity of destination buffer
 * @param[in]  opts         Transliteration options (or NULL for defaults)
 * @param[out] out_len      Optional pointer to receive length of transliterated string
 * @return EDC_Status       EDC_OK on success
 */
EDC_NODISCARD EDC_Status EDC_Persian_TransliterateToLatin(
    const char *in_utf8,
    size_t in_len,
    char *out_latin,
    size_t out_capacity,
    const EDC_TranslitOptions *opts,
    size_t *out_len
);

#endif /* EDC_FEATURE_TRANSLITERATION */

#endif /* EDC_FEATURE_PERSIAN */

#ifdef __cplusplus
}
#endif

#endif /* EDC_PERSIAN_H */
