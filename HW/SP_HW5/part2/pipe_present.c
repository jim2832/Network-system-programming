/*
 *  pipe_present.c :  check for |
 */

#include <stdio.h>
#include <string.h>
#include "shell.h"

/*
 * Return index offset into argv of where "|" is,
 * -1 if in an illegal position (first or last index in the array),
 * or 0 if not present.
 */
int pipe_present(char ** myCurrentArgv) {
	int index = 0;

  	/* Search through myCurrentArgv for a match on "|". */

	while(myCurrentArgv[index] != NULL){
        if(!strcmp(myCurrentArgv[index], "|")){
			/* At the beginning or at the end. */
            if(index == 0 || myCurrentArgv[index + 1] == NULL){
                return -1;
            }
			/* In the middle. */
			else{
				// printf("index: %d", index);
                return index;
            }
        }
        index++;
    }

	/* Off the end. */
	return 0;
}
