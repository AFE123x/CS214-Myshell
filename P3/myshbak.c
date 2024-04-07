//===================this file just holds the backup for the old execute thing=====================

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
