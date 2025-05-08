#ifndef MATRIX_UTIL_H
#define MATRIX_UTIL_H

#include "hash_table.h"

// Matrix struct in COO format
// Contains hashmap for more efficient multiplication
// Map field only initialized from null when required for multiplication or if matrix created from map
typedef struct {
    int rows;
    int cols;
    int nnz; 
    int* row_ind;
    int* col_ind;
    double* values;
    HashMap* map; 
} Matrix;

Matrix* matrix_create(int rows, int cols, Node* entries);
void matrix_free(Matrix* mat);
Matrix* multiply(const Matrix* a, Matrix* b);
void print_matrix(Matrix* matrix);

#endif
