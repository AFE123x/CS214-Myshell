
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include "./parser.h"
/**
 *this function will split the input string into an array of words. 
 *@arg input string we're splitting
 *
 */
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

struct data* capygetline(int fd){
    char buffer[1024];
    memset(buffer,0,1024);
    write(STDOUT_FILENO,"mysh> ",6);
    int boi = read(fd,buffer,1024);
    char** myarray = malloc(sizeof(char*) * 100);
    int length;
    splitInput(buffer,myarray,&length);
    struct data* toreturn = malloc(sizeof(struct data));
    toreturn->myarray = myarray;
    toreturn->size = length;
    return toreturn;
}
