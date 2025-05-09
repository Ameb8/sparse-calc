#include <stdio.h>
#include <stdlib.h>
#include "list.h"

typedef struct {
    double val;
    Node* next;
} Node;

// Function to create a new node
Node* node_create(double val) {
    // Allocate memory for the new node
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        // Handle memory allocation failure
        fprintf(stderr, "Memory allocation failed for new node.\n");
        exit(1);
    }

    // Initialize the node with the given value and set next to NULL
    new_node->val = val;
    new_node->next = NULL;

    return new_node;
}

List* list_create() {
    // Allocate memory for the List
    List* list = (List*)malloc(sizeof(List));
    if (list == NULL) {
        // Handle memory allocation failure
        fprintf(stderr, "Memory allocation failed for List.\n");
        exit(1);
    }

    // Initialize the list: head is NULL and size is 0
    list->head = NULL;
    list->size = 0;

    return list;
}

void list_append(List* list, double val) {
    Node* node = node_create(val);
    list->size++;
    
    if(list->head == NULL)
        list->head = node;
    else {
        Node* temp = list->head;
        
        while(temp->next != NULL)
            temp = temp->next;
        
        temp->next = node;
    }
}

void list_free(List* list) {
    Node* current = list->head;
    Node* next;

    // Iterate through the list and free each node
    while (current != NULL) {
        next = current->next; // Save the next node
        free(current);    // Free the current node
        current = next;       // Move to the next node
    }

    // Finally, free the list structure itself
    free(list);
}
