#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>
#include "../include/eval_expr.h"
#include "../include/runtime_data.h"


typedef enum {
    SCALAR,
    MATRIX,
    ERROR
} OperandType;


typedef struct {
    OperandType type;

    union {
        double val;
        Matrix* matrix;
        char* err;
    } data;
} Operand;


// Create operand to hold scalar
Operand operand_scalar(double val) {
    Operand op;
    op.type = SCALAR;
    op.data.val = val;

    return op;
}


// Create operand to hold matrix
Operand operand_matrix(Matrix* matrix) {
    Operand op;
    op.type = MATRIX;
    op.data.matrix = matrix;

    return op;
}


// Create operand to indicate error
Operand operand_error(char* err_msg) {
    Operand op;
    op.type = ERROR;
    op.data.err = err_msg;

    return op;
}


bool is_right_associative(Token* tok) {
    return tok->type == TOKEN_UN_OP && strcmp(tok->symbol, "operand") == 0;
}

bool is_left_associative(Token* tok) {
    return tok->type == TOKEN_UN_OP && strcmp(tok->symbol, "operand'") == 0;
}

// Convert infix tokenized expression to RPN
Token* convert_rpn(Token* tokens, int num_tokens, int* out_rpn_len, char** err_msg) {
    Token* output = malloc(sizeof(Token) * num_tokens); // Holds output rpn list
    Token** op_stack = malloc(sizeof(Token*) * num_tokens); // Operator stack
    int output_pos = 0;
    int op_top = -1;

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
                    *err_msg = "Missing Opening Parentheses\n";
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
                    *err_msg = "Missing Opening Determinant Delimiter\n";
                    return NULL;
                }
                break;

            default:
                break;
        }
    }

    while (op_top >= 0) {
        if(op_stack[op_top]->type == TOKEN_LPAREN || op_stack[op_top]->type == TOKEN_RPAREN) {
            *err_msg = "Missing Closing Parentheses\n";
            return NULL;
        }

        if(op_stack[op_top]->type == TOKEN_DET_L || op_stack[op_top]->type == TOKEN_DET_R) {
            *err_msg = "Missing Closing Determinant Delimiter\n";
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
    if(a.type == MATRIX && b.type == MATRIX) { // Matrix multiplication
        Matrix* result = matrix_mult(b.data.matrix, a.data.matrix);

        if(result) // Return dot product
            return operand_matrix(result);

        return operand_error("Size Mismatch When Multiplying Matrices\n"); // Multiplication failed
    } else if(a.type == MATRIX) { // Scalar multiplication
        Matrix* result = matrix_scalar_mult(a.data.matrix, b.data.val);
        return operand_matrix(result);
    } else if(b.type == MATRIX) { // Scalar multiplication
        Matrix* result = matrix_scalar_mult(b.data.matrix, a.data.val);
        return operand_matrix(result);
    }

    // Numeric multiplication
    return operand_scalar(a.data.val * b.data.val);
}


Operand handle_element_mult(Operand a, Operand b) {
    if(a.type == SCALAR && b.type == SCALAR) // Scalar product
        return operand_scalar(a.data.val * b.data.val);

    if(a.type != MATRIX || b.type != MATRIX) // Check both operands are matrices
        return operand_error("Element-Wise Multiplication '@' cannot be Applied to Matrix and Scalar\n");
    
    // Get result
    Matrix* result = matrix_element_mult(a.data.matrix, b.data.matrix);
    
    if(!result) // Check if dimensions match
        return operand_error("Matrix Dimensions Must Match to Perform Element-Wise Multiplication\n");

    return operand_matrix(result);
}


// Evaluate addition
Operand handle_add(Operand a, Operand b) {
    if(a.type == MATRIX && b.type == MATRIX) { // Matrix addition
        Matrix* result = matrix_add(a.data.matrix, b.data.matrix);

        if(result) // Return result matrix
            return operand_matrix(result);
        
        return operand_error("Size Mismatch When Adding Matrices\n"); // Addition failed
    } else if(a.type == MATRIX) { // Scalar addition
        Matrix* result = matrix_scalar_add(a.data.matrix, b.data.val);
        return operand_matrix(result);
    } else if(b.type == MATRIX) { // Scalar addition
        Matrix* result = matrix_scalar_add(b.data.matrix, a.data.val);
        return operand_matrix(result);
    }

    // Numeric addition
    return operand_scalar(a.data.val + b.data.val);
}


// Evaluate subtraction
Operand handle_sub(Operand a, Operand b) {

    if(a.type == MATRIX && b.type == MATRIX) { // Matrix subtraction
        Matrix* result = matrix_sub(a.data.matrix, b.data.matrix);
        
        if(result) // Return matrix subtraction result
            return operand_matrix(result);

        return operand_error("Size Mismatch When Subtracting Matrices\n");  // Subtraction failed
    } else if(a.type == MATRIX) { // Scalar subtraction
        Matrix* result =  matrix_scalar_add(a.data.matrix, b.data.val * -1);
        return operand_matrix(result);
    } else if(b.type == MATRIX) { // Subtract each element in a from b
        Matrix* result = matrix_scalar_subr(b.data.matrix, a.data.val);
        
        #ifdef DBG
        printf("Subtract R result:\n");
        matrix_print(result);
        #endif

        return operand_matrix(result);
    }

    // Numeric subtraction
    return operand_scalar(a.data.val - b.data.val);
}


// Evaluate exponents
Operand handle_exp(Operand a, Operand b) {
    if(b.type == MATRIX) // Invalid, power is matrix
        return operand_error("Matrix Cannot Be Exponent\n");
    if(a.type == MATRIX) { // Base is matrix
        if(a.data.matrix->rows != a.data.matrix->cols) // Matrix a must be square
            return operand_error("Exponents can Only Apply to Square Matrices\n");

        // Check if exponent is integer
        long long int_val = (long long)b.data.val; // Get floor of exponent
        if(b.data.val != (double)int_val) // Only matrix to the power of integers is supported
            return operand_error("Matrices Raised to Non-Integer Exponents is not Supported\n");

        if(b.data.val == 0) // Return identity matrix if power is 0
            return operand_matrix(matrix_identity(a.data.matrix->rows));

        if(b.data.val < 0) { // Find inverse of a to power of b
            Matrix* result = matrix_inverse(a.data.matrix);

            if(!result) // Matrix has no inverse
                return operand_error("Matrix has no Inverse, Thus cannot be Raised to Negative Exponent\n");

            Matrix* temp_inverse = matrix_scalar_add(result, 0);
            
            for(int i = 1; i < b.data.val * -1; i++) // Take inverse of a to power of b
                result = matrix_mult(result, temp_inverse);

            matrix_free(temp_inverse); // Deallocate temp matrix

            return operand_matrix(result);
        }

        // Raise matrix to positive integer power
        if(a.data.matrix->rows != a.data.matrix->cols) // Ensure matrix is square
            return operand_error("Exponents can Only be Applied to Square Matrices\n");

        Matrix* result = matrix_scalar_add(a.data.matrix, 0); // Matrix to store result

        for(int i = 1; i < b.data.val; i++)
            result = matrix_mult(result, a.data.matrix);
        
        return operand_matrix(result);
    }

    // Check if exponeent produces imaginary or complex number
    if(a.data.val < 0) { // Check if base is negative
        long long int_val = (long long)b.data.val; // Get floor of exponent
        if(b.data.val != (double)int_val) // Check if exponent is non-integer
            return operand_error("Negative Number cannot be Raised to Non-Integer Power\n");

    }

    // Both operands are numeric and does not produce complex number
    return operand_scalar(pow(a.data.val, b.data.val));
}


// Evaluate division
Operand handle_div(Operand a, Operand b) {
    if(b.type == MATRIX) // Cannot divide by matrix
        return operand_error("Matrix cannot be Divisor\n");

    if(b.data.val == 0) // Check for division by zero
        return operand_error("Cannot Divide by Zero\n");
    
    if(a.type == MATRIX) { // Divide matrix by scalar
        Matrix* res = matrix_scalar_mult(a.data.matrix, 1 / b.data.val);
        return operand_matrix(res);
    }

    // Numeric division
    return operand_scalar(a.data.val / b.data.val);
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
    if(!strcmp(op, "/")) // Division
        return handle_div(b, a);
    if(!strcmp(op, "@")) // Element-wise multiplication
        return handle_element_mult(a, b);

    
    return operand_error("Unrecognized Character\n"); // Not recognized
}


// Handle unary operators
Operand apply_un_op(char* op, Operand a) {
    if(!strcmp(op, "-")) { // Handle unary negative
        if(a.type != MATRIX)
            return operand_scalar(a.data.val * -1);

        Matrix* result = matrix_scalar_mult(a.data.matrix, -1);
        return operand_matrix(result);
    } else if(!strcmp(op, "'")) { // Handle Transpose
        if(a.type != MATRIX) // Transpose of scalar is itself
            return operand_scalar(a.data.val);

        // Calculate transpose
        Matrix* result = matrix_transpose(a.data.matrix);
        return operand_matrix(result);
    } else if(!strcmp(op, "det")) { // Handle determinant
        if(a.type != MATRIX) // Determinant is itself of 1 x 1 matrix (scalar)
            return operand_scalar(a.data.val);

        double result = matrix_determinant(a.data.matrix); // Calculate determinant

        #ifdef DBG
        printf("Determinant = %f\n", result);
        #endif

        if(result == -DBL_MAX) // Matrix has no determinant
            return operand_error("Determinant of Non-Square Matrices is Invalid");
        
        return operand_scalar(result);
    } else {
        return operand_error("Unrecognized Character");
    }
}


// Evaluate tokenized expression
Matrix* eval_expr(Token* infix_expr, char** err_msg, int infix_len) {
    int len; // Length of rpn expression
    Token* expr = convert_rpn(infix_expr, infix_len, &len, err_msg); // get rpn expression

    if(!expr) // Conversion to rpn failed
        return NULL;
    
    // Create operand stack
    Operand stack[32];
    int top = -1;

    for(int i = 0; i < len; i++) { // Iterate through tokens

        #ifdef DBG
        /*
        printf("\nCurrent token (iteration %d): %d\t%s\n", i, expr[i].type, expr[i].symbol);
        printf("Operand stack:\n");
        for(int j = top; j >= 0; j--) {
            if(stack[j].matrix != NULL)
                matrix_print(stack[j].matrix);
            else
                printf("\nVal:\t%.2f\n", stack[j].val);

        } */
        #endif

        if(expr[i].type == TOKEN_MATRIX) { // Add matrix to stack
            Matrix* matrix = rd_get_matrix(expr[i].symbol); // Get matrix
            stack[++top] = operand_matrix(matrix); // Add to stack
        } else if(expr[i].type == TOKEN_NUMERIC) { // Add number to stack
            stack[++top] = operand_scalar(expr[i].val); // Add number to stack
        } else {
            if(expr[i].type == TOKEN_UN_OP) { // Apply unary operator
                if(top < 0) { // Missing operand
                    *err_msg = "Operators Missing Operands\n";
                    return NULL;
                }

                // Apply operator
                Operand op = stack[top--]; // Get operand
                Operand result = apply_un_op(expr[i].symbol, op); // Apply operator

                if(result.type == ERROR) { // Check if result is valid
                    // Error occurred while applying operator
                    *err_msg = result.data.err; 
                    return NULL;
                }

                stack[++top] = result; // Push result
            } else if(expr[i].type == TOKEN_BIN_OP) { // Apply binary operator
                if(top < 0) { // Missing operand
                    *err_msg = "Operator Missing Operand\n";
                    return NULL;
                }

                // Get operands
                Operand a = stack[top--];
                Operand b = stack[top--];

                // Apply operator
                Operand result = apply_bin_op(expr[i].symbol, a, b);

                if(result.type == ERROR) { // Check if result is valid
                    // Error occurred while applying operator
                    *err_msg = result.data.err;
                    return NULL;
                }

                stack[++top] = result; // Push result
            } else { // Error, parentheses in RPN expression
                *err_msg = "Missing Closing Parenthesis\n";
                return NULL;
            }
        }
    }

    if(top != 0) { // Invalid expression, missing operators
        *err_msg = "Operator Missing Operands\n";
        return NULL;
    }

    Operand result = stack[top]; // Get result form top of stack 
    
    if(result.type == ERROR) { // Return error
        *err_msg = result.data.err;
        return NULL;
    }

    if(result.type == SCALAR) { // Result is scalar
        // Return as 1 x 1 matrix
        Matrix* res_matrix = matrix_create(1, 1);
        matrix_set(res_matrix, 0, 0, result.data.val);

        return res_matrix;
    }

    return result.data.matrix; // Result is matrix
}