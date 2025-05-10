#ifndef LIST_ITERATOR_H
#define LIST_ITERATOR_H

#include <stdbool.h>
#include "list.h"

typedef struct {
    Node* current;
} ListIterator;

ListIterator list_iter_create(List* list);
bool list_iter_has_next(ListIterator* it);
void list_iter_next(ListIterator* it, int* row, int* col, double* val);

#endif