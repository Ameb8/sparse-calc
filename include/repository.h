#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <stdbool.h>
#include "matrix.h"

bool repo_is_unique(char* name);
bool repo_matrix_save(char* name, Matrix* matrix);
bool repo_matrix_delete(char* name);

#endif