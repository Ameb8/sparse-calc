#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "../include/parse_input.h"
#include "../include/matrix.h"
#include "../include/eval_expr.h"



typedef bool (*CommandFn)(char* input);
#define NUM_COMMANDS 4
#define MAX_MATRICES 50


typedef struct {
    const char* name;
    CommandFn fn;
} Command;

// Function prototypes
bool list_matrices(char* input);
bool show_matrix(char* input);
bool help(char* input);
bool set_matrix(char* input);


char* matrix_names[MAX_MATRICES];
Matrix* matrices[MAX_MATRICES];
int num_matrices = 0;

Command commands[] = {
    {"matrix", set_matrix},
    {"list", list_matrices},
    {"show", show_matrix},
    {"help", help},
};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

bool set_matrix(char* input) {
    return false;
}

bool show_matrix(char* input) {
    return false;
}

bool help(char* input) {
    return false;
}

bool list_matrices(char* input) {
    return false;
}

bool set_user_matrix(char* name, Matrix* matrix) {
    if(num_matrices >= MAX_MATRICES)
        return false;
    
    matrix_names[num_matrices] = name;
    matrices[num_matrices++] = matrix;

    return true;
}

Matrix* get_user_matrix(const char* name) {
    #ifdef DEBUG
    printf("Num user matrices:\t%d", num_matrices);
    #endif

    for(int i = 0; i < num_matrices; i++) {
        #ifdef DEBUG
        printf("User matrix %d\t%s", i + 1, matrix_names[i]);
        #endif

        if(!strcmp(matrix_names[i], name)) {
            
            #ifdef DEBUG
            printf("Matrix found:\n");
            matrix_print(matrices[i]);
            #endif
            
            return matrices[i];

        }
    }

    #ifdef DEBUG
    printf("Matrix '%s' not found\n", name);
    #endif

    return NULL;
}

// Gets matrix name, row, and col from matrix indexing
void get_matrix_index(const char* input, char** name, int* row, int* col) {
    char temp_name[100];
    int temp_row, temp_col;

    if (sscanf(input, "%99[^[][%d][%d]", temp_name, &temp_row, &temp_col) == 3) {
        *name = strdup(temp_name);
        *row = temp_row;
        *col = temp_col;
    } else {
        *name = NULL;
        *row = 0;
        *col = 0;
    }
}

char* get_val(char* input) {
    int row, col;
    char* name;

    get_matrix_index(input, &name, &row, &col);

    #ifdef DEBUG
    printf("Matrix vals:\n%s[%d][%d]\n", name, row, col);
    printf("name pointer: %p, name string: %s\n", (void*)name, name);
    printf("User matrices:\t(%d)\n", num_matrices);

    for(int i = 0; i < num_matrices; i++) {
        printf("\nMatrix name:\t%s\n", matrix_names[i]);
        printf("Matrix:\n");
        matrix_print(matrices[i]);
    }
    #endif

    Matrix* matrix = get_user_matrix(name);

    #ifdef DEBUG
    printf("User matrix:\n");
    matrix_print(matrix);
    #endif

    char* result = malloc(32);
    sprintf(result, "%g", matrix_get(matrix, row, col));
    return result;
}

// Checks if a character is valid in matrix name
int is_name_char(char c) {
    return isalpha(c) || c == '_';
}

// Checks if the string at pos is a full pattern and gets its length
int parse_pattern(const char* str, int start, int* end) {
    int i = start;
    // Parse name
    if (!is_name_char(str[i])) return 0;
    while (is_name_char(str[i])) i++;
    
    // Parse row index
    if (str[i] != '[') return 0;
    i++;
    if (str[i] == '-') i++; // optional minus
    if (!isdigit(str[i])) return 0;
    while (isdigit(str[i])) i++;
    if (str[i] != ']') return 0;
    i++;

    // Parse column index
    if (str[i] != '[') return 0;
    i++;
    if (str[i] == '-') i++;
    if (!isdigit(str[i])) return 0;
    while (isdigit(str[i])) i++;
    if (str[i] != ']') return 0;
    i++;

    *end = i;
    return 1;
}

char* replace_all(const char* input_str) {
    int len = strlen(input_str);
    char* output = malloc(len * 2);  // allocate more than enough
    output[0] = '\0';

    int i = 0;
    while (i < len) {
        int end;
        if (parse_pattern(input_str, i, &end)) {
            int match_len = end - i;
            char* match = malloc(match_len + 1);
            strncpy(match, &input_str[i], match_len);
            match[match_len] = '\0';

            #ifdef DEBUG
            printf("Pattern matched: '%s'\n", match);
            #endif

            char* replacement = get_val(match);

            #ifdef DEBUG
            printf("Replacing '%s' with '%s'\n", match, replacement);
            #endif

            strcat(output, replacement);

            free(match);
            free(replacement);

            i = end;
        } else {
            int len_out = strlen(output);
            output[len_out] = input_str[i];
            output[len_out + 1] = '\0';
            i++;
        }
    }

    return output;
}



Token* parse_expr(char* expr, int* token_count) {
    int capacity = 16;
    Token* tokens = malloc(sizeof(Token) * capacity);
    *token_count = 0;

    expr = replace_all(expr);

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