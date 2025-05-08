#include "../include/hash_table.h"

// Create a new hash map
HashMap* create_hash_map() {
    HashMap* map = (HashMap*)malloc(sizeof(HashMap)); // Allocate memory
    map->size=0; //set size to zero
    
    // Set elements to null
    for (int i = 0; i < HASH_MAP_SIZE; i++) 
        map->table[i] = NULL;
    
    return map;
}

// Hash function to generate index for a key
unsigned int hash(int key) {
    return key % HASH_MAP_SIZE;
}

// Insert a key-value pair into the hash map
void insert(HashMap* map, int row, int col, double value) {
    unsigned int index = hash(row); // Get map table index
    add_node(&(map->table[index]), row, col, value); // Insert new value
    map->size++; // Increment size
}

// Returns linked list of all non-zero values in row
Node* find(HashMap* map, int row) {
    // Get elements at row's hash index
    unsigned int index = hash(row);
    Node* current = map->table[index];

    // Create nodes for list being returned
    Node* result_head = NULL;
    Node* result_tail = NULL;

    // Copy linked list, removing non-zero values mapped to this index with different key
    while (current) {
        if (current->row == row) { // Node in correct row
            Node* new_node = copy_node(current);

            // Append to list being returned
            if(result_head == NULL) {
                result_head = new_node;
                result_tail = new_node;
            } else {
                result_tail->next = new_node;
                result_tail = new_node;
            }
        }

        current = current->next;
    }

    return result_head;
}

// Create list of all non-zero values in hashmap
Node* map_to_list(HashMap* map) {
    Node* head = NULL; // Initialize list head
    
    for(int i = 0; i < HASH_MAP_SIZE; i++) { // Iterate through all indices in map table
        if(map->table[i] != NULL) { // Index contains non-zero val(s)
            // Add vals to list
            if(head != NULL)
                append_node(head, map->table[i]);
            else
                head = map->table[i];
        }
    }

    return head;
} 

// Free the hash map
void free_hash_map(HashMap* map) {
    for (int i = 0; i < HASH_MAP_SIZE; i++) {
        free_list(map->table[i]);
    }
    free(map);
}
