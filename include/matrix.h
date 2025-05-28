#ifndef MATRIX_H
#define MATRIX_H

#include "hash_map.h"
#include "row_map.h"

typedef struct {
    int rows;
    int cols;
    HashMap* vals;
    RowMap* mult_vals;
    double scalar_val;
} Matrix;

Matrix* matrix_create(int rows, int cols);
int matrix_size(Matrix* matrix);
Matrix* matrix_add(Matrix* a, Matrix* b);
Matrix* matrix_sub(Matrix* a, Matrix* b);
Matrix* matrix_scalar_mult(Matrix* matrix, double scalar);
Matrix* matrix_scalar_add(Matrix* matrix, double scalar);
Matrix* matrix_scalar_subr(Matrix* matrix, double scalar);
Matrix* matrix_transpose(Matrix* matrix);
Matrix* matrix_mult(Matrix* a, Matrix* b);
Matrix* matrix_element_mult(Matrix* a, Matrix* b);
void matrix_set(Matrix* matrix, int row, int col, double val);
double matrix_get(Matrix* matrix, int row, int col);
void matrix_print(Matrix* matrix);
Matrix* matrix_copy(Matrix* matrix);
void matrix_free(Matrix* matrix);
Matrix* scalar_subr(Matrix* matrix, double val);
double matrix_determinant(Matrix* a);
Matrix* matrix_identity(int n);
Matrix* matrix_inverse(Matrix* a);

#endif