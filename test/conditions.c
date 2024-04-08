#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
// Function to execute a command
void execute_command(char** args) {
    // Implement command execution logic here
    // For simplicity, let's just print the command
    printf("Executing command: %s\n", args[0]);
}

// Function to check if the previous command succeeded
int previous_command_succeeded(int status) {
    return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

int main() {
    char* input = "then foo < bar baz"; // Example input, replace this with actual user input
    char* token = strtok(input, " ");
    int conditional_flag = 0;
    int prev_command_status = 0; // Initial status assumed to be successful

    while (token != NULL) {
        if (strcmp(token, "then") == 0) {
            conditional_flag = 1; // Set the conditional flag for "then"
        } else if (strcmp(token, "else") == 0) {
            conditional_flag = 2; // Set the conditional flag for "else"
        } else if (strcmp(token, "|") == 0) {
            if (conditional_flag != 0) {
                printf("Error: Invalid usage of then/else after a pipeline\n");
                exit(EXIT_FAILURE);
            }
        } else {
            char* args[] = {token, NULL}; // Assuming token is a command
            int status;

            // Execute the command only if it meets the conditional criteria
            if (!(conditional_flag == 1 && !previous_command_succeeded(prev_command_status)) &&
                !(conditional_flag == 2 && previous_command_succeeded(prev_command_status))) {
                execute_command(args);
                wait(&status);
                prev_command_status = status;
            }

            // Reset the conditional flag after executing the command
            conditional_flag = 0;
        }

        token = strtok(NULL, " ");
    }

    return 0;
}
