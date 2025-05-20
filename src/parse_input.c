#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "../include/parse_input.h"
#include "../include/matrix.h"
#include "../include/eval_expr.h"
#include "../include/parse_expr.h"
#include "../include/matrix_cli.h"
#include "../include/runtime_data.h"
#include "../include/repository.h"

typedef bool (*CommandFn)(char* input);
#define NUM_COMMANDS 5
#define MAX_MATRICES 50

typedef struct {
    const char* name;
    CommandFn fn;
} Command;

bool list_matrices(char* input);
bool show_matrix(char* input);
bool help(char* input);
bool set_matrix(char* input);
bool clear_terminal(char* input);
bool save_as_matrix(char* input);
bool save_matrix(char* input);

Command commands[] = {
    {"matrix", set_matrix},
    {"list", list_matrices},
    {"show", show_matrix},
    {"help", help},
    {"clear", clear_terminal},
    {"save as", save_as_matrix},
    {"save", save_matrix}
};

char** split_input(const char* input, int* count) {
    return NULL;
}

bool create_matrix(char* name) {
    Matrix* new = get_matrix_user(name);
    if(!new) {
        printf("Error: Matrix not created\n");
        return false;
    }

    bool res = rd_save_matrix(name, new);
    if(!res) {
        printf("Error: Matrix with name %s already exists\n", name);
        return false;
    }
    return true;
}

bool set_matrix(char* input) {
    int num_args = 0;
    char** args = get_args(input, &num_args);

    if(!args || num_args == 0) {
        printf("Error: No Matrix name provided\n");
        return false;
    }


    for(int i = 0; i < num_args; i++) {
        if(!args[i]) {
            printf("Error: Invalid argument %d\n", i);
            continue;
        }
        create_matrix(args[i]);
    }

    return true;
}

bool show_matrix(char* input) {
    int num_args = 0;
    char** args = get_args(input, &num_args);

    if(!args || num_args == 0) {
        printf("Error: No Matrix name provided\n");
        return false;
    }

    for(int i = 0; i < num_args; i++) {
        if(!args[i]) {
            printf("Error: Invalid argument %d\n", i);
            continue;
        }

        // Check if matrix exists
        Matrix* matrix = rd_get_matrix(args[i]);
        if(!matrix) {
            printf("Error: Matrix %s not found\n", args[i]);
            continue;
        }

        matrix_print(matrix);
    }

    return true;
}

bool help(char* input) {
    return false;
}

bool list_matrices(char* input) {
    rd_print_all();
    return true;
}

bool clear_terminal(char* input) {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

    return true;
}


bool save_matrix_repo(char* name, char* save_name) {
    if(!name || !save_name) { // Check if argument valid
        printf("Error: Invalid matrix name %s\n", name);
        return false;
    }

    // Get argument matrix
    Matrix* matrix = get_matrix_user(trim(save_name));

    if(!matrix) { // argument is not valid matrix name
        printf("Matrix %s not found\n", trim(save_name));
        return false;
    }

    // Check if name unique
    bool save = repo_is_unique(save_name);
    
    if(!save) // If name not unique, check if user wants to overwrite
        save = replace_saved_matrix(save_name);
    
    if(save) // Save matrix
        return repo_matrix_save(save_name, matrix);
    
    return false; // Matrix not saved
}

bool save_as_matrix(char* input) {
    // Get arguments
    int num_args = 0;
    char** args = get_args(input, &num_args);

    // Check if arguments
    if(!args || num_args == 0) {
        printf("Error: No Matrix name provided\n");
        return false;
    }

    //Iterate through arguments
    for(int i = 0; i < num_args; i++) {
        // Get name to save as
        char* save_name = get_matrix_name();
        bool saved = false;

        if(save_name)
            saved = save_matrix_repo(args[i], save_name);
        else
            printf("Matrix name invalid\n");

        if(saved)
            printf("Matrix %s saved as %s\n", trim(args[i]), save_name);
        else
            printf("Matrix %s not saved\n", trim(args[i]));
    }

    return true;
}

bool save_matrix(char* input) {
    // Get arguments
    int num_args = 0;
    char** args = get_args(input, &num_args);
    
    // Check if arguments
    if(!args || num_args == 0) {
        printf("Error: No Matrix name provided\n");
        return false;
    }
    
    //Iterate through arguments
    for(int i = 0; i < num_args; i++) {
        bool saved = false;

        if(saved)
            printf("Matrix %s\n", trim(args[i]), trim(args[i]));
        else
            printf("Matrix %s not saved\n", trim(args[0]));
    }

    return true;
}

bool starts_with(const char* input, const char* command) {
    // Check if str2 is longer than str1
    if (strlen(command ) > strlen(input)) {
        return false; // input can't start with command if command is longer
    }
    
    // Compare characters of both strings
    while(*command) {
        if (*input != *command) {
            return false; // Doesn't match, return false
        }
        input++;
        command++;
    }
    
    return true; // Characters match, return true
}

bool find_command(char* input) {
    for(int i = 0; i < NUM_COMMANDS; i++) {
        if(starts_with(input, commands[i].name)) {
            return commands[i].fn(input);
        }
    }

    return false;
}


char** split_expr(const char* str, int* count) {
    if (!str || !count) return NULL;

    char* copy = strdup(str);  // Create a modifiable copy
    if (!copy) return NULL;

    char** result = NULL;
    int size = 0;
    int capacity = 4;

    result = malloc(capacity * sizeof(char*));
    if (!result) {
        free(copy);
        return NULL;
    }

    char* token = strtok(copy, "=");
    while (token != NULL) {
        if (size >= capacity) {
            capacity *= 2;
            char** temp = realloc(result, capacity * sizeof(char*));
            if (!temp) {
                // Free previously allocated memory
                for (int i = 0; i < size; i++) free(result[i]);
                free(result);
                free(copy);
                return NULL;
            }
            result = temp;
        }

        result[size++] = strdup(token);  // Copy the token into a new string
        token = strtok(NULL, "=");
    }

    *count = size;
    free(copy);
    return result;
}

bool save_result(Matrix* result, char* target) {
    Matrix* matrix = NULL;
    matrix == rd_get_matrix(target);
    bool res_flag = true;

    if(!matrix) {
        res_flag = rd_overwrite_matrix(target, result);
    } else {
        res_flag = rd_overwrite_matrix(target, result);

    }

    return res_flag;
}


bool handle_input(char* input) {
    if(find_command(input))
        return true;

    int num_args = 0;
    char** expr = split_expr(input, &num_args);
    if(!expr || num_args != 2) {
        printf("Error: Invalid expression\n");
        return false;
    }
    expr[0] = trim(expr[0]);

    #ifdef DEBUG
    printf("expr[0]: %s\n", expr[0]);
    
    printf("ASCII dump of expr[0]:\n");
    for(int i = 0; expr[0][i] != '\0'; i++) {
        printf("  expr[0][%d] = '%c' (ASCII %d)\n", i, expr[0][i], (unsigned char)expr[0][i]);
    }
    #endif

    int end, row, col;
    char* name;
    Matrix* matrix = NULL; // Will contain valid Matrix if index is being assigned

    // Check if matrix index is being assigned
    if(parse_pattern(expr[0], 0, &end) && expr[0][end] == '\0') {
        #ifdef DEBUG
        printf("parse_pattern success. end = %d, expr[end] = '%c'\n", end, expr[0][end]);
        #endif

        get_matrix_index(expr[0], &name, &row, &col);
        
        #ifdef DEBUG
        printf("%s[%d][%d] = ?\n", name, row, col);
        #endif

        if(name != NULL) 
            matrix = rd_get_matrix(name);

    }


    Matrix* result = solve_expr(expr[1]);

    #ifdef DEBUG
    printf("\nMatrix %s:\n", name);
    matrix_print(matrix);
    printf("\nResult MAtrix:\n");
    matrix_print(result);
    #endif

    if(!result)
        return false;

    if(matrix) {
        #ifdef DEBUG
        printf("Matrix Valid (handle_input())\n");
        #endif

        if(!result ||result->rows != 1 || result->cols != 1) {
            printf("Error: Expression must produce number to assign to matrix index\n");
            return false;
        }

        if(row >= matrix->rows || row < 0 || col >= matrix->cols || col < 0) {
            printf("Error: Indices [%d][%d] out of bounds for matrix %s\n", row, col, name);
            return false;
        }

        matrix_set(matrix, row, col, matrix_get(result, 0, 0));
        return true;
    } else {
        #ifdef DEBUG
        printf("Matrix Invalid (handle_input())\n");
        #endif

        return save_result(result, expr[0]);
    }
}