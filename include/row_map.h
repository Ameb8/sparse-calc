#ifndef ROW_MAP_H
#define ROW_MAP_H

#include "list.h"

#define ROW_MAP_SIZE 1000  // Number of buckets for the hashmap

// Define a struct for each row's entry in the hashmap
typedef struct RowNode {
    int row;            // Row index
    List* col_vals;     // List of (col, val) pairs for this row
    struct RowNode* next; // Pointer to next row node in case of a collision
} RowNode;

// Define the RowMap struct
typedef struct {
    RowNode* table[ROW_MAP_SIZE];  // Array of buckets (linked lists)
    long size;  // Number of rows currently in use
} RowMap;

// Function prototypes
RowMap* row_map_create();
unsigned int row_map_hash(int row);
void row_map_insert(RowMap* map, int row, int col, double val);
double row_map_get(RowMap* map, int row, int col);
List* row_map_get_row(RowMap* map, int row);
double row_map_increment(RowMap* map, int row, int col, double val);
void row_map_remove(RowMap* map, int row, int col);
void free_row_map(RowMap* map);

#endif
