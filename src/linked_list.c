#include "../include/linked_list.h"

// Function to create a new node
Node* create_node(int row, int col, double value) {
    // Allocate memory for new node
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (!new_node) { // Allocation failed, exit program
        fprintf(stderr, "Memory allocation failed for new node.\n");
        exit(1);
    }

    // Initialize fields of new node
    new_node->row = row;
    new_node->col = col;
    new_node->value = value;
    new_node->next = NULL;

    return new_node;
}

// Create copy of node
Node* copy_node(Node* current) {
    return create_node(current->row, current->col, current->value);
}

// Function to add a node to the front of the list
void add_node(Node** head, int row, int col, double value) {
    Node* new_node = create_node(row, col, value); //  Create new node
    
    // Adjust pointers to add to front of list
    new_node->next = *head;
    *head = new_node;
}

// Add node to end of list
void append_node(Node* head, Node* add) {
    Node* temp = head;
    
    // Find last node
    while(temp->next != NULL)
        temp = temp->next;

    temp->next = add; // Add new node
}

// Function to free the linked list
void free_list(Node* head) {
    Node* current = head;
    while (current != NULL) {
        Node* temp = current;
        current = current->next;
        free(temp);
    }
}

// Prints full linked list
void print_list(Node* head) {
    if(head == NULL) { // List is null
        printf("\nList is null!!!\n");
    }

    Node* temp = head;
    while(temp != NULL) { // Iterate through nodes
        // Print node
        printf("Row: %d, Col: %d, Val: %.2f\n", temp->row, temp->col, temp->value);
        temp=temp->next;
    }
    printf("\n");
}
