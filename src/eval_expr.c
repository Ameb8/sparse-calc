#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "../include/eval_expr.h"
#include "../include/runtime_data.h"


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

    #ifdef DEBUG
    if(matrix != NULL) {
        printf("\nOperand matrix field:\n");
        matrix_print(op.matrix);
        printf("Matrix argument:\n");
        matrix_print(matrix);
    }

    #endif

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
                    return NULL;
                }
                break;

            default:
                break;
        }
    }

    while (op_top >= 0) {
        if (op_stack[op_top]->type == TOKEN_LPAREN || op_stack[op_top]->type == TOKEN_RPAREN) {
            fprintf(stderr, "Mismatched parentheses\n");
            return NULL;
        }
        output[output_pos++] = *op_stack[op_top--];
    }

    free(op_stack);
    *out_rpn_len = output_pos;
    return output;
}


Operand handle_mult(Operand a, Operand b) {
    if(a.matrix != NULL && b.matrix != NULL) { // Matrix multiplication
        Matrix* result = matrix_mult(b.matrix, a.matrix);
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
    #ifdef DEBUG
    printf("SUBTRACTION:\n");
    if(!a.matrix) {
        printf("Operand 1: %.2f\n", a.val);
    } else {
        printf("Operand 1:\n");
        matrix_print(a.matrix);
    }
    if(!b.matrix) {
        printf("Operand 2: %.2f\n", b.val);
    } else {
        printf("Operand 2:\n");
        matrix_print(b.matrix);
    }
    #endif

    Matrix* result;
    
    if(a.matrix != NULL && b.matrix != NULL) // Matrix subtraction
        result = matrix_sub(a.matrix, b.matrix);
    else if(a.matrix != NULL) { // Scalar subtraction
        #ifdef DEBUG
        printf("scalar sub attempted:\n");
        #endif

        result = matrix_scalar_add(a.matrix, b.val * -1);
    //} else if(b.matrix != NULL) {// Scalar subtraction
        //result = matrix_scalar_subr(b.matrix, a.val * -1);
        //#ifdef DEBUG
        //printf("Scalar R sub attempted\n");
        //#endif
    } else {// Numeric subtraction
        #ifdef DEBUG
        printf("Numeric sub attempted\n");
        #endif

        return operand_create(NULL, a.val - b.val);
    }

    if(!result)
        return operand_error();

    #ifdef DEBUG
    matrix_print(result);
    #endif

    return operand_create(result, 0);
}

Operand handle_numeric(char* op, Operand a, Operand b) {
    if(a.matrix == NULL || b.matrix == NULL)
        return operand_error();

    if(!strcmp(op, "/"))
        return operand_create(NULL, a.val / b.val);
    if(!strcmp(op, "^"))
        return operand_create(NULL, pow(a.val, b.val));

    return operand_error();
}

Operand apply_bin_op(char* op, Operand a, Operand b) {
    if(!strcmp(op, "*")) 
        return handle_mult(a, b);
    if(!strcmp(op, "+"))
        return handle_add(a, b);
    if(!strcmp(op, "-"))
        return handle_sub(b, a);
    
    return handle_numeric(op, a, b);

}

Operand apply_un_op(char* op, Operand a) {
    #ifdef DEBUG
    printf("\napply_un_op() params:\n");
    printf("op: %s\n", op);
    if(a.matrix == NULL)
        printf("values: %.2f\n", a.val);
    else
        matrix_print(a.matrix);
    #endif

    if(!strcmp(op, "-")) { // Handle unary negative

        #ifdef DEBUG
        printf("\n Unary minus operator selected\n");
        #endif

        if(a.matrix == NULL) {
            return operand_create(NULL, a.val * -1);
        } else {
            Matrix* result = matrix_scalar_mult(a.matrix, -1);
            return operand_create(result, 0);
        }
    } else if(!strcmp(op, "'")) { // Handle Transpose

        #ifdef DEBUG
        printf("\n Transpose operator selected\n");
        #endif

        if(a.matrix == NULL) {
            return operand_create(NULL, a.val);
        } else {

            #ifdef DEBUG
            printf("\n Transpose Initiated\n");
            #endif

            // ERROR HERE !!! ********************************************
            Matrix* result = matrix_transpose(a.matrix);

            #ifdef DEBUG
            printf("\nTranspose result:\n");
            matrix_print(result);
            #endif

            return operand_create(result, 0);
        }
    } else {

        #ifdef DEBUG
        printf("\n Unary operator not recognized\n");
        #endif

        return operand_error();
    }
}

Matrix* eval_expr(Token* infix_expr, int infix_len) {
    int len;
    Token* expr = convert_rpn(infix_expr, infix_len, &len);

    #ifdef DBG
    printf("\nRPN expression (%d):\n", len);
    for(int i = 0; i <len; i++) 
    printf("Token type: %d\tToken value: %s\n", expr[i].type, expr[i].symbol);
    #endif

    if(!expr)
        return NULL;
    
    Operand stack[32];
    int top = -1;

    for(int i = 0; i < len; i++) { // Iterate through tokens

        #ifdef DBG
        printf("\nCurrent token (iteration %d): %d\t%s\n", i, expr[i].type, expr[i].symbol);
        printf("Operand stack:\n");
        for(int j = top; j >= 0; j--) {
            if(stack[j].matrix != NULL)
                matrix_print(stack[j].matrix);
            else
                printf("\nVal:\t%.2f\n", stack[j].val);

        }
        #endif

        if(expr[i].type == TOKEN_MATRIX) { // Add matrix to stack
            #ifdef DBG
            printf("Matrix Token\n");
            #endif

            //Matrix* matrix = get_matrix(expr[i].symbol);
            Matrix* matrix = rd_get_matrix(expr[i].symbol);
            stack[++top] = operand_create(matrix, 0);
        } else if(expr[i].type == TOKEN_NUMERIC) { // Add number to stack

            #ifdef DBG // Reaches Here with '0' input
            printf("numeric token recognized\n");
            #endif

            stack[++top] = operand_create(NULL, expr[i].val);
        } else {
            if(expr[i].type == TOKEN_UN_OP) { // Apply unary operator
                #ifdef DBG
                printf("Unary Op Token\n");
                #endif

                if(top < 0) { // Missing operand
                    printf("Expression error, not enough operands\n");
                    return NULL;
                }

                Operand op = stack[top--]; // Get operand
                Operand result = apply_un_op(expr[i].symbol, op);
                if(result.err) return NULL; // Error occurred while applying operator
                stack[++top] = result; // Push result
            } else if(expr[i].type == TOKEN_BIN_OP) { // Apply binary operator
                #ifdef DBG
                printf("Binary Op Token\n");
                #endif

                if(top < 0) { // Missing operand
                    printf("Expression error, not enough operands\n");
                    return NULL;
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

    if(top != 0) {
        printf("Expression error, not enough operators\n");
        return NULL;
    }

    Operand result = stack[top];
    
    if(result.matrix == NULL) {
        Matrix* res_matrix = matrix_create(1, 1);
        matrix_set(res_matrix, 0, 0, result.val);
        return res_matrix;
    }

    return result.matrix;
}