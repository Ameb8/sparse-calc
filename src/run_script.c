#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../include/run_script.h"

#define MAX_COMMAND_LEN 1024

bool handle_input(char *input);

// Function to execute commands from a file with commands separated by ';'
void execute_file(const char* filepath) {
    #ifdef DBG
    printf("File Path: %s\n", filepath);
    #endif

    FILE* file = fopen(filepath, "r");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    char buffer[MAX_COMMAND_LEN];
    size_t buf_len = 0;

    int c;
    while((c = fgetc(file)) != EOF) {
        if (c == ';') {
            // End of a command
            buffer[buf_len] = '\0';

            // Trim leading and trailing spaces
            char* cmd_start = buffer;
            while (*cmd_start == ' ' || *cmd_start == '\t' || *cmd_start == '\n') cmd_start++;

            // Remove trailing whitespace
            for (int i = strlen(cmd_start) - 1; i >= 0; i--) {
                if (cmd_start[i] == ' ' || cmd_start[i] == '\t' || cmd_start[i] == '\n') {
                    cmd_start[i] = '\0';
                } else {
                    break;
                }
            }

            if (strlen(cmd_start) > 0) {
                #ifdef DBG
                printf("Executing line: %s\n", cmd_start);
                #endif

                handle_input(cmd_start);
            }

            buf_len = 0; // Reset buffer for next command
        } else {
            if (buf_len < MAX_COMMAND_LEN - 1) {
                buffer[buf_len++] = (char)c;
            } else {
                fprintf(stderr, "Command too long, skipping...\n");
                // Skip until next semicolon
                while ((c = fgetc(file)) != EOF && c != ';');
                buf_len = 0;
            }
        }
    }

    // If file didn't end with ';', process remaining buffer
    if (buf_len > 0) {
        buffer[buf_len] = '\0';
        handle_input(buffer);
    }

    fclose(file);
}

