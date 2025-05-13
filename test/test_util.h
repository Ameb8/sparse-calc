#ifdef TEST

#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../include/matrix.h"

#define EPSILON 1e-6

char* end_test(char* test_name);
void test_passed();
void test_failed();
void matrix_assert_eq(Matrix* expected, Matrix* actual);


#define ASSERT_INT_EQ(actual, expected) do { \
    test_passed(); \
    if((actual) != (expected)) { \
        test_failed(); \
        printf("FAILED in %s: %s != %s at %s:%d\n", __func__, #actual, #expected, __FILE__, __LINE__); \
    } \
} while(0)

#define ASSERT_DOUBLE_EQ(actual, expected) do { \
    test_passed(); \
    if(fabs((actual) - (expected)) > (EPSILON)) { \
        test_failed(); \
        printf("FAILED in %s: %s == %s (%.6f != %.6f) at %s:%d\n", \
            __func__, #actual, #expected, (actual), (expected), __FILE__, __LINE__); \
    } \
} while(0)

#define ASSERT_STR_EQ(actual, expected) do { \
    test_passed(); \
    if((actual) == NULL || (expected) == NULL || strcmp((actual), (expected)) != 0) { \
        test_failed(); \
        printf("FAILED in %s: %s != %s (%s != %s) at %s:%d\n", \
            __func__, #actual, #expected, \
            (actual) ? (actual) : "NULL", \
            (expected) ? (expected) : "NULL", \
            __FILE__, __LINE__); \
    } \
} while(0)

#define ASSERT_MATRIX_EQ(expected, actual) \
    matrix_assert_eq((expected), (actual))

#endif
#endif

/*
#define ASSERT_INT_EQ(actual, expected) do { \
    total_tests++; \
    if ((actual) != (expected)) { \
        failed_tests++; \
        printf("FAILED in %s: %s != %s at %s:%d\n", __func__, #actual, #expected, __FILE__, __LINE__); \
    } \
} while (0)

#define ASSERT_DOUBLE_EQ(actual, expected, epsilon) do { \
    total_tests++; \
    if (fabs((actual) - (expected)) > (EPSILON)) { \
        failed_tests++; \
        printf("FAILED in %s: %s == %s (%.6f != %.6f) at %s:%d\n", \
            __func__, #actual, #expected, (actual), (expected), __FILE__, __LINE__); \
    } \
} while (0)

#define ASSERT_STR_EQ(actual, expected) do { \
    total_tests++; \
    if ((actual) == NULL || (expected) == NULL || strcmp((actual), (expected)) != 0) { \
        failed_tests++; \
        printf("FAILED in %s: %s != %s (%s != %s) at %s:%d\n", \
            __func__, #actual, #expected, \
            (actual) ? (actual) : "NULL", \
            (expected) ? (expected) : "NULL", \
            __FILE__, __LINE__); \
    } \
} while (0)

#define ASSERT_MATRIX_EQ(expected, actual) \
    matrix_assert_eq((expected), (actual), __FILE__, __LINE__, __func__)

*/

