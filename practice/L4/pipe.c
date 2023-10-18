#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#define BSIZE 1024

main(){
    int pd[2];
    char buf[BSIZE];

    if(pipe(pd) == -1){
        perror("pipe");
        exit(1);
    }

    switch(fork()){
        case -1:
            perror("fork");
            exit(1);
            break;

        /* child */
        case 0:
            /* Close read side, won't use it */
            close(pd[0]);
            write( pd[1], "\nHi, Mom, I'm your kid!", 24);
            close(pd[1]);
            exit(0);
            break;

        /* parent */
        default:
            break;
    }

    /* Close write side, won't use it */
    close(pd[1]);

    /* Assumes a single string of */
    /* size less than BSIZE was written. */
    read(pd[0], buf, BSIZE);
    puts(buf);
    close(pd[0]);
    
    if(waitpid(-1, (int *)NULL, 0) == -1){
        perror("waitpid");
        exit(1);
    }
    return 0;
}