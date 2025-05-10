#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "list.h"

#define HASH_MAP_SIZE 100

typedef struct {
    List* table[HASH_MAP_SIZE];
    int size;
    List* used_buckets;
} HashMap;

HashMap* map_create();
void map_insert(HashMap* map, int row, int col, double val);
void map_set(HashMap* map, int row, int col, double val);
double map_get(HashMap* map, int row, int col);
void free_hash_map(HashMap* map);



#endif