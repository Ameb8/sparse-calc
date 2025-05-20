#include <stdio.h>
#include <string.h>
#include "../include/runtime_data.h"
#include "../include/matrix_cli.h"

#define MAX_MATRICES 100

Matrix* matrices[MAX_MATRICES];
char* matrix_names[MAX_MATRICES];
int num_matrices = 0;

int find_matrix(char* name) {
    name = name;
    for(int i = 0; i < num_matrices; i++) {
        if(!strcmp(name, matrix_names[i]))
            return i;
    }

    return -1;
}

void shift_vals(int index) {
    for(int i = index; i < num_matrices - 1; i++) {
        matrices[i] = matrices[i + 1];
        matrix_names[i] = matrix_names[i + 1];
    }
}

bool rd_overwrite_matrix(char* name, Matrix* matrix) {
    name = trim(name);
    int index = find_matrix(name);

    if(index >= 0) {
        matrices[index] = matrix;
        return true;
    }

    if(num_matrices >= MAX_MATRICES)
        return false;
    
    matrix_names[num_matrices] = name;
    matrices[num_matrices++] = matrix;

    return true;
}

bool rd_save_matrix(char* name, Matrix* matrix) {
    char* trimmed = trim(name);

    if(num_matrices >= MAX_MATRICES)
        return false;
    
    if(find_matrix(trimmed) > 0)
        return false;

    matrix_names[num_matrices] = strdup(trimmed);
    matrices[num_matrices++] = matrix;

    return true;
}

bool rd_delete_matrix(char* name) {
    int matrix_index = find_matrix(name);

    if(matrix_index < 0)
        return false;

    matrix_free(matrices[matrix_index]);
    shift_vals(matrix_index);
    
    return true;
}

Matrix* rd_get_matrix(char* name) {
    int matrix_index = find_matrix(name);
    
    if(matrix_index < 0)
        return NULL;

    return matrices[matrix_index];
}

void rd_print_all() {
    for(int i = 0; i < num_matrices; i++)
        printf("Matrix %s (%d x %d)\n", matrix_names[i], matrices[i]->rows, matrices[i]->cols);
}