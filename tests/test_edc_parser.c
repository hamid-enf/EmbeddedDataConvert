#include "unity.h"
#include "edc_parser.h"
#include <string.h>

void test_Parser_KeyValue(void) {
    EDC_KVPair pairs[8];
    size_t count = 0;

    const char *payload = "LAT=31.1234,LON=48.1234,ALT=125.5,UNIT=\"m/s\"";
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_KV_Parse(payload, 0, ',', '=', pairs, 8, &count));
    TEST_ASSERT_EQUAL_UINT(4, count);

    /* Check keys and values */
    const char *val = NULL;
    size_t val_len = 0;

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_KV_Find(pairs, count, "LAT", 3, &val, &val_len));
    TEST_ASSERT_EQUAL_UINT(7, val_len);
    TEST_ASSERT_EQUAL_MEMORY("31.1234", val, 7);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_KV_Find(pairs, count, "LON", 3, &val, &val_len));
    TEST_ASSERT_EQUAL_UINT(7, val_len);
    TEST_ASSERT_EQUAL_MEMORY("48.1234", val, 7);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_KV_Find(pairs, count, "ALT", 3, &val, &val_len));
    TEST_ASSERT_EQUAL_UINT(5, val_len);
    TEST_ASSERT_EQUAL_MEMORY("125.5", val, 5);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_KV_Find(pairs, count, "UNIT", 4, &val, &val_len));
    TEST_ASSERT_EQUAL_UINT(3, val_len);
    TEST_ASSERT_EQUAL_MEMORY("m/s", val, 3);

    /* Not found key */
    TEST_ASSERT_EQUAL_INT(EDC_ERR_NOT_FOUND, EDC_KV_Find(pairs, count, "TEMP", 4, &val, &val_len));
}

void test_Parser_CSV(void) {
    EDC_CSV_Tokenizer tok;
    const char *csv_line = "100, 200, \"hello, world\", 400";
    EDC_CSV_Init(&tok, csv_line, 0, ',', true);

    const char *token = NULL;
    size_t token_len = 0;

    /* Token 1 */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_CSV_Next(&tok, &token, &token_len));
    TEST_ASSERT_EQUAL_UINT(3, token_len);
    TEST_ASSERT_EQUAL_MEMORY("100", token, 3);

    /* Token 2 */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_CSV_Next(&tok, &token, &token_len));
    TEST_ASSERT_EQUAL_UINT(3, token_len);
    TEST_ASSERT_EQUAL_MEMORY("200", token, 3);

    /* Token 3 (quoted with comma) */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_CSV_Next(&tok, &token, &token_len));
    TEST_ASSERT_EQUAL_UINT(14, token_len);
    TEST_ASSERT_EQUAL_MEMORY("\"hello, world\"", token, 14);

    /* Token 4 */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_CSV_Next(&tok, &token, &token_len));
    TEST_ASSERT_EQUAL_UINT(3, token_len);
    TEST_ASSERT_EQUAL_MEMORY("400", token, 3);

    /* End of tokens */
    TEST_ASSERT_EQUAL_INT(EDC_ERR_NOT_FOUND, EDC_CSV_Next(&tok, &token, &token_len));
}

void test_Parser_StreamingFrame(void) {
    char buf[32];
    EDC_StreamParser sp;
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_StreamParser_Init(&sp, buf, sizeof(buf), '$', '\n'));

    const char *packet = "junk$DATA123\nmorejunk";
    const char *frame = NULL;
    size_t frame_len = 0;
    EDC_Status st = EDC_ERR_INCOMPLETE;

    for (size_t i = 0; i < strlen(packet); i++) {
        st = EDC_StreamParser_Feed(&sp, (uint8_t)packet[i], &frame, &frame_len);
        if (packet[i] == '\n') {
            TEST_ASSERT_EQUAL_INT(EDC_OK, st);
            TEST_ASSERT_EQUAL_STRING("$DATA123", frame);
            TEST_ASSERT_EQUAL_UINT(8, frame_len);
            break;
        }
    }
}

void run_parser_tests(void) {
    RUN_TEST(test_Parser_KeyValue);
    RUN_TEST(test_Parser_CSV);
    RUN_TEST(test_Parser_StreamingFrame);
}
