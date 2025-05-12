#ifndef PARSE_INPUT_H
#define PARSE_INPUT_H

#include "hash_map.h"
#include "token.h"

Token* parse_expr(char* expr, int* token_count);

// Allows test.c to access methods for unit testing
#ifdef TEST 
#include "matrix.h"
bool set_user_matrix(char* name, Matrix* matrix);
Matrix* get_user_matrix(const char* name);
char* replace_all(const char* input_str);
#endif

#endif
