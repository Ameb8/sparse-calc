#ifndef MATRIX_CLI_H
#define MATRIX_CLI_H

#include "matrix.h"

Matrix* get_matrix_user(char* matrix_name);
char** get_args(const char* input, int* count);
char* trim(const char* str);
bool replace_saved_matrix(char* name);
char* get_matrix_name();
#endif