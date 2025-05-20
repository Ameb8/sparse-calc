#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "../include/matrix.h"
#include "../include/repository.h"

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

Matrix* get_matrix_user(char* matrix_name) {
    printf("\nPlease enter the dimensions of matrix %s\n", matrix_name);
    int rows = get_user_int("Height:\t" , 1, INT_MAX);
    int cols = get_user_int("Width:\t", 1, INT_MAX);
    Matrix* matrix = matrix_create(rows, cols);
    return matrix;
}


// Helper to trim leading and trailing whitespace from a word
char* trim(const char* str) {
    if(!str)
        return NULL;


    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) 
        return strdup("");  // all spaces

    const char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    size_t len = end - str + 1;
    char* trimmed = malloc(len + 1);
    if (trimmed) {
        strncpy(trimmed, str, len);
        trimmed[len] = '\0';
    }
    return trimmed;
}

char** get_args(const char* input, int* count) {
    char* copy = strdup(input); // modifiable copy
    char* token = strtok(copy, " \t\n"); // first word
    if (!token) {
        *count = 0;
        free(copy);
        return NULL;
    }

    // Now collect all remaining tokens
    char** result = NULL;
    int capacity = 4;
    int size = 0;
    result = malloc(capacity * sizeof(char*));

    while ((token = strtok(NULL, " \t\n")) != NULL) {
        if (size >= capacity) {
            capacity *= 2;
            result = realloc(result, capacity * sizeof(char*));
        }
        result[size++] = trim(token);
    }

    *count = size;
    free(copy);
    return result;
}

bool replace_saved_matrix(char* name) {
    char input[10];  // Buffer for user input

    // Prompt user
    printf("Matrix %s already saved. Would you like to overwrite? (y/n): ", name);

    // Read input
    if(fgets(input, sizeof(input), stdin) != NULL) {
        // Strip newline character if present
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        // Check if the first character is 'y' or 'Y'
        if (tolower(input[0]) == 'y')
            return true;
    }

    return false;
}

char* get_matrix_name() {
    char buffer[256]; // Temporary buffer
    printf("Enter input (only letters and underscores allowed): ");

    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return NULL; // Read error
    }

    // Remove trailing newline
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
        len--;
    }

    if (len == 0) // Empty input
        return NULL;

    // Validate each character
    for (size_t i = 0; i < len; i++) {
        if (!(isalpha(buffer[i]) || buffer[i] == '_'))
            return NULL; // Name invalid
    }

    // Allocate memory for a copy of the valid string
    char* result = malloc(len + 1);
    if (result == NULL) // Allocation failure
        return NULL; 

    strcpy(result, buffer); // Copy input to return string
    
    return trim(result);
}