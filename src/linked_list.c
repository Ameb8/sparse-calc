#include "../include/linked_list.h"

typedef struct {
    int row;
    int col;
    double val;
} Entry;

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

int compare_entries(const void* a, const void* b) {
    Entry* ea = (Entry*)a;
    Entry* eb = (Entry*)b;

    if (ea->row != eb->row)
        return ea->row - eb->row;
    return ea->col - eb->col;
}

Node* sort_list(Node* head) {
    Node* temp = head;
    int length = 0;

    while(temp != NULL) {
        length++;
        temp = temp->next;
    }

    // Create an array of Entry structs
    Entry* entries = malloc(length * sizeof(Entry));
    if (!entries) { // Allocation failed, exit program
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }

    temp = head;
    int i = 0;

    while(temp != NULL) { // Populate entries array
        entries[i].row = temp->row;
        entries[i].col = temp->col;
        entries[i].val = temp->value;
        i++;
        temp = temp->next;
    }

    // Sort entries array
    qsort(entries, length, sizeof(Entry), compare_entries);

    // Convert back to linked list
    Node* sorted_head = create_node(entries[0].row, entries[0].col, entries[0].val);
    temp = sorted_head;
    for(int i = 1; i < length; i++) {
        temp->next = create_node(entries[i].row, entries[i].col, entries[i].val);
        temp = temp->next;
    }

    free(entries);

    return sorted_head;
}