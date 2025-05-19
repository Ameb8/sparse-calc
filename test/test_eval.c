#ifdef TEST

#include <stdlib.h>
#include "../include/token.h"
#include "../include/runtime_data.h"
#include "../include/parse_expr.h"
#include "../include/eval_expr.h"
#include "test_util.h"
#include "test_eval.h"


void test_tokenizer() {
    int token_count = 0;

    char* input = "A * 2.5 + B - (A[4][3])";
    Token* tokens = parse_expr(input, &token_count);

    // Expected tokens
    TokenType expected_types[] = {
        TOKEN_MATRIX, TOKEN_BIN_OP, TOKEN_NUMERIC,
        TOKEN_BIN_OP, TOKEN_MATRIX, TOKEN_BIN_OP,
        TOKEN_LPAREN, TOKEN_NUMERIC, TOKEN_RPAREN
    };
    const double expected_vals[] = {
        -1.0, 1.0, 2.5, 0.0, -1.0, 0.0, 3.0, 11.0, 3.0
    };
    const char* expected_symbols[] = {
        "A", "*", "2.5", "+", "B", "-", "(", "11", ")"
    };

    int expected_count = sizeof(expected_types) / sizeof(TokenType);

    ASSERT_INT_EQ(token_count, expected_count); // Check token count

    for (int i = 0; i < expected_count; i++) {
        ASSERT_INT_EQ(tokens[i].type, expected_types[i]); // Check token types
        ASSERT_STR_EQ(tokens[i].symbol, expected_symbols[i]); // Check token symbol

        if(expected_vals[i] != -1) // Check token values
            ASSERT_DOUBLE_EQ(tokens[i].val, expected_vals[i]);
        
    }

    // Free tokens
    for (int i = 0; i < token_count; i++) 
        free(tokens[i].symbol);
    
    free(tokens);
}

void test_convert_rpn() {
  // Expression: operand 3 + (4 * 2) - 5'
    // Assumed postfix: 3 4 2 * + 5 ' -
    Token tokens[] = {
        {TOKEN_NUMERIC, "3", 0.0},
        {TOKEN_BIN_OP, "+", 1.0},
        {TOKEN_LPAREN, "(", 0.0},
        {TOKEN_NUMERIC, "4", 0.0},
        {TOKEN_BIN_OP, "*", 2.0},
        {TOKEN_NUMERIC, "2", 0.0},
        {TOKEN_RPAREN, ")", 0.0},
        {TOKEN_BIN_OP, "-", 1.0},
        {TOKEN_NUMERIC, "5", 0.0},
        {TOKEN_UN_OP, "'", 0.0}  
    };
    int num_tokens = sizeof(tokens) / sizeof(Token);
    int rpn_len = 0;
    Token* rpn = convert_rpn(tokens, num_tokens, &rpn_len);

    const char* expected[] = {"3", "4", "2", "*", "+", "5", "'", "-"};
    int expected_len = sizeof(expected) / sizeof(char*);

    ASSERT_INT_EQ(rpn_len, expected_len);

    for (int i = 0; i < expected_len; i++) 
        ASSERT_STR_EQ(rpn[i].symbol, expected[i]);

    free(rpn);
}

void test_replace_all() {
    char* result = replace_all("A[1][1] and A[4][4]");
    ASSERT_STR_EQ(result, "4 and 0");

    free(result);
}

void test_eval_expr_case(char* expr, Matrix* exp_result) {
    Matrix* result = solve_expr(expr);
    
    printf("Test result:\n");
    matrix_print(result);

    ASSERT_MATRIX_EQ(exp_result, result);
    matrix_free(result);
}

void test_eval_expr_invalid() {
    char* expr_1 = "A + B + C";
    test_eval_expr_case(expr_1, NULL);
    char* expr_2 = "A + B)";
    test_eval_expr_case(expr_2, NULL);
    char* expr_3 = "(A + B";
    test_eval_expr_case(expr_3, NULL);
    char* expr_4 = "A ++ B";
    test_eval_expr_case(expr_4, NULL);
    char* expr_5 = "";
    test_eval_expr_case(expr_5, NULL);
    char* expr_6 = "$";
    test_eval_expr_case(expr_6, NULL);

}

void test_eval_expr() {
    char* expr_1 = "A + B * 3";
    Matrix* res_1 = matrix_create(5, 5);
    matrix_set(res_1, 0, 1, 42);
    matrix_set(res_1, 0, 2, 7);
    matrix_set(res_1, 1, 1, 4);
    matrix_set(res_1, 1, 4, 60);
    matrix_set(res_1, 2, 0, 1);
    matrix_set(res_1, 2, 2, 54);
    matrix_set(res_1, 3, 0, 6);
    matrix_set(res_1, 3, 4, 14);
    matrix_set(res_1, 4, 3, 35);
    test_eval_expr_case(expr_1, res_1);

    char* expr_2 = "A' * 1.342 * (B + B)";
    Matrix* res_2 = matrix_create(5, 5);
    matrix_set(res_2, 0, 2, 48.312);
    matrix_set(res_2, 1, 4, 214.72);
    matrix_set(res_2, 2, 1, 263.032);
    matrix_set(res_2, 3, 3, 236.192);
    matrix_set(res_2, 4, 0, 75.152);
    test_eval_expr_case(expr_2, res_2);

    char* expr_3 = "A * B";
    Matrix* res_3 = matrix_create(5, 5);
    matrix_set(res_3, 0, 2, 126);
    matrix_set(res_3, 1, 4, 80);
    matrix_set(res_3, 2, 1, 14);
    matrix_set(res_3, 3, 3, 112);
    matrix_set(res_3, 4, 0, 22);
    test_eval_expr_case(expr_3, res_3);

    char* expr_4 = "A' * (B + B)";
    Matrix* res_4 = matrix_create(5, 5);
    matrix_set(res_4, 0, 2, 36);
    matrix_set(res_4, 1, 4, 160);
    matrix_set(res_4, 2, 1, 196);
    matrix_set(res_4, 3, 3, 176);
    matrix_set(res_4, 4, 0, 56);
    test_eval_expr_case(expr_4, res_4);

    char* expr_5 = "A[1][1] * B[1][4]";
    Matrix* res_5 = matrix_create(1, 1);
    matrix_set(res_5, 0, 0, 80);
    test_eval_expr_case(expr_5, res_5);

    matrix_free(res_1);
    matrix_free(res_2);
    matrix_free(res_3);
    matrix_free(res_4);
    matrix_free(res_5);
    //matrix_free(res_6);
}

void init_user_matrices() {
    // Create matrices
    Matrix* a = matrix_create(5, 5);
    Matrix* b = matrix_create(5, 5);

    for(int i = 0; i < 5; i++) { // Populate matrices with values
        matrix_set(a, (i + 1045) * 3 % 5, (i + 31) * 42 % 5, (i + 1) * 24 % 17);
        matrix_set(b, (i + 124) * 4 % 5, (i + 17) * 17 % 5, (i + 4) * 38 % 22);
    }

    rd_save_matrix("A", a);
    rd_save_matrix("B", b);
}

void test_eval() {
    init_user_matrices();
    test_tokenizer();
    test_convert_rpn();
    test_replace_all();
    test_eval_expr();
    test_eval_expr_invalid();
    end_test("Expression Evaluation");
}

#endif