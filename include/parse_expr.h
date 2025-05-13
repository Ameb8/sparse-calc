#ifndef PARSE_EXPR_H
#define PARSE_EXPR_H

#include "matrix.h"

Matrix* solve_expr(char* expr);

#ifdef TEST
Token* parse_expr(char* expr, int* token_count);
char* replace_all(const char* input_str);

#endif

#endif