#include "unity.h"

extern void run_numeric_tests(void);
extern void run_string_tests(void);
extern void run_unicode_tests(void);
extern void run_persian_tests(void);
extern void run_gps_tests(void);
extern void run_units_tests(void);
extern void run_parser_tests(void);
extern void run_buffer_tests(void);

int main(void) {
    UnityBegin("EmbeddedDataConvert Complete Test Suite");

    run_numeric_tests();
    run_string_tests();
    run_unicode_tests();
    run_persian_tests();
    run_gps_tests();
    run_units_tests();
    run_parser_tests();
    run_buffer_tests();

    return UnityEnd();
}
