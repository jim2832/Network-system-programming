/*
 * parse.c : use whitespace to tokenise a line
 * Initialise a vector big enough
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "shell.h"

/* Parse a commandline string into an argv array. */
char ** parse(char *line){

  	static char delim[] = " \t\n"; /* SPACE or TAB or NL */
  	int count = 0;
  	char * token;
  	char **newArgv;

  	/* Nothing entered. */
  	if(line == NULL){
    	return NULL;
  	}


  	/* Init strtok with commandline, then get first token.
     * Return NULL if no tokens in line.
	 *
	 * Fill in code.
     */
	token = strtok(line, delim); // the first token
	if(token == NULL){
		return NULL;
	}


  	/* Create array with room for first token.
  	 *
	 * Fill in code.
	 */

	// Allocate memory for the array of pointers
    newArgv = (char **)malloc(sizeof(char *));
    if(newArgv == NULL){
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

	newArgv[0] = (char *)malloc(strlen(token) + 1);
    if(newArgv[0] == NULL){
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }
    strcpy(newArgv[0], token);
	printf("[%d] : %s\n", count, token);
    count++;


  	/* While there are more tokens...
	 *
	 *  - Get next token.
	 *	- Resize array.
	 *  - Give token its own memory, then install it.
	 * 
  	 * Fill in code.
	 */
	 while (token != NULL){
        // Get the next token
        token = strtok(NULL, delim);
		
        if(token != NULL){
			printf("[%d] : %s\n", count, token);
            // Resize the array to hold more pointers
            newArgv = (char **)realloc(newArgv, (count + 1) * sizeof(char *));
            if(newArgv == NULL){
                perror("Memory allocation error");
                exit(EXIT_FAILURE);
            }

            // Allocate memory for the token and copy it
            newArgv[count] = (char *)malloc(strlen(token) + 1);
            if(newArgv[count] == NULL){
                perror("Memory allocation error");
                exit(EXIT_FAILURE);
            }
            strcpy(newArgv[count], token);
            count++;
        }
    }


  	/* Null terminate the array and return it.
	 *
  	 * Fill in code.
	 */
	newArgv = (char **)realloc(newArgv, (count + 1) * sizeof(char *));
    if(newArgv == NULL){
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    newArgv[count] = NULL;  // Null-terminate the array

  	return newArgv;
}


/*
 * Free memory associated with argv array passed in.
 * Argv array is assumed created with parse() above.
 */
void free_argv(char **oldArgv){

	int i = 0;

	/* Free each string hanging off the array.
	 * Free the oldArgv array itself.
	 *
	 * Fill in code.
	 */

	// Free each string and then free the array of pointers
    while (oldArgv[i] != NULL){
        free(oldArgv[i]);  // Free each string
        i++;
    }

    free(oldArgv);  // Free the array of pointers
}
