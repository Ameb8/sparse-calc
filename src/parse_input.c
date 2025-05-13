#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "../include/parse_input.h"
#include "../include/matrix.h"
#include "../include/eval_expr.h"
#include "../include/parse_expr.h"

typedef bool (*CommandFn)(char* input);
#define NUM_COMMANDS 4
#define MAX_MATRICES 50

typedef struct {
    const char* name;
    CommandFn fn;
} Command;

bool list_matrices(char* input);
bool show_matrix(char* input);
bool help(char* input);
bool set_matrix(char* input);

Command commands[] = {
    {"matrix", set_matrix},
    {"list", list_matrices},
    {"show", show_matrix},
    {"help", help},
};

char** split_input(const char* input, int* count) {
    return NULL;
}

bool set_matrix(char* input) {
    return false;
}

bool show_matrix(char* input) {

}

bool help(char* input) {
    return false;
}

bool list_matrices(char* input) {
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

bool handle_input(char* input) {
    if(find_command(input))
        return true;

    int num_tokens = 0;
    int rpn_len = 0;
    Token* expr = parse_expr(input, &num_tokens);
    Token* rpn_expr = convert_rpn(expr, num_tokens, &rpn_len);
    Matrix* result = eval_expr(rpn_expr, rpn_len);
    if(result == NULL)
        return false;
    
    return true;

}