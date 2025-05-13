#ifndef EVAL_EXPR_H
#define EVAL_EXPR_H

#include "token.h"
#include "matrix.h"

Matrix* eval_expr(Token* expr, int len);

#ifdef TEST
Token* convert_rpn(Token* tokens, int num_tokens, int* out_rpn_len);
#endif

#endif