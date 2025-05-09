#ifndef MATRIX_MATH_H
#define MATRIX_MATH_H

#include "hash_table.h"
#include "linked_list.h"
#include "matrix_util.h"

Matrix* multiply_matrix(const Matrix* a, Matrix* b);
Matrix* matrix_add(const Matrix* a, const Matrix* b);
Matrix* matrix_scalar(const Matrix* matrix, double scalar);
Matrix* matrix_scalar_add(Matrix* matrix, double scalar);
Matrix* matrix_transpose(const Matrix* matrix);

#endif