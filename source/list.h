#ifndef LIST_H
#define LIST_H

typedef struct Node Node;

typedef struct {
    Node* head;
    long size;
} List;

List* list_create();
void list_append(List* list, double val);
void list_free(List* list);


#endif