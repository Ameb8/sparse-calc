#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/matrix.h"


char* generate_name(const char* name) {
    time_t now = time(NULL);
    struct tm* local = localtime(&now);

    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "_%Y-%m-%d_%H:%M:%S.csv", local);

    // Allocate result buffer
    size_t len = strlen(name) + strlen(timestamp) + 1;
    char* result = malloc(len);
    if(!result) // Allocation failed
        return NULL;

    snprintf(result, len, "%s%s", name, timestamp); // Concat strings

    return result;
}


bool export_csv(Matrix* matrix, const char* filename) {
    char* name = generate_name(filename); // Generate filename
    
    if(!name)  // Name generation failed
        return false;

    FILE* file = fopen(name, "w"); // Open file to write

    if (!file) { // File open failed
        printf("Error: %s failed to open\n", name);
        return false;
    }

    for(int i = 0; i < matrix->rows; ++i) { 
        for (int j = 0; j < matrix->cols; ++j) {
            double val = matrix_get(matrix, i, j); // Get value
            fprintf(file, "%f", val); // Print value
            if(j < matrix->cols - 1) // Print comma
                fprintf(file, ",");
        }

        fprintf(file, "\n"); // New row
    }

    printf("Matrix saved as %s\n", name);

    free(name);
    fclose(file);

    return true;
}


int count_rows(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Cannot open file for counting rows");
        return -1;
    }

    int rows = 0;
    int c;
    int last_char_was_newline = 1;

    while((c = fgetc(file)) != EOF) {
        if(c == '\n') {
            rows++;
            last_char_was_newline = 1;
        } else if(c != '\r') {
            last_char_was_newline = 0;
        }
    }
    if (!last_char_was_newline) {
        rows++; // last line may not end with newline
    }

    fclose(file);
    return rows;
}


int count_columns(const char* filename) {
    FILE* file = fopen(filename, "r");
    if(!file) {
        perror("Cannot open file for counting columns");
        return -1;
    }

    int cols = 1;  // At least one column
    int c;
    while((c = fgetc(file)) != EOF && c != '\n' && c != '\r') {
        if(c == ',') {
            cols++;
        }
    }

    fclose(file);
    return cols;
}


Matrix* import_csv(const char* filename) {
    int rows = count_rows(filename);
    int cols = count_columns(filename);
    if(rows < 1 || cols < 1) 
        return NULL;

    FILE* file = fopen(filename, "r");
    if(!file) {
        perror("Failed to open file");
        return NULL;
    }

    Matrix* matrix = matrix_create(rows, cols);
    if(!matrix) {
        fclose(file);
        return NULL;
    }

    char number_buf[64]; // more than enough for any double
    int buf_index = 0;
    int row = 0, col = 0;
    int c;

    while((c = fgetc(file)) != EOF) {
        if((isdigit(c) || c == '.' || c == '-' || c == 'e' || c == 'E' || c == '+') && buf_index < (int)sizeof(number_buf) - 1) {
            number_buf[buf_index++] = (char)c;
        } else if(c == ',' || c == '\n' || c == '\r' || c == EOF) {
            if (buf_index > 0) {
                number_buf[buf_index] = '\0';
                double val = strtod(number_buf, NULL);
                if(val != 0.0) 
                    matrix_set(matrix, row, col, val);
                buf_index = 0;
                col++;
            }

            if(c == '\n' || c == '\r') {
                if(col != cols) {
                    fprintf(stderr, "Warning: inconsistent column count at row %d (got %d, expected %d)\n", row, col, cols);
                }
                row++;
                col = 0;
                if(c == '\r') {
                    int next = fgetc(file);
                    if(next != '\n' && next != EOF) 
                        ungetc(next, file);
                }
            }
        }
    }

    // Handle last value if file doesn't end with newline
    if(buf_index > 0 && row < rows && col < cols) {
        number_buf[buf_index] = '\0';
        double val = strtod(number_buf, NULL);
        if(val != 0.0)
            matrix_set(matrix, row, col, val);
    }

    fclose(file);
    return matrix;
}
