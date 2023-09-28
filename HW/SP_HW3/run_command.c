/*
 * run_command.c :    do the fork, exec stuff, call other functions
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include "shell.h"

void run_command(char **myArgv) {
    pid_t pid;

    /* Create a new child process. */
    pid = fork();

    /* determine whether the process is on the background */
    int do_wait = !(is_background(myArgv));
    
    /* remove & */
    if(!do_wait){
        int i = 0;
        while(myArgv[i]){
            i++;
        }
        free(myArgv[i-1]);
        myArgv[i-1] = NULL;
    }

    switch (pid) {

        /* Error. */
        case -1 :
            perror("fork");
            exit(errno);

        /* Child. */
        case 0 :
            /* Run command in child process. */
            execvp(myArgv[0], myArgv);

            /* Handle error return from exec */
			exit(errno);
        
        /* Parent. */
        default :
            /* Wait for child to terminate. */
            if(do_wait){
                int stat;
                pid_t wait_pid = waitpid(pid, &stat, WUNTRACED);
                
                while(!WIFEXITED(stat) && !WIFSIGNALED(stat)){
                    wait_pid = waitpid(pid, &stat, WUNTRACED);
                    if(wait_pid == -1){
                        perror("wiatpid");
                        exit(EXIT_FAILURE);
                    }

                    /* Optional: display exit status.  (See wstat(5).) */
                    if(WIFEXITED(stat)){
                        printf("Child with PID %d has exited with status: %d\n", wait_pid, WEXITSTATUS(stat));
                    }
                    else{
                        printf("Child with PID %d did not exit normally\n", wait_pid);
                    }
                }
                
            }
            break;

            return;
    }
}
