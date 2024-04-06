#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include<stdio.h>
int fd = -1;
char wasopened = 1;
char aoeustdin = 0;

/**
 *  this is our function that'll read one line from the fd specified by the user
 */
char* readline() {
    if(fd == -1){
        write(STDERR_FILENO,"Error: file descriptor not specified\n",strlen("Error: file descriptor not specified\n"));
        return NULL;
    }
    char buf;
    char* buffer = calloc(sizeof(char), 100);
    if (buffer == NULL) {
        write(STDERR_FILENO,"Memory allocation failed",25);
        return NULL;
    }
    

    size_t bytesRead;
    bytesRead = read(fd, buffer, 100);

    if (bytesRead == 0) {
        free(buffer);
        return NULL; // Return NULL if end of file is reached
    }
    return buffer;
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

