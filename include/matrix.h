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


Matrix* matrix_create(int rows, int cols) {
    // Allocate memory for a new Matrix
    Matrix* matrix = (Matrix*)malloc(sizeof(Matrix));
    if (matrix == NULL) {
        // Handle memory allocation failure
        fprintf(stderr, "Memory allocation failed for the HashMap.\n");
        exit(1);
    }

    // Instantiate fields
    matrix->rows = rows;
    matrix->cols = cols;
    matrix->vals = map_create();
    matrix->mult_vals = NULL;
}

int matrix_size(Matrix* matrix);
Matrix* matrix_add(Matrix* a, Matrix* b);
Matrix* matrix_scalar_mult(Matrix* matrix, double scalar);
Matrix* matrix_scalar_add(Matrix* matrix, double scalar);
Matrix* transpose(Matrix* matrix);
Matrix* matrix_mult(Matrix* a, Matrix* b);
void matrix_set(Matrix* matrix, int row, int col, int val);


#endif