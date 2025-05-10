#include <stdio.h>
#include "hash_map.h"
#include "map_iterator.h"

int main() {
    // Create a new hashmap
    HashMap* map = map_create();

    // Insert some values
    map_insert(map, 1, 2, 3.5);
    map_insert(map, 2, 3, 7.0);
    map_insert(map, 1, 2, 1.5); // This will accumulate the value
    map_insert(map, 2, 3, -7.0);

    // Retrieve and print values
    printf("Value at (1, 2): %f\n", map_get(map, 1, 2)); // Should be 5.0 (3.5 + 1.5)
    printf("Value at (2, 3): %f\n", map_get(map, 2, 3)); // Should be 7.0
    printf("Size: %d\n", map->size);

    MapIterator map_it = map_iterator_create(map);

    while(map_iterator_has_next(&map_it)) {
        int row, col;
        double val;

        map_iterator_next(&map_it, &row, &col, &val);
        printf("matrix[%d][%d] = %.2f\n", row, col, val);
    }

    // Free the hashmap
    free_hash_map(map);

    return 0;
}