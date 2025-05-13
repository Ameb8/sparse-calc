#ifdef TEST

#include <stdlib.h>
#include <float.h>
#include "../include/list.h"
#include "../include/hash_map.h"
#include "../include/map_iterator.h"
#include "test_util.h"
#include "test_data_struct.h"



void test_list_remove_val() {
    List* list = list_create();
    
    // Append values
    list_append(list, 0, 0, 1.1);
    list_append(list, 0, 1, 2.2);
    list_append(list, 1, 0, 3.3);
    
    // Confirm initial size
    ASSERT_INT_EQ(list->size, 3);

    // Remove middle element
    double removed_val = list_remove_val(list, 0, 1);
    ASSERT_DOUBLE_EQ(removed_val, 2.2);
    ASSERT_INT_EQ(list->size, 2);
    ASSERT_DOUBLE_EQ(list_get_val(list, 0, 1), 0.0);

    // Remove head
    removed_val = list_remove_val(list, 0, 0);
    ASSERT_DOUBLE_EQ(removed_val, 1.1);
    ASSERT_INT_EQ(list->size, 1);
    ASSERT_DOUBLE_EQ(list_get_val(list, 0, 0), 0.0);

    // Remove tail
    removed_val = list_remove_val(list, 1, 0);
    ASSERT_DOUBLE_EQ(removed_val, 3.3);
    ASSERT_INT_EQ(list->size, 0);
    ASSERT_DOUBLE_EQ(list_get_val(list, 1, 0), 0.0);

    // Remove from empty list
    removed_val = list_remove_val(list, 5, 5);
    ASSERT_DOUBLE_EQ(removed_val, -DBL_MAX);

    free(list); // Assuming this frees the list correctly
}


void test_map_get_empty() {
    HashMap* map = map_create();

    // Check for a few different (row, col) pairs
    ASSERT_INT_EQ(map_get(map, 0, 0), 0);
    ASSERT_INT_EQ(map_get(map, 5, 10), 0);
    ASSERT_INT_EQ(map_get(map, -3, 7), 0);
    ASSERT_INT_EQ(map_get(map, 1000, 1000), 0);

    free_hash_map(map);
}

void test_map_it_has_next() {
    HashMap* map = map_create();
    MapIterator map_it = map_iterator_create(map);
    bool res = map_iterator_has_next(&map_it);
    ASSERT_INT_EQ((int) res, 0);
    map_set(map, 1, 1, 1);
    map_it = map_iterator_create(map);
    res = map_iterator_has_next(&map_it);
    ASSERT_INT_EQ((int)res, 1);
    free_hash_map(map);
}

void test_data_struct() {
    test_list_remove_val();
    test_map_get_empty();
    test_map_it_has_next();
    end_test("Data structures");
}

#endif