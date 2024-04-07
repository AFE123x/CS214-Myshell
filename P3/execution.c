#include<sys/wait.h>
#include "./execution.h"
#include "./univ.h"
/**
* this function will execute a program, and write to the file descriptors
* specified by the user. 
* @args args which contains the program to be executed.
* @args inputfd the input file descriptor specified by caller
* @args outputfd the output file descriptor specified by caller
* @return the status code if program was run successfully, or -1 in certain circumstances
*/
unsigned int execute_command(char* args[], int inputfd, int outputfd) {
    int TempO = dup(STDOUT_FILENO);
    int TempI = dup(STDIN_FILENO);
    dup2(outputfd, STDOUT_FILENO);
    dup2(inputfd, STDIN_FILENO);
    pid_t process = fork();
    if (process < 0) {
        perror("Fork died D-:");
        exit(EXIT_FAILURE);
    } else if (process == 0) {
        execv(args[0], args);
        perror("Execution failed, It's joever");
        exit(EXIT_FAILURE);
    } else {
		int statuscode;
		waitpid(process,&statuscode,0);
        dup2(TempO, STDOUT_FILENO);
        dup2(TempI, STDIN_FILENO);
        close(TempO);
        close(TempI);
		return statuscode;
    }
	return -1;
}

/**
* A dummy main function to illustrate what the caller should look like. 
* Shows how Redirection and piping works with the execute function. 
* @arg argc, number of arguments user passes in
* @arg argv, an array of arguments user passes in. 
* @return exit status, depending on whether code succeeds or fails. 
*/
static int testmain(int argc, char** argv) {

	//===================simulating redirection with STDOUT========================
    int outputfile = open("./output.txt", O_CREAT | O_TRUNC | O_WRONLY, 0640);
    if (outputfile == -1) {
        perror("Failed to open output file, sorry");
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
    return 0;
}
