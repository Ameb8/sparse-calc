#ifndef MAP_UTIL_H
#define MAP_UTIL_H

#include "list.h"

// HashMap-like functionality for row-based maps

int hash_function(int row);  // Simple hash function for row indices

void map_insert(List* table[], int size, int row, int col, double val);
double map_get(List* table[], int size, int row, int col);
void free_map(List* table[], int size);

#endif
