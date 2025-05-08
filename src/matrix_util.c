#include <stdio.h>
#include <stdlib.h>
#include "../include/matrix_util.h"
#include "../include/hash_table.h"
#include "../include/linked_list.h"


// Function to create a Matrix from the linked list
Matrix* matrix_create(int rows, int cols, Node* entries) {
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
    Node* current = entries;
    while (current != NULL) { // Iterate linked list
        nnz++;
        current = current->next;
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
    current = entries;
    int i = 0;
    while (current != NULL) { // Iterate linked list of non-zero vals
        // Add non-zero coordinates to arrays
        row_ind[i] = current->row;
        col_ind[i] = current->col;
        values[i] = current->value;
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
        free(mat);
    }
}

// Convert row->(col, val) hashmap into matrix
Matrix* matrix_from_map(int rows, int cols, HashMap* val_map) {
    Node* val_list = map_to_list(val_map); // Get map as list
    
    Matrix* matrix = matrix_create(rows, cols, val_list); // Create matrix from map
    matrix->map = val_map; // Initialize map field as map already exists
    return matrix;
}

// initialize map field of matrix struct
void build_map(Matrix* matrix) {
    HashMap* matr_vals = create_hash_map(); // Initialize hashmap

    for(int i = 0; i < matrix->nnz; i++) // Add non-zero vals to hashmap
        insert(matr_vals, matrix->row_ind[i], matrix->col_ind[i], matrix->values[i]);

    matrix->map = matr_vals; // Initialize map field
}

// Multiply two matrices and return result as new matrix
Matrix* multiply(const Matrix* a, Matrix* b) {
    if(a->rows != b->cols) // Dimensions invalid for multiplication
        return NULL;

    if(b->map == NULL) // Initialize map field of b if it doesn't exist
        build_map(b);

    HashMap* result_map = create_hash_map(); // Initialize hashmap to store result

    // Iterate through each non-zero entry in a 
    for (int i = 0; i < a->nnz; ++i) {
        // Set row, col, and value of non-zero value
        int row_a = a->row_ind[i];
        int col_a = a->col_ind[i];
        double val_a = a->values[i];

        // Get linked list of all telements in b that match a's column
        Node* col_b_vals = find(b->map, col_a);

        // Iterate all values in b row matching a column
        Node* temp = col_b_vals;
        while(temp != NULL) {
            // Add product to result
            double product = temp->value * val_a;
            insert(result_map, row_a, temp->col, product);
            temp = temp->next;
        }
    }

    // Convert map to matrix and return
    return matrix_from_map(a->rows, b->cols, result_map);
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


