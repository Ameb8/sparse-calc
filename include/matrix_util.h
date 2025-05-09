#ifndef MATRIX_UTIL_H
#define MATRIX_UTIL_H

#include <stdbool.h>
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
void print_matrix(Matrix* matrix);
bool add_val(Matrix* matrix, int row, int col, double val);
void build_map(Matrix* matrix);
Matrix* matrix_from_map(int rows, int cols, HashMap* val_map);
int compare_coords(int row_a, int col_a, int row_b, int col_b);

#endif
