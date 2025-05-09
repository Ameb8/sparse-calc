#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "../include/list.h"


// Function to create a new node
Node* node_create(int row, int col, double val) {
    // Allocate memory for the new node
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        // Handle memory allocation failure
        fprintf(stderr, "Memory allocation failed for new node.\n");
        exit(1);
    }

    // Initialize the node with the passed data and set next to NULL
    new_node->row = row;
    new_node->col = col;
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

void list_append(List* list, int row, int col, double val) {
    Node* node = node_create(row, col, val);
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

void list_prepend(List* list, int row, int col, double val) {
    Node* node = node_create(row, col, val);
    list->size++;

    node->next = list->head;
    list->head = node;
}

double list_get_val(List* list, int row, int col) {
    Node* temp = list->head;

    while(temp != NULL) {
        if(temp->row == row && temp->col == col)
            return temp->val;

        temp = temp->next;
    }

    return 0;
}

double list_update_val(List* list, int row, int col, double val) {
    Node* temp = list->head;

    while(temp != NULL) {
        if(temp->row == row && temp->col == col)
            temp->val = val;

        temp = temp->next;
    }

    return 0;
}

double list_increment_val(List* list, int row, int col, double val) {
    Node* temp = list->head;

    while(temp != NULL) {
        if(temp->row == row && temp->col == col) {
            temp->val += val;
            return temp->val;
        }

        temp = temp->next;
    }

    return 0;
}

double list_remove_val(List* list, int row, int col) {
    Node* temp = list->head;

    // List is empty
    if(temp == NULL)
        return -DBL_MAX;

    list->size--;
    
    // First node matches
    if(temp->row == row && temp->col == col) {
        list->head = temp->next;
        double val = temp->val;
        free(temp);
        
        return val;
    }

    while(temp->next != NULL) {
        if(temp->next->row == row && temp->next->col == col) {
            Node* to_delete = temp->next;
            double val = to_delete->val;
            temp->next = to_delete->next;
            free(to_delete);

            return val;
        }

        temp = temp->next;
    }

    list->size++;

    return -DBL_MAX;
}

void list_free(List* list) {
    if(list == NULL)
        return;

    Node* current = list->head;
    Node* next;

    // Iterate through the list and free each node
    while (current != NULL) {
        next = current->next;
        free(current); 
        current = next;
    }

    free(list);
}

void list_print(List* list) {
    if(list == NULL || list->head == NULL)
        return;

    Node* temp = list->head;

    while(temp != NULL) {
        printf("[(%d, %d) = %.2f", temp->row, temp->col, temp->val);

        if(temp->next != NULL)
            printf(", ");

        temp=temp->next;
    }

    printf("\n");
}
