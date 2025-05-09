#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/matrix_util.h"
#include "../include/matrix_cli.h"

#define MAX_INPUT 100
#define MAX_NAME 32

typedef struct {
    char name[MAX_NAME];
    Matrix* mat;
} NamedMatrix;

NamedMatrix* matrix_list[100];
int matrix_count = 0;

void list_matrices() {
    for(int i = 0; i < matrix_count; i++) {
        Matrix* matrix = matrix_list[i]->mat;
        printf("%s: (%d x %d)\n", matrix_list[i]->name, matrix->rows, matrix->cols);
    }
}

Matrix* find_matrix_by_name(const char* name) {
    for (int i = 0; i < matrix_count; i++) {
        if (strcmp(matrix_list[i]->name, name) == 0) {
            return matrix_list[i]->mat;
        }
    }

    return NULL;
}

void save_matrix(const char* name, Matrix* mat) {
    NamedMatrix* new_entry = malloc(sizeof(NamedMatrix));
    strncpy(new_entry->name, name, MAX_NAME);
    new_entry->mat = mat;
    matrix_list[matrix_count++] = new_entry;
}

void parse_command(char* input) {
    if (strncmp(input, "matrix ", 7) == 0) {
        char name[MAX_NAME];
        sscanf(input + 7, "%s", name);
        Matrix* m = get_user_matrix();
        save_matrix(name, m);
    } else if (strcmp(input, "list") == 0) {
        list_matrices();
    } else if (strncmp(input, "show ", 5) == 0) {
        char name[MAX_NAME];
        sscanf(input + 5, "%s", name);
        Matrix* m = find_matrix_by_name(name);
        if (m) print_matrix(m);
        else printf("Matrix '%s' not found.\n", name);
    } else if (strstr(input, "=")) {
        // handle operation: name = A * B or name = A + B
        char result[MAX_NAME], op1[MAX_NAME], op2[MAX_NAME], operator;
        if (sscanf(input, "%s = %s %c %s", result, op1, &operator, op2) == 4) {
            Matrix* m1 = find_matrix_by_name(op1);
            Matrix* m2 = find_matrix_by_name(op2);
            if (!m1 || !m2) {
                printf("Matrix not found.\n");
                return;
            }
            Matrix* res = NULL;
            if (operator == '+')  {
                res = matrix_add(m1, m2);
                if(res == NULL)
                    printf("Matrices %s and %s could not be added\n", op1, op2);
            } else if (operator == '*') {
                res = multiply_matrix(m1, m2);
                if(res == NULL)
                    printf("Matrices %s and %s could not be multiplied\n", op1, op2);
            } else {
                printf("Unsupported operator: %c\n", operator);
                return;
            }

            save_matrix(result, res);
        } else {
            printf("Invalid operation format. Use: result = A + B\n");
        }
    } else {
        printf("Unknown command.\n");
    }
}

int run_program() {
    char input[MAX_INPUT];
    printf("Sparse Matrix CLI. Type commands below:\n");

    while (1) {
        printf("> ");
        if (!fgets(input, MAX_INPUT, stdin)) break;
        input[strcspn(input, "\n")] = 0;  // Remove newline
        if (strcmp(input, "exit") == 0) break;
        parse_command(input);
    }

    // Free all matrices
    for (int i = 0; i < matrix_count; i++) {
        matrix_free(matrix_list[i]->mat);  // Assuming you have this
        free(matrix_list[i]);
    }

    return 0;
}

int main() {
    run_program();

    return 0;
}