/**
 * @file benchmark_main.c
 * @brief Benchmark suite for EmbeddedDataConvert measuring execution time and throughput.
 */

#define _POSIX_C_SOURCE 199309L
#include "edc.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

#define BENCH_ITERATIONS  100000

static double GetTimeSeconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + ((double)ts.tv_nsec * 1e-9);
}

static void Benchmark_Integer(void) {
    char buf[32];
    uint64_t u64_val = 18446744073709551ULL;
    int64_t parsed_i64 = 0;
    const char *test_str = "-922337203685477";
    volatile EDC_Status st;

    /* U64 to String */
    double t0 = GetTimeSeconds();
    for (int i = 0; i < BENCH_ITERATIONS; i++) {
        st = EDC_U64_ToStringEx(u64_val + (uint64_t)i, buf, sizeof(buf), 10, true, 0, NULL);
        (void)st;
    }
    double t1 = GetTimeSeconds();
    double ns_per_op_tostr = ((t1 - t0) / (double)BENCH_ITERATIONS) * 1e9;

    /* String to I64 */
    t0 = GetTimeSeconds();
    for (int i = 0; i < BENCH_ITERATIONS; i++) {
        st = EDC_StringToI64(test_str, 0, 10, &parsed_i64, NULL);
        (void)st;
    }
    t1 = GetTimeSeconds();
    double ns_per_op_parse = ((t1 - t0) / (double)BENCH_ITERATIONS) * 1e9;

    printf(" | %-32s | %8.1f ns | %10.2f MOps/s |\n", "Integer U64 -> String (Dec)", ns_per_op_tostr, 1000.0 / ns_per_op_tostr);
    printf(" | %-32s | %8.1f ns | %10.2f MOps/s |\n", "String -> Integer I64 (Dec)", ns_per_op_parse, 1000.0 / ns_per_op_parse);
}

static void Benchmark_Float(void) {
    char buf[64];
    double d_val = 12345.6789;
    double parsed_d = 0.0;
    const char *float_str = "12345.6789";
    volatile EDC_Status st;

    /* Double to String */
    double t0 = GetTimeSeconds();
    for (int i = 0; i < BENCH_ITERATIONS; i++) {
        st = EDC_Double_ToString(d_val + (double)i * 0.001, buf, sizeof(buf), 4, NULL);
        (void)st;
    }
    double t1 = GetTimeSeconds();
    double ns_per_op_tostr = ((t1 - t0) / (double)BENCH_ITERATIONS) * 1e9;

    /* String to Double */
    t0 = GetTimeSeconds();
    for (int i = 0; i < BENCH_ITERATIONS; i++) {
        st = EDC_StringToDouble(float_str, 0, &parsed_d, NULL);
        (void)st;
    }
    t1 = GetTimeSeconds();
    double ns_per_op_parse = ((t1 - t0) / (double)BENCH_ITERATIONS) * 1e9;

    printf(" | %-32s | %8.1f ns | %10.2f MOps/s |\n", "Double -> String (4 Decimals)", ns_per_op_tostr, 1000.0 / ns_per_op_tostr);
    printf(" | %-32s | %8.1f ns | %10.2f MOps/s |\n", "String -> Double", ns_per_op_parse, 1000.0 / ns_per_op_parse);
}

static void Benchmark_FixedPoint(void) {
    int32_t q = 0;
    char buf[32];
    volatile EDC_Status st;

    double t0 = GetTimeSeconds();
    for (int i = 0; i < BENCH_ITERATIONS; i++) {
        st = EDC_Float_To_Q(1.5f + (float)i * 0.0001f, 15, false, &q);
        (void)st;
    }
    double t1 = GetTimeSeconds();
    double ns_per_op_q = ((t1 - t0) / (double)BENCH_ITERATIONS) * 1e9;

    t0 = GetTimeSeconds();
    for (int i = 0; i < BENCH_ITERATIONS; i++) {
        st = EDC_Q_ToString(49152 + i, 15, buf, sizeof(buf), 2, NULL);
        (void)st;
    }
    t1 = GetTimeSeconds();
    double ns_per_op_qstr = ((t1 - t0) / (double)BENCH_ITERATIONS) * 1e9;

    printf(" | %-32s | %8.1f ns | %10.2f MOps/s |\n", "Float -> Q15 Fixed Point", ns_per_op_q, 1000.0 / ns_per_op_q);
    printf(" | %-32s | %8.1f ns | %10.2f MOps/s |\n", "Q15 -> String (No Float FPU)", ns_per_op_qstr, 1000.0 / ns_per_op_qstr);
}

static void Benchmark_Unicode(void) {
    const uint8_t sample_utf8[] = "س"; /* 2 bytes */
    uint32_t cp = 0;
    size_t consumed = 0;
    uint8_t out_enc[4];
    size_t out_len = 0;
    volatile EDC_Status st;

    double t0 = GetTimeSeconds();
    for (int i = 0; i < BENCH_ITERATIONS; i++) {
        st = EDC_UTF8_DecodeChar(sample_utf8, 2, &cp, &consumed);
        (void)st;
    }
    double t1 = GetTimeSeconds();
    double ns_per_op_dec = ((t1 - t0) / (double)BENCH_ITERATIONS) * 1e9;

    t0 = GetTimeSeconds();
    for (int i = 0; i < BENCH_ITERATIONS; i++) {
        st = EDC_UTF8_EncodeChar(0x0633, out_enc, sizeof(out_enc), &out_len);
        (void)st;
    }
    t1 = GetTimeSeconds();
    double ns_per_op_enc = ((t1 - t0) / (double)BENCH_ITERATIONS) * 1e9;

    printf(" | %-32s | %8.1f ns | %10.2f MOps/s |\n", "UTF-8 Decode Character (2-byte)", ns_per_op_dec, 1000.0 / ns_per_op_dec);
    printf(" | %-32s | %8.1f ns | %10.2f MOps/s |\n", "UTF-8 Encode Character (2-byte)", ns_per_op_enc, 1000.0 / ns_per_op_enc);
}

static void Benchmark_Persian(void) {
    char buf[64];
    char inplace_buf[64];
    volatile EDC_Status st;

    double t0 = GetTimeSeconds();
    for (int i = 0; i < BENCH_ITERATIONS; i++) {
        memcpy(inplace_buf, "۱۲۳۴۵۶۷۸۹۰", 21);
        size_t len = 20;
        st = EDC_PersianDigitsToEnglish_InPlace(inplace_buf, &len);
        (void)st;
    }
    double t1 = GetTimeSeconds();
    double ns_per_op_digits = ((t1 - t0) / (double)BENCH_ITERATIONS) * 1e9;

    const char *persian_text = "شركت بازرگاني و صنعتي كرمان";
    t0 = GetTimeSeconds();
    for (int i = 0; i < BENCH_ITERATIONS; i++) {
        st = EDC_Persian_Normalize(persian_text, 0, buf, sizeof(buf), EDC_NORM_DEFAULT_PERSIAN, NULL);
        (void)st;
    }
    t1 = GetTimeSeconds();
    double ns_per_op_norm = ((t1 - t0) / (double)BENCH_ITERATIONS) * 1e9;

    printf(" | %-32s | %8.1f ns | %10.2f MOps/s |\n", "Persian Digits In-Place (10 dg)", ns_per_op_digits, 1000.0 / ns_per_op_digits);
    printf(" | %-32s | %8.1f ns | %10.2f MOps/s |\n", "Persian Character Normalization", ns_per_op_norm, 1000.0 / ns_per_op_norm);
}

static void Benchmark_GPS(void) {
    const char *gga = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
    EDC_GPS_Sentence gps;
    volatile EDC_Status st;

    double t0 = GetTimeSeconds();
    for (int i = 0; i < BENCH_ITERATIONS; i++) {
        st = EDC_GPS_ParseNMEA(gga, 0, &gps);
        (void)st;
    }
    double t1 = GetTimeSeconds();
    double ns_per_op_gps = ((t1 - t0) / (double)BENCH_ITERATIONS) * 1e9;

    printf(" | %-32s | %8.1f ns | %10.2f MOps/s |\n", "NMEA $GPGGA Sentence Parse", ns_per_op_gps, 1000.0 / ns_per_op_gps);
}

static void Benchmark_Hex(void) {
    uint8_t raw_bytes[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    char hex_buf[40];
    uint8_t out_bytes[16];
    size_t out_len = 0;
    volatile EDC_Status st;

    double t0 = GetTimeSeconds();
    for (int i = 0; i < BENCH_ITERATIONS; i++) {
        st = EDC_BytesToHex(raw_bytes, 16, hex_buf, sizeof(hex_buf), true, false, NULL);
        (void)st;
    }
    double t1 = GetTimeSeconds();
    double ns_per_op_btohex = ((t1 - t0) / (double)BENCH_ITERATIONS) * 1e9;

    t0 = GetTimeSeconds();
    for (int i = 0; i < BENCH_ITERATIONS; i++) {
        st = EDC_HexToBytes("0102030405060708090A0B0C0D0E0F10", 32, out_bytes, sizeof(out_bytes), &out_len);
        (void)st;
    }
    t1 = GetTimeSeconds();
    double ns_per_op_hextob = ((t1 - t0) / (double)BENCH_ITERATIONS) * 1e9;

    printf(" | %-32s | %8.1f ns | %10.2f MOps/s |\n", "Bytes -> Hex (16 Bytes)", ns_per_op_btohex, 1000.0 / ns_per_op_btohex);
    printf(" | %-32s | %8.1f ns | %10.2f MOps/s |\n", "Hex -> Bytes (16 Bytes)", ns_per_op_hextob, 1000.0 / ns_per_op_hextob);
}

int main(void) {
    printf("\n=======================================================================\n");
    printf(" EmbeddedDataConvert (EDC) Performance & Benchmark Suite\n");
    printf(" Benchmark Iterations: %d per benchmark\n", BENCH_ITERATIONS);
    printf("=======================================================================\n");
    printf(" | %-32s | %11s | %17s |\n", "Operation Benchmark", "Avg Latency", "Throughput");
    printf(" |----------------------------------|-------------|-------------------|\n");

    Benchmark_Integer();
    Benchmark_Float();
    Benchmark_FixedPoint();
    Benchmark_Unicode();
    Benchmark_Persian();
    Benchmark_GPS();
    Benchmark_Hex();

    printf("=======================================================================\n\n");
    return 0;
}
