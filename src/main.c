#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/hash_map.h"
#include "../include/map_iterator.h"
#include "../include/matrix.h"
#include "../include/test.h"
#include "../include/matrix_cli.h"

void run_app() {
    printf("run_app() called\n");
    Matrix* a = get_user_matrix();

}


int main() {
#ifdef TEST
    run_tests();
#else
    run_app();
#endif
    return 0;
}
