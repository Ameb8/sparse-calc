#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/hash_map.h"
#include "../include/map_iterator.h"
#include "../include/matrix.h"
#include "../test/test.h"
#include "../include/matrix_cli.h"
#include "../include/parse_input.h"


#define MAX_INPUT_LENGTH 256

void run_app() {
    char input[MAX_INPUT_LENGTH];

    while (1) {
        printf(">");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            // Input error or EOF
            break;
        }

        // Remove newline character if present
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0) {
            break;
        }

        handle_input(input);
    }
}


int main() {
#ifdef TEST
    run_tests();
#else
    run_app();
#endif
    return 0;
}
