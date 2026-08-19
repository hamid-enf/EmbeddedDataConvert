#include "unity.h"
#include "edc_string.h"

void test_String_Length_And_Copy(void) {
    char buf[16];
    size_t len = 0;

    TEST_ASSERT_EQUAL_UINT(5, EDC_StrLen("hello", 100));
    TEST_ASSERT_EQUAL_UINT(3, EDC_StrLen("hello", 3));
    TEST_ASSERT_EQUAL_UINT(0, EDC_StrLen(NULL, 10));

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_StrCopy(buf, sizeof(buf), "Embedded", 100, &len));
    TEST_ASSERT_EQUAL_STRING("Embedded", buf);
    TEST_ASSERT_EQUAL_UINT(8, len);

    /* Buffer truncation protection */
    TEST_ASSERT_EQUAL_INT(EDC_ERR_BUFFER_TOO_SMALL, EDC_StrCopy(buf, 5, "EmbeddedData", 100, &len));
    TEST_ASSERT_EQUAL_STRING("Embe", buf);
    TEST_ASSERT_EQUAL_UINT(4, len);
}

void test_String_Compare_And_Trim(void) {
    int cmp = 0;
    const char *trimmed = NULL;
    size_t trimmed_len = 0;

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_StrCompare("abc", 3, "abc", 3, &cmp));
    TEST_ASSERT_EQUAL_INT(0, cmp);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_StrCompareCase("ABC", 3, "abc", 3, &cmp));
    TEST_ASSERT_EQUAL_INT(0, cmp);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_StrTrim("   STM32 HAL   ", 0, &trimmed, &trimmed_len));
    TEST_ASSERT_EQUAL_UINT(9, trimmed_len);
    TEST_ASSERT_EQUAL_MEMORY("STM32 HAL", trimmed, 9);
}

void test_String_Search_And_Case(void) {
    size_t idx = 0;
    char buf[32];

    TEST_ASSERT_TRUE(EDC_StrStartsWith("EmbeddedData", 0, "Embed", 0));
    TEST_ASSERT_FALSE(EDC_StrStartsWith("EmbeddedData", 0, "ESP", 0));

    TEST_ASSERT_TRUE(EDC_StrEndsWith("sensor_data.csv", 0, ".csv", 0));
    TEST_ASSERT_FALSE(EDC_StrEndsWith("sensor_data.csv", 0, ".json", 0));

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_StrFind("hello embedded world", 0, "embedded", 0, &idx));
    TEST_ASSERT_EQUAL_UINT(6, idx);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_StrToUpper("stm32", 0, buf, sizeof(buf), NULL));
    TEST_ASSERT_EQUAL_STRING("STM32", buf);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_StrToLower("ESP32-S3", 0, buf, sizeof(buf), NULL));
    TEST_ASSERT_EQUAL_STRING("esp32-s3", buf);
}

void test_Hex_Conversions(void) {
    char hex[64];
    uint8_t bytes[16];
    size_t count = 0;

    uint8_t input_bytes[] = {0xDE, 0xAD, 0xBE, 0xEF};
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_BytesToHex(input_bytes, 4, hex, sizeof(hex), true, false, NULL));
    TEST_ASSERT_EQUAL_STRING("DEADBEEF", hex);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_BytesToHex(input_bytes, 4, hex, sizeof(hex), true, true, NULL));
    TEST_ASSERT_EQUAL_STRING("DE AD BE EF", hex);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_HexToBytes("DEADBEEF", 0, bytes, sizeof(bytes), &count));
    TEST_ASSERT_EQUAL_UINT(4, count);
    TEST_ASSERT_EQUAL_HEX8(0xDE, bytes[0]);
    TEST_ASSERT_EQUAL_HEX8(0xAD, bytes[1]);
    TEST_ASSERT_EQUAL_HEX8(0xBE, bytes[2]);
    TEST_ASSERT_EQUAL_HEX8(0xEF, bytes[3]);

    /* Space and prefix handling */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_HexToBytes("0x1A:0x2B 0x3C", 0, bytes, sizeof(bytes), &count));
    TEST_ASSERT_EQUAL_UINT(3, count);
    TEST_ASSERT_EQUAL_HEX8(0x1A, bytes[0]);
    TEST_ASSERT_EQUAL_HEX8(0x2B, bytes[1]);
    TEST_ASSERT_EQUAL_HEX8(0x3C, bytes[2]);
}

void run_string_tests(void) {
    RUN_TEST(test_String_Length_And_Copy);
    RUN_TEST(test_String_Compare_And_Trim);
    RUN_TEST(test_String_Search_And_Case);
    RUN_TEST(test_Hex_Conversions);
}
