cmake_minimum_required(VERSION 3.10)
project(EmbeddedDataConvert VERSION 1.0.0 LANGUAGES C)

# Set C99 Standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Compiler warnings and optimization for safety-critical embedded systems
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic)
endif()

# Include directories
include_directories(include)

# EDC Core Library Source Files
set(EDC_SOURCES
    src/edc_common.c
    src/edc_numeric.c
    src/edc_string.c
    src/edc_unicode.c
    src/edc_persian.c
    src/edc_gps.c
    src/edc_units.c
    src/edc_parser.c
    src/edc_buffer.c
)

add_library(edc STATIC ${EDC_SOURCES})
target_include_directories(edc PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Test Suite Target
option(EDC_BUILD_TESTS "Build EDC Unit Tests" ON)
if(EDC_BUILD_TESTS)
    enable_testing()
    add_executable(edc_tests
        tests/test_main.c
        tests/test_edc_numeric.c
        tests/test_edc_string.c
        tests/test_edc_unicode.c
        tests/test_edc_persian.c
        tests/test_edc_gps.c
        tests/test_edc_units.c
        tests/test_edc_parser.c
        tests/test_edc_buffer.c
        tests/unity/unity.c
    )
    target_include_directories(edc_tests PRIVATE tests/unity)
    target_link_libraries(edc_tests PRIVATE edc)
    add_test(NAME AllTests COMMAND edc_tests)
endif()

# Fuzz Testing Target
option(EDC_BUILD_FUZZ "Build EDC Fuzz Harness" ON)
if(EDC_BUILD_FUZZ)
    add_executable(edc_fuzz fuzz/fuzz_runner.c)
    target_link_libraries(edc_fuzz PRIVATE edc)
endif()

# Benchmark Target
option(EDC_BUILD_BENCH "Build EDC Benchmark Suite" ON)
if(EDC_BUILD_BENCH)
    add_executable(edc_bench benchmarks/benchmark_main.c)
    target_link_libraries(edc_bench PRIVATE edc)
endif()

# Examples
option(EDC_BUILD_EXAMPLES "Build EDC Examples" ON)
if(EDC_BUILD_EXAMPLES)
    add_executable(example_stm32 examples/stm32/main_stm32_example.c)
    target_link_libraries(example_stm32 PRIVATE edc)

    add_executable(example_esp32 examples/esp32/main_esp32_example.c)
    target_link_libraries(example_esp32 PRIVATE edc)

    add_executable(example_gps_tracker examples/gps_tracker/main_gps_tracker.c)
    target_link_libraries(example_gps_tracker PRIVATE edc)

    add_executable(example_uart_streaming examples/uart_streaming/main_uart_streaming.c)
    target_link_libraries(example_uart_streaming PRIVATE edc)
endif()
