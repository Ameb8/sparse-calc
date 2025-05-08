#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "linked_list.h"

#define HASH_MAP_SIZE 100

struct Node;
typedef struct Node Node;

// HashMap structure: An array of linked lists
typedef struct HashMap {
    Node* table[HASH_MAP_SIZE];
    int size;
} HashMap;


HashMap* create_hash_map();
unsigned int hash(int key);
void insert(HashMap* map, int row, int col, double value);
Node* find(HashMap* map, int key);
Node* map_to_list(HashMap* map);
void free_hash_map(HashMap* map);

#endif
