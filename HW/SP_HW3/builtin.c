/*
 * builtin.c : check for shell built-in commands
 * structure of file is
 * 1. definition of builtin functions
 * 2. lookup-table
 * 3. definition of is_builtin and do_builtin
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include "shell.h"




/****************************************************************************/
/* builtin function definitions                                             */
/****************************************************************************/

/* "echo" command.  Does not print final <CR> if "-n" encountered. */
static void bi_echo(char **argv){
  	/* Fill in code. */

	// get argc
	int count = 0;
    while(argv[count] != NULL){
        count++;
    }

    int flag = 0;

    // -n flag
    if(argv[1] && !(strcmp(argv[1], "-n"))){
        flag = 1;
        int num = atoi(argv[2]);
        printf("%s\n", argv[num+2]);
    }
    // default
    else{
        for(int i=1; i<count; i++){
            printf("%s ", argv[i]);
        }
    }

    if(!flag){
        printf("\n");
    }
}

/* exit command */
static void bi_exit(char **argv){
    free_argv(argv);
    exit(EXIT_SUCCESS);
}


/****************************************************************************/
/* lookup table                                                             */
/****************************************************************************/

static struct cmd {
	char * keyword;				/* When this field is argv[0] ... */
	void (* do_it)(char **);	/* ... this function is executed. */
} inbuilts[] = {

	/* Fill in code. */

	{ "echo", bi_echo },	/* When "echo" is typed, bi_echo() executes.  */
    { "quit", bi_exit },
    { "exit", bi_exit },
    { "logout", bi_exit },
    { "bye", bi_exit },
	{ NULL, NULL }			/* NULL terminated. */
};




/****************************************************************************/
/* is_builtin and do_builtin                                                */
/****************************************************************************/

static struct cmd * this; 		/* close coupling between is_builtin & do_builtin */

/* Check to see if command is in the inbuilts table above.
Hold handle to it if it is. */
int is_builtin(char *cmd) {
  	struct cmd *tableCommand;

  	for (tableCommand = inbuilts ; tableCommand->keyword != NULL; tableCommand++)
    	if (strcmp(tableCommand->keyword,cmd) == 0) {
			this = tableCommand;
			return 1;
		}
  	return 0;
}


/* Execute the function corresponding to the builtin cmd found by is_builtin. */
int do_builtin(char **argv) {
  	this->do_it(argv);
}
