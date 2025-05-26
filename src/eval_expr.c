#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>
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

// Convert infix tokenized expression to RPN
Token* convert_rpn(Token* tokens, int num_tokens, int* out_rpn_len) {
    Token* output = malloc(sizeof(Token) * num_tokens); // Holds output rpn list
    Token** op_stack = malloc(sizeof(Token*) * num_tokens); // Operator stack
    int output_pos = 0, op_top = -1;

    for(int i = 0; i < num_tokens; i++) { // Iterate through infix token list
        Token* tok = &tokens[i];

        switch (tok->type) {
            // Handle matrix and scalar operands
            case TOKEN_NUMERIC:
            case TOKEN_MATRIX:
                // Add operand to output
                output[output_pos++] = *tok;
                break;

            // Handle binary and unary operators
            case TOKEN_UN_OP:
            case TOKEN_BIN_OP:
                while(op_top >= 0) { // Iterate through operator stack
                    Token* top = op_stack[op_top]; // Pop token
                    if(top->type == TOKEN_BIN_OP || top->type == TOKEN_UN_OP) {
                        bool is_right = is_right_associative(tok);
                        bool is_left = is_left_associative(tok) || tok->type == TOKEN_BIN_OP;
                        if ((is_left && top->val >= tok->val) ||
                            (is_right && top->val > tok->val)) {
                            output[output_pos++] = *top;
                            op_top--;
                        } else {
                            break;
                        }
                    } else { // Skip parentheses in stack
                        break;
                    }
                }
                op_stack[++op_top] = tok;
                break;

            case TOKEN_LPAREN: // Add left parentheses to stack
                op_stack[++op_top] = tok;
                break;

            case TOKEN_RPAREN: // Handle right parentheses]
                // Find accompanying left parentheses
                while (op_top >= 0 && op_stack[op_top]->type != TOKEN_LPAREN)
                    output[output_pos++] = *op_stack[op_top--];
                
                if(op_top >= 0 && op_stack[op_top]->type == TOKEN_LPAREN) {
                    op_top--; // discard left parentheses
                } else { // Opening parentheses missing
                    fprintf(stderr, "Mismatched parentheses\n");
                    return NULL;
                }
                break;

            // Handle opening determinant bracket
            case TOKEN_DET_L: // Add left determinant to stack
                op_stack[++op_top] = tok;
                break;

            case TOKEN_DET_R: // Handle closing determinant bracket
                // Find corresponding left determinant
                while (op_top >= 0 && op_stack[op_top]->type != TOKEN_DET_L)
                    output[output_pos++] = *op_stack[op_top--];
                
                if (op_top >= 0 && op_stack[op_top]->type == TOKEN_DET_L) {
                    op_top--; // discard left determinant
                    // Add determinant unary operator token to output
                    Token det_token;
                    det_token.type = TOKEN_UN_OP;
                    det_token.symbol = "det";
                    det_token.val = 0;
                    output[output_pos++] = det_token;
                } else { // Missing left determinant
                    fprintf(stderr, "Mismatched determinant delimiters\n");
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


// Evaluate multiplication
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


// Evaluate addition
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


// Evaluate subtraction
Operand handle_sub(Operand a, Operand b) {
    Matrix* result;
    
    if(a.matrix != NULL && b.matrix != NULL) // Matrix subtraction
        result = matrix_sub(a.matrix, b.matrix);
    else if(a.matrix != NULL) { // Scalar subtraction
        result = matrix_scalar_add(a.matrix, b.val * -1);
    } else { // Numeric subtraction
        return operand_create(NULL, a.val - b.val);
    }

    if(!result)
        return operand_error();

    return operand_create(result, 0);
}


// Evaluate exponents
Operand handle_exp(Operand a, Operand b) {
    if(b.matrix) // Invalid, power is matrix
        return operand_error();
    if(a.matrix) { // Base is matrix
        if(a.matrix->rows != a.matrix->cols)
            return operand_error();

        // Check if exponent is integer
        long long int_val = (long long)b.val;
        if(b.val != (double)int_val) // Only matrix to the power of integers is supported
            return operand_error();

        if(b.val == 0) // Return identity matrix if power is 0
            return operand_create(matrix_identity(a.matrix->rows, a.matrix->cols), 0);

        if(b.val < 0) { // Find inverse of a to power of b
            Matrix* result = matrix_inverse(a.matrix);

            if(!result) // Matrix has no inverse
                return operand_error();

            Matrix* temp_inverse = matrix_scalar_add(result, 0);
            
            for(int i = 1; i < b.val; i++) // Take inverse to power of b
                result = matrix_mult(result, temp_inverse);

            matrix_free(temp_inverse);

            return operand_create(result, 0);
        }

        // Raise matrix to positive integer power
        Matrix* result = matrix_scalar_add(a.matrix, 0); // Matrix to store result
        for(int i = 1; i < b.val; i++)
            result = matrix_mult(result, a.matrix);
        
        return operand_create(result, 0);
    } else { // Both operands numeric
        return operand_create(NULL, pow(a.val, b.val));
    }
}


// Evaluate division
Operand handle_div(Operand a, Operand b) {
    if(b.matrix) { // Cannot divide by matrix
        return operand_error();
    } if(a.matrix) { // Divide matrix by scalar
        Matrix* res = matrix_scalar_mult(a.matrix, 1 / b.val);
        return operand_create(res, 0);
    } else { // Numeric division
        return operand_create(NULL, a.val / b.val);
    }

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


// Handle binary operators
Operand apply_bin_op(char* op, Operand a, Operand b) {
    if(!strcmp(op, "*")) // Multiplication
        return handle_mult(a, b);
    if(!strcmp(op, "+")) // Addition
        return handle_add(a, b);
    if(!strcmp(op, "-")) // Subtraction
        return handle_sub(b, a);
    if(!strcmp(op, "^")) // Exponents
        return handle_exp(b, a);
    if(!strcmp(op, "/")) // Divison
        return handle_div(b, a);
    
    return operand_error(); // Not recognized
}


// Handle unary operators
Operand apply_un_op(char* op, Operand a) {
    if(!strcmp(op, "-")) { // Handle unary negative
        if(!a.matrix)
            return operand_create(NULL, a.val * -1);

        Matrix* result = matrix_scalar_mult(a.matrix, -1);
        return operand_create(result, 0);
    } else if(!strcmp(op, "'")) { // Handle Transpose
        if(!a.matrix) // Transpose of scalar is itself
            return operand_create(NULL, a.val);

        // Calculate transpose
        Matrix* result = matrix_transpose(a.matrix);
        return operand_create(result, 0);
    } else if(!strcmp(op, "det")) { // Handle determinant
        #ifdef DBG
        printf("Determ. recognized\n");
        #endif

        if(!a.matrix) // Cannot take determinant of scalar
            return operand_error();

        double result = matrix_determinant(a.matrix); // Calculate determinant

        #ifdef DBG
        printf("Determinant = %f\n", result);
        #endif

        if(result == -DBL_MAX) // Matrix has no determinant
            return operand_error();
        
        return operand_create(NULL, result);
    } else {
        return operand_error();
    }
}


// Evaluate tokenized expression
Matrix* eval_expr(Token* infix_expr, int infix_len) {
    int len; // Length of rpn expression
    Token* expr = convert_rpn(infix_expr, infix_len, &len); // get rpn expression

    if(!expr) // Conversion to rpn failed
        return NULL;
    
    // Create operand stack
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
            Matrix* matrix = rd_get_matrix(expr[i].symbol); // Get matrix
            stack[++top] = operand_create(matrix, 0); // Add to stack
        } else if(expr[i].type == TOKEN_NUMERIC) { // Add number to stack
            stack[++top] = operand_create(NULL, expr[i].val); // Add number to stack
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

    if(top != 0) { // Invalid expression, missing operators
        printf("Expression error, not enough operators\n");
        return NULL;
    }

    Operand result = stack[top]; // Get result form top of stack 
    
    if(result.matrix == NULL) { // Result is scalar
        Matrix* res_matrix = matrix_create(1, 1);
        matrix_set(res_matrix, 0, 0, result.val);
        return res_matrix;
    }

    return result.matrix;
}