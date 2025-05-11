#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/parse_input.h"


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
