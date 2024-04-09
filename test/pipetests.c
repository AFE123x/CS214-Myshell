#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<string.h>
/**
 * Execute a command with input and output redirection
 * @param args The command to execute
 * @param inputfd The file descriptor for input redirection
 * @param outputfd The file descriptor for output redirection
 * @return void
 * 
*/
int execute_command(char* args[], int inputfd, int outputfd) {
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

/**
 * Main function
 * @param argc Argument count
 * @param argv Argument vector
 * @return int
 * 
*/
int main(int argc, char** argv) {

	//===================simulating redirection with STDOUT========================
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
	//=======================simulating ls -l | grep .c==========================
	int afd[2];
	int status = pipe(afd);
	if(status == -1){
		perror("it's capy");
		exit(EXIT_FAILURE);
	}
	//myargs has ls stuff
	char* grepstuff[] = {"/bin/grep",".c",NULL};
	execute_command(myargs,STDIN_FILENO,afd[1]);
	close(afd[1]);
	execute_command(grepstuff,afd[0],STDOUT_FILENO);
	close(afd[0]);
	
	//===================simulating ./sum < input.txt > output.txt ===============
	
	char redirection = {"./sum","<","input.txt",">","output.txt",NULL};
	int multfrd[2];
	pipe(multfrd);
	inputfile = open("./input.txt",O_RDONLY);
	int* ptr = NULL;
	if(inputfile == -1)  *ptr = 0x1A4;
	char* sumargs[] = {"./sum",NULL};
	execute_command(sumargs,inputfile,multfrd[1]);
	close(multfrd[1]);
	outputfile = open("./output.txt",O_CREAT | O_TRUNC | O_WRONLY, 0640);
	char mybufer[100];
	memset(mybufer,0,100);
	read(multfrd[0],mybufer,100);
	write(outputfile,mybufer,100);
	close(multfrd[0]);
    return 0;
}
