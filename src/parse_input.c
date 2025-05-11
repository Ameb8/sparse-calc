#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "../include/parse_input.h"
#include "../include/matrix.h"


typedef void (*CommandFn)(char* input);
#define NUM_COMMANDS 4


typedef struct {
    const char* name;
    CommandFn fn;
} Command;

// Function prototypes
void list_matrices(char* input);
void show_matrix(char* input);
void help(char* input);
void set_matrix(char* input);


char* matrix_names[50];
Matrix* matrices[50];
Command commands[] = {
    {"matrix", set_matrix},
    {"list", list_matrices},
    {"show", show_matrix},
    {"help", help},
};



Token* parse_expr(char* expr, int* token_count) {
    int capacity = 16;
    Token* tokens = malloc(sizeof(Token) * capacity);
    *token_count = 0;

    int i = 0;
    while (expr[i] != '\0') {
        char c = expr[i];

        if (isspace(c)) {
            i++;
            continue;
        }

        if (*token_count >= capacity) {
            capacity *= 2;
            tokens = realloc(tokens, sizeof(Token) * capacity);
        }

        Token token;
        token.symbol = NULL;
        token.val = 0;

        switch(c) {
            case '+':
                token.type = TOKEN_BIN_OP;
                token.symbol = strdup("+");
                token.val = 0;
                i++;
                break;
            case '-':
                token.type = TOKEN_BIN_OP;
                token.symbol = strdup("-");
                token.val = 0;
                i++;
                break;
            case '*':
                token.type = TOKEN_BIN_OP;
                token.symbol = strdup("*");
                token.val = 1;
                i++;
                break;
            case '\'':
                token.type = TOKEN_UN_OP;
                token.symbol = strdup("'");
                token.val = 2;
                i++;
                break;
            case '(':
                token.type = TOKEN_LPAREN;
                token.symbol = strdup("(");
                token.val = 3;
                i++;
                break;
            case ')':
                token.type = TOKEN_RPAREN;
                token.symbol = strdup(")");
                token.val = 3;
                i++;
                break;

            default:
                if (isalpha(c) || c == '_') {
                    int start = i;
                    while (isalpha(expr[i]) || expr[i] == '_') i++;
                    int length = i - start;
                    token.type = TOKEN_MATRIX;
                    token.symbol = strndup(expr + start, length);
                    token.val = 3; // unused
                } else if (isdigit(c) || (c == '.' && isdigit(expr[i + 1]))) {
                    int start = i;
                    int has_dot = 0;

                    while (isdigit(expr[i]) || (!has_dot && expr[i] == '.')) {
                        if (expr[i] == '.') has_dot = 1;
                        i++;
                    }

                    int length = i - start;
                    char* num_str = strndup(expr + start, length);
                    if(has_dot < 2) {
                        token.type = TOKEN_NUMERIC;
                        token.symbol = num_str;
                        token.val = strtod(num_str, NULL); 
                    } else {
                        token.type = TOKEN_INVALID;
                        token.symbol = num_str;
                    }
                } else {
                    token.type = TOKEN_INVALID;
                    token.symbol = strndup(&c, 1);
                    i++;
                }
        }

        tokens[(*token_count)++] = token;
    }

    return tokens;
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
            commands[i].fn(input);
            return true;
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