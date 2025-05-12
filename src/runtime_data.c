#include <stdio.h>
#include <string.h>
#include "../include/runtime_data.h"

#define MAX_MATRICES 100

Matrix* matrices[MAX_MATRICES];
char* matrix_names[MAX_MATRICES];
int num_matrices = 0;

int find_matrix(char* name) {
    for(int i = 0; i < num_matrices; i++) {
        if(!strcmp(name, matrix_names[i]))
            return i;
    }
}

void shift_vals(int index) {
    for(int i = index; i < num_matrices - 1; i++) {
        matrices[i] = matrices[i + 1];
        matrix_names[i] = matrix_names[i + 1];
    }
}

bool rd_save_matrix(char* name, Matrix* matrix) {
    if(num_matrices >= MAX_MATRICES)
        return false;
    
    if(find_matrix < 0)
        return false;

    matrix_names[num_matrices] = name;
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