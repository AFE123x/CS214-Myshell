#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
int main(int argc, char** argv) {
    //create an int array to store the file descriptors
    int fds[2];
    //create a pipe
    pipe(fds);
    //create a child process
    pid_t process = fork();
    //create a temp process to store the stdout file descriptor
    pid_t tempproc = dup(STDOUT_FILENO);


    if (process == 0) {
        dup2(fds[1], STDOUT_FILENO);
        close(fds[0]);
        char* capy[] = {"ls", NULL};
        execvp(capy[0], capy);
    } else {
        wait(NULL);
        dup2(fds[0], STDIN_FILENO);
        dup2(tempproc, STDOUT_FILENO);
        close(fds[1]);
        char* penis[] = {"grep", "capy", NULL};
        execvp(penis[0], penis);
    }

    return 0;
}

