#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <signal.h>
#include <glob.h>

#include "./parser.h"


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
unsigned int hasredirection(char** array, int numargs){
    for(int i = 0; i < numargs; i++){
        if(!strcmp(array[i],"|") || !strcmp(array[i],">") || !strcmp(array[i],"<")) return i;
    }
    return -1;
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

//function that runs programs with fork
void run_program(char** program) {
    pid_t p;
    //gotta have detection for "|", "<", and ">"
    //and values to hold their locations
    //if a pipe exists in the array
    short pipe_status = 0;
    //if a pipe exists, this is the pipes location
    short pipe_index = 0;
    //if a input redirect exists in the array
    short input_status = 0;
    //if a input redirect exists, this is the location
    short input_index = 0;
    //if a output redirect exists in the array
    short output_status = 0;
    //if a output redirect exists, this is the location
    short output_index = 0;

    //just a check to see what which returns
    // char* react = which(program[0],0);
    // printf("This is the react: %s\n", react);


    //check how many arguments were passed in
    short argc = 0;
    while (program[argc] != NULL) {
        argc++;
    }
    if(hasredirection(program,argc) != -1){ //shows it has redirection.
		pipe_status = 1;
		pipe_index = hasredirection(program,argc);
		char timmyepiccool[20];
		int length = 0;
		length = snprintf(timmyepiccool, sizeof(timmyepiccool), "%d", pipe_index);
		write(STDOUT_FILENO,timmyepiccool,length);
    }

    //check if wildcard was passed in
    int wildcard_status = 0;
    for (int i = 1; i < argc; i++) {
        //check if the wild card is found in the argument
        if (strchr(program [i], '*') != NULL) {
            wildcard_status = 1;
            break;
        }
    } 
    //now check if that wildcard is a bluff. I could put this in the above loop but I am lazy
    //(check if the glob count is 0)
    if (wildcard_status) {
        glob_t globbycheck;
        glob(program[1], GLOB_ERR, NULL, &globbycheck);
        if (globbycheck.gl_pathc == 0) {
            //printf("No matching files found\n");
                wildcard_status = 0;
        }
        globfree(&globbycheck);
    }


    //if a pipe exists, run the program with a pipe

    //when there is no wildcard run the program normally with fork
    //wildcard = 0
    if (!wildcard_status) {
        p = fork();
        if (p == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (p == 0) {
            //run the child processs
            char* executable = which(program[0],0);
            //exit if the executable is NULL
            if (executable == NULL) {
                perror("Passed in executable is NULL\n");
                exit(EXIT_FAILURE);
            }
            execv(executable,program);
            perror("error");
        } else {
            wait(NULL);
        }
    } 
    //run the parent
    //wild card = 1
    else {
        //storing the results of the glob from the wildcard
        glob_t globby;
        
        //do glob error handling
        //if this returns 0 then we are good
        if (glob(program[1], GLOB_ERR, NULL, &globby) != 0) {
            perror("glob");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < globby.gl_pathc; i++) {
            p = fork();
            if (p == -1){
                perror("fork");
                exit(EXIT_FAILURE);
            } else if (p == 0) { //run the child! 
                char* argv[argc + 1];
                argv[0] = program[0]; //the name of the program being stored in the array
                //add the first matching name to the array
                argv[1] = globby.gl_pathv[i];
                argv[2] = NULL; //have a "null terminator" in the array

                //find the program
                char* executable = which(program[0],0);

                execv(executable,argv);
                perror("execv");
                exit(EXIT_FAILURE);
            } else {
                wait(NULL);
            }
        }
        //then we must free the glob we created
        //thats so cool that globs dynamically malloc themselves
        globfree(&globby);
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
                run_program(commandlist);
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
