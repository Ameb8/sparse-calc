#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/linked_list.h"

#define HASH_MAP_SIZE 100

typedef struct {
    Node* table[HASH_MAP_SIZE];
    int size;
    Node* used_buckets;
} HashMap;

// Function to create a new hashmap
HashMap* map_create() {
    // Allocate memory for a new HashMap
    HashMap* map = (HashMap*)malloc(sizeof(HashMap));
    if (map == NULL) {
        // Handle memory allocation failure
        fprintf(stderr, "Memory allocation failed for the HashMap.\n");
        exit(1);
    }

    map->size = 0;
    map->used_buckets = NULL;

    // Initialize the table array to NULL
    for(int i = 0; i < HASH_MAP_SIZE; ++i) 
        map->table[i] = NULL;
    

    return map;
}

unsigned int hash(int row, int col) {
    unsigned int hash = 17;
    hash = 31 * hash + row;
    hash = 31 * hash + col;
    return hash % HASH_MAP_SIZE;
}

void insert(HashMap* map, int row, int col, double value) {
    unsigned int index = hash(row, col);

    if(map->table[index] == NULL) {
        if(map->used_buckets == NULL)
            map->used_buckets = create_node(0, 0, index);
        else
            add_node(map->used_buckets, 0, 0, index);
    }

    Node* current = map->table[index];

    // Check for existing (row, col) entry
    while (current) {
        if (current->row == row && current->col == col) {
            current->value += value;  // Accumulate if key exists
            return;
        }
        current = current->next;
    }

    // Key doesn't exist, add new node
    add_node(&(map->table[index]), row, col, value);
    map->size++;
}

double get(HashMap* map, int row, int col) {
    unsigned int index = hash(row, col);
    Node* current = map->table[index];

    while (current) {
        if (current->row == row && current->col == col) {
            return current->value;
        }
        current = current->next;
    }

    return 0.0; // Default if not found
}


void free_hash_map(HashMap* map) {
    for (int i = 0; i < HASH_MAP_SIZE; ++i) {
        Node* curr = map->table[i];
        while (curr) {
            Node* temp = curr;
            curr = curr->next;
            free(temp);
        }
    }
    free(map);
}


int main() {
    // Create a new hashmap
    HashMap* map = (HashMap*)malloc(sizeof(HashMap));
    if (!map) {
        // Handle memory allocation failure if necessary
        return -1;
    }

    // Insert some values
    insert(map, 1, 2, 3.5);
    insert(map, 2, 3, 7.0);
    insert(map, 1, 2, 1.5); // This will accumulate the value

    // Retrieve and print values
    printf("Value at (1, 2): %f\n", get(map, 1, 2)); // Should be 5.0 (3.5 + 1.5)
    printf("Value at (2, 3): %f\n", get(map, 2, 3)); // Should be 7.0

    // Free the hashmap
    free_hash_map(map);

    return 0;
}
