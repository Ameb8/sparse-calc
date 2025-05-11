#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/matrix.h"
#include "../include/map_iterator.h"

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

    if(matrix->mult_vals != NULL)
        matrix->mult_vals = NULL;
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

void matrix_decrement(Matrix* dest, Matrix* source) {
    MapIterator map_it = map_iterator_create(source->vals);

    while(map_iterator_has_next(&map_it)) {
        int row, col;
        double val;
        map_iterator_next(&map_it, &row ,&col, &val);
        double old_val = map_get(dest->vals, row, col);
        matrix_set(dest, row, col, old_val - val);
    }
}

Matrix* subtract(Matrix* a, Matrix* b) {
    if(a->rows != b->rows || a->cols != b->cols)
        return NULL;

    Matrix* result = matrix_create(a->rows, a->cols);

    matrix_append(result, a);
    matrix_decrement(result, b);

    return result;
}

Matrix* matrix_scalar_mult(Matrix* matrix, double scalar) {
    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    MapIterator map_it = map_iterator_create(matrix->vals);

    while(map_iterator_has_next(&map_it)) {
        int row, col;
        double val;
        map_iterator_next(&map_it, &row ,&col, &val);
        matrix_set(result, row, col, val * scalar);
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
        matrix_set(result, row, col, val + scalar);
    }

    return result;
}

Matrix* transpose(Matrix* matrix) {
    Matrix* result = matrix_create(matrix->cols, matrix->rows);
    MapIterator map_iter= map_iterator_create(result->vals);

    while(map_iterator_has_next) {
        int row, col;
        double val;
        map_iterator_next(&map_iter, &row, &col, &val);
        matrix_inc_val(result, col, row, val);
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

    Matrix* result = matrix_create(a->rows, b->cols);

    if(b->mult_vals == NULL) {
        init_mult_vals(b);
    }

    MapIterator a_iter = map_iterator_create(a->vals);
    
    while(map_iterator_has_next(&a_iter)) {
        int row_a, col_a;
        double val_a;
        map_iterator_next(&a_iter, &row_a, &col_a, &val_a);
        List* row_b_list = row_map_get_row(b->mult_vals, col_a);
        if(row_b_list == NULL) continue;
        ListIterator row_b_iter = list_iter_create(row_b_list);

        while(list_iter_has_next(&row_b_iter)) {
            int row_b, col_b;
            double val_b;
            list_iter_next(&row_b_iter, &row_b, &col_b, &val_b);
            double old_val = map_get(result->vals, row_a, col_b);
            map_set(result->vals, row_a, col_b, val_a * val_b + old_val);
        }
    }

    return result;
}

// Set value in matrix
void matrix_set(Matrix* matrix, int row, int col, double val) {
    if(matrix->mult_vals != NULL) matrix->mult_vals = NULL;
    map_set(matrix->vals, row, col, val);
}

void matrix_print(Matrix* matrix) {
    int width = 9; // Width per cell
    char sep_line[1024] = "+";

    // Build horizontal separator line
    for (int i = 0; i < matrix->cols; ++i) {
        for (int j = 0; j < width - 1; ++j) strcat(sep_line, "-");
        strcat(sep_line, "+");
    }

    for (int i = 0; i < matrix->rows; ++i) { // Iterate through rows
        printf("%s\n", sep_line); // Print horizontal border
        for (int j = 0; j < matrix->cols; ++j) { // Iterate columns
            double val = map_get(matrix->vals, i, j); // Get value to print
            printf("| %6.2f ", val); // Print val
        }
        printf("|\n");
    }
    printf("%s\n", sep_line); // Final bottom border
}

Matrix* matrix_copy(Matrix* matrix) {
    Matrix* copy = matrix_create(matrix->rows, matrix->cols);
    matrix_append(copy, matrix);
    return copy;
}

void matrix_free(Matrix* matrix) {
    if(matrix == NULL) 
        return;

    // Free the vals HashMap
    if(matrix->vals != NULL)
        free_hash_map(matrix->vals);

    // Free the mult_vals RowMap
    if(matrix->mult_vals != NULL)
        free_row_map(matrix->mult_vals);

    free(matrix);
}
