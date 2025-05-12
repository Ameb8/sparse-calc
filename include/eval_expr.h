#ifndef EVAL_EXPR_H
#define EVAL_EXPR_H

#include "token.h"

Token* convert_rpn(Token* tokens, int num_tokens, int* out_rpn_len);
Matrix* eval_expr(Token* expr, int len);

#endif