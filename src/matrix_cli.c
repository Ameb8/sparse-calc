#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "../include/matrix.h"

int get_user_int(const char *prompt, int lower, int upper) {
    char buffer[100];
    int value;
    char *endptr;

    while (1) {
        printf("%s [%d-%d]: ", prompt, lower, upper);
        
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            // Input error
            fprintf(stderr, "Error reading input. Please try again.\n");
            continue;
        }

        // Remove trailing newline
        buffer[strcspn(buffer, "\n")] = 0;

        // Check if input is empty
        if (strlen(buffer) == 0) {
            printf("Input cannot be empty.\n");
            continue;
        }

        value = strtol(buffer, &endptr, 10);

        // Check for conversion errors
        if (*endptr != '\0') {
            printf("Invalid input. Please enter an integer.\n");
            continue;
        }

        if (value < lower || value >= upper) {
            printf("Value out of bounds. Must be between %d and %d.\n", lower, upper);
            continue;
        }

        return value;
    }
}

Matrix* get_user_matrix(char* matrix_name) {
    printf("\nPlease enter the dimensions of matrix %s\n", matrix_name);
    int rows = get_user_int("Height:\t" , 1, INT_MAX);
    int cols = get_user_int("Width:\t", 1, INT_MAX);
    Matrix* matrix = matrix_create(rows, cols);
    return matrix;
}