#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/list.h"
#include "../include/hash_map.h"

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
    map->used_buckets = list_create();

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

void map_set(HashMap* map, int row, int col, double val) {
    unsigned int index = hash(row, col); // Get hash of (row, col)

    if(!map->table[index] && val != 0) { // Add used bucket to list if necessary
        list_prepend(map->used_buckets, index, 0, 0);
        map->table[index] = list_create();
    }

    List* list = map->table[index];
    double cur_val = list_get_val(map->table[index], row, col);

    if(cur_val == 0) {
        if(val != 0) {
            list_prepend(list, row, col, val);
            map->size++;
        }
    } else {
        // Remove existing value
        list_remove_val(list, row, col);

        if(val != 0) { // Add new value
            list_prepend(list, row, col, val);
        } else { // Remove current from list if new value zero
            map->size--;
            if(map->table[index]->head == NULL) {
                free(map->table[index]);
                map->table[index] = NULL;
            }
        }
    }
}

void map_insert(HashMap* map, int row, int col, double val) {
    unsigned int index = hash(row, col); // Get hash of (row, col)

    if(val == 0)
        return;

    if(map->table[index] == NULL) { // Add used bucket to list if necessary
        list_prepend(map->used_buckets, index, 0, 0);
        map->table[index] = list_create();
    }

    List* list = map->table[index];
    double cur_val = list_get_val(map->table[index], row, col);

    if(cur_val == 0) {
        list_prepend(list, row, col, val);
        map->size++;
    } else if(cur_val + val == 0) {
        list_remove_val(list, row, col);
        map->size--;

        if(list->head == NULL)
            list_remove_val(map->used_buckets, row, col);
    } else {
        list_update_val(list, row, col, cur_val + val);
    }
}

double map_get(HashMap* map, int row, int col) {
    unsigned int index = hash(row, col);

    if (map->table[index] == NULL)
        return 0;

    return list_get_val(map->table[index], row, col);
}


void free_hash_map(HashMap* map) {
    for (int i = 0; i < HASH_MAP_SIZE; ++i)
        list_free(map->table[i]);
        
    list_free(map->used_buckets);
    free(map);
}

