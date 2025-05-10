#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include "map_iterator.h"

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

void matrix_inc_val(Matrix* matrix, int row, int col, double val) {
    map_insert(matrix->vals, row, col, val);
}

void matrix_set_val(Matrix* matrix, int row, int col, double val) {
    map_set(matrix->vals, row, col, val);
}

int matrix_size(Matrix* matrix) {
    return matrix->vals->size;
}

void matrix_append(Matrix* dest, Matrix* source) {
    MapIterator map_it = map_iterator_create(source->vals);

    while(map_iterator_has_next(&map_it)) {
        int row, col;
        double val;
        map_iterator_next(&map_it, &row ,&col, &val);
        matrix_inc_val(dest, row, col, val);
    }
}

Matrix* matrix_add(Matrix* a, Matrix* b) {
    if(a->rows != b->rows || a->cols != b->cols)
        return NULL;

    Matrix* result = matrix_create(a->rows, a->cols);

    matrix_append(result, a);
    matrix_append(result, b);

    return result;
}

Matrix* matrix_scalar_mult(Matrix* matrix, double scalar) {
    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    MapIterator map_it = map_iterator_create(matrix->vals);

    while(map_iterator_has_next(&map_it)) {
        int row, col;
        double val;
        map_iterator_next(&map_it, &row ,&col, &val);
        matrix_set_val(result, row, col, val * scalar);
    }

    return result;
}

Matrix* matrix_scalar_add(Matrix* matrix, double scalar) {
    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    MapIterator map_it = map_iterator_create(matrix->vals);

    while(map_iterator_has_next(&map_it)) {
        int row, col;
        double val;
        map_iterator_next(&map_it, &row ,&col, &val);
        matrix_set_val(result, row, col, val + scalar);
    }

    return result;
}

void init_mult_vals(Matrix* matrix) {
    RowMap* mult_vals = row_map_create();
    MapIterator map_iter = map_iterator_create(matrix->vals);

    while(map_iterator_has_next(&map_iter)) {
        int row, col;
        double val;
        map_iterator_next(&map_iter, &row, &col, &val);
        row_map_increment(mult_vals, row, col, val);
    }

    matrix->mult_vals = mult_vals;
}

Matrix* matrix_mult(Matrix* a, Matrix* b) {
    if(a->cols != b->rows)
        return NULL;

    if(b->mult_vals == NULL) {

    }
}