// Created by AJ DiLeo
// For use in CS211 Spring 2024 ONLY

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS_SIZE 100

typedef struct team {
	int id;
	char* name;
	int yearFounded;
	char* city;
	char* sport;
	double popularityIndex;

	int isDeleted; // internal field, not directly accessible to user
} team_t;

// step 1: create structs for the other two tables: athletes
// and competitions
// include internal field
typedef struct athlete {
	int id;
	char* name;
	int birthYear;
	char* nationality;
	char* sport;
	int isDeleted;
}athlete_t;

typedef struct competition {
	int id;
	char* name;
	int year;
	char* sport;
	char* location;
	int isDeleted;
}competition_t;
// step 2: create typedef struct for storing metadata
typedef struct metadata{
	int count;
	int nextIndex;
	int maxCount;
}metadata_t;
// step 3: declare the two other arrays of structs
// teams has been defined for you already
// TODO: add athletes and competitions
team_t* teams;
athlete_t* athletes;
competition_t* competitions;

// step 4: declare 3 metadata structs, one for each table
metadata_t teamsMetadata = { .count = 0, .nextIndex = 0, .maxCount = 0 };
metadata_t athletesMetadata = { .count = 0, .nextIndex = 0, .maxCount = 0 };
metadata_t competitionsMetadata = { .count = 0, .nextIndex = 0, .maxCount = 0 };
// step 5: jump to L167


// This function takes the user's input and splits it by spaces
// into an array of strings, ignoring spaces that are wrapped in quotes
// There is no need to modify this code.
// You do not need to understand this code
// but you are welcome to research its application
void splitInput(char* input, char** args, int* arg_count) {
    *arg_count = 0;
    int in_quotes = 0; // Flag to track whether we are inside quotes
    char* token_start = input;

    for (char* ptr = input; *ptr != '\0'; ptr++) {
        if (*ptr == '"') {
            in_quotes = !in_quotes; // Toggle the in_quotes flag when a quote is encountered
        }

        if ((*ptr == ' ' || *ptr == '\n') && !in_quotes) {
            // If not inside quotes and a space or newline is found, consider it as a separator
            *ptr = '\0'; // Replace space or newline with null terminator
            args[(*arg_count)++] = token_start;
            token_start = ptr + 1; // Start of the next token
        }
    }

    // Add the last token (if any) after the loop
    if (*token_start != '\0') {
        // Remove leading and trailing double quotes if they exist
        if (token_start[0] == '"' && token_start[strlen(token_start) - 1] == '"') {
            token_start[strlen(token_start) - 1] = '\0'; // Remove trailing quote
            args[(*arg_count)++] = token_start + 1; // Remove leading quote
        } else {
            args[(*arg_count)++] = token_start;
        }
    }
    args[*arg_count] = NULL;
}

// step 7: implement setup function
// this function is responsible for dynamically allocating the
// particular table. Use the tables declared on L27.
void setup(char* table, int numRows) {
	if (strcmp(table, "teams") == 0) {
		teams = (team_t*)malloc(numRows * sizeof(team_t));
		teamsMetadata.maxCount = numRows;
	}
	if (strcmp(table, "athletes") == 0){
		athletes = (athlete_t*)malloc(numRows * sizeof(athlete_t));
		athletesMetadata.maxCount = numRows;
	}
	if (strcmp(table, "competitions") == 0){
		competitions = (competition_t*)malloc(numRows * sizeof(competition_t));
		competitionsMetadata.maxCount = numRows;
	}
	// DO NOT TOUCH THIS PRINT
	// REQUIRED FOR AUTOGRADER
	printf("setup complete\n");
}

// step 8: implement insert function
// this function is responsible for updating the corresponding
// fields of the struct located at the next available index
// make sure to use and update your metadata.

// autograder print for insufficient capacity:
// fprintf(stderr, "cannot insert due to insufficient capacity.\n");
void insert(char** args) {
    char* table = args[1];
    int index;

    if (strcmp(table, "teams") == 0) {
        index = teamsMetadata.nextIndex;
    } else if (strcmp(table, "athletes") == 0) {
        index = athletesMetadata.nextIndex;
    } else if (strcmp(table, "competitions") == 0) {
        index = competitionsMetadata.nextIndex;
    }

    if (index >= teamsMetadata.maxCount || index >= athletesMetadata.maxCount || index >= competitionsMetadata.maxCount) {
        fprintf(stderr, "cannot insert due to insufficient capacity.\n");
        return;
    }

    if (strcmp(table, "teams") == 0) {
        teams[index].id = index;
        teams[index].name = strdup(args[3]);
        teams[index].yearFounded = atoi(args[4]);
        teams[index].city = strdup(args[5]);
        teams[index].sport = strdup(args[6]);
        teams[index].popularityIndex = atof(args[7]);
        teams[index].isDeleted = 0; // Initialize the isDeleted field
    } else if (strcmp(table, "athletes") == 0) {
        athletes[index].id = index;
        athletes[index].name = strdup(args[3]);
        athletes[index].birthYear = atoi(args[4]);
        athletes[index].nationality = strdup(args[5]);
        athletes[index].sport = strdup(args[6]);
        athletes[index].isDeleted = 0; // Initialize the isDeleted field
    } else if (strcmp(table, "competitions") == 0) {
        competitions[index].id = index;
        competitions[index].name = strdup(args[3]);
        competitions[index].year = atoi(args[4]);
        competitions[index].sport = strdup(args[5]);
        competitions[index].location = strdup(args[6]);
        competitions[index].isDeleted = 0; // Initialize the isDeleted field
    }

    if (strcmp(table, "teams") == 0) {
        teamsMetadata.count++;
        teamsMetadata.nextIndex++;
    } else if (strcmp(table, "athletes") == 0) {
        athletesMetadata.count++;
        athletesMetadata.nextIndex++;
    } else if (strcmp(table, "competitions") == 0) {
        competitionsMetadata.count++;
        competitionsMetadata.nextIndex++;
    }

    // DO NOT TOUCH THIS PRINT
    // REQUIRED FOR AUTOGRADER
    printf("insert complete\n");
}


// step 9: implement soft delete function
// this function is responsible for marking a record as deleted
// you should be updating an internal field flag so that get will
// not display this record. 
// You should not be attempting to free/overwrite this record - it remains alloc
// with a updated field
// make use of your metadata
void delete(char* table, int id) {
	int index;
	
	if (strcmp(table, "teams") == 0){
		index = id;
	} else if (strcmp(table, "athletes") == 0){
		index = id;
	} else if (strcmp(table, "competitions") == 0){
		index = id;
	}
	
	if (strcmp(table, "teams") == 0 && index < teamsMetadata.maxCount){
    		teams[index].isDeleted = 1;
	} else if (strcmp(table, "athletes") == 0 && index < athletesMetadata.maxCount){
    		athletes[index].isDeleted = 1;
	} else if (strcmp(table, "competitions") == 0 && index < competitionsMetadata.maxCount){
   		competitions[index].isDeleted = 1;
	}


	// DO NOT TOUCH THIS PRINT
	// REQUIRED FOR AUTOGRADER
	printf("delete complete\n");
}

// step 10: implement modify function
// this function is responsible for overwriting all of the contents of all
// records that match an ID.
// make use of your metadata
// !!!NOTE: The structs store pointers. Make sure to free any allocated
// memory before overwriting it!!!
void modify(char** args) {
	char* table = args[1];
    	int id = atoi(args[2]);
    	int index;

    	if (strcmp(table, "teams") == 0) {
        	index = id;
    	} else if (strcmp(table, "athletes") == 0) {
        	index = id;
    	} else if (strcmp(table, "competitions") == 0) {
        	index = id;
    	}

    	if (index < 0 || index >= teamsMetadata.maxCount) {
        	fprintf(stderr, "Invalid index for modification.\n");
        	return;
    	}

    	if (strcmp(table, "teams") == 0) {
        	free(teams[index].name);
        	free(teams[index].city);
        	free(teams[index].sport);

        	teams[index].name = strdup(args[3]);
        	teams[index].yearFounded = atoi(args[4]);
        	teams[index].city = strdup(args[5]);
        	teams[index].sport = strdup(args[6]);
        	teams[index].popularityIndex = atof(args[7]);

		teamsMetadata.count++;
    		teamsMetadata.nextIndex++;

    	} else if (strcmp(table, "athletes") == 0) {
        	free(athletes[index].name);
        	free(athletes[index].nationality);
        	free(athletes[index].sport);

        	athletes[index].name = strdup(args[3]);
        	athletes[index].birthYear = atoi(args[4]);
        	athletes[index].nationality = strdup(args[5]);
        	athletes[index].sport = strdup(args[6]);

		athletesMetadata.count++;
    		athletesMetadata.nextIndex++;
    	} else if (strcmp(table, "competitions") == 0) {
        	free(competitions[index].name);
        	free(competitions[index].location);
        	free(competitions[index].sport);

        	competitions[index].name = strdup(args[3]);
        	competitions[index].year = atoi(args[4]);
        	competitions[index].location = strdup(args[5]);
        	competitions[index].sport = strdup(args[6]);

		competitionsMetadata.count++;
    		competitionsMetadata.nextIndex++;
    	}

    	if (strcmp(table, "teams") == 0 && teams[index].isDeleted) {
        	teams[index].isDeleted = 0;
    	} else if (strcmp(table, "athletes") == 0 && athletes[index].isDeleted) {
        	athletes[index].isDeleted = 0;
    	} else if (strcmp(table, "competitions") == 0 && competitions[index].isDeleted) {
       		competitions[index].isDeleted = 0;
    	}
	// DO NOT TOUCH THIS PRINT
	// REQUIRED FOR AUTOGRADER
	printf("modify complete\n");
}

// step 11: implement get function
// this function is responsible for fetching all non-deleted records
// make use of your metadata
// Make sure to follow guidelines for format in writeup
// see examples as well
// Use %lf for formatting double data type
void get(char* table) {	
	int index;
	if (strcmp(table, "teams") == 0){
		index = teamsMetadata.nextIndex;
		printf("id, name, yearFounded, city, sport, popularityIndex\n");
		for (int i = 0; i < index; i++) {
            		if (!teams[i].isDeleted) {
                		printf("%d, %s, %d, %s, %s, %lf\n", teams[i].id, teams[i].name, teams[i].yearFounded, teams[i].city, teams[i].sport, teams[i].popularityIndex);
            		}
        	}
	} else 	if (strcmp(table, "athletes") == 0){
		index = athletesMetadata.nextIndex;
		printf("id, name, birthYear, nationality, sport\n");
		for (int i = 0; i < index; i++) {
            		if (!athletes[i].isDeleted) {
                		printf("%d, %s, %d, %s, %s\n", athletes[i].id, athletes[i].name, athletes[i].birthYear, athletes[i].nationality, athletes[i].sport);
            		}
        	}
			
	} else 	if (strcmp(table, "competitions") == 0){
		index = competitionsMetadata.nextIndex;
		printf("id, name, year, sport, location\n");
		for (int i = 0; i < index; i++) {
            		if (!competitions[i].isDeleted) {
                		printf("%d, %s, %d, %s, %s\n", competitions[i].id, competitions[i].name, competitions[i].year, competitions[i].sport, competitions[i].location);
            		}
        	}
			
	}
}

// step 12: implement exit function
// this function should free all allocated memory
// Make sure to avoid memory leaks by freeing any allocated memory
// inside a struct (char*) before freeing the struct pointer
void exitProgram() {
    	for (int i = 0; i < teamsMetadata.maxCount; i++) {
        	free(teams[i].name);
        	free(teams[i].city);
        	free(teams[i].sport);
    	}
    	free(teams);

    	for (int i = 0; i < athletesMetadata.maxCount; i++) {
        	free(athletes[i].name);
        	free(athletes[i].nationality);
        	free(athletes[i].sport);
    	}
    	free(athletes);

    	for (int i = 0; i < competitionsMetadata.maxCount; i++) {
        	free(competitions[i].name);
	        free(competitions[i].location);
	        free(competitions[i].sport);
	}
	free(competitions);

    	printf("exit complete\n");
}

// this code is responsible for parsing the user's
// input, and determining based on the command
// which function to send it to.
// You do not have to modify this code, but you should
// understand it.
void execute_cmd(char** args, int arg_count) {
	char* cmd = args[0];
	if (strcmp(cmd, "setup") == 0) {
		setup(args[1], atoi(args[2]));
	} else if (strcmp(cmd, "insert") == 0) {
		insert(args);
	} else if (strcmp(cmd, "delete") == 0) {
		delete(args[1], atoi(args[2]));
	} else if (strcmp(cmd, "modify") == 0) {
		modify(args);
	} else if (strcmp(cmd, "get") == 0) {
		get(args[1]);
	} else if (strcmp(cmd, "exit") == 0) {
		exitProgram();
	} else {
		printf("\n");
	}
}

// step 6: initialize the default metadata values here
// jump to L76
void initializeMetadata() {
	teamsMetadata.count = 0;
	teamsMetadata.nextIndex = 0;
	teamsMetadata.maxCount = 0;
	
	athletesMetadata.count = 0;
	athletesMetadata.nextIndex = 0;
	athletesMetadata.maxCount = 0;
	
	competitionsMetadata.count = 0;
	competitionsMetadata.nextIndex = 0;
	competitionsMetadata.maxCount = 0;
}

// this code creates the interactive shell
// you do not need to modify this
// You do not need to understand this code
// but you are welcome to research its application
void cmd_loop() {
	char input[MAX_INPUT_SIZE];
    ssize_t bytes_read;
	printf("Usage: \n");
	printf("setup {table} {numRows}\n");
	printf("insert {table} {data}\n");
	printf("delete {table} {id}\n");
	printf("modify {table} {id} {data}\n");
	printf("get {table}\n\n");
	initializeMetadata();
    while (1) {
        printf("CS211> ");
		fflush(stdout);
        
        // Read user input using the read() system call
        bytes_read = read(STDIN_FILENO, input, sizeof(input));
        
        if (bytes_read == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        
        if (bytes_read == 0) {
			printf("\n");
            break;
        }
        
        // Null-terminate the input	
        input[bytes_read] = '\0';

		char** args = (char**)malloc(MAX_ARGS_SIZE * sizeof(char*));
		int arg_count;

		splitInput(input, args, &arg_count);
        
        // Execute the user's command
        execute_cmd(args, arg_count);
		free(args);
    }
}


int main() {
	cmd_loop();
}
