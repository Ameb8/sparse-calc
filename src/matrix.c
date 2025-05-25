#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "../include/matrix.h"
#include "../include/map_iterator.h"

#define MATRIX_REGISTRY "matrix_registry.dat"

Matrix* matrix_create(int rows, int cols) {
    if(rows < 1 || cols < 1)
        return NULL;

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
    matrix->scalar_val = 0;

    return matrix;
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

    result->scalar_val = a->scalar_val + b->scalar_val;

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

Matrix* matrix_sub(Matrix* a, Matrix* b) {
    if(a->rows != b->rows || a->cols != b->cols)
        return NULL;

    Matrix* result = matrix_create(a->rows, a->cols);

    matrix_append(result, a);
    matrix_decrement(result, b);

    result->scalar_val = a->scalar_val -  b->scalar_val;

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

    matrix->scalar_val *= scalar;

    return result;
}

Matrix* matrix_scalar_add(Matrix* matrix, double scalar) {
    Matrix* result = matrix_create(matrix->rows, matrix->cols);
    matrix_append(result, matrix);
    result->scalar_val = matrix->scalar_val + scalar;

    return result;
}


Matrix* matrix_transpose(Matrix* matrix) {
    if(!matrix)
        return NULL;

    Matrix* result = matrix_create(matrix->cols, matrix->rows);
    MapIterator map_iter= map_iterator_create(matrix->vals);

    #ifdef DEBUG
    matrix_print(matrix);
    #endif

    while(map_iterator_has_next(&map_iter)) {
        int row, col;
        double val;
        map_iterator_next(&map_iter, &row, &col, &val);
        
        matrix_set(result, col, row, val);
    }

    result->scalar_val = matrix->scalar_val;

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


// Add 1st operand multiplied by b-sized matrix of only its scalar val to result
void scalar_a(Matrix* matrix, Matrix* result, double scalar_val) {
    MapIterator map_it = map_iterator_create(matrix->vals);

    while(map_iterator_has_next(&map_it)) {
        // Get next non-zero value in a
        int row, col;
        double val;
        map_iterator_next(&map_it, &row, &col, &val);
        
        for(int i = 0; i < result->cols; i++) {
            double old_val = matrix_get(result, row, i);
            matrix_set(result, row, i, val * scalar_val + old_val);
        }
    }
}

// Add 2st operand multiplied by a-sized matrix of only its scalar val to result
void scalar_b(Matrix* matrix, Matrix* result, double scalar_val) {
    MapIterator map_it = map_iterator_create(matrix->vals);

    while (map_iterator_has_next(&map_it)) {
        // Get next non-zero value in b
        int row, col;
        double val;
        map_iterator_next(&map_it, &row, &col, &val);

        for (int i = 0; i < result->rows; i++) {
            double old_val = matrix_get(result, i, col);
            matrix_set(result, i, col, val * scalar_val + old_val);
        }
    }
}


Matrix* matrix_mult(Matrix* a, Matrix* b) {
    if(!a || !b) 
        return NULL;

    if(a->cols != b->rows)
        return NULL; // Dimensions invalid for dot product

    // Create matrix to hold result
    Matrix* result = matrix_create(a->rows, b->cols);

    if(b->mult_vals == NULL)
        init_mult_vals(b); // Initialize RowMap in b if necessary

    MapIterator a_iter = map_iterator_create(a->vals);

    while(map_iterator_has_next(&a_iter)) { // Iterate through non-zero a values
        // Get next non-zero a value
        int row_a, col_a;
        double val_a;
        map_iterator_next(&a_iter, &row_a, &col_a, &val_a);
        
        // Get b row
        List* row_b_list = row_map_get_row(b->mult_vals, col_a);
        if(row_b_list == NULL) 
            continue;
        ListIterator row_b_iter = list_iter_create(row_b_list);

        // Iterate through values in b row
        while(list_iter_has_next(&row_b_iter)) {
            // Get next non-zero element from b row
            int row_b, col_b;
            double val_b;
            list_iter_next(&row_b_iter, &row_b, &col_b, &val_b);

            // Compute product and sum to result element
            double old_val = map_get(result->vals, row_a, col_b);
            map_set(result->vals, row_a, col_b, val_a * val_b + old_val);
        }
    }


    // Calculate a and b times each other's scalar values added to result
    if(b->scalar_val != 0)
        scalar_a(a, result, b->scalar_val);
    if(a->scalar_val != 0)
        scalar_b(b, result, a->scalar_val);

    // Set results scalar value
    if(a->scalar_val != 0 && b->scalar_val != 0)
        result->scalar_val = a->scalar_val * b->scalar_val * a->cols;
        

    return result;
}

Matrix* matrix_identity(int rows, int cols) {
    Matrix* result = matrix_create(rows, cols);

    // Ensure valid matrix dimensions
    if(rows != cols || rows < 1 || cols < 1)
        return NULL;

    // Set diagonal vals to 1
    for(int i = 0; i < rows; i++)
        matrix_set(result, i, i, 1.0);
    
    return result;
}


void matrix_swap_rows(Matrix* matrix, int row1, int row2) {
    if (!matrix || row1 == row2 || row1 < 0 || row2 < 0 || row1 >= matrix->rows || row2 >= matrix->rows) {
        return; // Invalid input or no-op
    }

    for (int col = 0; col < matrix->cols; col++) {
        double val1 = matrix_get(matrix, row1, col);
        double val2 = matrix_get(matrix, row2, col);

        matrix_set(matrix, row1, col, val2);
        matrix_set(matrix, row2, col, val1);
    }
}


double matrix_determinant(Matrix* a) {
    if(a->rows != a->cols)
        return -DBL_MAX; // Matrix must be square

    int n = a->rows;

    // Initialize matrices as p * a = l * u
    Matrix* l = matrix_create(a->rows, a->cols); // Zero matrix
    Matrix* u = matrix_scalar_add(a, 0); // Copy of a

    // Create permutation vector p (identity initially)
    int* p = (int*)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        p[i] = i;
    }

    int sign = 1;  // Track parity of row swaps

    for (int k = 0; k < n; k++) {
        // Partial pivoting: find pivot row r in column k starting at k
        int r = k;
        double max_val = fabs(matrix_get(u, k, k));
        for (int i = k + 1; i < n; i++) {
            double val = fabs(matrix_get(u, i, k));
            if (val > max_val) {
                max_val = val;
                r = i;
            }
        }

        if (r != k) {
            // Swap rows r and k in U
            matrix_swap_rows(u, k, r);

            // Swap rows in L for columns < k
            for (int col = 0; col < k; col++) {
                double temp = matrix_get(l, k, col);
                matrix_set(l, k, col, matrix_get(l, r, col));
                matrix_set(l, r, col, temp);
            }

            // Swap in permutation vector
            int temp = p[k];
            p[k] = p[r];
            p[r] = temp;

            sign = -sign;  // flip determinant sign
        }

        double pivot = matrix_get(u, k, k);
        if(fabs(pivot) < 1e-15) {
            // Singular matrix => determinant is zero
            free(p);
            //matrix_destroy(l);
            //matrix_destroy(u);
            return 0.0;
        }

        // Elimination below pivot
        for(int i = k + 1; i < n; i++) {
            double multiplier = matrix_get(u, i, k) / pivot;
            matrix_set(l, i, k, multiplier);
            for (int j = k; j < n; j++) {
                double val = matrix_get(u, i, j) - multiplier * matrix_get(u, k, j);
                matrix_set(u, i, j, val);
            }
        }

        matrix_set(l, k, k, 1.0);  // L diagonal = 1
    }

    // Compute determinant = sign * product of U diagonal
    double det = (double)sign;
    for(int i = 0; i < n; i++) {
        det *= matrix_get(u, i, i);
    }

    // Cleanup
    free(p);
    //matrix_destroy(l);
    //matrix_destroy(u);

    return det;
}


// Set value in matrix
void matrix_set(Matrix* matrix, int row, int col, double val) {
    if(matrix->mult_vals != NULL) 
        matrix->mult_vals = NULL;
    
    map_set(matrix->vals, row, col, val - matrix->scalar_val);
}

double matrix_get(Matrix* matrix, int row, int col) {
    if(row < 0 || row > matrix->rows || col < 0 || col > matrix->cols)
        return -DBL_MAX;

    return map_get(matrix->vals, row, col) + matrix->scalar_val;
}

void matrix_print(Matrix* matrix) {
    if(!matrix) {
        printf("Matrix does not exist\n");
        return;
    }

    int width = 9; // Width per cell
    char sep_line[1024] = "+";

    // Build horizontal separator line
    for(int i = 0; i < matrix->cols; ++i) {
        for(int j = 0; j < width - 1; ++j) strcat(sep_line, "-");
        strcat(sep_line, "+");
    }

    for (int i = 0; i < matrix->rows; ++i) { // Iterate through rows
        printf("%s\n", sep_line); // Print horizontal border
        for (int j = 0; j < matrix->cols; ++j) { // Iterate columns
            double val = map_get(matrix->vals, i, j) + matrix->scalar_val; // Get value to print
            printf("| %6.2f ", val); // Print val
        }
        printf("|\n");
    }
    printf("%s\n", sep_line); // Final bottom border
}


Matrix* matrix_copy(Matrix* matrix) {
    Matrix* copy = matrix_create(matrix->rows, matrix->cols);
    matrix_append(copy, matrix);
    copy->scalar_val = matrix->scalar_val;

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