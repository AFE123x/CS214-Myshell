#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>




#include "./parser.h"


#define BUFFER_SIZE 1024
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

static int search_directory(const char *path, const char* program) {
    DIR *directory;
    struct dirent *entry;
    struct stat info;

    directory = opendir(path);
    if (directory == NULL) {
        perror("opendir");
        return -1;
    }

    while ((entry = readdir(directory)) != NULL) {
        char full_path[BUFFER_SIZE];
        memset(full_path, 0, BUFFER_SIZE);
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        if (strcmp(entry->d_name, program) == 0) {
            write(STDOUT_FILENO, full_path, strlen(full_path));
            write(STDOUT_FILENO, "\n", 1); 
            closedir(directory); 
            return 1; 
        }
    }

    // Close the directory
    closedir(directory);
    return 0;
}

//prints the path mysh will use for a given program
//will only print something IF the program is found
//will not print if the program is built-in or if the program is not found

void which(char* program) {
    write(STDOUT_FILENO, "You are in the which function", 29);
    int toreturn = 0;
    toreturn = search_directory(DirectoryOne,program);
    if(!toreturn) toreturn = search_directory(DirectoryTwo,program);
    if(!toreturn) toreturn = search_directory(DirectoryThree,program);
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
void run_program(char* program) {



}


int main (int argc, char** argv) {
    //need the values for the first round so batch mode can grab from it
    // struct data* capybara = capygetline(STDIN_FILENO);
    // int data = capybara->size;
    // char** joever = capybara->myarray;

    //Enter Batch Mode
    if ((!isatty(STDIN_FILENO) && argc == 1) || argc == 2) {
        printf("I am in batch mode?!?!?!");
        //attempt to open file and see if it exits
        int file;
        file = open(argv[1], O_RDONLY);

        if (file==-1) {
            perror(argv[1]);
        }



    }
    
    //Enter Interactive Mode
    if(argc == 1 && isatty(STDIN_FILENO)) {
        //print the welcome statement for interactive mode
        write(STDOUT_FILENO,"Welcome to my fiendish little bomb\n",36);
        //have a loop for our shell
        while(1){ 
            struct data* capybara = capygetline(STDIN_FILENO);
            int data = capybara->size;
            char** joever = capybara->myarray;
            //if first entry matches built in commands
            if(!strcmp(joever[0],"exit")) shell_exit();
            if(!strcmp(joever[0],"cd")) cd(joever[1]);
            if(strcmp(joever[0],"which" == 0)) which(joever[1]);
            if(!strcmp(joever[0],"pwd")) pwd();
            //if first entry matches programs in directories
            //not a built in command
            

            //command does not match any known program
            //does not match built-in commands nor found through traversal


            free(joever);
            free(capybara);
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
