#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include "../include/parse_input.h"
#include "../include/matrix.h"
#include "../include/eval_expr.h"
#include "../include/parse_expr.h"
#include "../include/matrix_cli.h"
#include "../include/runtime_data.h"
#include "../include/repository.h"
#include "../include/export.h"


typedef bool (*CommandFn)(char *input);
#define NUM_COMMANDS 14
#define MAX_MATRICES 200

typedef struct
{
    const char *name;
    CommandFn fn;
} Command;

bool list_matrices(char* input);
bool show_matrix(char* input);
bool help(char* input);
bool set_matrix(char* input);
bool clear_terminal(char* input);
bool list_saved(char* input);
bool save_as_matrix(char* input);
bool save_matrix(char* input);
bool load_matrix(char* input);
bool drop_matrix(char* input);
bool delete_matrix(char* input);
bool exec_script(char* input);
bool export(char* input);
bool import(char* input);

Command commands[] = {
    {"matrix", set_matrix},
    {"list", list_matrices},
    {"show", show_matrix},
    {"help", help},
    {"clear", clear_terminal},
    {"saved", list_saved},
    {"save as", save_as_matrix},
    {"save", save_matrix},
    {"load", load_matrix},
    {"drop", drop_matrix},
    {"delete", delete_matrix},
    {"exec", exec_script},
    {"export", export},
    {"import", import}
};


bool create_matrix(char *name)
{
    Matrix *new = get_matrix_user(name);
    if (!new)
    {
        printf("Error: Matrix not created\n");
        return false;
    }

    bool res = rd_save_matrix(name, new);
    if (!res)
    {
        printf("Error: Matrix with name %s already exists\n", name);
        return false;
    }

    return true;
}

bool set_matrix(char *input) {
    int num_args = 0;
    char **args = get_args(input, &num_args);

    if (!args || num_args == 0) {
        printf("Error: No Matrix name provided\n");
        return false;
    }

    for (int i = 0; i < num_args; i++) {
        if (!args[i]) {
            printf("Error: Invalid argument %d\n", i);
            continue;
        }
        create_matrix(args[i]);
    }

    return true;
}

bool show_matrix(char *input) {
    int num_args = 0;
    char **args = get_args(input, &num_args);

    if(!args || num_args == 0)
    {
        printf("Error: No Matrix name provided\n");
        return false;
    }

    for (int i = 0; i < num_args; i++)
    {
        if (!args[i])
        {
            printf("Error: Invalid argument %d\n", i);
            continue;
        }

        // Check if matrix exists
        Matrix *matrix = rd_get_matrix(args[i]);
        if (!matrix)
        {
            printf("Error: Matrix %s not found\n", args[i]);
            continue;
        }

        matrix_print(matrix);
    }

    return true;
}

bool help(char *input)
{
    return false;
}

bool list_matrices(char *input)
{
    rd_print_all();
    return true;
}

bool clear_terminal(char *input)
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    return true;
}

bool list_saved(char *input)
{
    repo_list();
    return true;
}

bool save_matrix_repo(char *name, char *save_name)
{
    if (!name || !save_name)
    { // Check if argument valid
        printf("Error: Invalid matrix name %s\n", name);
        return false;
    }

    // Get argument matrix
    Matrix *matrix = rd_get_matrix(trim(save_name));

    if (!matrix)
    { // argument is not valid matrix name
        printf("Matrix %s not found\n", trim(save_name));
        return false;
    }

    // Check if name unique
    bool save = repo_is_unique(save_name);

    if (!save) // If name not unique, check if user wants to overwrite
        save = replace_saved_matrix(save_name);

    if (save) // Save matrix
        return repo_matrix_save(save_name, matrix);

    return false; // Matrix not saved
}

bool save_as_matrix(char *input)
{
    // Get arguments
    int num_args = 0;
    char **args = get_args(input, &num_args);

    // Check if arguments
    if (!args || num_args == 0)
    {
        printf("Error: No Matrix name provided\n");
        return false;
    }

    // Iterate through arguments
    for (int i = 0; i < num_args; i++)
    {
        // Get name to save as
        char *save_name = get_matrix_name();
        bool saved = false;

        if (save_name)
            saved = save_matrix_repo(args[i], save_name);
        else
            printf("Matrix name invalid\n");

        if (saved)
            printf("Matrix %s saved as %s\n", trim(args[i]), save_name);
        else
            printf("Matrix %s not saved\n", trim(args[i]));
    }

    return true;
}

bool save_matrix(char *input)
{
    // Get arguments
    int num_args = 0;
    char **args = get_args(input, &num_args);

    // Check if arguments
    if (!args || num_args == 0)
    {
        printf("Error: No Matrix name provided\n");
        return false;
    }

    // Iterate through arguments
    for (int i = 0; i < num_args; i++)
    {
        bool saved = save_matrix_repo(trim(args[i]), trim(args[i]));

        if (saved)
            printf("Matrix %s saved\n", trim(args[i]));
        else
            printf("Matrix %s not saved\n", trim(args[i]));
    }

    return true;
}

bool load_matrix(char *input)
{
    // Get arguments
    int num_args = 0;
    char **args = get_args(input, &num_args);

    // Check if arguments
    if (!args || num_args == 0)
    {
        printf("Error: No Matrix name provided\n");
        return false;
    }

    // Iterate through arguments
    for (int i = 0; i < num_args; i++)
    {
        Matrix *matrix = repo_matrix_load(trim(args[i]));

        if (!matrix)
            return false;

        rd_save_matrix(trim(args[i]), matrix);
    }

    return true;
    ;
}

// Delete matrix from runtime memory
bool drop_matrix(char *input)
{
    int num_args = 0;
    char **args = get_args(input, &num_args);

    // Check if arguments valid
    if (!args || num_args == 0)
    {
        printf("Error: No Matrix name provided\n");
        return false;
    }

    // Iterate through arguments
    for (int i = 0; i < num_args; i++)
    {
        // Attempt to delete
        bool exists = rd_delete_matrix(trim(args[i]));

        if (!exists)
        { // Matrix not found
            printf("Matrix %s does not exist\n", trim(args[0]));
            return false;
        }
    }

    return true;
}

// Delete matrix from persisted data
bool delete_matrix(char *input)
{
    int num_args = 0;
    char **args = get_args(input, &num_args);

    // Check if arguments valid
    if (!args || num_args == 0)
    {
        printf("Error: No Matrix name provided\n");
        return false;
    }

    // Iterate through arguments
    for (int i = 0; i < num_args; i++)
    {
        // Attempt to delete
        bool exists = repo_matrix_delete(trim(args[i]));

        if (!exists)
        { // Matrix not found
            printf("Matrix %s was not deleted\n", trim(args[0]));
            return false;
        }
    }

    return true;
}


bool exec_script(char* input) {
    int num_args = 0;
    char **args = get_args(input, &num_args);

    #ifdef DBG
    #include <unistd.h>
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("Current working dir: %s\n", cwd);
    printf("Num args: %d\n", num_args);
    for(int i = 0; i < num_args; i++)
        printf("(%d: %s),\n", i, args[i]);
    #endif

    // Check if arguments valid
    if(!args || num_args == 0)
    {
        printf("Error: No Filepath provided\n");
        return false;
    }

    bool has_err = false;

    for(int i = 0; i < num_args; i++) {
        if(!execute_file(trim(args[i])))
            has_err = false;
    }
    
    return has_err;
}


bool export(char* input) {
    int num_args = 0;
    char **args = get_args(input, &num_args);

    // Check if arguments valid
    if(!args || num_args == 0)
    {
        printf("Error: No Filepath provided\n");
        return false;
    }

    for(int i = 0; i < num_args; i++) {
        Matrix* saving = rd_get_matrix(trim(args[i]));

        if(!saving) // Matrix name not valid
            printf("Matrix %s does not exist\n", trim(args[i]));
        else
            export_csv(saving, trim(args[i]));
    }

    return true;
}


bool import(char* input) {
    int num_args = 0;
    char **args = get_args(input, &num_args);

    // Check if arguments valid
    if(!args || num_args == 0)
    {
        printf("Error: No Filepath provided\n");
        return false;
    }

    for(int i = 0; i < num_args; i++) {
        Matrix* matrix = import_csv(trim(args[i]));
        
        if(!matrix)
            printf("Matrix could not be loaded from %s\n", trim(args[i]));
        else {
            char* save_name = NULL;

            if(trim(args[i + 1][0]) == '-') // Matrix name specified
                save_name = trim(args[++i] + 1);
            else
                save_name = trim(args[i]);
            
            rd_save_matrix(save_name, matrix);
        }
    }

    return true;
}


bool starts_with(const char *input, const char *command) {
    // Check if str2 is longer than str1
    if (strlen(command) > strlen(input))
        return false; // input can't start with command if command is longer

    // Compare characters of both strings
    while(*command) {
        if (*input != *command)
        {
            return false; // Doesn't match, return false
        }
        input++;
        command++;
    }

    return true; // Characters match, return true
}


bool find_command(char *input)
{
    for (int i = 0; i < NUM_COMMANDS; i++)
    {
        if (starts_with(input, commands[i].name))
            return commands[i].fn(input);
    }

    return false;
}

char** split_expr(const char *str, int *count) {
    if(!str || !count)
        return NULL;

    char *copy = strdup(str); // Create a modifiable copy
    if(!copy)
        return NULL;

    char **result = NULL;
    int size = 0;
    int capacity = 4;

    result = malloc(capacity * sizeof(char *));
    if(!result) {
        free(copy);
        return NULL;
    }

    char *token = strtok(copy, "=");
    while (token != NULL)
    {
        if (size >= capacity)
        {
            capacity *= 2;
            char **temp = realloc(result, capacity * sizeof(char *));
            if (!temp)
            {
                // Free previously allocated memory
                for (int i = 0; i < size; i++)
                    free(result[i]);
                free(result);
                free(copy);
                return NULL;
            }
            result = temp;
        }

        result[size++] = strdup(token); // Copy the token into a new string
        token = strtok(NULL, "=");
    }

    *count = size;
    free(copy);
    return result;
}

bool save_result(Matrix *result, char *target) {
    Matrix *matrix = NULL;
    matrix = rd_get_matrix(target);
    bool res_flag = true;

    if(!matrix)
        res_flag = rd_overwrite_matrix(target, result);
    else
        res_flag = rd_overwrite_matrix(target, result);

    return res_flag;
}

bool handle_input(char *input) {
    if(find_command(input))
        return true;

    int num_args = 0;
    char **expr = split_expr(input, &num_args);
    if(!expr || num_args != 2) {
        printf("Error: Invalid expression\n");
        return false;
    }

    expr[0] = trim(expr[0]);

    #ifdef DBG
    printf("Expression: %s\n", expr[0]);
    #endif

    int end, row, col;
    char* name;
    Matrix* matrix = NULL; // Will contain valid Matrix if index is being assigned

    // Check if matrix index is being assigned
    if(parse_pattern(expr[0], 0, &end) && expr[0][end] == '\0') {

        #ifdef DBG
        printf("Pattern parse succeeded\n");
        #endif

        get_matrix_index(expr[0], &name, &row, &col);

        #ifdef DBG
        printf("Assignment to:\t%s[%d][%d]\n", name, row, col);
        #endif

        if(name != NULL)
            matrix = rd_get_matrix(name);
    }

    char* err_msg = NULL;
    Matrix *result = solve_expr(expr[1], &err_msg);
    
    if(err_msg) {
        printf("Expression could not be Evaluated: %s", err_msg);
        return false;
    }

    if(!result)
        return false;

    if(matrix) {
        if (!result || result->rows != 1 || result->cols != 1) {
            printf("Error: Expression must produce number to assign to matrix index\n");
            return false;
        }

        if (row >= matrix->rows || row < 0 || col >= matrix->cols || col < 0) {
            printf("Error: Indices [%d][%d] out of bounds for matrix %s\n", row, col, name);
            return false;
        }

        matrix_set(matrix, row, col, matrix_get(result, 0, 0));
   
        return true;
    }

    else
        return save_result(result, expr[0]);
}