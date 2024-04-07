#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<stdlib.h>

void execute_command(char* args[], int inputfd, int outputfd) {
    int TempO = dup(STDOUT_FILENO);
    int TempI = dup(STDIN_FILENO);
    dup2(outputfd, STDOUT_FILENO);
    dup2(inputfd, STDIN_FILENO);
    pid_t process = fork();
    if (process < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (process == 0) {
        execv(args[0], args);
        perror("Execution failed");
        exit(EXIT_FAILURE);
    } else {
		wait(NULL);
        dup2(TempO, STDOUT_FILENO);
        dup2(TempI, STDIN_FILENO);
        close(TempO);
        close(TempI);
    }
}

int main(int argc, char** argv) {

	//This part of the program tests the redirection, simulating ls -l > output.txt
    int outputfile = open("./output.txt", O_CREAT | O_TRUNC | O_WRONLY, 0640);
    if (outputfile == -1) {
        perror("Failed to open output file");
        exit(EXIT_FAILURE);
    }

    char* myargs[] = {"/bin/ls", "-l", NULL};
    execute_command(myargs, STDIN_FILENO, outputfile);
    close(outputfile);
	
	//====================simulating redirection with STDIN.======================
	int inputfile = open("./input.txt", O_RDONLY);
	if(inputfile == -1){
		perror("invalid file");
		exit(EXIT_FAILURE);
	}
	char* myargs2[] = {"./sum",NULL};
	execute_command(myargs2,inputfile,STDOUT_FILENO);
	close(inputfile);
    return 0;
}
