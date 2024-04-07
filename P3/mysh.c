#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>
#include <sys/types.h>
#include <signal.h>
#include <glob.h>

#include "./parser.h"

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
int hasredirection(char** array, int numargs){
    for(int i = 0; i < numargs; i++){
        if(!strcmp(array[i],"|") || !strcmp(array[i],">") || !strcmp(array[i],"<")) return i;
    }
    return -1;
}
int handleredirection(char** commandlist,int numargs,int location){
	if(location >= numargs - 1 || location < 1){
		char* errorstring = "Error: improper redirect location\n";
		write(STDERR_FILENO,errorstring,strlen(errorstring));
		return -1;
	} 
	if(strcmp(commandlist[location],"<") == 0){
		char* commands[location + 1];
		for(int i = 0; i < location; i++){
			commands[i] = commandlist[i];
		}
		commands[location] = NULL;
		int inputfd = open(commandlist[location + 1],O_RDONLY);
		if(inputfd == -1){
			perror("error");
			return -1;
		}
		return execute_command(commands,inputfd,STDOUT_FILENO);
	} // ./capy arg1 arg2 > output.txt				and numargs will be 5
	 //    0     1    2   3     4 
	else if(strcmp(commandlist[location],">") == 0){
		int arraysize = (numargs - location) + 2;
		char* commands[arraysize];
		for(int i = 0; i < arraysize - 1; i++){
		commands[i] = commandlist[i];
		}
		commands[arraysize - 1] = NULL;
		write(STDOUT_FILENO,commands[location + 1],strlen(commands[location + 1]));
		write(STDOUT_FILENO,"\n",1);
		int outputfd = open(commands[location + 1], O_CREAT | O_TRUNC | O_WRONLY, 0640);
		if(outputfd == -1){
			perror("");
			return -1;
		}
		return execute_command(commands,STDIN_FILENO,outputfd);
	}
	else { 
		// ./capy arg1 arg2 arg3 | grep arg1 arg2 arg3 arg4 
		//    0    1    2    3   4  5    6    7    8     9   numargs 10
		int pipefd[2];
		pipe(pipefd);
		int numargsize = location + 1;
		
		// Populates left arguments
		char* leftargs[numargsize];
		for(int i = 0; i < numargsize; i++){
			leftargs[i] = commandlist[i];
		}
		leftargs[numargsize - 1] = NULL;
		
		// Populates right arguments
		numargsize = numargs - location;
		char* rightargs[numargsize];
		for(int i = location + 1; i < numargs; i++){
			rightargs[i - location - 1] = commandlist[i];
		}
		rightargs[numargsize - 1] = NULL;
		
		int toreturn;
		
		// Execute left command
		toreturn = execute_command(leftargs, STDIN_FILENO, pipefd[1]);
		close(pipefd[1]);
		
		// Execute right command
		toreturn = execute_command(rightargs, pipefd[0], STDOUT_FILENO);
		close(pipefd[0]);
		
		return toreturn;
	}

}
//function that runs programs with fork -> check myshbak.c
int run(char** commandlist, int numargs){
	int checkboi = hasredirection(commandlist,numargs);
	if(checkboi != -1){
		return handleredirection(commandlist,numargs,checkboi);
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
    //Pee in my pants
    signal(SIGINT, goodbye);
    if ((!isatty(STDIN_FILENO) && argc == 1) || argc == 2) {
        printf("I am in batch mode?!?!?!");
        //attempt to open file and see if it exits
        int file;
        file = open(argv[1], O_RDONLY);
        parserconstruct(file);
        if (file==-1) {
            perror(argv[1]);
        }
    } 
    //Enter Interactive Mode
    if(argc == 1 && isatty(STDIN_FILENO)) {
        //print the welcome statement for interactive mode
        write(STDOUT_FILENO,"Welcome to my fiendish little bomb\n",36);
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
