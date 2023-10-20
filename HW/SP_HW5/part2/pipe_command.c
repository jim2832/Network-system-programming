/* 
 * pipe_command.c  :  deal with pipes
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "shell.h"

#define STD_OUTPUT 1
#define STD_INPUT  0

void pipe_and_exec(char **myArgv){
  	int pipe_argv_index = pipe_present(myArgv);
	printf("pipe_argv_index: %d", pipe_argv_index);
  	int pipefds[2];
	char **left_argv;
	char **right_argv;

  	switch (pipe_argv_index){

    	case -1:	/* Pipe at beginning or at end of argv;  See pipe_present(). */
      		fputs("Missing command next to pipe in commandline.\n", stderr);
      		errno = EINVAL;	/* Note this is NOT shell exit. */
      		break;

    	case 0:	/* No pipe found in argv array or at end of argv array.
			See pipe_present().  Exec with whole given argv array. */
			execvp(myArgv[0], myArgv);
            perror("Execution failed");
            exit(errno);
      		break;

    	default:	/* Pipe in the middle of argv array.  See pipe_present(). */

      		/* Split arg vector into two where the pipe symbol was found.
       		 * Terminate first half of vector.
			 *
       		 * Fill in code. */
			left_argv = myArgv;
            right_argv = &myArgv[pipe_argv_index + 1];
            myArgv[pipe_argv_index] = NULL;

      		/* Create a pipe to bridge the left and right halves of the vector. 
			 *
			 * Fill in code. */
			if(pipe(pipefds) == -1){
                perror("Pipe fail");
                exit(errno);
            }

      		/* Create a new process for the right side of the pipe.
       		 * (The left side is the running "parent".)
       		 *
			 * Fill in code to replace the underline. */
			int child_pid = fork();

      		switch(child_pid){

        		case -1 :
					perror("Fork fail");
	  				break;

        		/* Talking parent.  Remember this is a child forked from shell. */
        		default :

	  				/* - Redirect output of "parent" through the pipe.
	  				 * - Don't need read side of pipe open.  Write side dup'ed to stdout.
	 	 			 * - Exec the left command.
					 *
					 * Fill in code. */
                    close(pipefds[0]); // close read
                    dup2(pipefds[1], STD_OUTPUT);
                    close(pipefds[1]); // close write

                    // Execute the left command.
                    execvp(left_argv[0], left_argv);
                    perror("Execution of left command failed");
                    exit(errno);
                    break;

        		/* Listening child. */
        		case 0 :

	  				/* - Redirect input of "child" through pipe.
					  * - Don't need write side of pipe. Read side dup'ed to stdin.
				  	 * - Exec command on right side of pipe and recursively deal with other pipes
					 *
					 * Fill in code. */
                    close(pipefds[1]); // close write
                    dup2(pipefds[0], STD_INPUT);
                    close(pipefds[0]); // close read
					 
          			pipe_and_exec(&myArgv[pipe_argv_index+1]);
			}
	}
	perror("Couldn't fork or exec child process");
  	exit(errno);
}
