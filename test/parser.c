#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include<stdio.h>
int fd = 0;
char wasopened = 1;
char aoeustdin = 0;
char* readline(char* filepath) {
    if (wasopened && !aoeustdin) {
        fd = open(filepath, O_RDONLY);
        if (fd == -1) {
            write(STDERR_FILENO,"Error opening file",18);
            return NULL;
        }
        wasopened = 0;
    }
    
    char buf;
    char* buffer = calloc(sizeof(char), 100);
    if (buffer == NULL) {
        write(STDERR_FILENO,"Memory allocation failed",25);
        return NULL;
    }
    
    int i = 0;
    ssize_t bytesRead;
    while ((bytesRead = read(fd, &buf, 1)) > 0 && buf != '\n') {
        buffer[i++] = buf;
    }
    
    if (bytesRead == 0 && i == 0) {
        free(buffer);
        return NULL; // Return NULL if end of file is reached
    }

    return buffer;
}


char** tokenize_string(char *str, const char *delim, int *num_tokens) {
    char **tokens = NULL;
    char *token;
    int count = 0;

    // Copy the input string to avoid modification
    char *str_copy = strdup(str);
    if (str_copy == NULL) {
        // Memory allocation failed
        return NULL;
    }

    // Get the first token
    token = strtok(str_copy, delim);

    // Count the tokens
    while (token != NULL) {
        count++;
        token = strtok(NULL, delim);
    }

    // Allocate memory for array of pointers to tokens
    tokens = (char **)malloc((count + 1) * sizeof(char *));
    if (tokens == NULL) {
        // Memory allocation failed
        free(str_copy);
        return NULL;
    }

    // Reset str_copy and get the tokens again
    strcpy(str_copy, str);
    token = strtok(str_copy, delim);
    count = 0;

    // Store the tokens in the array
    while (token != NULL) {
        tokens[count] = strdup(token);
        if (tokens[count] == NULL) {
            // Memory allocation failed
            for (int i = 0; i < count; i++) {
                free(tokens[i]);
            }
            free(tokens);
            free(str_copy);
            return NULL;
        }
        count++;
        token = strtok(NULL, delim);
    }

    // Set num_tokens to the count
    *num_tokens = count;

    // Add NULL terminator to the array
    tokens[count] = NULL;

    // Free the temporary copy of the string
    free(str_copy);

    return tokens;
}

int main(int argc, char** argv) {
    char* filepath;
    if (argc < 2) {
        fd = dup(STDIN_FILENO);
        filepath = "./";
        aoeustdin = 'a';
    }
    else{
        aoeustdin = 0;
        filepath = argv[1];
    }
    char* capy = readline(filepath);
    char delim = ' ';
    int numtokens = 0;
    while(capy != NULL){
        char** array = tokenize_string(capy,&delim,&numtokens);
        for(int i = 0; i < numtokens; i++){
            printf("%s,",array[i]);
            free(array[i]);
        }
        free(array);
        printf("\n");
        free(capy);
        capy = readline(filepath);
    }
    close(fd);
    return EXIT_SUCCESS;
}

