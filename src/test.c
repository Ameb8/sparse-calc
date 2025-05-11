#ifdef TEST

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include "../include/list.h"
#include "../include/hash_map.h"
#include "../include/test.h"
#include "../include/matrix.h"

int total_tests = 0;
int failed_tests = 0;

#define ASSERT_INT_EQ(actual, expected) do { \
    total_tests++; \
    if ((actual) != (expected)) { \
        failed_tests++; \
        printf("FAILED in %s: %s != %s at %s:%d\n", __func__, #actual, #expected, __FILE__, __LINE__); \
    } \
} while (0)

#define ASSERT_DOUBLE_EQ(actual, expected, epsilon) do { \
    total_tests++; \
    if (fabs((actual) - (expected)) > (epsilon)) { \
        failed_tests++; \
        printf("FAILED: %s == %s (%.6f != %.6f) at %s:%d\n", \
            #actual, #expected, (actual), (expected), __FILE__, __LINE__); \
    } \
} while (0)


void test_list_remove_val() {
    List* list = list_create();
    
    // Append values
    list_append(list, 0, 0, 1.1);
    list_append(list, 0, 1, 2.2);
    list_append(list, 1, 0, 3.3);
    
    // Confirm initial size
    ASSERT_INT_EQ(list->size, 3);

    // Remove middle element
    double removed_val = list_remove_val(list, 0, 1);
    ASSERT_DOUBLE_EQ(removed_val, 2.2, 1e-6);
    ASSERT_INT_EQ(list->size, 2);
    ASSERT_DOUBLE_EQ(list_get_val(list, 0, 1), 0.0, 1e-6);

    // Remove head
    removed_val = list_remove_val(list, 0, 0);
    ASSERT_DOUBLE_EQ(removed_val, 1.1, 1e-6);
    ASSERT_INT_EQ(list->size, 1);
    ASSERT_DOUBLE_EQ(list_get_val(list, 0, 0), 0.0, 1e-6);

    // Remove tail
    removed_val = list_remove_val(list, 1, 0);
    ASSERT_DOUBLE_EQ(removed_val, 3.3, 1e-6);
    ASSERT_INT_EQ(list->size, 0);
    ASSERT_DOUBLE_EQ(list_get_val(list, 1, 0), 0.0, 1e-6);

    // Remove from empty list
    removed_val = list_remove_val(list, 5, 5);
    ASSERT_DOUBLE_EQ(removed_val, -DBL_MAX, 1e-6);

    free(list); // Assuming this frees the list correctly
}


void test_map_get_empty() {
    HashMap* map = map_create();

    // Check for a few different (row, col) pairs
    ASSERT_INT_EQ(map_get(map, 0, 0), 0);
    ASSERT_INT_EQ(map_get(map, 5, 10), 0);
    ASSERT_INT_EQ(map_get(map, -3, 7), 0);
    ASSERT_INT_EQ(map_get(map, 1000, 1000), 0);

    //printf("All map_get tests for unset keys passed.\n");

    free_hash_map(map);
}

void test_matrix_set() {
    Matrix* m = matrix_create(3, 3);
    matrix_set(m, 1, 2, 42.5);
    double val = map_get(m->vals, 1, 2);
    ASSERT_DOUBLE_EQ(val, 42.5, 1e-6);
}

void test_matrix_mult() {
    // Create and set up matrix A
    Matrix* a = matrix_create(2, 3); // 2x3 matrix
    matrix_set(a, 0, 0, 1);
    matrix_set(a, 0, 1, 2);
    matrix_set(a, 0, 2, 3);
    matrix_set(a, 1, 0, 4);
    matrix_set(a, 1, 1, 5);
    matrix_set(a, 1, 2, 6);

    // Create and set up matrix B
    Matrix* b = matrix_create(3, 2); // 3x2 matrix
    matrix_set(b, 0, 0, 7);
    matrix_set(b, 0, 1, 8);
    matrix_set(b, 1, 0, 9);
    matrix_set(b, 1, 1, 10);
    matrix_set(b, 2, 0, 11);
    matrix_set(b, 2, 1, 12);

    // Perform matrix multiplication: C = A * B
    Matrix* c = matrix_mult(a, b);

    // Verify the results (expected values after multiplying A * B)
    ASSERT_DOUBLE_EQ(map_get(c->vals, 0, 0), 58.0, 1e-6); // (1*7 + 2*9 + 3*11)
    ASSERT_DOUBLE_EQ(map_get(c->vals, 0, 1), 64.0, 1e-6); // (1*8 + 2*10 + 3*12)
    ASSERT_DOUBLE_EQ(map_get(c->vals, 1, 0), 139.0, 1e-6); // (4*7 + 5*9 + 6*11)
    ASSERT_DOUBLE_EQ(map_get(c->vals, 1, 1), 154.0, 1e-6); // (4*8 + 5*10 + 6*12)

    // Clean up
    matrix_free(a);
    matrix_free(b);
    matrix_free(c);
}

void test_matrix_add() {
    // Create two 2x2 matrices
    Matrix* matrix_a = matrix_create(2, 2);
    Matrix* matrix_b = matrix_create(2, 2);

    // Populate matrix A with some values
    matrix_set(matrix_a, 0, 0, 1.0);
    matrix_set(matrix_a, 0, 1, 2.0);
    matrix_set(matrix_a, 1, 0, 3.0);
    matrix_set(matrix_a, 1, 1, 4.0);

    // Populate matrix B with some values
    matrix_set(matrix_b, 0, 0, 5.0);
    matrix_set(matrix_b, 0, 1, 6.0);
    matrix_set(matrix_b, 1, 0, 7.0);
    matrix_set(matrix_b, 1, 1, 8.0);

    // Perform the matrix addition
    Matrix* result = matrix_add(matrix_a, matrix_b);

    // Check the values in the result matrix
    ASSERT_DOUBLE_EQ(map_get(result->vals, 0, 0), 6.0, 1e-6);
    ASSERT_DOUBLE_EQ(map_get(result->vals, 0, 1), 8.0, 1e-6);
    ASSERT_DOUBLE_EQ(map_get(result->vals, 1, 0), 10.0, 1e-6);
    ASSERT_DOUBLE_EQ(map_get(result->vals, 1, 1), 12.0, 1e-6);

    // Free the matrices after the test
    matrix_free(matrix_a);
    matrix_free(matrix_b);
    matrix_free(result);
}

void test_matrix_scalar_mult() {
    // Step 1: Create a matrix and set initial values
    Matrix* matrix = matrix_create(2, 2);  // Create a 2x2 matrix
    matrix_set(matrix, 0, 0, 1.0);  // Set value at (0, 0) to 1.0
    matrix_set(matrix, 0, 1, 2.0);  // Set value at (0, 1) to 2.0
    matrix_set(matrix, 1, 0, 3.0);  // Set value at (1, 0) to 3.0
    matrix_set(matrix, 1, 1, 4.0);  // Set value at (1, 1) to 4.0

    // Step 2: Perform scalar multiplication with scalar = 2.0
    Matrix* result = matrix_scalar_mult(matrix, 2.0);

    // Step 3: Test the result using assertions
    // Assert the values after scalar multiplication
    ASSERT_DOUBLE_EQ(map_get(result->vals, 0, 0), 2.0, 0.0001);  // 1.0 * 2 = 2.0
    ASSERT_DOUBLE_EQ(map_get(result->vals, 0, 1), 4.0, 0.0001);  // 2.0 * 2 = 4.0
    ASSERT_DOUBLE_EQ(map_get(result->vals, 1, 0), 6.0, 0.0001);  // 3.0 * 2 = 6.0
    ASSERT_DOUBLE_EQ(map_get(result->vals, 1, 1), 8.0, 0.0001);  // 4.0 * 2 = 8.0

    // Step 4: Clean up
    matrix_free(matrix);
    matrix_free(result);
}

void test_matrix_subtract() {
    Matrix* A = matrix_create(2, 2);
    Matrix* B = matrix_create(2, 2);

    matrix_set(A, 0, 0, 5.0);
    matrix_set(A, 1, 1, 3.0);

    matrix_set(B, 0, 0, 2.0);
    matrix_set(B, 1, 0, 4.0);

    Matrix* result = subtract(A, B);

    ASSERT_DOUBLE_EQ(map_get(result->vals, 0, 0), 3.0, 1e-6); // 5 - 2
    ASSERT_DOUBLE_EQ(map_get(result->vals, 1, 1), 3.0, 1e-6); // 3 - 0
    ASSERT_DOUBLE_EQ(map_get(result->vals, 1, 0), -4.0, 1e-6); // 0 - 4
    ASSERT_DOUBLE_EQ(map_get(result->vals, 0, 1), 0.0, 1e-6); // 0 - 0

    matrix_free(A);
    matrix_free(B);
    matrix_free(result);
}


void run_tests() {
    test_list_remove_val();
    test_map_get_empty();
    test_matrix_set();
    test_matrix_mult();
    test_matrix_add();
    test_matrix_scalar_mult();
    test_matrix_subtract();
    printf("\n%d out of %d tests passed\n", total_tests - failed_tests, total_tests);
}

#endif
