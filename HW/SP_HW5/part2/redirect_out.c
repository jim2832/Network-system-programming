/*
 * redirect_out.c   :   check for >
 */

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"
#include <sys/stat.h>
#define STD_OUTPUT 1
#define STD_INPUT  0

/* command > output_file */

/*
 * Look for ">" in myArgv, then redirect output to the file.
 * Returns 0 on success, sets errno and returns -1 on error.
 */
int redirect_out(char ** myArgv){
	int i = 0;
  	int fd;

    	/* 1) Open file.
    	 * 2) Redirect to use it for output.
    	 * 3) Cleanup / close unneeded file descriptors.
    	 * 4) Remove the ">" and the filename from myArgv.
		 *
    	 * Fill in code. */

	while(myArgv[i]){
        if(strcmp(myArgv[i], ">") == 0){
            // Found ">" in the command line arguments.

            if(myArgv[i + 1] == NULL){
                // Error: No filename specified after ">".
                errno = EINVAL;
                return -1;
            }

            // 1) Open the file.
            fd = open(myArgv[i + 1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if(fd == -1){
                perror("open");
                return -1;
            }

            // 2) Redirect stdout to use the file for output.
            if(dup2(fd, STD_OUTPUT) == -1){
                perror("dup2");
                return -1;
            }

            // 3) Cleanup / close unneeded file descriptors.
            close(fd);

            // 4) Remove the ">" and the filename from myArgv.
            free(myArgv[i]);      // Free ">".
            free(myArgv[i + 1]);  // Free the filename.
            
            // Shift the remaining arguments to the left.
            int j = i + 2;
            while(myArgv[j] != NULL){
                myArgv[j - 2] = myArgv[j];
                j++;
            }
            myArgv[j - 2] = NULL; 

            return 0;
        }
        i++;
    }

  	return -1;
}
