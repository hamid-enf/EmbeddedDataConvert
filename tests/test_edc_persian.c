#include "unity.h"
#include "edc_persian.h"
#include <string.h>

void test_Persian_Digit_Conversion(void) {
    char out[64];
    size_t out_len = 0;

    /* Persian Digits -> English Digits */
    const char *persian_num = "۰۱۲۳۴۵۶۷۸۹";
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_PersianDigitsToEnglish(persian_num, 0, out, sizeof(out), &out_len));
    TEST_ASSERT_EQUAL_STRING("0123456789", out);
    TEST_ASSERT_EQUAL_UINT(10, out_len);

    /* Arabic Digits -> English Digits */
    const char *arabic_num = "٠١٢٣٤٥٦٧٨٩";
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_PersianDigitsToEnglish(arabic_num, 0, out, sizeof(out), &out_len));
    TEST_ASSERT_EQUAL_STRING("0123456789", out);

    /* Persian Float with Separators: "۱۲۳٫۴۵" -> "123.45" */
    const char *persian_float = "۱۲۳٫۴۵";
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_PersianDigitsToEnglish(persian_float, 0, out, sizeof(out), &out_len));
    TEST_ASSERT_EQUAL_STRING("123.45", out);

    /* In-place conversion */
    char inplace_buf[32] = "دما: ۲۵٫۶ درجه";
    size_t in_len = strlen(inplace_buf);
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_PersianDigitsToEnglish_InPlace(inplace_buf, &in_len));
    TEST_ASSERT_EQUAL_STRING("دما: 25.6 درجه", inplace_buf);

    /* English Digits -> Persian Digits */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_EnglishDigitsToPersian("123.45", 0, out, sizeof(out), &out_len));
    TEST_ASSERT_EQUAL_STRING("۱۲۳٫۴۵", out);

    /* English Digits -> Arabic Digits */
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_EnglishDigitsToArabic("123", 0, out, sizeof(out), &out_len));
    TEST_ASSERT_EQUAL_STRING("١٢٣", out);
}

void test_Persian_Normalization(void) {
    char out[128];
    size_t out_len = 0;

    /* Arabic Yeh & Kaf Normalization: "ي" -> "ی", "ك" -> "ک" */
    const char *arabic_text = "شركت بازرگاني";
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_Persian_Normalize(arabic_text, 0, out, sizeof(out), EDC_NORM_DEFAULT_PERSIAN, &out_len));
    TEST_ASSERT_EQUAL_STRING("شرکت بازرگانی", out);

    /* Teh Marbuta and Harakat removal: "مَجْمُوعَةٌ" -> "مجموعه" */
    const char *harakat_text = "مَجْمُوعَةٌ";
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_Persian_Normalize(harakat_text, 0, out, sizeof(out), EDC_NORM_DEFAULT_PERSIAN, &out_len));
    TEST_ASSERT_EQUAL_STRING("مجموعه", out);

    /* Tatweel removal: "ســـــلام" -> "سلام" */
    const char *tatweel_text = "ســـــلام";
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_Persian_Normalize(tatweel_text, 0, out, sizeof(out), EDC_NORM_REMOVE_TATWEEL, &out_len));
    TEST_ASSERT_EQUAL_STRING("سلام", out);
}

void test_Persian_Transliteration(void) {
    char out[128];
    size_t out_len = 0;

    /* "سلام دنیا" -> "salam dnya" or transliterated */
    const char *greeting = "سلام";
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_Persian_TransliterateToLatin(greeting, 0, out, sizeof(out), NULL, &out_len));
    TEST_ASSERT_EQUAL_STRING("slam", out); /* Note: vowels in unvocalized Persian are omitted in transliteration */

    /* "تهران" -> "thran" */
    const char *tehran = "تهران";
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_Persian_TransliterateToLatin(tehran, 0, out, sizeof(out), NULL, &out_len));
    TEST_ASSERT_EQUAL_STRING("thran", out);

    /* Custom transliteration options: uppercase first */
    EDC_TranslitOptions opts;
    EDC_Translit_GetDefaultOptions(&opts);
    opts.uppercase_first = true;
    TEST_ASSERT_EQUAL_INT(EDC_OK, EDC_Persian_TransliterateToLatin("پارس", 0, out, sizeof(out), &opts, &out_len));
    TEST_ASSERT_EQUAL_STRING("Pars", out);
}

void run_persian_tests(void) {
    RUN_TEST(test_Persian_Digit_Conversion);
    RUN_TEST(test_Persian_Normalization);
    RUN_TEST(test_Persian_Transliteration);
}
