#ifndef MATRIX_H
#define MATRIX_H

#include "hash_map.h"
#include "row_map.h"

typedef struct {
    int rows;
    int cols;
    HashMap* vals;
    RowMap* mult_vals;
} Matrix;


Matrix* matrix_create(int rows, int cols);
int matrix_size(Matrix* matrix);
Matrix* matrix_add(Matrix* a, Matrix* b);
Matrix* subtract(Matrix* a, Matrix* b);
Matrix* matrix_scalar_mult(Matrix* matrix, double scalar);
Matrix* matrix_scalar_add(Matrix* matrix, double scalar);
Matrix* transpose(Matrix* matrix);
Matrix* matrix_mult(Matrix* a, Matrix* b);
void matrix_set(Matrix* matrix, int row, int col, double val);
void matrix_print(Matrix* matrix);
Matrix* matrix_copy(Matrix* matrix);
void matrix_free(Matrix* matrix);



#endif