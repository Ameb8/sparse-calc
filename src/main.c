#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/hash_map.h"
#include "../include/map_iterator.h"
#include "../include/matrix.h"
#include "../include/test.h"

void run_app() {
    printf("Production version running\n\n\n");
    
    Matrix* a = matrix_create(10, 10); 

    for(int i = 0; i < 10; i++) {
        matrix_set(a, i, 9 - i,  ++i);
        matrix_print(a);
    }

    matrix_print(a);
    matrix_set(a, 3, 7, 0);
    printf("0 added\n");

}


int main() {
#ifdef TEST
    run_tests();
#else
    run_app();
#endif
    return 0;
}
