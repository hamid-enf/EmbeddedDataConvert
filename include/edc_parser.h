/**
 * @file edc_parser.h
 * @brief Zero-allocation Key-Value, CSV, Delimiter, and UART Streaming Parsers.
 *
 * @note MISRA-C:2012 Compliant, Zero Dynamic Allocation, Thread-Safe.
 * @author EmbeddedDataConvert Team
 */

#ifndef EDC_PARSER_H
#define EDC_PARSER_H

#include "edc_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#if EDC_FEATURE_PARSER

/* ========================================================================== */
/*                    KEY-VALUE PAIR PARSER (ZERO-COPY SLICES)                */
/* ========================================================================== */

typedef struct {
    const char *key;      /**< Pointer to key inside source string */
    size_t      key_len;  /**< Length of key */
    const char *val;      /**< Pointer to value inside source string */
    size_t      val_len;  /**< Length of value */
} EDC_KVPair;

/**
 * @brief Parse a delimiter-separated Key/Value string into zero-copy slices.
 *
 * Example: "LAT=31.1234,LON=48.1234,ALT=125.5" with item_delim=',', kv_delim='='
 *
 * @param[in]  str          Source string
 * @param[in]  len          Length of source string (or 0 for null-terminated)
 * @param[in]  item_delim   Delimiter between items (e.g. ',' or '&' or ';')
 * @param[in]  kv_delim     Delimiter between key and value (e.g. '=' or ':')
 * @param[out] pairs        Array of EDC_KVPair to populate
 * @param[in]  max_pairs    Maximum pairs the array can hold
 * @param[out] out_count    Number of pairs successfully parsed
 * @return EDC_Status       EDC_OK on success
 */
EDC_NODISCARD EDC_Status EDC_KV_Parse(
    const char *str,
    size_t len,
    char item_delim,
    char kv_delim,
    EDC_KVPair *pairs,
    size_t max_pairs,
    size_t *out_count
);

/**
 * @brief Find a key inside a parsed EDC_KVPair array.
 *
 * @param[in]  pairs        Parsed pairs array
 * @param[in]  pair_count   Number of pairs in array
 * @param[in]  key          Key name to find (null-terminated or bounded)
 * @param[in]  key_len      Length of key (or 0 for null-terminated)
 * @param[out] out_val      Pointer to receive value string slice
 * @param[out] out_val_len  Pointer to receive value string length
 * @return EDC_Status       EDC_OK if found, EDC_ERR_NOT_FOUND otherwise
 */
EDC_NODISCARD EDC_Status EDC_KV_Find(
    const EDC_KVPair *pairs,
    size_t pair_count,
    const char *key,
    size_t key_len,
    const char **out_val,
    size_t *out_val_len
);

/* ========================================================================== */
/*                    CSV / DELIMITER TOKENIZER                               */
/* ========================================================================== */

typedef struct {
    const char *source;
    size_t      remaining_len;
    char        delimiter;
    bool        trim_spaces;
} EDC_CSV_Tokenizer;

/**
 * @brief Initialize CSV / Delimiter tokenizer.
 */
void EDC_CSV_Init(
    EDC_CSV_Tokenizer *tok,
    const char *source,
    size_t length,
    char delimiter,
    bool trim_spaces
);

/**
 * @brief Get next token from tokenizer.
 *
 * @param[in,out] tok         Tokenizer state
 * @param[out]    out_token   Pointer to token start inside source
 * @param[out]    out_len     Length of extracted token
 * @return EDC_Status         EDC_OK when a token is retrieved,
 *                            EDC_ERR_NOT_FOUND when end of string reached.
 */
EDC_NODISCARD EDC_Status EDC_CSV_Next(
    EDC_CSV_Tokenizer *tok,
    const char **out_token,
    size_t *out_len
);

/* ========================================================================== */
/*                    STREAMING BYTE-BY-BYTE FRAME PARSER                     */
/* ========================================================================== */

typedef struct {
    char   *buffer;           /**< User-provided frame buffer */
    size_t  capacity;         /**< Maximum capacity of buffer */
    size_t  length;           /**< Current number of accumulated bytes */
    char    start_byte;       /**< Frame start marker (0 if none required) */
    char    end_byte;         /**< Frame end marker (e.g. '\n' or '\r') */
    bool    in_frame;         /**< True when inside an active frame */
} EDC_StreamParser;

/**
 * @brief Initialize a streaming frame parser.
 */
EDC_NODISCARD EDC_Status EDC_StreamParser_Init(
    EDC_StreamParser *parser,
    char *buffer,
    size_t capacity,
    char start_byte,
    char end_byte
);

/**
 * @brief Feed a single byte into the stream parser.
 *
 * @param[in,out] parser        Parser state
 * @param[in]     byte          Incoming byte from UART / SPI / CAN
 * @param[out]    out_frame     Pointer to start of completed frame (null-terminated)
 * @param[out]    out_frame_len Length of completed frame
 * @return EDC_Status           EDC_OK when complete frame received,
 *                              EDC_ERR_INCOMPLETE when accumulating,
 *                              EDC_ERR_BUFFER_TOO_SMALL on overflow.
 */
EDC_NODISCARD EDC_Status EDC_StreamParser_Feed(
    EDC_StreamParser *parser,
    uint8_t byte,
    const char **out_frame,
    size_t *out_frame_len
);

/**
 * @brief Reset stream parser to receive next frame.
 */
void EDC_StreamParser_Reset(EDC_StreamParser *parser);

#endif /* EDC_FEATURE_PARSER */

#ifdef __cplusplus
}
#endif

#endif /* EDC_PARSER_H */
