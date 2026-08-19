#include "unity.h"
#include "edc_buffer.h"

static uint8_t IncByte(uint8_t b) {
    return (uint8_t)(b + 1U);
}

void test_Buffer_Slice_And_InPlace(void) {
    uint8_t data[] = {1, 2, 3, 4, 5};

    EDC_ConstSlice c_slice = EDC_ConstSlice_Make(data, sizeof(data));
    TEST_ASSERT_EQUAL_UINT(5, c_slice.length);
    TEST_ASSERT_EQUAL_HEX8(1, c_slice.data[0]);

    EDC_MutSlice m_slice = EDC_MutSlice_Make(data, sizeof(data), sizeof(data));
    TEST_ASSERT_EQUAL_UINT(5, m_slice.length);
    TEST_ASSERT_EQUAL_UINT(5, m_slice.capacity);

    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_Buffer_ApplyInPlace(data, sizeof(data), IncByte));
    TEST_ASSERT_EQUAL_HEX8(2, data[0]);
    TEST_ASSERT_EQUAL_HEX8(3, data[1]);
    TEST_ASSERT_EQUAL_HEX8(4, data[2]);
    TEST_ASSERT_EQUAL_HEX8(5, data[3]);
    TEST_ASSERT_EQUAL_HEX8(6, data[4]);
}

void run_buffer_tests(void) {
    RUN_TEST(test_Buffer_Slice_And_InPlace);
}
