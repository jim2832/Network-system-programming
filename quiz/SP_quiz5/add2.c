#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// #include "apue.h"
#define MAXLINE	4096			/* max line length */
#define DIE(x) perror(x); exit(1);

int main(void) {
    int  int1, int2;
    char line[MAXLINE];

    // set line buffering for stdin and stdout
    if (setvbuf(stdin, NULL, _IOLBF, 0) != 0){
        // err_sys("setvbuf error");
        DIE("setvbuf error")
    }
    if (setvbuf(stdout, NULL, _IOLBF, 0) != 0){
        // err_sys("setvbuf error");
        DIE("setvbuf error")
    }

    while (fgets(line, MAXLINE, stdin) != NULL) {

        // scan two integers
        if (sscanf(line, "%d%d", &int1, &int2) == 2) {
            if (printf("%d\n", int1 + int2) == EOF){
                // err_sys("printf error");
                DIE("printf error")
            }
        }

        // invalid args
        else {
            if (printf("invalid args\n") == EOF){
                // err_sys("printf error");
                DIE("printf error")
            }
        }
    }
    exit(0);
}