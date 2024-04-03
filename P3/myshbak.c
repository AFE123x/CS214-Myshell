#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>



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

 int search_directory(const char *path, char* program) {
     DIR *directory;
     struct dirent *entry;
     struc stat info;
    write(STDOUT_FILENO, program,strlen(program));
    write(STDOUT_FILENO,"\n",1);
     directory = opendir(path);
     if (directory == NULL) {
         perror("opendir");
         return 1;
     }
    char backup[strlen(program) + 1];
    strcpy(backup,program);
     while ((entry = readdir(directory)) != NULL) {
         char full_path[BUFFER_SIZE];
         memset(full_path, 0, BUFFER_SIZE);
         strcpy(full_path,path);
         strcat(full_path,entry->d_name);
         if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
             continue;

        write(STDOUT_FILENO, full_path, strlen(full_path));
        write(STDOUT_FILENO, " == ",4);
        write(STDOUT_FILENO, program, strlen(program));
        write(STDOUT_FILENO,"\t",1);
        write(STDOUT_FILENO, !strcmp(entry->d_name, backup) ? "good" : "badd", 4);
        write(STDOUT_FILENO, "\n", 1); // Add newline

         if (strcmp(entry->d_name, backup) == 0) {
             write(STDOUT_FILENO, full_path, strlen(full_path));
             write(STDOUT_FILENO, "\n", 1); // Add newline
             closedir(directory); // Close directory since program is found
             return 0; // Exit the function
         }
     }

     // Close the directory
     closedir(directory);
     return 1;
 }

//prints the path mysh will use for a given program
//will only print something IF the program is found
//will not print if the program is built-in or if the program is not found

void which(char* program) {
    int toreturn = 0;
    if(!DEBUG){
    toreturn = search_directory(DirectoryOne,program);
    if(toreturn == 0) {toreturn = search_directory(DirectoryTwo,program);}
    if(!toreturn == 0) toreturn = search_directory(DirectoryThree,program);
    }
    else{
        search_directory("./",program);
    }
}


//prints the working directory
void pwd() {
//can we use getcwd?

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

int main (int argc, char** argv) {
    // write(STDOUT_FILENO, "Hewwo\n", 7);

    //Enter Batch Mode
    if ((!isatty(STDIN_FILENO) && argc == 1) || argc == 2) {
        printf("I am in batch mode?!?!?!");
    }
    
    //Enter Interactive Mode
    if(argc == 1 && isatty(STDIN_FILENO)) {
        write(STDOUT_FILENO,"Welcome to my fiendish little bomb\n",36);
        while(1){ 
        struct data* capybara = capygetline(STDIN_FILENO);
        int data = capybara->size;
        char** joever = capybara->myarray;
        if (!strcmp(joever[0], "exit")) {
            shell_exit();
        } else if (!strcmp(joever[0], "cd")) {
            cd(joever[1]);
        } else if (!strcmp(joever[0], "which")) {
            which(joever[1]);
        } else if (!strcmp(joever[0], "pwd")) {
            pwd();
        }

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
