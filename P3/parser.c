#include <string.h>
#include "./univ.h"
int fd = -1;
char wasopened = 1;
char aoeustdin = 0;
int temptempcapy = 0;
/**
 *  this is our function that'll read one line from the fd specified by the user
 */
char* readline() {
    if (fd == -1) {
        temptempcapy = write(STDERR_FILENO, "Error: file descriptor not specified\n", strlen("Error: file descriptor not specified\n"));
        return NULL;
    }

    char* buffer = malloc(101); // Allocate memory for the buffer
    if (buffer == NULL) {
        temptempcapy = write(STDERR_FILENO, "Memory allocation failed\n", strlen("Memory allocation failed\n"));
        return NULL;
    }

    char ch;
    size_t index = 0;
    ssize_t bytes_read;

    while ((bytes_read = read(fd, &ch, 1)) > 0) {
        if (ch == '\n') {
            buffer[index] = '\0'; // Null-terminate the string
            return buffer; // Return the line read
        } else {
            buffer[index++] = ch; // Add character to buffer
            if (index >= 100) {
                buffer[index] = '\0'; // Null-terminate the string
                return buffer; // Return the line read (maximum length reached)
            }
        }
    }

    if (bytes_read == 0 && index == 0) {
        // End of file reached
        free(buffer);
        return NULL;
    }

    // Error occurred during read
    free(buffer);
    temptempcapy = write(STDERR_FILENO, "Error reading from file\n", strlen("Error reading from file\n"));
    return NULL;
}
char* mystrdup2(char* str){
    unsigned int length = strlen(str);
    char* toreturn = calloc(sizeof(char) , length + 1);
    strcpy(toreturn,str);
    return toreturn;
}
//char** tokenize_string(char *str, const char *delim, int *num_tokens)

void splitInput(char* input, char** args, int* arg_count) {
    *arg_count = 0;
    int in_quotes = 0; // Flag to track whether we are inside quotes
    char* token_start = input;

    for (char* ptr = input; *ptr != '\0'; ptr++) {
        if (*ptr == '"') {
            in_quotes = !in_quotes; // Toggle the in_quotes flag when a quote is encountered
        }

        if ((*ptr == ' ' || *ptr == '\n') && !in_quotes) {
            // If not inside quotes and a space or newline is found, consider it as a separator
            *ptr = '\0'; // Replace space or newline with null terminator
            args[(*arg_count)++] = token_start;
            token_start = ptr + 1; // Start of the next token
        }
    }

    // Add the last token (if any) after the loop
    if (*token_start != '\0') {
        // Remove leading and trailing double quotes if they exist
        if (token_start[0] == '"' && token_start[strlen(token_start) - 1] == '"') {
            token_start[strlen(token_start) - 1] = '\0'; // Remove trailing quote
            args[(*arg_count)++] = token_start + 1; // Remove leading quote
        } else {
            args[(*arg_count)++] = token_start;
        }
    }
    args[*arg_count] = NULL;
}

void parserconstruct(int ofd) {
    
    fd = dup(ofd);
    if(fd == STDIN_FILENO){
        aoeustdin = 1;
    }


}

