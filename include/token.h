#ifndef TOKEN_H
#define TOKEN_H

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

#endif
