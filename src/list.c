#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include "../include/list.h"


// Function to create a new node
Node* node_create(int row, int col, double val) {
    // Allocate memory for the new node
    Node* new_node = (Node*)malloc(sizeof(Node));

    if (new_node == NULL) { // Handle memory allocation failure
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


// Create list struct
List* list_create() {
    // Allocate memory for the List
    List* list = (List*)malloc(sizeof(List));
    
    // Handle memory allocation failure
    if (list == NULL) {
        fprintf(stderr, "Memory allocation failed for List.\n");
        exit(1);
    }

    // Initialize the list: head is NULL and size is 0
    list->head = NULL;
    list->size = 0;

    return list;
}


// Append entry to end of list
void list_append(List* list, int row, int col, double val) {
    Node* node = node_create(row, col, val); // Create new node
    list->size++; // Increment list size
    

    if(list->head == NULL) // List empty, set new node to head
        list->head = node;
    else {
        Node* temp = list->head; // Create node to iterate list
        
        // Find list node
        while(temp->next != NULL)
            temp = temp->next;
        
        temp->next = node; // Add new node to end
    }
}


// Prepend entry to beggining of list
void list_prepend(List* list, int row, int col, double val) {
    Node* node = node_create(row, col, val); // Create new node
    list->size++; // Increment list size

    // Set pointers
    node->next = list->head;
    list->head = node;
}


// Get value from list given row and column
double list_get_val(List* list, int row, int col) {
    if(!list || !list->head)
        return 0; // L=Return 0 if list null or empty

    Node* temp = list->head; // Create temp node to traverse list

    while(temp != NULL) { // Search list
        if(temp->row == row && temp->col == col)
            return temp->val; // Node found, return value

        temp = temp->next; // Increment temp
    }

    return 0; // Node not found
}


// Update row, col entry to new value
void list_update_val(List* list, int row, int col, double val) {
    Node* temp = list->head; // Create temp node to traverse list

    while(temp != NULL) { // Search list
        if(temp->row == row && temp->col == col)
            temp->val = val; // Node found, update value

        temp = temp->next; // Increment temp node
    }
}


// Increment val by amount in val parameter
double list_increment_val(List* list, int row, int col, double val) {
    Node* temp = list->head; // Create temp node to traverse list

    while(temp != NULL) { // Search list
        if(temp->row == row && temp->col == col) { // Node found
            temp->val += val; // Increment val
            return temp->val; // Return new val
        }

        temp = temp->next; // Iterate temp
    }

    return 0; // Node not found
}


// Remove entry from list
double list_remove_val(List* list, int row, int col) {
    Node* temp = list->head; // Create temop node to iterate through list

    // List is empty
    if(temp == NULL)
        return -DBL_MAX;

    list->size--; // Decrement list size
    
    // First node matches
    if(temp->row == row && temp->col == col) {
        // Remove node from list
        list->head = temp->next;
        double val = temp->val;
        free(temp); // Deallocate removed node's memory
        
        return val; // Return nodes original value
    }

    while(temp->next != NULL) { // Search list
        // Node found
        if(temp->next->row == row && temp->next->col == col) {
            // Remove node from list
            Node* to_delete = temp->next;
            double val = to_delete->val;
            temp->next = to_delete->next;
            free(to_delete); // Deallocate removed node's memory

            return val; // Return node's original value
        }

        temp = temp->next; // Iterate temp node
    }

    list->size++; // Increment size to original if node not found

    return -DBL_MAX; // Return not found flag
}


// Deallocate list's memory
void list_free(List* list) {
    if(!list) // List is null
        return;

    // Create nodes to traverse list
    Node* current = list->head;
    Node* next;

    // Iterate through the list and free each node
    while(current) {
        next = current->next; // Save next node
        free(current); // Deallocate node's memory
        current = next; // Iterate node
    }

    free(list); // Deallocate memory for list struct
}


// Prints list
void list_print(List* list) {
    if(list == NULL || list->head == NULL)
        return; // List is null or empty

    Node* temp = list->head; // Create temp node to traverse list

    while(temp != NULL) { // Iterate through list
        // Print node row col and value
        printf("[(%d, %d) = %.2f", temp->row, temp->col, temp->val);

        // Add comma if more nodes follow
        if(temp->next != NULL)
            printf(", ");

        temp=temp->next; // Iterate tempo through list
    }

    printf("\n"); // Add newline after print
}
