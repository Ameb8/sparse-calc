#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "../include/linked_list.h"
#include "../include/matrix_util.h"

Matrix* get_user_matrix() {
    return NULL;
}


int get_user_int(const char* name, int max_val) {
    char buffer[100]; // Enough for most inputs
    char* endptr;
    long val;

    while (1) {
        printf("%s (less than %d, negative to stop): ", name, max_val);

        if (!fgets(buffer, sizeof(buffer), stdin)) {
            printf("Input error. Try again.\n");
            continue;
        }

        // Remove trailing newline if present
        buffer[strcspn(buffer, "\n")] = '\0';

        val = strtol(buffer, &endptr, 10);

        // Check if nothing was parsed or there were trailing characters
        if (endptr == buffer || *endptr != '\0') {
            printf("Invalid input. Please enter an integer.\n");
            continue;
        }

        if (val < 0 || val < max_val) {
            return (int)val;
        }

        printf("%s must be less than %d. Try again.\n", name, max_val);
    }
}

/*
// Function to collect int less than max_val from user
int get_user_int(const char* name, int max_val) {
    int val;
    char c;

    while (1) {
        printf("%s (less than %d, negative to stop): ", name, max_val);

        if (scanf("%d", &val) != 1) {
            // Clear invalid input
            while ((c = getchar()) != '\n' && c != EOF);
            printf("Invalid input. Please enter an integer.\n");
            continue;
        }

        if (val < 0 || val < max_val) {
            return val;
        }

        printf("%s must be less than %d. Try again.\n", name, max_val);
    }
}*/

bool is_duplicate(Node* head, int row, int col) {
    Node* current = head;
    while (current != NULL) {
        if (current->row == row && current->col == col) {
            return 1; // Duplicate found
        }
        current = current->next;
    }
    return 0; // No duplicate
}


// Gets linked list of non-zero values in matrix from user
Node* get_nz_vals(int height, int width) {
    int row, col;
    double value;
    Node* head = NULL;
    Node* tail = NULL;

    printf("Enter matrix entries (row col value). Use negative row or col to finish:\n");

    while (1) {
        row = get_user_int("Row", height);
        if (row < 0) break;

        col = get_user_int("Col", width);
        if (col < 0) break;

        if (is_duplicate(head, row, col)) {
            printf("Duplicate entry at (%d, %d) ignored. Try again with a different position.\n", row, col);
            continue;
        }

        printf("Value: ");
        if (scanf("%lf", &value) != 1) {
            // Clear invalid input
            while ((getchar()) != '\n');
            printf("Invalid value. Try again.\n");
            continue;
        }

        Node* new_node = create_node(row, col, value);

        if (head == NULL) {
            head = new_node;
            tail = new_node;
        } else {
            tail->next = new_node;
            tail = new_node;
        }
    }

    return head;

}

int main() {
    Node* vals = get_nz_vals(100, 100);
    print_list(vals);

    return 0;
}