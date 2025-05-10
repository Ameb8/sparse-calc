#include <stdio.h>
#include "../include/map_iterator.h"

MapIterator map_iterator_create(HashMap* map) {
    MapIterator it;
    it.map = map;
    it.list_it = list_iter_create(map->used_buckets); // Start by iterating used bucket indices
    it.current_bucket_it = (ListIterator){NULL};    // No current list yet
    return it;
}

bool map_iterator_has_next(MapIterator* it) {
    // If current bucket has more items, return true
    if (list_iter_has_next(&it->current_bucket_it))
        return true;

    // Advance to next non-empty bucket in map->used_buckets
    while (list_iter_has_next(&it->list_it)) {
        int index, unused_col;
        double unused_val;

        list_iter_next(&it->list_it, &index, &unused_col, &unused_val);
        List* bucket = it->map->table[index];

        if (bucket && bucket->head != NULL) {
            it->current_bucket_it = list_iter_create(bucket); // reset bucket iterator
            return list_iter_has_next(&it->current_bucket_it);
        }
    }

    return false;
}

void map_iterator_next(MapIterator* it, int* row, int* col, double* val) {
    if (!list_iter_has_next(&it->current_bucket_it)) {
        // Advance if current bucket is exhausted
        while (list_iter_has_next(&it->list_it)) {
            int index, unused_col;
            double unused_val;

            list_iter_next(&it->list_it, &index, &unused_col, &unused_val);
            List* bucket = it->map->table[index];

            if (bucket && bucket->head != NULL) {
                it->current_bucket_it = list_iter_create(bucket);
                break;
            }
        }
    }

    list_iter_next(&it->current_bucket_it, row, col, val);
}


