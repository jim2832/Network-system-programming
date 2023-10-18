#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]){
    int pd[2];

    if(pipe(pd) == -1){
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    switch(fork()){
        /* fail */
        case -1:
            perror("fork");
            exit(1);
            break;

        /* child */
        case 0:
            char *command[] = {"ls", "-al", NULL};
            close(pd[0]); // close read
            dup2(pd[1], STDOUT_FILENO);
            execvp(command[0], command);
            exit(EXIT_SUCCESS);
            break;

        /* parent */
        default:
            char c;
            close(pd[1]); // close write
            while((read(pd[0], &c, 1)) != 0){
                write(STDOUT_FILENO, &c, 1);
            }
            close(pd[0]); // close read
            break;
    }

    return 0;
}