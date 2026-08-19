/**
 * @file edc_persian.c
 * @brief High-performance Persian/Arabic digit converter, normalizer, and transliterator.
 *
 * @note MISRA-C:2012 Compliant, Zero Dynamic Allocation.
 * @author EmbeddedDataConvert Team
 */

#include "edc_persian.h"
#include "edc_unicode.h"
#include "edc_string.h"

#if EDC_FEATURE_PERSIAN

/* ========================================================================== */
/*                    DIGIT CONVERSION IMPLEMENTATIONS                        */
/* ========================================================================== */

EDC_Status EDC_PersianDigitsToEnglish(
    const char *in_utf8,
    size_t in_len,
    char *out_buf,
    size_t out_capacity,
    size_t *out_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (in_utf8 == NULL || out_buf == NULL || out_capacity == 0U) {
        return EDC_ERR_NULL_PTR;
    }
#endif

    size_t len = (in_len == 0U) ? EDC_StrLen(in_utf8, 8192U) : in_len;
    size_t in_pos = 0;
    size_t out_pos = 0;

    while (in_pos < len && in_utf8[in_pos] != '\0') {
        if (out_pos + 1U >= out_capacity) {
            return EDC_ERR_BUFFER_TOO_SMALL;
        }

        uint8_t b1 = (uint8_t)in_utf8[in_pos];

        if (in_pos + 1U < len) {
            uint8_t b2 = (uint8_t)in_utf8[in_pos + 1U];

            /* Persian Digits: U+06F0 .. U+06F9 (0xDB 0xB0 .. 0xDB 0xB9) */
            if (b1 == 0xDBU && b2 >= 0xB0U && b2 <= 0xB9U) {
                out_buf[out_pos++] = (char)('0' + (b2 - 0xB0U));
                in_pos += 2U;
                continue;
            }

            /* Arabic-Indic Digits: U+0660 .. U+0669 (0xD9 0xA0 .. 0xD9 0xA9) */
            if (b1 == 0xD9U && b2 >= 0xA0U && b2 <= 0xA9U) {
                out_buf[out_pos++] = (char)('0' + (b2 - 0xA0U));
                in_pos += 2U;
                continue;
            }

            /* Persian Decimal Separator: U+066B (0xD9 0xAB) -> '.' */
            if (b1 == 0xD9U && b2 == 0xABU) {
                out_buf[out_pos++] = '.';
                in_pos += 2U;
                continue;
            }

            /* Persian Thousands Separator: U+066C (0xD9 0xAC) -> ',' */
            if (b1 == 0xD9U && b2 == 0xACU) {
                out_buf[out_pos++] = ',';
                in_pos += 2U;
                continue;
            }
        }

        /* Pass-through standard byte */
        out_buf[out_pos++] = (char)b1;
        in_pos++;
    }

    out_buf[out_pos] = '\0';
    if (out_len != NULL) {
        *out_len = out_pos;
    }

    return EDC_OK;
}

EDC_Status EDC_PersianDigitsToEnglish_InPlace(
    char *str_buf,
    size_t *len_in_out
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (str_buf == NULL) return EDC_ERR_NULL_PTR;
#endif

    size_t len = (len_in_out != NULL && *len_in_out > 0U) ?
                 *len_in_out : EDC_StrLen(str_buf, 8192U);

    size_t read_pos = 0;
    size_t write_pos = 0;

    while (read_pos < len && str_buf[read_pos] != '\0') {
        uint8_t b1 = (uint8_t)str_buf[read_pos];

        if (read_pos + 1U < len) {
            uint8_t b2 = (uint8_t)str_buf[read_pos + 1U];

            /* Persian Digits: 0xDB 0xB0..0xB9 */
            if (b1 == 0xDBU && b2 >= 0xB0U && b2 <= 0xB9U) {
                str_buf[write_pos++] = (char)('0' + (b2 - 0xB0U));
                read_pos += 2U;
                continue;
            }

            /* Arabic Digits: 0xD9 0xA0..0xA9 */
            if (b1 == 0xD9U && b2 >= 0xA0U && b2 <= 0xA9U) {
                str_buf[write_pos++] = (char)('0' + (b2 - 0xA0U));
                read_pos += 2U;
                continue;
            }

            /* Decimal Separator */
            if (b1 == 0xD9U && b2 == 0xABU) {
                str_buf[write_pos++] = '.';
                read_pos += 2U;
                continue;
            }

            /* Thousands Separator */
            if (b1 == 0xD9U && b2 == 0xACU) {
                str_buf[write_pos++] = ',';
                read_pos += 2U;
                continue;
            }
        }

        str_buf[write_pos++] = (char)b1;
        read_pos++;
    }

    str_buf[write_pos] = '\0';
    if (len_in_out != NULL) {
        *len_in_out = write_pos;
    }

    return EDC_OK;
}

EDC_Status EDC_EnglishDigitsToPersian(
    const char *in_ascii,
    size_t in_len,
    char *out_utf8,
    size_t out_capacity,
    size_t *out_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (in_ascii == NULL || out_utf8 == NULL || out_capacity == 0U) {
        return EDC_ERR_NULL_PTR;
    }
#endif

    size_t len = (in_len == 0U) ? EDC_StrLen(in_ascii, 8192U) : in_len;
    size_t in_pos = 0;
    size_t out_pos = 0;

    while (in_pos < len && in_ascii[in_pos] != '\0') {
        char c = in_ascii[in_pos++];

        if (c >= '0' && c <= '9') {
            if (out_pos + 2U >= out_capacity) return EDC_ERR_BUFFER_TOO_SMALL;
            out_utf8[out_pos++] = (char)0xDB;
            out_utf8[out_pos++] = (char)(0xB0 + (c - '0'));
        } else if (c == '.') {
            if (out_pos + 2U >= out_capacity) return EDC_ERR_BUFFER_TOO_SMALL;
            out_utf8[out_pos++] = (char)0xD9;
            out_utf8[out_pos++] = (char)0xAB;
        } else {
            if (out_pos + 1U >= out_capacity) return EDC_ERR_BUFFER_TOO_SMALL;
            out_utf8[out_pos++] = c;
        }
    }

    out_utf8[out_pos] = '\0';
    if (out_len != NULL) {
        *out_len = out_pos;
    }

    return EDC_OK;
}

EDC_Status EDC_EnglishDigitsToArabic(
    const char *in_ascii,
    size_t in_len,
    char *out_utf8,
    size_t out_capacity,
    size_t *out_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (in_ascii == NULL || out_utf8 == NULL || out_capacity == 0U) {
        return EDC_ERR_NULL_PTR;
    }
#endif

    size_t len = (in_len == 0U) ? EDC_StrLen(in_ascii, 8192U) : in_len;
    size_t in_pos = 0;
    size_t out_pos = 0;

    while (in_pos < len && in_ascii[in_pos] != '\0') {
        char c = in_ascii[in_pos++];

        if (c >= '0' && c <= '9') {
            if (out_pos + 2U >= out_capacity) return EDC_ERR_BUFFER_TOO_SMALL;
            out_utf8[out_pos++] = (char)0xD9;
            out_utf8[out_pos++] = (char)(0xA0 + (c - '0'));
        } else {
            if (out_pos + 1U >= out_capacity) return EDC_ERR_BUFFER_TOO_SMALL;
            out_utf8[out_pos++] = c;
        }
    }

    out_utf8[out_pos] = '\0';
    if (out_len != NULL) {
        *out_len = out_pos;
    }

    return EDC_OK;
}

/* ========================================================================== */
/*                    PERSIAN / ARABIC NORMALIZATION                          */
/* ========================================================================== */

EDC_Status EDC_Persian_Normalize(
    const char *in_utf8,
    size_t in_len,
    char *out_utf8,
    size_t out_capacity,
    uint32_t flags,
    size_t *out_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (in_utf8 == NULL || out_utf8 == NULL || out_capacity == 0U) {
        return EDC_ERR_NULL_PTR;
    }
#endif

    size_t len = (in_len == 0U) ? EDC_StrLen(in_utf8, 8192U) : in_len;
    size_t in_pos = 0;
    size_t out_pos = 0;

    while (in_pos < len && in_utf8[in_pos] != '\0') {
        uint32_t cp = 0;
        size_t consumed = 0;
        EDC_Status s = EDC_UTF8_DecodeChar((const uint8_t *)&in_utf8[in_pos], len - in_pos, &cp, &consumed);
        if (s != EDC_OK) {
            return s;
        }
        in_pos += consumed;

        /* Strip Tatweel (ـ U+0640) */
        if ((flags & EDC_NORM_REMOVE_TATWEEL) && cp == 0x0640U) {
            continue;
        }

        /* Strip Harakat (U+064B .. U+0655, U+0670) */
        if ((flags & EDC_NORM_REMOVE_HARAKAT) &&
            ((cp >= 0x064BU && cp <= 0x0655U) || cp == 0x0670U)) {
            continue;
        }

        /* Strip ZWNJ (U+200C) */
        if ((flags & EDC_NORM_REMOVE_ZWNJ) && cp == 0x200CU) {
            continue;
        }

        /* Normalize Arabic Yeh (ي U+064A, ى U+0649) -> Persian Yeh (ی U+06CC) */
        if ((flags & EDC_NORM_YEH_TO_PERSIAN) && (cp == 0x064AU || cp == 0x0649U)) {
            cp = 0x06CCU;
        }

        /* Normalize Arabic Kaf (ك U+0643) -> Persian Kaf (ک U+06A9) */
        if ((flags & EDC_NORM_KAF_TO_PERSIAN) && cp == 0x0643U) {
            cp = 0x06A9U;
        }

        /* Normalize Teh Marbuta (ة U+0629) -> Persian Heh (ه U+0647) */
        if ((flags & EDC_NORM_TEH_MARBUTA_TO_HEH) && (cp == 0x0629U || cp == 0x06C0U)) {
            cp = 0x0647U;
        }

        /* Normalize Hamza (أ U+0623, إ U+0625) -> Alef (ا U+0627) */
        if ((flags & EDC_NORM_HAMZA_TO_BARE_ALEF) && (cp == 0x0623U || cp == 0x0625U)) {
            cp = 0x0627U;
        }

        /* Standardize Digits */
        if (flags & EDC_NORM_DIGITS_TO_ENGLISH) {
            if (cp >= 0x06F0U && cp <= 0x06F9U) {
                cp = (uint32_t)('0' + (cp - 0x06F0U));
            } else if (cp >= 0x0660U && cp <= 0x0669U) {
                cp = (uint32_t)('0' + (cp - 0x0660U));
            } else if (cp == 0x066BU) {
                cp = (uint32_t)'.';
            } else if (cp == 0x066CU) {
                cp = (uint32_t)',';
            }
        } else if (flags & EDC_NORM_DIGITS_TO_PERSIAN) {
            if (cp >= '0' && cp <= '9') {
                cp = 0x06F0U + (cp - '0');
            } else if (cp >= 0x0660U && cp <= 0x0669U) {
                cp = 0x06F0U + (cp - 0x0660U);
            }
        }

        /* Encode normalized codepoint */
        uint8_t enc[4];
        size_t enc_len = 0;
        s = EDC_UTF8_EncodeChar(cp, enc, sizeof(enc), &enc_len);
        if (s != EDC_OK) return s;

        if (out_pos + enc_len >= out_capacity) {
            return EDC_ERR_BUFFER_TOO_SMALL;
        }

        for (size_t i = 0; i < enc_len; i++) {
            out_utf8[out_pos++] = (char)enc[i];
        }
    }

    out_utf8[out_pos] = '\0';
    if (out_len != NULL) {
        *out_len = out_pos;
    }

    return EDC_OK;
}

EDC_Status EDC_Persian_Normalize_InPlace(
    char *str_buf,
    size_t *len_in_out,
    uint32_t flags
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (str_buf == NULL) return EDC_ERR_NULL_PTR;
#endif

    size_t len = (len_in_out != NULL && *len_in_out > 0U) ?
                 *len_in_out : EDC_StrLen(str_buf, 8192U);

    size_t read_pos = 0;
    size_t write_pos = 0;

    while (read_pos < len && str_buf[read_pos] != '\0') {
        uint32_t cp = 0;
        size_t consumed = 0;
        EDC_Status s = EDC_UTF8_DecodeChar((const uint8_t *)&str_buf[read_pos], len - read_pos, &cp, &consumed);
        if (s != EDC_OK) return s;
        read_pos += consumed;

        if ((flags & EDC_NORM_REMOVE_TATWEEL) && cp == 0x0640U) continue;
        if ((flags & EDC_NORM_REMOVE_HARAKAT) && ((cp >= 0x064BU && cp <= 0x0655U) || cp == 0x0670U)) continue;
        if ((flags & EDC_NORM_REMOVE_ZWNJ) && cp == 0x200CU) continue;

        if ((flags & EDC_NORM_YEH_TO_PERSIAN) && (cp == 0x064AU || cp == 0x0649U)) cp = 0x06CCU;
        if ((flags & EDC_NORM_KAF_TO_PERSIAN) && cp == 0x0643U) cp = 0x06A9U;
        if ((flags & EDC_NORM_TEH_MARBUTA_TO_HEH) && (cp == 0x0629U || cp == 0x06C0U)) cp = 0x0647U;
        if ((flags & EDC_NORM_HAMZA_TO_BARE_ALEF) && (cp == 0x0623U || cp == 0x0625U)) cp = 0x0627U;

        if (flags & EDC_NORM_DIGITS_TO_ENGLISH) {
            if (cp >= 0x06F0U && cp <= 0x06F9U) cp = (uint32_t)('0' + (cp - 0x06F0U));
            else if (cp >= 0x0660U && cp <= 0x0669U) cp = (uint32_t)('0' + (cp - 0x0660U));
            else if (cp == 0x066BU) cp = (uint32_t)'.';
            else if (cp == 0x066CU) cp = (uint32_t)',';
        }

        uint8_t enc[4];
        size_t enc_len = 0;
        s = EDC_UTF8_EncodeChar(cp, enc, sizeof(enc), &enc_len);
        if (s != EDC_OK) return s;

        for (size_t i = 0; i < enc_len; i++) {
            str_buf[write_pos++] = (char)enc[i];
        }
    }

    str_buf[write_pos] = '\0';
    if (len_in_out != NULL) {
        *len_in_out = write_pos;
    }

    return EDC_OK;
}

/* ========================================================================== */
/*                    TRANSLITERATION IMPLEMENTATION                          */
/* ========================================================================== */

#if EDC_FEATURE_TRANSLITERATION

void EDC_Translit_GetDefaultOptions(EDC_TranslitOptions *opts) {
    if (opts != NULL) {
        opts->uppercase_first = false;
        opts->map_vav_to_v    = true;
        opts->map_yeh_to_y    = true;
        opts->map_khe_to_kh   = true;
        opts->map_ghaf_to_gh  = true;
    }
}

EDC_Status EDC_Persian_TransliterateToLatin(
    const char *in_utf8,
    size_t in_len,
    char *out_latin,
    size_t out_capacity,
    const EDC_TranslitOptions *opts,
    size_t *out_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (in_utf8 == NULL || out_latin == NULL || out_capacity == 0U) {
        return EDC_ERR_NULL_PTR;
    }
#endif

    EDC_TranslitOptions default_opts;
    if (opts == NULL) {
        EDC_Translit_GetDefaultOptions(&default_opts);
        opts = &default_opts;
    }

    size_t len = (in_len == 0U) ? EDC_StrLen(in_utf8, 8192U) : in_len;
    size_t in_pos = 0;
    size_t out_pos = 0;
    bool at_word_start = true;

    while (in_pos < len && in_utf8[in_pos] != '\0') {
        uint32_t cp = 0;
        size_t consumed = 0;
        EDC_Status s = EDC_UTF8_DecodeChar((const uint8_t *)&in_utf8[in_pos], len - in_pos, &cp, &consumed);
        if (s != EDC_OK) return s;
        in_pos += consumed;

        const char *replacement = NULL;
        char single_char_buf[2] = {0, 0};

        switch (cp) {
            case 0x0622: /* آ */
            case 0x0627: /* ا */ replacement = "a"; break;
            case 0x0628: /* ب */ replacement = "b"; break;
            case 0x067E: /* پ */ replacement = "p"; break;
            case 0x062A: /* ت */
            case 0x0637: /* ط */ replacement = "t"; break;
            case 0x062B: /* ث */
            case 0x0633: /* س */
            case 0x0635: /* ص */ replacement = "s"; break;
            case 0x062C: /* ج */ replacement = "j"; break;
            case 0x0686: /* چ */ replacement = "ch"; break;
            case 0x062D: /* ح */
            case 0x0647: /* ه */
            case 0x0629: /* ة */ replacement = "h"; break;
            case 0x062E: /* خ */ replacement = opts->map_khe_to_kh ? "kh" : "x"; break;
            case 0x062F: /* د */ replacement = "d"; break;
            case 0x0630: /* ذ */
            case 0x0632: /* ز */
            case 0x0636: /* ض */
            case 0x0638: /* ظ */ replacement = "z"; break;
            case 0x0631: /* ر */ replacement = "r"; break;
            case 0x0698: /* ژ */ replacement = "zh"; break;
            case 0x0634: /* ش */ replacement = "sh"; break;
            case 0x0639: /* ع */ replacement = "a"; break;
            case 0x063A: /* غ */ replacement = "gh"; break;
            case 0x0641: /* ف */ replacement = "f"; break;
            case 0x0642: /* ق */ replacement = opts->map_ghaf_to_gh ? "gh" : "q"; break;
            case 0x06A9: /* ک */
            case 0x0643: /* ك */ replacement = "k"; break;
            case 0x06AF: /* گ */ replacement = "g"; break;
            case 0x0644: /* ل */ replacement = "l"; break;
            case 0x0645: /* م */ replacement = "m"; break;
            case 0x0646: /* ن */ replacement = "n"; break;
            case 0x0648: /* و */ replacement = opts->map_vav_to_v ? "v" : "u"; break;
            case 0x06CC: /* ی */
            case 0x064A: /* ي */
            case 0x0649: /* ى */ replacement = opts->map_yeh_to_y ? "y" : "i"; break;
            case 0x0640: /* Tatweel */
            case 0x200C: /* ZWNJ */ replacement = ""; break;
            default:
                if (cp >= 0x06F0U && cp <= 0x06F9U) {
                    single_char_buf[0] = (char)('0' + (cp - 0x06F0U));
                    replacement = single_char_buf;
                } else if (cp >= 0x0660U && cp <= 0x0669U) {
                    single_char_buf[0] = (char)('0' + (cp - 0x0660U));
                    replacement = single_char_buf;
                } else if (cp <= 0x7FU) {
                    single_char_buf[0] = (char)cp;
                    replacement = single_char_buf;
                } else {
                    replacement = "";
                }
                break;
        }

        if (replacement != NULL) {
            size_t r_len = 0;
            while (replacement[r_len] != '\0') {
                r_len++;
            }

            if (out_pos + r_len >= out_capacity) {
                return EDC_ERR_BUFFER_TOO_SMALL;
            }

            for (size_t i = 0; i < r_len; i++) {
                char ch = replacement[i];
                if (at_word_start && opts->uppercase_first && i == 0) {
                    ch = EDC_CharToUpper(ch);
                }
                out_latin[out_pos++] = ch;
            }

            if (r_len > 0) {
                at_word_start = (replacement[0] == ' ' || replacement[0] == '\t' || replacement[0] == '\n');
            }
        }
    }

    out_latin[out_pos] = '\0';
    if (out_len != NULL) {
        *out_len = out_pos;
    }

    return EDC_OK;
}

#endif /* EDC_FEATURE_TRANSLITERATION */

#endif /* EDC_FEATURE_PERSIAN */
