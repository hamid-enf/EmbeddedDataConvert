/**
 * @file edc_parser.c
 * @brief Zero-allocation Key-Value, CSV, Delimiter, and UART Streaming Parsers.
 *
 * @note MISRA-C:2012 Compliant, Zero Dynamic Allocation.
 * @author EmbeddedDataConvert Team
 */

#include "edc_parser.h"
#include "edc_string.h"

#if EDC_FEATURE_PARSER

/* ========================================================================== */
/*                    KEY-VALUE PAIR PARSER                                   */
/* ========================================================================== */

EDC_Status EDC_KV_Parse(
    const char *str,
    size_t len,
    char item_delim,
    char kv_delim,
    EDC_KVPair *pairs,
    size_t max_pairs,
    size_t *out_count
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (str == NULL || pairs == NULL || max_pairs == 0U) {
        return EDC_ERR_NULL_PTR;
    }
#endif

    size_t slen = (len == 0U) ? EDC_StrLen(str, 8192U) : len;
    size_t pair_idx = 0;
    size_t i = 0;

    while (i < slen && str[i] != '\0' && pair_idx < max_pairs) {
        /* Skip leading whitespace and item delimiters or packet start markers */
        while (i < slen && (EDC_IsSpace(str[i]) || str[i] == item_delim || str[i] == '$' || str[i] == '#' || str[i] == '!')) {
            i++;
        }
        if (i >= slen || str[i] == '\0') {
            break;
        }

        /* Read Key */
        size_t key_start = i;
        while (i < slen && str[i] != kv_delim && str[i] != item_delim && str[i] != '\0' && str[i] != '\r' && str[i] != '\n') {
            i++;
        }

        size_t key_end = i;
        /* Trim trailing spaces from key */
        while (key_end > key_start && EDC_IsSpace(str[key_end - 1U])) {
            key_end--;
        }

        if (key_end == key_start) {
            /* Empty key: advance past current character/delimiter */
            if (i < slen) {
                i++;
            }
            continue;
        }

        pairs[pair_idx].key = &str[key_start];
        pairs[pair_idx].key_len = key_end - key_start;
        pairs[pair_idx].val = NULL;
        pairs[pair_idx].val_len = 0;

        if (i < slen && str[i] == kv_delim) {
            i++; /* Skip kv_delim */

            /* Skip spaces before value */
            while (i < slen && EDC_IsSpace(str[i]) && str[i] != item_delim) {
                i++;
            }

            size_t val_start = i;

            if (i < slen && (str[i] == '\"' || str[i] == '\'')) {
                char quote = str[i];
                i++;
                val_start = i;
                while (i < slen && str[i] != quote && str[i] != '\0') {
                    i++;
                }
                size_t val_end = i;
                if (i < slen && str[i] == quote) {
                    i++; /* Skip closing quote */
                }
                pairs[pair_idx].val = &str[val_start];
                pairs[pair_idx].val_len = val_end - val_start;
            } else {
                while (i < slen && str[i] != item_delim && str[i] != '\0' && str[i] != '\r' && str[i] != '\n') {
                    i++;
                }
                size_t val_end = i;
                while (val_end > val_start && EDC_IsSpace(str[val_end - 1U])) {
                    val_end--;
                }
                pairs[pair_idx].val = &str[val_start];
                pairs[pair_idx].val_len = val_end - val_start;
            }
        }

        pair_idx++;

        if (i < slen && str[i] == item_delim) {
            i++;
        }
    }

    if (out_count != NULL) {
        *out_count = pair_idx;
    }

    return EDC_OK;
}

EDC_Status EDC_KV_Find(
    const EDC_KVPair *pairs,
    size_t pair_count,
    const char *key,
    size_t key_len,
    const char **out_val,
    size_t *out_val_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (pairs == NULL || key == NULL || out_val == NULL) {
        return EDC_ERR_NULL_PTR;
    }
#endif

    size_t klen = (key_len == 0U) ? EDC_StrLen(key, 256U) : key_len;

    for (size_t i = 0; i < pair_count; i++) {
        if (pairs[i].key_len == klen) {
            bool match = true;
            for (size_t j = 0; j < klen; j++) {
                if (pairs[i].key[j] != key[j]) {
                    match = false;
                    break;
                }
            }
            if (match) {
                *out_val = pairs[i].val;
                if (out_val_len != NULL) {
                    *out_val_len = pairs[i].val_len;
                }
                return EDC_OK;
            }
        }
    }

    return EDC_ERR_NOT_FOUND;
}

/* ========================================================================== */
/*                    CSV / DELIMITER TOKENIZER                               */
/* ========================================================================== */

void EDC_CSV_Init(
    EDC_CSV_Tokenizer *tok,
    const char *source,
    size_t length,
    char delimiter,
    bool trim_spaces
) {
    if (tok != NULL) {
        tok->source = source;
        tok->remaining_len = (source != NULL) ?
            ((length == 0U) ? EDC_StrLen(source, 8192U) : length) : 0U;
        tok->delimiter = delimiter;
        tok->trim_spaces = trim_spaces;
    }
}

EDC_Status EDC_CSV_Next(
    EDC_CSV_Tokenizer *tok,
    const char **out_token,
    size_t *out_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (tok == NULL || out_token == NULL || out_len == NULL) {
        return EDC_ERR_NULL_PTR;
    }
#endif

    if (tok->source == NULL || tok->remaining_len == 0U) {
        return EDC_ERR_NOT_FOUND;
    }

    const char *start = tok->source;
    size_t idx = 0;
    bool inside_quotes = false;

    while (idx < tok->remaining_len) {
        char c = start[idx];

        if (c == '\"') {
            inside_quotes = !inside_quotes;
        } else if (c == tok->delimiter && !inside_quotes) {
            break;
        }
        idx++;
    }

    const char *token_start = start;
    size_t token_length = idx;

    if (tok->trim_spaces) {
        while (token_length > 0 && EDC_IsSpace(*token_start)) {
            token_start++;
            token_length--;
        }
        while (token_length > 0 && EDC_IsSpace(token_start[token_length - 1U])) {
            token_length--;
        }
    }

    /* Advance cursor */
    if (idx < tok->remaining_len && start[idx] == tok->delimiter) {
        tok->source = &start[idx + 1U];
        tok->remaining_len -= (idx + 1U);
    } else {
        tok->source = &start[idx];
        tok->remaining_len = 0U;
    }

    *out_token = token_start;
    *out_len = token_length;
    return EDC_OK;
}

/* ========================================================================== */
/*                    STREAMING FRAME PARSER                                  */
/* ========================================================================== */

EDC_Status EDC_StreamParser_Init(
    EDC_StreamParser *parser,
    char *buffer,
    size_t capacity,
    char start_byte,
    char end_byte
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (parser == NULL || buffer == NULL || capacity == 0U) {
        return EDC_ERR_NULL_PTR;
    }
#endif

    parser->buffer = buffer;
    parser->capacity = capacity;
    parser->length = 0;
    parser->start_byte = start_byte;
    parser->end_byte = end_byte;
    parser->in_frame = (start_byte == 0);
    parser->buffer[0] = '\0';

    return EDC_OK;
}

EDC_Status EDC_StreamParser_Feed(
    EDC_StreamParser *parser,
    uint8_t byte,
    const char **out_frame,
    size_t *out_frame_len
) {
#if EDC_CONFIG_STRICT_CHECKS
    if (parser == NULL || out_frame == NULL || out_frame_len == NULL) {
        return EDC_ERR_NULL_PTR;
    }
#endif

    char c = (char)byte;

    if (!parser->in_frame) {
        if (parser->start_byte != 0 && c == parser->start_byte) {
            parser->in_frame = true;
            parser->length = 0;
            parser->buffer[parser->length++] = c;
        }
        return EDC_ERR_INCOMPLETE;
    }

    /* Frame is active */
    if (parser->start_byte != 0 && c == parser->start_byte && parser->length > 0) {
        /* Re-synchronize on duplicate start byte */
        parser->length = 0;
        parser->buffer[parser->length++] = c;
        return EDC_ERR_INCOMPLETE;
    }

    if (c == parser->end_byte) {
        if (parser->length < parser->capacity) {
            parser->buffer[parser->length] = '\0';
        }
        *out_frame = parser->buffer;
        *out_frame_len = parser->length;

        /* Reset for next frame */
        parser->in_frame = (parser->start_byte == 0);
        parser->length = 0;
        return EDC_OK;
    }

    if (parser->length + 1U < parser->capacity) {
        parser->buffer[parser->length++] = c;
        return EDC_ERR_INCOMPLETE;
    }

    /* Overflow: reset frame */
    parser->length = 0;
    parser->in_frame = (parser->start_byte == 0);
    return EDC_ERR_BUFFER_TOO_SMALL;
}

void EDC_StreamParser_Reset(EDC_StreamParser *parser) {
    if (parser != NULL) {
        parser->length = 0;
        parser->in_frame = (parser->start_byte == 0);
        if (parser->buffer != NULL && parser->capacity > 0) {
            parser->buffer[0] = '\0';
        }
    }
}

#endif /* EDC_FEATURE_PARSER */
