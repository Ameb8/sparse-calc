#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

//typedef struct Node Node;

typedef struct Node {
    int row;
    int col;
    double val;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
    long size;
} List;

List* list_create();
void list_append(List* list, int row, int col, double val);
void list_prepend(List* list, int row, int col, double val);
double list_get_val(List* list, int row, int col);
double list_increment_val(List* list, int row, int col, double val);
void list_update_val(List* list, int row, int col, double val);
double list_remove_val(List* list, int row, int col);
void list_free(List* list);
void list_print(List* list);
bool list_save(List* list, const char* filename);
List* list_load(const char* filename);

#endif