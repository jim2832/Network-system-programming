/*
 * builtin.c : check for shell built-in commands
 * structure of file is
 * 1. definition of builtin functions
 * 2. lookup-table
 * 3. definition of is_builtin and do_builtin
*/

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <string.h>
#include "shell.h"

/****************************************************************************/
/* builtin function definitions                                             */
/****************************************************************************/
static void bi_builtin(char ** argv);	/* "builtin" command tells whether a command is builtin or not. */
static void bi_cd(char **argv) ;		/* "cd" command. */
static void bi_echo(char **argv);		/* "echo" command.  Does not print final <CR> if "-n" encountered. */
static void bi_hostname(char ** argv);	/* "hostname" command. */
static void bi_id(char ** argv);		/* "id" command shows user and group of this process. */
static void bi_pwd();		/* "pwd" command. */
static void bi_quit(char **argv);		/* quit/exit/logout/bye command. */




/****************************************************************************/
/* lookup table                                                             */
/****************************************************************************/

static struct cmd {
  	char * keyword;					/* When this field is argv[0] ... */
  	void (* do_it)(char **);		/* ... this function is executed. */
} inbuilts[] = {
  	{ "builtin",    bi_builtin },   /* List of (argv[0], function) pairs. */

    /* Fill in code. */
    { "echo",       bi_echo },
    { "quit",       bi_quit },
    { "exit",       bi_quit },
    { "bye",        bi_quit },
    { "logout",     bi_quit },
    { "cd",         bi_cd },
    { "pwd",        bi_pwd },
    { "id",         bi_id },
    { "hostname",   bi_hostname },
    {  NULL,        NULL }          /* NULL terminated. */
};


static void bi_builtin(char ** argv) {
	// get argc
	int argc = 0;
    while(argv[argc] != NULL){
        argc++;
    }

	// error check
	if(argc != 2)
		printf("Usage: builtin <command>\n");

	// chech whether it is a builtin function
	if(is_builtin(argv[1]))
		printf("%s is a builtin feature.\n", argv[1]);
	else
		printf("%s is NOT a builtin feature.\n", argv[1]);
}

static void bi_cd(char **argv) {
	/* Fill in code. */

	// get argc
	int argc = 0;
    while(argv[argc] != NULL){
        argc++;
    }

	// error check
	if(argc != 2)
		printf("Usage: cd <path>\n");
	
	// change directory
	if(chdir(argv[1]) == 0)
		bi_pwd();
	else
		perror("chdir");
}

static void bi_echo(char **argv) {
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

static void bi_hostname(char ** argv) {
	/* Fill in code. */
	system("echo -n \"hostname: \" ; hostname");
}

static void bi_id(char ** argv) {
 	/* Fill in code. */
	uid_t uid = getuid();
    gid_t gid = getgid();
    
    printf("User ID: %d, Group ID: %d\n", uid, gid);
}

static void bi_pwd() {
	/* Fill in code. */
	char buffer[512];
    getcwd(buffer, sizeof(buffer));
    puts(buffer);
}

static void bi_quit(char **argv) {
	exit(0);
}


/****************************************************************************/
/* is_builtin and do_builtin                                                */
/****************************************************************************/

static struct cmd * this; /* close coupling between is_builtin & do_builtin */

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
