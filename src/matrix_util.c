#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/matrix_util.h"
#include "../include/hash_table.h"
#include "../include/linked_list.h"

// Return -1 if a < b, 0 if locations are same, otherwise 1
int compare_coords(int row_a, int col_a, int row_b, int col_b) {
    if(row_a < row_b)
        return -1;
    if(row_a > row_b)
        return 1;
    if(col_a < col_b)
        return -1;
    if(col_a > col_b)
        return 1;
    return 0
}

// Function to create a Matrix from the linked list
Matrix* matrix_create(int rows, int cols, Node* vals) {
    // Allocate memory for matrix
    Matrix* mat = (Matrix*)malloc(sizeof(Matrix));
    if (mat == NULL) { // Allocation failed, exit program
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }

    // initialize row and column fields
    mat->rows = rows;
    mat->cols = cols;

    // Count the number of non-zero entries
    int nnz = 0;
    Node* current = vals;
    
    // Track if list is sorted
    int last_row = 0;
    int last_col = 0;
    bool sorted = true;

    while (current != NULL) { // Iterate linked list
        if(current->value != 0) {
            if(compare_coords(last_row, last_col, current->row, current->col) > 0)
                sorted = false; // List is not sorted

            nnz++; // Count node if non-zero value
        }   
        current = current->next;
    }

    // Sort list if unsorted
    if(!sorted) {
        Node* sorted_list = sort_list(vals);
        free_list(vals);
        vals = sorted_list;
    }

    // Allocate arrays to store row indices, column indices, and values
    int* row_ind = (int*)malloc(nnz * sizeof(int));
    int* col_ind = (int*)malloc(nnz * sizeof(int));
    double* values = (double*)malloc(nnz * sizeof(double));

    if (!row_ind || !col_ind || !values) { // Allocation failed, exit program
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }

    
    // Populate the arrays from the linked list
    current = vals;
    int i = 0;
    while (current != NULL) { // Iterate linked list of non-zero vals
        if(current->value != 0) {
            // Add non-zero coordinates to arrays
            row_ind[i] = current->row;
            col_ind[i] = current->col;
            values[i] = current->value;
        }

        i++;
        current = current->next;
    }

    // Store the arrays of coordinates in the matrix struct
    mat->row_ind = row_ind;
    mat->col_ind = col_ind;
    mat->values = values;
    mat->nnz = nnz;

    return mat;
}



// Free the allocated memory for the matrix
void matrix_free(Matrix* mat) {
    if(mat) {
        free(mat->row_ind);
        free(mat->col_ind);
        free(mat->values);
        free_hash_map(mat->map);
        free(mat);
    }
}

// Convert row->(col, val) hashmap into matrix
Matrix* matrix_from_map(int rows, int cols, HashMap* val_map) {
    Node* val_list = map_to_list(val_map); // Get map as list

    Matrix* matrix = matrix_create(rows, cols, val_list); // Create matrix from map
    matrix->map = val_map; // Initialize map field as map already exists
    free_list(val_list);

    return matrix;
}

// initialize map field of matrix struct
void build_map(Matrix* matrix) {
    HashMap* matr_vals = create_hash_map(); // Initialize hashmap

    for(int i = 0; i < matrix->nnz; i++) // Add non-zero vals to hashmap
        insert(matr_vals, matrix->row_ind[i], matrix->col_ind[i], matrix->values[i]);

    matrix->map = matr_vals; // Initialize map field
}

void print_matrix(Matrix* matrix) {
    if (!matrix) {
        printf("Matrix is NULL.\n");
        return;
    }

    // If map hasn't been built yet, construct it
    if (!matrix->map)
        build_map(matrix);  // Cast to non-const if needed

    printf("Matrix (%d x %d):\n", matrix->rows, matrix->cols);

    for (int row = 0; row < matrix->rows; ++row) {
        // Get linked list of non-zero values in this row
        Node* list = find(matrix->map, row);

        // Create a temporary dense row filled with 0s
        double* temp_row = calloc(matrix->cols, sizeof(double));
        Node* current = list;

        while (current) {
            if (current->col >= 0 && current->col < matrix->cols) {
                temp_row[current->col] = current->value;
            }
            current = current->next;
        }

        // Print the row
        for (int col = 0; col < matrix->cols; ++col) {
            printf("%8.2f ", temp_row[col]);
        }
        printf("\n");
        free(temp_row);
    }
}


bool add_val(Matrix* matrix, int row, int col, double val) {
    if(row < 0 || row >= matrix->rows || col < 0 || col >= matrix->cols)
        return false; // new value out of bounds

    Node* new_vals = create_node(0, 0, 0);
    Node* last = new_vals;
    bool val_added = false;

    for(int i = 0; i < matrix->nnz; i++) {
        int compare = compare_coords(row, col, matrix->row_ind[i], matrix->col_ind[i]);
        
        if(compare < 1) { // Add new value if before or equal current old value
            last->next = create_node(row, col, val);
            last = last->next;
            val_added = true;
        }
        
        if(compare != 0) { // Add old value if not replaced
            last->next = create_node(matrix->row_ind[i], matrix->col_ind[i], matrix->values[i]);
            last = last->next;
        }
    }

    
    Matrix* updated = matrix_create(matrix->rows, matrix->cols, new_vals);
    
    // Preserve matrix field if already initialized
    if(matrix->map != NULL) {
        HashMap* preserved_map = matrix->map;
        insert(preserved_map, row, col, val);
        updated->map = preserved_map;
        matrix->map = NULL;
    }
    
    free_list(new_vals);
    matrix_free(matrix);
    matrix = updated;

    return true;
}



