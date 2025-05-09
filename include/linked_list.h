#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdio.h>
#include <stdlib.h>

// Node to store non-zero vals in linked list
typedef struct Node {
    int row;
    int col;
    double value;
    struct Node* next;
} Node;

Node* create_node(int row, int col, double value);
Node* copy_node(Node* current);
void add_node(Node** head, int row, int col, double value);
void free_list(Node* head);
void append_node(Node* head, Node* add);
void print_list(Node* head);
Node* sort_list(Node* head);

#endif
