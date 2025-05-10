#ifndef MATRIX_H
#define MATRIX_H

#include "hash_map.h"
#include "row_map.h"

typedef struct {
    int rows;
    int cols;
    HashMap* vals;
    RowMap* mult_vals;
} Matrix;



#endif