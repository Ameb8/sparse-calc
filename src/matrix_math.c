#include <stdio.h>
#include <stdlib.h>
#include "../include/matrix_math.h"

Matrix* matrix_add(const Matrix* a, const Matrix* b) {
    if(a->rows != b->rows || a->cols != b->cols)
        return NULL;
    
    Node* result_vals = create_node(0, 0, 0);
    Node* last = result_vals;

    int a_index = 0;
    int b_index = 0;

    while(a_index < a->nnz && b_index < b->nnz) {
        int a_less = compare_coords(a->row_ind[a_index], a->col_ind[a_index], b->row_ind[b_index], b->col_ind[b_index]);
        
        if(a_less < 0) { // Val in a comes first
            last->next = create_node(a->row_ind[a_index], a->col_ind[a_index], a->values[a_index++]);
        } else if(a_less > 0) { // Val in b comes first
            last->next = create_node(b->row_ind[b_index], b->col_ind[b_index], b->values[b_index++]);
        } else { // Vals share coordinates
            last->next = create_node(a->row_ind[a_index], a->col_ind[a_index], a->values[a_index++] + b->values[b_index++]);
            
        }

        last = last->next;
    }
    
    return matrix_create(a->rows, a->cols, result_vals);
}

Matrix* matrix_scalar(const Matrix* matrix, double scalar) {
    Node* head = create_node(matrix->row_ind[0], matrix->col_ind[0], matrix->values[0]);
    Node* temp = head;

    for(int i = 1; i < matrix->nnz; i++) {
        temp->next = create_node(matrix->row_ind[i], matrix->col_ind[i], matrix->values[i]);
        temp = temp->next;
    }

    return matrix_create(matrix->rows, matrix->cols, head);
}

// Multiply two matrices and return result as new matrix
Matrix* multiply_matrix(const Matrix* a, Matrix* b) {
    if(a->cols != b->rows) // Dimensions invalid for multiplication
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

Matrix* matrix_transpose(const Matrix* matrix) {
    Node* new_vals = create_node(0, 0, 0);
    Node* last = new_vals;

    for(int i = 0; i < matrix->nnz; i++) { 
        last->next = create_node(matrix->col_ind[i], matrix->row_ind[i], matrix->values[i]);
        last = last->next;
    }

    Matrix* result = matrix_create(matrix->cols, matrix->rows, new_vals);
    free_list(new_vals);
    return result;
}


Matrix* matrix_scalar_add(Matrix* matrix, double scalar) {
    Node* new_vals = create_node(0, 0, 0);
    Node* last = new_vals;

    for(int i = 0; i < matrix->nnz; i++) {
        last->next = create_node(matrix->row_ind[i], matrix->col_ind[i], matrix->values[i] + scalar);
        last = last->next;
    }

    Matrix* result = matrix_create(matrix->rows, matrix->cols, new_vals);
    free_list(new_vals);
    return result;
}