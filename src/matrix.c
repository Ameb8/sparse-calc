#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "../include/matrix.h"
#include "../include/map_iterator.h"

#define MATRIX_REGISTRY "matrix_registry.dat"

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

Matrix* matrix_sub(Matrix* a, Matrix* b) {
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
        
        #ifdef DEBUG
        printf("\nresult[%d][%d] = %.2f\n", row, col, val);
        #endif

        matrix_set(result, col, row, val);

        #ifdef DEBUG
        matrix_print(result);
        #endif
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
    if(!a || !b) {
        #ifdef TEST
        if(!a)
            printf("First operand is NULL\n");
        if(!b)
            printf("Second operand is NULL\n");
        #endif 

        return NULL;
    }

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

double matrix_get(Matrix* matrix, int row, int col) {
    if(row < 0 || row > matrix->rows || col < 0 || col > matrix->cols)
        return -DBL_MAX;

    return map_get(matrix->vals, row, col);
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


Matrix* matrix_save(Matrix* matrix, const char* name) {
    // Check if matrix name already exists
    FILE* registry = fopen(MATRIX_REGISTRY, "rb");
    if (registry) {
        char existing_name[100];
        while (fread(existing_name, sizeof(char), 100, registry) == 100) {
            int dummy1, dummy2;
            fread(&dummy1, sizeof(int), 1, registry);
            fread(&dummy2, sizeof(int), 1, registry);
            if (strcmp(existing_name, name) == 0) {
                fclose(registry);
                return NULL;  // Name already exists
            }
        }
        fclose(registry);
    }

    // Save matrix metadata to registry
    registry = fopen(MATRIX_REGISTRY, "ab");
    if (!registry) return NULL;

    char name_buf[100] = {0};
    strncpy(name_buf, name, 99);
    fwrite(name_buf, sizeof(char), 100, registry);
    fwrite(&matrix->rows, sizeof(int), 1, registry);
    fwrite(&matrix->cols, sizeof(int), 1, registry);
    fclose(registry);

    // Save HashMap vals to separate file
    char vals_filename[128];
    snprintf(vals_filename, sizeof(vals_filename), "matrix_%s.vals", name);

    if (!map_save(matrix->vals, vals_filename)) {
        return NULL;
    }

    // Do not save mult_vals
    return matrix;
}


Matrix* matrix_load(const char* name) {
    FILE* registry = fopen(MATRIX_REGISTRY, "rb");
    if (!registry) return NULL;  // Registry does not exist

    char existing_name[100];
    int rows = 0, cols = 0;
    bool found = false;

    // Search for the matrix metadata by name
    while (fread(existing_name, sizeof(char), 100, registry) == 100) {
        fread(&rows, sizeof(int), 1, registry);
        fread(&cols, sizeof(int), 1, registry);

        if (strcmp(existing_name, name) == 0) {
            found = true;
            break;
        }
    }

    fclose(registry);

    if (!found) return NULL;  // Name not found

    // Create matrix with rows and cols from registry
    Matrix* matrix = matrix_create(rows, cols);
    if (!matrix) return NULL;

    // Load hashmap vals from separate file
    char vals_filename[128];
    snprintf(vals_filename, sizeof(vals_filename), "matrix_%s.vals", name);

    HashMap* loaded_vals = map_load(vals_filename);
    if (!loaded_vals) {
        matrix_free(matrix);
        return NULL;
    }

    // Assign loaded HashMap to matrix->vals
    // Free the old empty vals hashmap created by matrix_create
    free_hash_map(matrix->vals);
    matrix->vals = loaded_vals;

    // mult_vals is not saved or loaded, set to NULL or create if needed
    matrix->mult_vals = NULL;

    return matrix;
}
