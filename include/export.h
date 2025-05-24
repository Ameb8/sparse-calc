#ifndef EXPORT_H
#define EXPORT_H

#include <stdbool.h>
#include "matrix.h"

bool export_csv(Matrix* matrix, const char* filename);
Matrix* import_csv(const char* filename);

#endif