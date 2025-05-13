#ifdef TEST

#include <stdlib.h> 
#include "../include/runtime_data.h"
#include "test_util.h"


void test_matrix_set() {
    Matrix* m = matrix_create(3, 3);
    matrix_set(m, 1, 2, 42.5);
    double val = map_get(m->vals, 1, 2);
    ASSERT_DOUBLE_EQ(val, 42.5);
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
    ASSERT_DOUBLE_EQ(map_get(c->vals, 0, 0), 58.0);
    ASSERT_DOUBLE_EQ(map_get(c->vals, 0, 1), 64.0);
    ASSERT_DOUBLE_EQ(map_get(c->vals, 1, 0), 139.0);
    ASSERT_DOUBLE_EQ(map_get(c->vals, 1, 1), 154.0); 

    // Clean up
    matrix_free(a);
    matrix_free(b);
    matrix_free(c);

    Matrix* x = rd_get_matrix("A");
    Matrix* y = rd_get_matrix("B");

    Matrix* exp_res = matrix_create(5, 5);
    matrix_set(exp_res, 0, 2, 126);
    matrix_set(exp_res, 1, 4, 80);
    matrix_set(exp_res, 2, 1, 14);
    matrix_set(exp_res, 3, 3, 112);
    matrix_set(exp_res, 4, 0, 22);

    Matrix* result = matrix_mult(x, y);
    ASSERT_MATRIX_EQ(exp_res, result);


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
    ASSERT_DOUBLE_EQ(map_get(result->vals, 0, 0), 6.0);
    ASSERT_DOUBLE_EQ(map_get(result->vals, 0, 1), 8.0);
    ASSERT_DOUBLE_EQ(map_get(result->vals, 1, 0), 10.0);
    ASSERT_DOUBLE_EQ(map_get(result->vals, 1, 1), 12.0);

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
    ASSERT_DOUBLE_EQ(map_get(result->vals, 0, 0), 2.0);  // 1.0 * 2 = 2.0
    ASSERT_DOUBLE_EQ(map_get(result->vals, 0, 1), 4.0);  // 2.0 * 2 = 4.0
    ASSERT_DOUBLE_EQ(map_get(result->vals, 1, 0), 6.0);  // 3.0 * 2 = 6.0
    ASSERT_DOUBLE_EQ(map_get(result->vals, 1, 1), 8.0);  // 4.0 * 2 = 8.0

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

    Matrix* result = matrix_sub(A, B);

    ASSERT_DOUBLE_EQ(map_get(result->vals, 0, 0), 3.0); // 5 - 2
    ASSERT_DOUBLE_EQ(map_get(result->vals, 1, 1), 3.0); // 3 - 0
    ASSERT_DOUBLE_EQ(map_get(result->vals, 1, 0), -4.0); // 0 - 4
    ASSERT_DOUBLE_EQ(map_get(result->vals, 0, 1), 0.0); // 0 - 0

    matrix_free(A);
    matrix_free(B);
    matrix_free(result);
}

void test_matrix_transpose() {
    Matrix* a = rd_get_matrix("A");
    Matrix* exp_res = matrix_create(5, 5);
    matrix_set(exp_res, 0, 2, 1);
    matrix_set(exp_res, 1, 1, 4);
    matrix_set(exp_res, 2, 0, 7);
    matrix_set(exp_res, 3, 4, 11);
    matrix_set(exp_res, 4, 3, 14);

    Matrix* result = matrix_transpose(a);

    ASSERT_MATRIX_EQ(exp_res, result);

    free(exp_res);
    free(result);
}

void test_matrix() {
    test_matrix_set();
    test_matrix_mult();
    test_matrix_add();
    test_matrix_scalar_mult();
    test_matrix_subtract();
    test_matrix_transpose();
    end_test("Matrices");
}

#endif