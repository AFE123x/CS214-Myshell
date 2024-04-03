#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int pipefd[2];
    pid_t cat_pid, grep_pid;

    // Create a pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork to create a child process
    cat_pid = fork();

    if (cat_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cat_pid == 0) { // Child process (cat)
        // Close the read end of the pipe
        close(pipefd[0]);

        // Redirect stdout to the write end of the pipe
        dup2(pipefd[1], STDOUT_FILENO);

        // Close unnecessary file descriptors
        close(pipefd[1]);

        // Execute cat command
        execlp("cat", "cat", "input.txt", NULL);
        
        // If execlp fails
        perror("execlp");
        exit(EXIT_FAILURE);
    } else { // Parent process
        // Fork again to create another child process
        grep_pid = fork();

        if (grep_pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (grep_pid == 0) { // Child process (grep)
            // Close the write end of the pipe
            close(pipefd[1]);

            // Redirect stdin to the read end of the pipe
            dup2(pipefd[0], STDIN_FILENO);

            // Close unnecessary file descriptors
            close(pipefd[0]);

            // Redirect stdout to the standard output
            dup2(STDOUT_FILENO, STDOUT_FILENO);

            // Execute grep command
            execlp("grep", "grep", "col", NULL);

            // If execlp fails
            perror("execlp");
            exit(EXIT_FAILURE);
        } else { // Parent process
            // Close both ends of the pipe in the parent process
            close(pipefd[0]);
            close(pipefd[1]);

            // Wait for the child processes to finish
            wait(NULL);
            wait(NULL);
        }
    }

    return 0;
}

