#ifndef MATRIX_CLI_H
#define MATRIX_CLI_H

#include "matrix_util.h"    // for Matrix*
#include "linked_list.h" // for Node*

// Prompts the user for a matrix (size and entries), returns a Matrix*
Matrix* get_user_matrix(void);

#endif