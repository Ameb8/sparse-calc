#include <stdio.h>
#include "../include/list_iterator.h"
#include "../include/list.h"


// Function to create ListIterator
ListIterator list_iter_create(List* list) {
    ListIterator iter; // Allocate memory statically
    iter.current = list->head; // Set first item in list to current

    return iter;
}


// Check if iterator has additional values
bool list_iter_has_next(ListIterator* iter) {
    if(iter->current == NULL)
        return false;

    return true;
}


// Get next value form list
void list_iter_next(ListIterator* iter, int* row, int* col, double* val) {
    if(iter->current != NULL) {
        // Assign row col and value
        *row = iter->current->row;
        *col = iter->current->col;
        *val = iter->current->val;

        // Get next list entry
        iter->current = iter->current->next;
    }
}