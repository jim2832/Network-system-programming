/*
 * redirect_in.c  :  check for <
 */

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "shell.h"
#define STD_OUTPUT 1
#define STD_INPUT  0

/* command < input_file */

/*
 * Look for "<" in myArgv, then redirect input to the file.
 * Returns 0 on success, sets errno and returns -1 on error.
 */
int redirect_in(char ** myArgv){
  	int i = 0;
  	int fd;

	/* 1) Open file.
		* 2) Redirect stdin to use file for input.
		* 3) Cleanup / close unneeded file descriptors.
		* 4) Remove the "<" and the filename from myArgv.
		*
		* Fill in code. */

	while(myArgv[i]){
		// Found "<" in the command line arguments.
        if(strcmp(myArgv[i], "<") == 0){

            if(myArgv[i + 1] == NULL){
                // Error: No filename specified after "<".
                return -1;
            }

            // 1) Open file.
            fd = open(myArgv[i + 1], O_RDONLY);
            if(fd == -1){
                perror("open");
                return -1;
            }

            // 2) Redirect stdin to use the file for input.
            if(dup2(fd, STD_INPUT) == -1){
                perror("dup2");
                return -1;
            }

            // 3) Cleanup / close unneeded file descriptors.
            close(fd);

            // 4) Remove the "<" and the filename from myArgv.
            free(myArgv[i]);      // Free "<".
            free(myArgv[i + 1]);  // Free the filename.
            
            // Shift the remaining arguments to the left.
            int j = i + 2;
            while(myArgv[j] != NULL){
                myArgv[j - 2] = myArgv[j];
                j++;
            }
            myArgv[j - 2] = NULL;  // Mark the end of arguments.

            return 0;  // Input redirection successfully set up.
        }
        i++;
    }

  	return 0;
}
