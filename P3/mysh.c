#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>
#include <sys/types.h>
#include <signal.h>
#include <glob.h>

#include "./parser.h"
#include "./execution.h"
//=======universal
#include "./univ.h"

#define BUFFER_SIZE 4096

//toggle debugging mode
#ifndef DEBUG
#define DEBUG 0
#endif
//the big 3 directories we will use to find programs
//should be checked sequentially for programs
//ONLY used when the first argument is NOT a built-in function or if the "/" is not present
char* DirectoryOne = "/usr/local/bin";
//ONLY used when the first argument is NOT a built-in function or if the "/" is not present
char* DirectoryTwo = "/usr/bin";
//ONLY used when the first argument is NOT a built-in function or if the "/" is not present
char* DirectoryThree = "/bin";


/////////////////////////built-in commands that need to be implemented:
//pwd, which, exit, cd

//moves you from current working directory to designated path
void cd(const char *path) {
    if (chdir(path) == -1) {
        perror("cd");
    }
}

char* mystrdup(char* word){
    unsigned length = strlen(word);
    char* toreturn = (char*)malloc(sizeof(char) * length + 1);
    strcpy(toreturn,word);
    return toreturn;
}
//searches the directory for the given program with the provided path
char* search_directory(const char *path, const char *program, char flag) {
    if(DEBUG){
        write(STDOUT_FILENO,path,strlen(path));
        write(STDOUT_FILENO,"\n",1);
    }
    //will hold the path to the program if found
    char* toreturn = NULL;
    //stores the directory as a directory stream object
    DIR* directory;
    //stores the entry of the directory
    struct dirent *entry;
    //stores the information of the file
    struct stat info;

    //open the directory with the given path
    directory = opendir(path);

    //check if the directory is NULL
    if (directory == NULL) {
        perror("opendir");
        return toreturn;
    }

    //attempt traversal of the directory
    while ((entry = readdir(directory)) != NULL) {
        char full_path[BUFFER_SIZE];
        strcpy(full_path, path);
        strcat(full_path,"/");
        strcat(full_path, entry->d_name);
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Once the correct directory is found, proceed to saving the program path
        if (strcmp(entry->d_name, program) == 0) {
            if(flag){
            write(STDOUT_FILENO, full_path, strlen(full_path));
            write(STDOUT_FILENO, "\n", 1); // Add newline
            }
            //save the path to the program
            toreturn = mystrdup(full_path);
            closedir(directory); // Close directory since program is found
            return toreturn; // Exit the function
        }
    }

    // Close the directory
    closedir(directory);
    return toreturn;
}

/**
* Performs which, checking specific directories for the program
* @arg program - a string containing the program name
* @arg flag - set to 0 if you don't want directory printed, not 0 otherwise. 
* @return a string containing the full filepath. 
*/
char* which(const char *program, char flag) {
    char* toreturn = NULL;
    if (!DEBUG) {
        toreturn = search_directory(DirectoryOne, program, flag);
        if (toreturn == NULL) {
            toreturn = search_directory(DirectoryTwo, program, flag);
            if (toreturn == NULL) {
                toreturn = search_directory(DirectoryThree, program, flag);
            }
        }
    } else {
        search_directory(".", program, flag);
    }
    return toreturn;
}

//prints the working directory
void pwd() {
    char buf[BUFFER_SIZE];

    if (getcwd(buf, sizeof(buf)) != NULL) {
        write(STDOUT_FILENO, buf, strlen(buf));
        //add a new line to be extra
        write(STDOUT_FILENO, "\n",1);
    } else {
        perror("getcwd() error");
        return 1;
    }

    return 0;
}

//quits the shell program
void shell_exit() {
    write(STDOUT_FILENO,"its so hard to say goodbye :(\n",31);
    exit(0);
}
////////////////////////
int haspipe(char** array, int numargs){
	for(int i = 0; i < numargs; i++){
		if(strcmp(array[i],"|") == 0){
			return i;
		}
	}
	return -1;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int handlepiping(char** command_list, int num_args, int location) {
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        return -1;
    }
	
    // Populate left arguments
    char* left_args[location + 1];
    char* my_string = which(command_list[0], 0);
    if (my_string != NULL)
        left_args[0] = my_string;
    else
        left_args[0] = command_list[0];
    
    for (int i = 1; i < location; i++) {
        left_args[i] = command_list[i];
    }
    left_args[location] = NULL;
    
    // Check for left redirection on left side (wouldn't exist on right side)
    int left_redirect_index = hasredirection(left_args, location);
    char has_left_redirection = 0; 
    if (left_redirect_index != -1) {
        has_left_redirection = 1;
        if (strcmp(left_args[left_redirect_index], "<") == 0) {
            int input_redir = open(left_args[left_redirect_index + 1], O_RDONLY);
            if (input_redir == -1) {
                perror("");
                return -1;
            }
            left_args[left_redirect_index] = NULL;
            execute_command(left_args, input_redir, pipe_fd[1]);
        }
    }
    
    // Populate right arguments
    char* right_args[num_args - location];
    my_string = which(command_list[location + 1], 0);
    if (my_string != NULL)
        right_args[0] = my_string;
    else
        right_args[0] = command_list[location + 1];
    
    for (int i = 1; i < num_args - location - 1; i++) {
        right_args[i] = command_list[i + location + 1];
    }
    right_args[num_args - location - 1] = NULL;
    
    // Execute left command
    int status = 0;
    if (!has_left_redirection) {
        status = execute_command(left_args, STDIN_FILENO, pipe_fd[1]);
    }
    close(pipe_fd[1]);
    
    // Check for right redirection
    int right_redirect_index = hasredirection(right_args, num_args - location - 1);
    if (right_redirect_index != -1) {
        if (strcmp(right_args[right_redirect_index], ">") == 0) {
            int outputfd = open(right_args[right_redirect_index + 1], O_CREAT | O_TRUNC | O_WRONLY, 0640);
            if (outputfd == -1) {
                perror("");
                return -1;
            }
            right_args[right_redirect_index] = NULL;
            status = execute_command(right_args, pipe_fd[0], outputfd);
            close(pipe_fd[0]);
            return status;
        }
    }
    
    // Execute right command
    status = execute_command(right_args, pipe_fd[0], STDOUT_FILENO);
    close(pipe_fd[0]);
    
    return status;
}

int hasredirection(char** array, int numargs){
    for(int i = 0; i < numargs; i++){
        if(array[i] != NULL){
			if(!strcmp(array[i],">") || !strcmp(array[i],"<")) return i;
		}
		else{
			return -1;
		}
    }
    return -1;
}

int handle2redirections(char** commandlist, int numargs, int location1, int location2){
    if (location1 < 0 || location1 >= numargs || location2 < 0 || location2 >= numargs) {
        char* errorstring = "Error: invalid redirect command locations\n";
        write(STDERR_FILENO, errorstring, strlen(errorstring));
        return -1;
    }
    if (location1 == location2 - 1) { // checks if they're adjacent to each other
        char* errorstring = "ERROR: Redirects cannot be adjacent to each other\n";
        write(STDERR_FILENO, errorstring, strlen(errorstring));
        return -1;
    }
    if (strcmp(commandlist[location1], "<") == 0 && strcmp(commandlist[location2], ">") == 0) {
        int fd[2];
        pipe(fd);

        int inputfile = open(commandlist[location1 + 1], O_RDONLY);
        if (inputfile == -1) {
            perror("Error opening input file");
            return -1;
        }

        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            dup2(inputfile, STDIN_FILENO); // Redirect stdin to input file
            dup2(fd[1], STDOUT_FILENO);    // Redirect stdout to writing end of the pipe
            close(fd[0]);  // Close unused reading end of the pipe
            close(fd[1]);  // Close writing end of the pipe

            char* program_name = commandlist[0];
            char** program_args = malloc((location1 + 1) * sizeof(char *));
            if (program_args == NULL) {
                perror("Error allocating memory for program arguments");
                exit(EXIT_FAILURE);
            }
            for (int i = 0; i < location1; i++) {
                program_args[i] = commandlist[i];
            }
            program_args[location1] = NULL;

            execvp(program_name, program_args);
            perror("execvp");
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            // Parent process
            close(fd[1]);  // Close writing end of the pipe

            int outputfile = open(commandlist[location2 + 1], O_CREAT | O_TRUNC | O_WRONLY, 0640);
            if (outputfile == -1) {
                perror("Error opening output file");
                return -1;
            }

            char buffer[4096];
            ssize_t bytes_read;
            while ((bytes_read = read(fd[0], buffer, sizeof(buffer))) > 0) {
                if (write(outputfile, buffer, bytes_read) == -1) {
                    perror("Error writing to output file");
                    return -1;
                }
            }
            if (bytes_read == -1) {
                perror("Error reading from pipe");
                return -1;
            }

            close(outputfile);
            close(fd[0]);  // Close reading end of the pipe
        } else {
            perror("fork");
            return -1;
        }
    } else {
        char* errorstring = "ERROR: Incorrect redirection sequence\n";
        write(STDERR_FILENO, errorstring, strlen(errorstring));
        return -1;
    }

    return 0;
}
int handleredirection(char** commandlist, int numargs, int location, int weglobbin) {
    if (location >= numargs - 1 || location < 1) {
        char* errorstring = "Error: improper redirect location\n";
        write(STDERR_FILENO, errorstring, strlen(errorstring));
        return -1;
    } //./sum < input.txt > output.txt 
	 //    0  1     2     3    4           numargs: 5, location: 1
		int hastworedirections = hasredirection(&commandlist[location + 1], numargs - location);
		if(hastworedirections != -1){
			return handle2redirections(commandlist, numargs, location, hastworedirections + location + 1);
		}

    if (strcmp(commandlist[location], "<") == 0) {
        //everything under this is for redirection <
        char* commands[location + 1];
        char* newfilepath = which(commandlist[0], 0);
        if (newfilepath != NULL) {
            commands[0] = newfilepath;
        } else {
            commands[0] = commandlist[0];
        }
        free(newfilepath);
        for (int i = 1; i < location; i++) {
            commands[i] = commandlist[i];
        }
        commands[location] = NULL;
        int inputfd = open(commandlist[location + 1], O_RDONLY);
        if (inputfd == -1) {
            perror("error");
            return -1;
        }
        int statuscode = execute_command(commands, inputfd, STDOUT_FILENO);
		if(newfilepath != NULL) free(newfilepath);
		return statuscode;
    } else if (strcmp(commandlist[location], ">") == 0) {
    
    //everything under this is for redirection >
	  char* commands[location + 1];
	  char* mycommands = which(commandlist[0],0);
	  if(mycommands != NULL){
		commands[0] = mycommands;
	  }
	  else{
		  commands[0] = commandlist[0];
	  }
	  for(int i = 1; i < location; i++){
		commands[i] = commandlist[i];
	  }
	  commands[location] = NULL;
	  int outputfd = open(commandlist[location + 1],O_CREAT | O_TRUNC | O_WRONLY,0640);
	  if(outputfd == -1){
		perror("");
		return -1;
	  }
	  int statuscode = execute_command(commands,STDIN_FILENO,outputfd);
	  if(mycommands != NULL) free(mycommands);
	  
    } 
}


//function that runs programs with fork -> check myshbak.c
int run(char** commandlist, int numargs){
	int checkboi = haspipe(commandlist,numargs);
	if(checkboi != -1){
		return handlepiping(commandlist,numargs,checkboi);
	}
    //check for wildcard in the command and number of arguments
    int weglobbin = haswildcard(commandlist,numargs);

	checkboi = hasredirection(commandlist,numargs);
	if(checkboi != -1){
		return handleredirection(commandlist,numargs,checkboi, weglobbin);
	}
    
	
	char* myarray[numargs + 1];
	char* returnpath = which(commandlist[0],0);
	if(returnpath != NULL){
		myarray[0] = returnpath;
	}
	else{
		myarray[0] = commandlist[0];
	}
	for(int i = 1; i < numargs; i++){
		myarray[i] = commandlist[i];
	}
	myarray[numargs] = NULL;

    //this is for when no redirection is found
    //AND if there is no wildcard
    if (!weglobbin) {
        pid_t process = fork();
	    if(!process){
		execv(myarray[0],myarray);
		
	    }
        else{
            int statusnum;
            waitpid(process,&statusnum,0);
            if(returnpath != NULL) free(returnpath);
            return statusnum;
        }
    } else 
    //this is for when no redirection is found
    //AND if there is a wildcard
    {
        glob_t globby;
        if (glob(commandlist[1], GLOB_ERR, NULL, &globby) != 0) {
            perror("glob");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < globby.gl_pathc; i++) {
            myarray[1] = globby.gl_pathv[i];
            pid_t p = fork();
            
            if (!p) {
                //find the program
                char* executable = which(myarray[0],0);
                //print my array
                execv(myarray[0], myarray);
                perror("execv");
                exit(EXIT_FAILURE);
            } else {
                wait(NULL);
            }
        }
        globfree(&globby);
        if (returnpath != NULL) free(returnpath);    
    }
	
}
void goodbye(){
    write(STDOUT_FILENO,"\nYou pressed control + c, goodbye!\n",strlen("\nYou pressed control + c, goodbye!\n"));
    exit(0);
}
int main (int argc, char** argv) {
    //need the values for the first round so batch mode can grab from it
    // struct data* capybara = capygetline(STDIN_FILENO);
    // int data = capybara->size;
    // char** joever = capybara->myarray;

    //Enter Batch Mode
    signal(SIGINT, goodbye);
    if ((!isatty(STDIN_FILENO) && argc == 1) || argc == 2) {
        //attempt to open file and see if it exits
        int file;
        file = open(argv[1], O_RDONLY);
        parserconstruct(file);
        if (file==-1) {
            perror(argv[1]);
			return -1;
        }
		char* line = readline();
		char* commandlist[100];
		while(line != NULL){
			//write(STDOUT_FILENO,line,strlen(line));
			//write(STDOUT_FILENO,"\n",1);
			memset(commandlist,0,sizeof(commandlist));
			int numberofcommands = 0;
			splitInput(line,commandlist,&numberofcommands);
			run(commandlist,numberofcommands);
			free(line);
			line = readline();
			
		}
		return 0;
    } 
    //Enter Interactive Mode
    if(argc == 1 && isatty(STDIN_FILENO)) {
        //print the welcome statement for interactive mode
        write(STDOUT_FILENO,"Welcome to my shell :-)\n",24);
        //have a loop for our shell

        parserconstruct(STDIN_FILENO);
        while(1){ 
            write(STDOUT_FILENO,"mysh>",5);
            char* commands = readline();
            int numberofcommands = 0;
            char* mycommand = " ";
            char* commandlist[100];
            memset(commandlist,0,sizeof(commandlist));
            splitInput(commands, commandlist, &numberofcommands);
            //if first entry matches built in commands

            if (!strcmp(commandlist[0], "exit")) {
            shell_exit();
            } else if (!strcmp(commandlist[0], "cd")) {
            cd(commandlist[1]);
            } else if (!strcmp(commandlist[0], "which")) {
            char* react = which(commandlist[1],1);
			free(react);
            } else if (!strcmp(commandlist[0], "pwd")) {
            pwd();
            }
            else{
                run(commandlist,numberofcommands);
            }

            //if first entry matches programs in directories
            //not a built in command
            

            //command does not match any known program
            //does not match built-in commands nor found through traversal





            free(commands);
        }
    }

    return 0;
}
// @@@@@@@@@@@@@@@@@@@@@ &&%%%%%%&&...&&   .,#%%%%%%%%%%%@     .....%@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@&&@&(,**..,     /....  ....(%%%%%%%%%%, ........@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@&,,,,(.../       %   . , ..... &%%%%%%%%# .......*@@@@@@@@@@@@
// @@@@@@@@@@@@@@@&&,....*  ./        (,%      ,...... %#%%&&&&  .......@@@@@@@@@@@
// @@@@@@@@@@@@@@%.. ...%              ,,,(      *   .   &%&&&&&,   ....,%@@@@@@@@@
// @@@@@@@@@@@@@.           .            ...       .       %&&&&&/    ...,&@@@@@@@@
// @@@@@@@@@@@@                            .        .        @&&@@*    ...,&@@@@@@@
// @@@@@@@@@@@                                               .%@@@%*......,,@@@@@@@
// @@@@@@@@@@         ,    %   .                      ...    ..*@@@.#.....,,@@@@@@@
// @@@@@@@@@&   .     (  .(&.                      . ...........#@@,*,..,,,,*@@@@@@
// @@@@@@@@@          & ..&&&          .  ,        ..............@@,**...,,,,@@@@@@
// @@@@@@@@&   ,      % .&%%%&           .  .       ............,.@,,*#...,,,&@@@@@
// @@@@@@@@#          #(*&%%%%&. .           /.   , ....,.........&/,*,.,,,,**@@@@@
// @@@@@@@@(  .   .    &&&&%%%%%,.&            ,.,  ,...,........../,,,,.,.,*/@@@@@
// @@@@@@@@# *.% ... . (&&&&&%%%%&,*,      .   , ..,.#.......,..,..(.,,/.*,&*#&@@@@
// @@@@@@@@@. ..%*..    %%&&%%%%%%%%&&(        ... ..*..,....,,,.*./.,,(,*(@*@@@@@@
// @@@@@@@@@.@. %*(      %&&%%%#(((###&&&   .  .*/.., (.,,..,,,,,#./,,,*,*@@/@@@@@@
// @@@@@@@@@/@&# ./(,  *&% %&&&&&.      ..//..#,,..**(#*(*.,,,,*,(./,,,/,&@@@@@@@@@
// @@@@@@@@@@@@@  /,..  ,,%%&                  /.(/**,#...,,,***((**,,/,*(@@@@@@@@@
// @@@@@@@@@@@@@@*(@ .,.%#,         .             **(.*..,,***#((/**/,#*@@@@@@@@@@@
// @@@@@@@@@@@@@@@/(@&,..,**(                        %....***/((/**/@&*@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@,,,*****      ....          //%...(%//(%/&(@@@&@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@&@@@@%*(&,*&(/             ////*/.&@#%@&%@%@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@@@@@@@@&@@@@@@,     &(((.   /*@@@%@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@&//*&&@@@@@@@@@@@@@@@@@@%@*              @&&@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@*,##%%%%%*//(&@@@@@@@@@@@@@@%%&%%%%%%%%%%%%%%%%&@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@*,,#######%%%%(/((##&@@@@@@@%////&&&&&&&&&&&&&&%&&@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// /*,/#########%%%%%###%%#///*(%%%%%&#(%&&&&&&&&%%@&&&%%@@@@@@@@@@@@@@@@*#%%&@@@@@
// **(/####%%%%%%%%%%%%%%%&&#%%%%#%&#&&%%#&&%%%%&&&&&%%@%%&//&@@/,,,,,,,(#%###%&@@@
// /(*#%%%%%%%%%%%%%%%%%%%%&#&&%###&&%%&&%%%%%%&&&%%%%&%%%%%#(((#**,,,*######%%%%@@
// /*###########%&&&&&&&&&&&%&#&&&%&&&%%%&&&&&%%%%%%%%%%%%%%%%####(/&###%%%%%%%%%&&
// */##%%%%%%%%##%%%%%%%%%@&&&&&##&&&&&%%%%&&&&&%%&%%%%&%%%%%%%%%##&//##%%%%%%%%&&&