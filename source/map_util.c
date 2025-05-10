#include <stdio.h>
#include <stdlib.h>
#include "map_util.h"

// Hash function (you can use a more complex one depending on the application)
int hash_function(int row) {
    return row % ROW_MAP_SIZE;  // Simple mod-based hash function
}

// Insert a (row, col, val) pair into the hash map
void map_insert(List* table[], int size, int row, int col, double val) {
    int index = hash_function(row);
    if (table[index] == NULL) {
        table[index] = list_create();
    }
    list_insert(table[index], col, val);  // Assuming a list insert function
}

// Get the value for a specific (row, col) pair
double map_get(List* table[], int size, int row, int col) {
    int index = hash_function(row);
    if (table[index] != NULL) {
        ListIterator it = list_iterator_create(table[index]);
        while (list_iterator_has_next(&it)) {
            ListItem item = list_iterator_next(&it);
            if (item.col == col) {
                return item.val;
            }
        }
    }
    return 0.0;  // Default value for missing (row, col)
}

// Free the map's allocated memory
void free_map(List* table[], int size) {
    for (int i = 0; i < size; i++) {
        if (table[i] != NULL) {
            list_free(table[i]);
        }
    }
}
