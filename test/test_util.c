#ifdef TEST

#include "../include/matrix.h"
#include "../include/map_iterator.h"
#include "test_util.h"


int failed_tests = 0;
int total_tests = 0;


void reset_test() {
    failed_tests = 0;
    total_tests = 0;
}

char* end_test(char* test_name) {
    printf("%s:\t%d out of %d tests passed\n", test_name, total_tests - failed_tests, total_tests);
    reset_test();
}

void test_passed() {
    total_tests++;
}

void test_failed() {
    total_tests++;
    failed_tests++;
}


void matrix_assert_eq(Matrix* expected, Matrix* actual) {

    // Compare number of non-zero elements
    ASSERT_INT_EQ(matrix_size(expected), matrix_size(actual));

    // Compare dimensions
    ASSERT_INT_EQ(expected->rows, actual->rows);
    ASSERT_INT_EQ(expected->cols, actual->cols);

    MapIterator exp_vals = map_iterator_create(expected->vals);

    // Compare element values and location
    while(map_iterator_has_next(&exp_vals)) {
        // Get expected  and actual value at location
        int row, col;
        double exp_val;
        map_iterator_next(&exp_vals, &row, &col, &exp_val);
        double actual_val = matrix_get(actual, row, col);

        ASSERT_DOUBLE_EQ(exp_val, actual_val);
    }
}

#endif