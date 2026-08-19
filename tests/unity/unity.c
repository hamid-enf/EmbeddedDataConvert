#include "unity.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

UnityStorageType Unity;

void UnityBegin(const char* filename) {
    Unity.TestFile = filename;
    Unity.CurrentTestName = NULL;
    Unity.CurrentTestLineNumber = 0;
    Unity.NumberOfTests = 0;
    Unity.TestFailures = 0;
    Unity.TestIgnores = 0;
    Unity.CurrentTestFailed = 0;
    Unity.CurrentTestIgnored = 0;
    printf("\n======================================================\n");
    printf(" Running Test Suite: %s\n", filename);
    printf("======================================================\n");
}

int UnityEnd(void) {
    printf("------------------------------------------------------\n");
    printf(" Total Tests : %u\n", (unsigned int)Unity.NumberOfTests);
    printf(" Failures    : %u\n", (unsigned int)Unity.TestFailures);
    printf(" Ignored     : %u\n", (unsigned int)Unity.TestIgnores);
    if (Unity.TestFailures == 0) {
        printf(" Status      : ALL TESTS PASSED (100%% OK)\n");
    } else {
        printf(" Status      : FAILED (%u errors)\n", (unsigned int)Unity.TestFailures);
    }
    printf("======================================================\n\n");
    return (int)Unity.TestFailures;
}

void UnityDefaultTestRun(void (*Func)(void), const char* FuncName, const int FuncLineNum) {
    Unity.CurrentTestName = FuncName;
    Unity.CurrentTestLineNumber = (uint32_t)FuncLineNum;
    Unity.NumberOfTests++;
    Unity.CurrentTestFailed = 0;
    Unity.CurrentTestIgnored = 0;

    setUp();
    Func();
    tearDown();

    if (Unity.CurrentTestFailed == 0) {
        printf("  [ PASS ] %s\n", FuncName);
    } else {
        printf("  [ FAIL ] %s (Line %u)\n", FuncName, (unsigned int)Unity.CurrentTestLineNumber);
    }
}

void UnityFail(const char* msg, uint32_t lineNumber) {
    Unity.TestFailures++;
    Unity.CurrentTestFailed = 1;
    Unity.CurrentTestLineNumber = lineNumber;
    printf("    -> Failure at %s:%u: %s\n", Unity.TestFile, (unsigned int)lineNumber, msg ? msg : "Test Assertion Failed");
}

void UnityAssertEqualNumber(int64_t expected, int64_t actual, const char* msg, uint32_t lineNumber) {
    if (expected != actual) {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s Expected %lld Was %lld", msg ? msg : "", (long long)expected, (long long)actual);
        UnityFail(buf, lineNumber);
    }
}

void UnityAssertEqualUnsigned(uint64_t expected, uint64_t actual, const char* msg, uint32_t lineNumber) {
    if (expected != actual) {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s Expected 0x%llX (%llu) Was 0x%llX (%llu)",
                 msg ? msg : "",
                 (unsigned long long)expected, (unsigned long long)expected,
                 (unsigned long long)actual, (unsigned long long)actual);
        UnityFail(buf, lineNumber);
    }
}

void UnityAssertEqualFloat(double expected, double actual, double delta, const char* msg, uint32_t lineNumber) {
    double diff = fabs(expected - actual);
    if (diff > delta) {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s Expected %f Was %f (diff: %f > %f)",
                 msg ? msg : "", expected, actual, diff, delta);
        UnityFail(buf, lineNumber);
    }
}

void UnityAssertEqualString(const char* expected, const char* actual, const char* msg, uint32_t lineNumber) {
    if (expected == NULL && actual == NULL) return;
    if (expected == NULL || actual == NULL || strcmp(expected, actual) != 0) {
        char buf[256];
        snprintf(buf, sizeof(buf), "%s Expected \"%s\" Was \"%s\"",
                 msg ? msg : "", expected ? expected : "NULL", actual ? actual : "NULL");
        UnityFail(buf, lineNumber);
    }
}

void UnityAssertEqualMemory(const void* expected, const void* actual, size_t length, const char* msg, uint32_t lineNumber) {
    if (expected == NULL && actual == NULL) return;
    if (expected == NULL || actual == NULL || memcmp(expected, actual, length) != 0) {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s Memory mismatch over %u bytes", msg ? msg : "", (unsigned int)length);
        UnityFail(buf, lineNumber);
    }
}
