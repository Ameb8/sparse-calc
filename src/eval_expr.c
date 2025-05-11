#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "../include/matrix.h"
#include "../include/parse_input.h"
#include "../include/eval_expr.h"

typedef struct {
    double val;
    Matrix* matrix;
    bool err;
} Operand;

Operand operand_create(Matrix* matrix, double val) {
    Operand op;
    op.matrix = matrix;
    op.val = val;
    op.err = false;
    return op;
}

Operand operand_error() {
    Operand op;
    op.err = true;
    return op;
}


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

Matrix* get_matrix(char* name) {
    return NULL;
}

Operand handle_mult(Operand a, Operand b) {
    if(a.matrix != NULL && b.matrix != NULL) { // Matrix multiplication
        Matrix* result = matrix_mult(a.matrix, b.matrix);
        return operand_create(result, 0);
    } else if(a.matrix != NULL) { // Scalar multiplication
        Matrix* result = matrix_scalar_mult(a.matrix, b.val);
        return operand_create(result, 0);
    } else if(b.matrix != NULL) { // Scalar multiplication
        Matrix* result = matrix_scalar_mult(b.matrix, a.val);
        return operand_create(result, 0);
    }

    // Numeric multiplication
    return operand_create(NULL, a.val * b.val);
}

Operand handle_add(Operand a, Operand b) {
    if(a.matrix != NULL && b.matrix != NULL) { // Matrix addition
        Matrix* result = matrix_add(a.matrix, b.matrix);
        return operand_create(result, 0);
    } else if(a.matrix != NULL) { // Scalar addition
        Matrix* result = matrix_scalar_add(a.matrix, b.val);
        return operand_create(result, 0);
    } else if(b.matrix != NULL) { // Scalar addition
        Matrix* result = matrix_scalar_add(b.matrix, a.val);
        return operand_create(result, 0);
    }

    // Numeric addition
    return operand_create(NULL, a.val + b.val);
}

Operand handle_sub(Operand a, Operand b) {
    Matrix* result;
    
    if(a.matrix != NULL && b.matrix != NULL) // Matrix subtraction
        result = matrix_sub(a.matrix, b.matrix);
    else if(a.matrix != NULL) // Scalar subtraction
        result = matrix_scalar_add(a.matrix, b.val * -1);
    else if(b.matrix != NULL) // Scalar subtraction
        result = matrix_scalar_subr(b.matrix, a.val * -1);
    else // Numeric subtraction
        return operand_create(NULL, a.val - b.val);

    if(result == NULL)
        return operand_error();

    return operand_create(result, 0);
}

Operand handle_numeric(char* op, Operand a, Operand b) {
    if(a.matrix == NULL || b.matrix == NULL)
        return operand_error();

    if(!strcmp(op, "/"))
        return operand_create(NULL, a.val / b.val);
    if(!strcmp(op, "^"))
        return operand_create(NULL, pow(a.val, b.val));

    return Operand_error();
}

Operand apply_bin_op(char* op, Operand a, Operand b) {
    if(!strcmp(op, "*")) 
        return handle_mult(a, b);
    if(!strcmp(op, "+"))
        return handle_add(a, b);
    if(!strcmp(op, "-"))
        return handle_sub(a, b);
    
    return handle_numeric(op, a, b);

}

Operand apply_un_op(char* op, Operand a) {

}

Matrix* eval_expr(Token* expr, int len) {
    Operand stack[32];
    int top = -1;

    for(int i = 0; i < len; i++) { // Iterate through tokens
        if(expr[i].type == TOKEN_MATRIX) { // Add matrix to stack
            Matrix* matrix = get_matrix(expr[i].symbol);
            stack[++i] = operand_create(matrix, 0);
        } else if(expr[i].type == TOKEN_NUMERIC) { // Add number to stack
            stack[++i] = operand_create(NULL, expr[i].val);
        } else {
                if(expr[i].type == TOKEN_UN_OP) { // Apply unary operator
                    if(top < 0) { // Missing operand
                        printf("Expression error, not enough operands\n");
                        return NULL;
                    }

                    Operand op = stack[top--]; // Get operand
                    Operand result = apply_un_op(expr[i].symbol, op);
                    if(result.err) return NULL; // Error occurred while applying operator
                    stack[++top] = result; // Push result
                } else if(expr[i].type == TOKEN_BIN_OP) { // Apply binary operator
                    if(top < 0) { // Missing operand
                        printf("Expression error, not enough operands\n");
                        return NULL;^
                    }

                    // Get operands
                    Operand a = stack[top--];
                    Operand b = stack[top--];

                    Operand result = apply_bin_op(expr[i].symbol, a, b); // Get result
                    if(result.err) return NULL; // Error occurred while applying operator
                    stack[++top] = result; // Push result
                } else { // Error, parentheses in RPN expression
                    printf("Expression error, mismatched parentheses\n");
                    return NULL;
                }
        }
    }
}