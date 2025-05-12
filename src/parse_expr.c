#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>
#include <ctype.h>
#include "../include/matrix.h"
#include "../include/token.h"
#include "../include/eval_expr.h"
#include "../include/runtime_data.h"


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

    Matrix* matrix = rd_get_matrix(name);

    if(matrix == NULL)
        return NULL;

    double val = matrix_get(matrix, row, col);

    if(val == -DBL_MAX)
        return NULL;

    #ifdef DEBUG
    printf("User matrix:\n");
    matrix_print(matrix);
    #endif

    char* result = malloc(32);
    sprintf(result, "%g", val);
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
            if(replacement == NULL) {
                printf("Expression error: index at %s is out of matrix bounds\n", input_str);
                return NULL;
            }


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
                        return NULL;
                        //token.type = TOKEN_INVALID;
                        //token.symbol = num_str;
                    }
                } else {
                    //token.type = TOKEN_INVALID;
                    //token.symbol = strndup(&c, 1);
                    //i++;
                    return NULL;
                }
        }

        tokens[(*token_count)++] = token;
    }

    return tokens;
}

Matrix* solve_expr(char* expr) {
    expr = replace_all(expr);
    int num_tokens;
    Token* token_expr = parse_expr(expr, &num_tokens);

    if(token_expr == NULL)
        return NULL;

    return eval_expr(token_expr, num_tokens);
}