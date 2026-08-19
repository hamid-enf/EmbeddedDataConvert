/* ==========================================
    Unity Project - A Test Framework for C
    Copyright (c) 2007-21 Mike Karlesky, Mark VanderVoord, Greg Williams
    [MIT License]
========================================== */

#ifndef UNITY_FRAMEWORK_H
#define UNITY_FRAMEWORK_H

#define UNITY_VERSION_MAJOR    2
#define UNITY_VERSION_MINOR    5
#define UNITY_VERSION_BUILD    4

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char* TestFile;
    const char* CurrentTestName;
    uint32_t CurrentTestLineNumber;
    uint32_t NumberOfTests;
    uint32_t TestFailures;
    uint32_t TestIgnores;
    uint32_t CurrentTestFailed;
    uint32_t CurrentTestIgnored;
} UnityStorageType;

extern UnityStorageType Unity;

void UnityBegin(const char* filename);
int  UnityEnd(void);
void UnityConcludeTest(void);
void UnityDefaultTestRun(void (*Func)(void), const char* FuncName, const int FuncLineNum);

void UnityAssertEqualNumber(int64_t expected, int64_t actual, const char* msg, uint32_t lineNumber);
void UnityAssertEqualUnsigned(uint64_t expected, uint64_t actual, const char* msg, uint32_t lineNumber);
void UnityAssertEqualFloat(double expected, double actual, double delta, const char* msg, uint32_t lineNumber);
void UnityAssertEqualString(const char* expected, const char* actual, const char* msg, uint32_t lineNumber);
void UnityAssertEqualMemory(const void* expected, const void* actual, size_t length, const char* msg, uint32_t lineNumber);
void UnityFail(const char* msg, uint32_t lineNumber);

#define TEST_PROTECT() 1
#define TEST_ABORT() return

#define UNITY_TEST_ASSERT(condition, line, message) \
    if (!(condition)) { UnityFail((message), (uint32_t)(line)); TEST_ABORT(); }

#define TEST_ASSERT(condition)                               UNITY_TEST_ASSERT((condition), __LINE__, "Expression Evaluated To FALSE")
#define TEST_ASSERT_TRUE(condition)                          UNITY_TEST_ASSERT((condition), __LINE__, "Expected TRUE Was FALSE")
#define TEST_ASSERT_FALSE(condition)                         UNITY_TEST_ASSERT(!(condition), __LINE__, "Expected FALSE Was TRUE")
#define TEST_ASSERT_NULL(pointer)                            UNITY_TEST_ASSERT((pointer) == NULL, __LINE__, "Expected NULL")
#define TEST_ASSERT_NOT_NULL(pointer)                        UNITY_TEST_ASSERT((pointer) != NULL, __LINE__, "Expected Non-NULL")
#define TEST_ASSERT_EQUAL_INT(expected, actual)              UnityAssertEqualNumber((int64_t)(expected), (int64_t)(actual), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_HEX8(expected, actual)             UnityAssertEqualUnsigned((uint64_t)(uint8_t)(expected), (uint64_t)(uint8_t)(actual), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_HEX16(expected, actual)            UnityAssertEqualUnsigned((uint64_t)(uint16_t)(expected), (uint64_t)(uint16_t)(actual), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_HEX32(expected, actual)            UnityAssertEqualUnsigned((uint64_t)(uint32_t)(expected), (uint64_t)(uint32_t)(actual), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_HEX64(expected, actual)            UnityAssertEqualUnsigned((uint64_t)(expected), (uint64_t)(actual), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_UINT(expected, actual)             UnityAssertEqualUnsigned((uint64_t)(expected), (uint64_t)(actual), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_FLOAT(expected, actual)            UnityAssertEqualFloat((double)(expected), (double)(actual), 0.0001, NULL, __LINE__)
#define TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual)    UnityAssertEqualFloat((double)(expected), (double)(actual), (double)(delta), NULL, __LINE__)
#define TEST_ASSERT_DOUBLE_WITHIN(delta, expected, actual)   UnityAssertEqualFloat((double)(expected), (double)(actual), (double)(delta), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_DOUBLE(expected, actual)           UnityAssertEqualFloat((double)(expected), (double)(actual), 0.000001, NULL, __LINE__)
#define TEST_ASSERT_EQUAL_STRING(expected, actual)           UnityAssertEqualString((expected), (actual), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_MEMORY(expected, actual, len)      UnityAssertEqualMemory((expected), (actual), (len), NULL, __LINE__)
#define TEST_FAIL_MESSAGE(message)                           UnityFail((message), __LINE__)

#define RUN_TEST(func) UnityDefaultTestRun(func, #func, __LINE__)

void setUp(void);
void tearDown(void);

#ifdef __cplusplus
}
#endif

#endif /* UNITY_FRAMEWORK_H */
