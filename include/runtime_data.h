#ifndef RUNTIME_DATA_H
#define RUNTIME_DATA_H

#include <stdbool.h>
#include "../include/matrix.h"

bool rd_save_matrix(char* name, Matrix* matrix);
bool rd_overwrite_matrix(char* name, Matrix* matrix);
bool rd_delete_matrix(char* name);
Matrix* rd_get_matrix(char* name);
void rd_print_all();

#endif
