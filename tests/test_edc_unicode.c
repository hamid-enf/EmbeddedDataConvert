#include "unity.h"
#include "edc_unicode.h"
#include <string.h>

void test_Unicode_Decode_Encode_Basic(void) {
    uint32_t cp = 0;
    size_t consumed = 0;
    uint8_t buf[8];
    size_t out_len = 0;

    /* 1-byte ASCII 'A' (U+0041) */
    uint8_t ascii[] = "A";
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_UTF8_DecodeChar(ascii, 1, &cp, &consumed));
    TEST_ASSERT_EQUAL_HEX32(0x0041, cp);
    TEST_ASSERT_EQUAL_UINT(1, consumed);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_UTF8_EncodeChar(0x0041, buf, sizeof(buf), &out_len));
    TEST_ASSERT_EQUAL_UINT(1, out_len);
    TEST_ASSERT_EQUAL_HEX8('A', buf[0]);

    /* 2-byte Persian 'س' (U+0633 -> 0xD8 0xB3) */
    uint8_t persian_sin[] = {0xD8, 0xB3};
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_UTF8_DecodeChar(persian_sin, 2, &cp, &consumed));
    TEST_ASSERT_EQUAL_HEX32(0x0633, cp);
    TEST_ASSERT_EQUAL_UINT(2, consumed);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_UTF8_EncodeChar(0x0633, buf, sizeof(buf), &out_len));
    TEST_ASSERT_EQUAL_UINT(2, out_len);
    TEST_ASSERT_EQUAL_HEX8(0xD8, buf[0]);
    TEST_ASSERT_EQUAL_HEX8(0xB3, buf[1]);

    /* 3-byte Euro sign '€' (U+20AC -> 0xE2 0x82 0xAC) */
    uint8_t euro[] = {0xE2, 0x82, 0xAC};
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_UTF8_DecodeChar(euro, 3, &cp, &consumed));
    TEST_ASSERT_EQUAL_HEX32(0x20AC, cp);
    TEST_ASSERT_EQUAL_UINT(3, consumed);

    /* 4-byte Emoji '😀' (U+1F600 -> 0xF0 0x9F 0x98 0x80) */
    uint8_t emoji[] = {0xF0, 0x9F, 0x98, 0x80};
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_UTF8_DecodeChar(emoji, 4, &cp, &consumed));
    TEST_ASSERT_EQUAL_HEX32(0x1F600, cp);
    TEST_ASSERT_EQUAL_UINT(4, consumed);
}

void test_Unicode_Validation_Security(void) {
    size_t invalid_off = 0;

    /* Valid Persian text "سلام دنیا" */
    const uint8_t valid_text[] = "سلام دنیا";
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_UTF8_Validate(valid_text, strlen((const char*)valid_text), &invalid_off));

    /* Overlong encoding of ASCII 'A' (0xC0 0x81) - MUST BE REJECTED */
    const uint8_t overlong_2[] = {0xC0, 0x81};
    TEST_ASSERT_EQUAL_INT(EDC_ERR_INVALID_ENCODING, EDC_UTF8_Validate(overlong_2, sizeof(overlong_2), &invalid_off));

    /* Overlong NUL (0xC0 0x80) - MUST BE REJECTED */
    const uint8_t overlong_nul[] = {0xC0, 0x80};
    TEST_ASSERT_EQUAL_INT(EDC_ERR_INVALID_ENCODING, EDC_UTF8_Validate(overlong_nul, sizeof(overlong_nul), &invalid_off));

    /* UTF-16 Surrogate U+D800 (0xED 0xA0 0x80) in UTF-8 - MUST BE REJECTED */
    const uint8_t surrogate[] = {0xED, 0xA0, 0x80};
    TEST_ASSERT_EQUAL_INT(EDC_ERR_INVALID_ENCODING, EDC_UTF8_Validate(surrogate, sizeof(surrogate), &invalid_off));

    /* Codepoint > U+10FFFF (0xF4 0x90 0x80 0x80) - MUST BE REJECTED */
    const uint8_t out_of_range[] = {0xF4, 0x90, 0x80, 0x80};
    TEST_ASSERT_EQUAL_INT(EDC_ERR_INVALID_ENCODING, EDC_UTF8_Validate(out_of_range, sizeof(out_of_range), &invalid_off));
}

void test_Unicode_Transcoding_UTF16_UTF32(void) {
    uint16_t u16_buf[32];
    uint8_t u8_buf[64];
    uint32_t u32_buf[32];
    size_t u16_len = 0;
    size_t u8_len = 0;
    size_t u32_len = 0;

    /* UTF-8 to UTF-16 */
    const uint8_t test_str[] = "سلام 😀"; /* Includes 2-byte Arabic and 4-byte Emoji surrogate */
    size_t test_len = strlen((const char*)test_str);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_UTF8_ToUTF16(test_str, test_len, u16_buf, 32, false, &u16_len));
    TEST_ASSERT_TRUE(u16_len > 0);

    /* UTF-16 back to UTF-8 */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_UTF16_ToUTF8(u16_buf, u16_len, false, u8_buf, sizeof(u8_buf), &u8_len));
    TEST_ASSERT_EQUAL_UINT(test_len, u8_len);
    TEST_ASSERT_EQUAL_MEMORY(test_str, u8_buf, test_len);

    /* UTF-8 to UTF-32 */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_UTF8_ToUTF32(test_str, test_len, u32_buf, 32, &u32_len));
    TEST_ASSERT_TRUE(u32_len > 0);

    /* UTF-32 back to UTF-8 */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_UTF32_ToUTF8(u32_buf, u32_len, u8_buf, sizeof(u8_buf), &u8_len));
    TEST_ASSERT_EQUAL_UINT(test_len, u8_len);
    TEST_ASSERT_EQUAL_MEMORY(test_str, u8_buf, test_len);
}

void test_Unicode_Streaming_Decoder(void) {
    EDC_UTF8_StreamDecoder dec;
    EDC_UTF8_StreamDecoder_Init(&dec);
    uint32_t cp = 0;

    /* Feed 3-byte character '€' (0xE2, 0x82, 0xAC) byte-by-byte */
    TEST_ASSERT_EQUAL_INT(EDC_ERR_INCOMPLETE, EDC_UTF8_StreamDecoder_Feed(&dec, 0xE2, &cp));
    TEST_ASSERT_EQUAL_INT(EDC_ERR_INCOMPLETE, EDC_UTF8_StreamDecoder_Feed(&dec, 0x82, &cp));
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_UTF8_StreamDecoder_Feed(&dec, 0xAC, &cp));
    TEST_ASSERT_EQUAL_HEX32(0x20AC, cp);
}

void run_unicode_tests(void) {
    RUN_TEST(test_Unicode_Decode_Encode_Basic);
    RUN_TEST(test_Unicode_Validation_Security);
    RUN_TEST(test_Unicode_Transcoding_UTF16_UTF32);
    RUN_TEST(test_Unicode_Streaming_Decoder);
}
