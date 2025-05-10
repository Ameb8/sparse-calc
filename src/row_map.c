#include <stdlib.h>
#include <stdio.h>
#include "../include/row_map.h"
#include "../include/list.h"

// Hash function to map row indices to bucket indices
unsigned int row_map_hash(int row) {
    return row % ROW_MAP_SIZE;
}

// Create a new RowMap
RowMap* row_map_create() {
    RowMap* map = malloc(sizeof(RowMap));
    if (!map) {
        fprintf(stderr, "Memory allocation failed for RowMap\n");
        exit(1);
    }
    map->size = 0;
    for (int i = 0; i < ROW_MAP_SIZE; i++) {
        map->table[i] = NULL;  // Initialize all entries to NULL
    }
    return map;
}

// Insert or update a (col, val) pair for a specific row
void row_map_insert(RowMap* map, int row, int col, double val) {
    unsigned int bucket_index = row_map_hash(row);

    RowNode* node = map->table[bucket_index];
    
    // Traverse the linked list at the hashed index
    while (node != NULL) {
        if (node->row == row) {
            // Found the row, update the value in the list
            list_update_val(node->col_vals, row, col, val);
            return;
        }
        node = node->next;
    }

    // If the row doesn't exist, create a new entry
    node = malloc(sizeof(RowNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed for RowNode\n");
        exit(1);
    }
    node->row = row;
    node->col_vals = list_create();
    list_append(node->col_vals, row, col, val);  // Add the (col, val) pair to the list

    // Insert the new node at the head of the linked list
    node->next = map->table[bucket_index];
    map->table[bucket_index] = node;
    map->size++;
}

// Get the value for a specific (row, col) pair
double row_map_get(RowMap* map, int row, int col) {
    unsigned int bucket_index = row_map_hash(row);
    RowNode* node = map->table[bucket_index];

    // Traverse the linked list at the hashed index
    while (node != NULL) {
        if (node->row == row) {
            // Found the row, get the value from the list
            return list_get_val(node->col_vals, row, col);
        }
        node = node->next;
    }

    // Return 0 if the row or (col, val) pair does not exist
    return 0.0;
}

// Increment the value for a specific (row, col) pair by a certain amount
double row_map_increment(RowMap* map, int row, int col, double val) {
    unsigned int bucket_index = row_map_hash(row);
    RowNode* node = map->table[bucket_index];

    // Traverse the linked list at the hashed index
    while (node != NULL) {
        if (node->row == row) {
            // Found the row, increment the value in the list
            double old_val =  list_increment_val(node->col_vals, row, col, val);
            list_prepend(node->col_vals, row, col, val + old_val);
            return val + old_val;
        }
        node = node->next;
    }

    // If the row does not exist, create a new entry
    node = malloc(sizeof(RowNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed for RowNode\n");
        exit(1);
    }
    node->row = row;
    node->col_vals = list_create();
    list_append(node->col_vals, row, col, val);  // Add the (col, val) pair to the list

    // Insert the new node at the head of the linked list
    node->next = map->table[bucket_index];
    map->table[bucket_index] = node;
    map->size++;

    return val;  // Return the incremented value
}

List* row_map_get_row(RowMap* map, int row) {
    unsigned int bucket_index = row_map_hash(row);
    RowNode* node = map->table[bucket_index];

    while(node != NULL) {
        if(node->row == row) {
            return node->col_vals;
        }

        node = node->next;
    }

    return NULL;
}

// Remove the (col, val) pair for a specific row
void row_map_remove(RowMap* map, int row, int col) {
    unsigned int bucket_index = row_map_hash(row);
    RowNode* node = map->table[bucket_index];
    RowNode* prev = NULL;

    // Traverse the linked list at the hashed index
    while (node != NULL) {
        if (node->row == row) {
            // Found the row, remove the (col, val) pair from the list
            list_remove_val(node->col_vals, row, col);
            if (node->col_vals->size == 0) {
                // If the list is empty, free the list and node
                list_free(node->col_vals);
                if (prev) {
                    prev->next = node->next;
                } else {
                    map->table[bucket_index] = node->next;
                }
                free(node);
                map->size--;
            }
            return;
        }
        prev = node;
        node = node->next;
    }
}

// Free all memory used by the RowMap
void free_row_map(RowMap* map) {
    for (int i = 0; i < ROW_MAP_SIZE; i++) {
        RowNode* node = map->table[i];
        while (node != NULL) {
            RowNode* next = node->next;
            list_free(node->col_vals);
            free(node);
            node = next;
        }
    }
    free(map);  // Free the RowMap structure
}

int main() {
    RowMap* map = row_map_create();

    // Insert some values
    row_map_increment(map, 5, 1, 3.5);
    row_map_increment(map, 5, 2, 4.2);
    row_map_insert(map, 105, 2, 1.1);
    row_map_increment(map, 5, 3, 4.2);

    printf("\nRow 10:\n");
    //list_print(row_map_get_row(map, 105));

    printf("\nRow 5:\n");
    list_print(row_map_get_row(map, 5));
    

    // Get and print values
    printf("Value at (5, 1): %.2f\n", row_map_get(map, 5, 1));  // Should print 3.5
    printf("Value at (5, 2): %.2f\n", row_map_get(map, 5, 2));  // Should print 4.2
    printf("Value at (5, 3): %.2f\n", row_map_get(map, 5, 3));  // Should print 4.2
    printf("Value at (105, 2): %.2f\n", row_map_get(map, 105, 2));  // Should print 4.2

    // Increment a value
    row_map_increment(map, 0, 1, 2.5);
    printf("New Value at (0, 1): %.2f\n", row_map_get(map, 0, 1));  // Should print 6.0

    // Remove a value
    row_map_remove(map, 0, 2);
    printf("Value at (0, 2) after removal: %.2f\n", row_map_get(map, 0, 2));  // Should print 0.0

    // Free memory
    free_row_map(map);

    return 0;
}