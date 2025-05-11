#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "../include/parse_input.h"
#include "../include/eval_expr.h"

bool is_right_associative(Token* tok) {
    return tok->type == TOKEN_UN_OP && strcmp(tok->symbol, "operand") == 0;
}

bool is_left_associative(Token* tok) {
    return tok->type == TOKEN_UN_OP && strcmp(tok->symbol, "operand'") == 0;
}

Token* convert_rpn(Token* tokens, int num_tokens, int* out_rpn_len) {
    Token* output = malloc(sizeof(Token) * num_tokens);
    Token** op_stack = malloc(sizeof(Token*) * num_tokens);
    int output_pos = 0, op_top = -1;

    for (int i = 0; i < num_tokens; i++) {
        Token* tok = &tokens[i];

        switch (tok->type) {
            case TOKEN_NUMERIC:
            case TOKEN_MATRIX:
                output[output_pos++] = *tok;
                break;

            case TOKEN_UN_OP:
            case TOKEN_BIN_OP:
                while (op_top >= 0) {
                    Token* top = op_stack[op_top];
                    if (top->type == TOKEN_BIN_OP || top->type == TOKEN_UN_OP) {
                        bool is_right = is_right_associative(tok);
                        bool is_left = is_left_associative(tok) || tok->type == TOKEN_BIN_OP;

                        if ((is_left && top->val >= tok->val) ||
                            (is_right && top->val > tok->val)) {
                            output[output_pos++] = *top;
                            op_top--;
                        } else {
                            break;
                        }
                    } else {
                        break;
                    }
                }
                op_stack[++op_top] = tok;
                break;

            case TOKEN_LPAREN:
                op_stack[++op_top] = tok;
                break;

            case TOKEN_RPAREN:
                while (op_top >= 0 && op_stack[op_top]->type != TOKEN_LPAREN) {
                    output[output_pos++] = *op_stack[op_top--];
                }
                if (op_top >= 0 && op_stack[op_top]->type == TOKEN_LPAREN) {
                    op_top--; // discard LPAREN
                } else {
                    fprintf(stderr, "Mismatched parentheses\n");
                }
                break;

            default:
                break;
        }
    }

    while (op_top >= 0) {
        if (op_stack[op_top]->type == TOKEN_LPAREN || op_stack[op_top]->type == TOKEN_RPAREN) {
            fprintf(stderr, "Mismatched parentheses\n");
        }
        output[output_pos++] = *op_stack[op_top--];
    }

    free(op_stack);
    *out_rpn_len = output_pos;
    return output;
}
