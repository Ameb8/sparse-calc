#include <stdio.h>
#include "list_iterator.h"
#include "list.h"

ListIterator list_iter_create(List* list) {
    ListIterator iter;
    iter.current = list->head;
    return iter;
}

bool list_iter_has_next(ListIterator* iter) {
    if(iter->current == NULL)
        return false;
    return true;
}

void list_iter_next(ListIterator* iter, int* row, int* col, double* val) {
    if (iter->current != NULL) {
        *row = iter->current->row;
        *col = iter->current->col;
        *val = iter->current->val;
        iter->current = iter->current->next;
    }
}