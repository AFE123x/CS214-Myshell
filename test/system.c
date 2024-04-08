#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main() {
    char command[100];

    printf("Simple Shell Program\n");
    printf("Enter 'exit' to quit.\n");

    while (1) {
        printf("$ "); // Print shell prompt
        fgets(command, sizeof(command), stdin); // Read user input

        // Remove newline character at the end of the input
        if (command[strlen(command) - 1] == '\n') {
            command[strlen(command) - 1] = '\0';
        }

        if (strcmp(command, "exit") == 0) {
            printf("Exiting...\n");
            break;
        }

        // Execute the command using the system function
        printf("Output:\n");
        system(command); // Execute command and print output to stdout
        printf("\n");
    }

    return 0;
}

