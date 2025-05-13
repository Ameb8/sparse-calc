#ifdef TEST

#include <stdio.h>
#include "test_data_struct.h"
#include "test_matrix.h"
#include "test_eval.h"
#include "test_util.h"
#include "test.h"


void run_tests() {
    test_data_struct();
    test_eval();
    test_matrix();
}

#endif