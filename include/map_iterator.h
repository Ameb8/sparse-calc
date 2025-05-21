#ifndef MAP_ITERATOR_H
#define MAP_ITERATOR_H

#include "list.h"
#include "hash_map.h"
#include "list_iterator.h"


typedef struct {
    ListIterator list_it;
    ListIterator current_bucket_it;
    HashMap* map;
} MapIterator;

MapIterator map_iterator_create(HashMap* map);
bool map_iterator_has_next(MapIterator* it);
void map_iterator_next(MapIterator* it, int* row, int* col, double* val);


#endif