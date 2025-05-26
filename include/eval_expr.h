#ifndef EVAL_EXPR_H
#define EVAL_EXPR_H

#include "token.h"
#include "matrix.h"

Matrix* eval_expr(Token* infix_expr, char** err_msg, int infix_len);

#ifdef TEST
Token* convert_rpn(Token* tokens, int num_tokens, int* out_rpn_len);
#endif

#endif