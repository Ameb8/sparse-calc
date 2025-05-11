#ifndef EXPR_EVALUATOR_H
#define EXPR_EVALUATOR_H

#include "hash_map.h"

typedef enum {
    TOKEN_BIN_OP,
    TOKEN_UN_OP,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_MATRIX,
    TOKEN_INVALID,
    TOKEN_NUMERIC
} TokenType;

typedef struct {
    TokenType type;
    char* symbol;
    double val;
} Token;

Token* parse_expr(char* expr, int* token_count);

#endif
