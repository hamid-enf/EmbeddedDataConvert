#include "unity.h"
#include "edc_numeric.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_Integer_ToString_Basic(void) {
    char buf[32];
    size_t len = 0;

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_U64_ToStringEx(0, buf, sizeof(buf), 10, true, 0, &len));
    TEST_ASSERT_EQUAL_STRING("0", buf);
    TEST_ASSERT_EQUAL_UINT(1, len);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_I64_ToStringEx(-1, buf, sizeof(buf), 10, true, 0, &len));
    TEST_ASSERT_EQUAL_STRING("-1", buf);
    TEST_ASSERT_EQUAL_UINT(2, len);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_U64_ToStringEx(12345, buf, sizeof(buf), 10, true, 0, &len));
    TEST_ASSERT_EQUAL_STRING("12345", buf);
    TEST_ASSERT_EQUAL_UINT(5, len);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_I64_ToStringEx(-9876543210LL, buf, sizeof(buf), 10, true, 0, &len));
    TEST_ASSERT_EQUAL_STRING("-9876543210", buf);
}

void test_Integer_ToString_Limits(void) {
    char buf[64];

    /* UINT64_MAX: 18446744073709551615 */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_U64_ToString(18446744073709551615ULL, buf, sizeof(buf), 10));
    TEST_ASSERT_EQUAL_STRING("18446744073709551615", buf);

    /* INT64_MAX: 9223372036854775807 */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_I64_ToString(9223372036854775807LL, buf, sizeof(buf), 10));
    TEST_ASSERT_EQUAL_STRING("9223372036854775807", buf);

    /* INT64_MIN: -9223372036854775808 */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_I64_ToString(-9223372036854775807LL - 1LL, buf, sizeof(buf), 10));
    TEST_ASSERT_EQUAL_STRING("-9223372036854775808", buf);
}

void test_Integer_ToString_Bases(void) {
    char buf[64];

    /* Hexadecimal */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_ToHex(255, buf, sizeof(buf), true));
    TEST_ASSERT_EQUAL_STRING("FF", buf);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_ToHex(255, buf, sizeof(buf), false));
    TEST_ASSERT_EQUAL_STRING("ff", buf);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_ToHex(0xDEADBEEF, buf, sizeof(buf), true));
    TEST_ASSERT_EQUAL_STRING("DEADBEEF", buf);

    /* Hex with Padding */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_ToHexPadded(0x1A, buf, sizeof(buf), 4, true));
    TEST_ASSERT_EQUAL_STRING("001A", buf);

    /* Binary */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_ToBinary(255, buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_STRING("11111111", buf);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_ToBinary(10, buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_STRING("1010", buf);

    /* Octal */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_ToOctal(64, buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_STRING("100", buf);
}

void test_StringToInteger_Parsing(void) {
    uint64_t u64 = 0;
    int64_t i64 = 0;
    size_t consumed = 0;

    /* Decimal */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_StringToU64("12345", 0, 10, &u64, &consumed));
    TEST_ASSERT_EQUAL_UINT(12345, u64);
    TEST_ASSERT_EQUAL_UINT(5, consumed);

    /* Signed negative */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_StringToI64("  -9876", 0, 10, &i64, &consumed));
    TEST_ASSERT_EQUAL_INT(-9876, i64);

    /* Auto Hex */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_StringToU64("0xFF", 0, 0, &u64, &consumed));
    TEST_ASSERT_EQUAL_UINT(255, u64);

    /* Auto Binary */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_StringToU64("0b1111", 0, 0, &u64, &consumed));
    TEST_ASSERT_EQUAL_UINT(15, u64);

    /* Overflow check */
    TEST_ASSERT_EQUAL_INT(EDC_ERR_OVERFLOW, EDC_StringToU64("18446744073709551616", 0, 10, &u64, &consumed));
    TEST_ASSERT_EQUAL_INT(EDC_ERR_OVERFLOW, EDC_StringToI64("9223372036854775808", 0, 10, &i64, &consumed));
    TEST_ASSERT_EQUAL_INT(EDC_ERR_UNDERFLOW, EDC_StringToI64("-9223372036854775809", 0, 10, &i64, &consumed));

    /* Partial parse with trailing chars */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_StringToU64("123ABC", 0, 10, &u64, &consumed));
    TEST_ASSERT_EQUAL_UINT(123, u64);
    TEST_ASSERT_EQUAL_UINT(3, consumed);
}

void test_Float_Formatting_And_Parsing(void) {
    char buf[64];
    size_t len = 0;
    double d_val = 0.0;
    float f_val = 0.0f;

    /* Basic Float to String */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_Float_ToString(123.456f, buf, sizeof(buf), 2, &len));
    TEST_ASSERT_EQUAL_STRING("123.46", buf);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_Float_ToString(123.456f, buf, sizeof(buf), 4, &len));
    TEST_ASSERT_EQUAL_STRING("123.4560", buf);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_Double_ToString(-0.00789, buf, sizeof(buf), 5, &len));
    TEST_ASSERT_EQUAL_STRING("-0.00789", buf);

    /* Scientific Notation */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_Double_ToScientific(12345.67, buf, sizeof(buf), 2, &len));
    TEST_ASSERT_EQUAL_STRING("1.23e+04", buf);

    /* String to Double */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_StringToDouble("3.1415926", 0, &d_val, NULL));
    TEST_ASSERT_DOUBLE_WITHIN(0.000001, 3.1415926, d_val);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_StringToDouble("-125.43", 0, &d_val, NULL));
    TEST_ASSERT_DOUBLE_WITHIN(0.001, -125.43, d_val);

    /* Scientific String to Double */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_StringToDouble("1.25e-3", 0, &d_val, NULL));
    TEST_ASSERT_DOUBLE_WITHIN(0.00001, 0.00125, d_val);

    /* String to Float */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_StringToFloat("42.5", 0, &f_val, NULL));
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 42.5f, f_val);
}

void test_FixedPoint_Q_Conversions(void) {
    int32_t q = 0;
    float f = 0.0f;
    char buf[32];

    /* Float to Q15 */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_Float_To_Q15(1.5f, false, &q));
    TEST_ASSERT_EQUAL_INT(49152, q); /* 1.5 * 32768 = 49152 */

    /* Q15 to Float */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_Q15_To_Float(49152, &f));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.5f, f);

    /* Q to String without float runtime */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_Q_ToString(49152, 15, buf, sizeof(buf), 2, NULL));
    TEST_ASSERT_EQUAL_STRING("1.50", buf);

    /* String to Q */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_StringTo_Q("1.50", 0, 15, &q, NULL));
    TEST_ASSERT_EQUAL_INT(49152, q);

    /* Saturation test */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_Float_To_Q(100000.0f, 15, true, &q));
    TEST_ASSERT_EQUAL_INT(0x7FFFFFFF, q);
}

void test_Boolean_Conversions(void) {
    char buf[16];
    bool b = false;

    /* Bool to String */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_Bool_ToString(true, EDC_BOOL_FMT_TRUE_FALSE, buf, sizeof(buf), NULL));
    TEST_ASSERT_EQUAL_STRING("true", buf);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_Bool_ToString(false, EDC_BOOL_FMT_TRUE_FALSE, buf, sizeof(buf), NULL));
    TEST_ASSERT_EQUAL_STRING("false", buf);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_Bool_ToString(true, EDC_BOOL_FMT_ONE_ZERO, buf, sizeof(buf), NULL));
    TEST_ASSERT_EQUAL_STRING("1", buf);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_Bool_ToString(true, EDC_BOOL_FMT_YES_NO_UPPER, buf, sizeof(buf), NULL));
    TEST_ASSERT_EQUAL_STRING("YES", buf);

    /* String to Bool */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_StringToBool("true", 0, &b, NULL));
    TEST_ASSERT_TRUE(b);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_StringToBool("FALSE", 0, &b, NULL));
    TEST_ASSERT_FALSE(b);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_StringToBool("1", 0, &b, NULL));
    TEST_ASSERT_TRUE(b);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_StringToBool("0", 0, &b, NULL));
    TEST_ASSERT_FALSE(b);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_StringToBool("on", 0, &b, NULL));
    TEST_ASSERT_TRUE(b);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_StringToBool("off", 0, &b, NULL));
    TEST_ASSERT_FALSE(b);
}

void run_numeric_tests(void) {
    RUN_TEST(test_Integer_ToString_Basic);
    RUN_TEST(test_Integer_ToString_Limits);
    RUN_TEST(test_Integer_ToString_Bases);
    RUN_TEST(test_StringToInteger_Parsing);
    RUN_TEST(test_Float_Formatting_And_Parsing);
    RUN_TEST(test_FixedPoint_Q_Conversions);
    RUN_TEST(test_Boolean_Conversions);
}
