#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// #include "apue.h"
#define DIE(x) perror(x); exit(1);
#define MAXLINE	4096			/* max line length */

int main(void) {
    int n, int1, int2;
    char line[MAXLINE];

    while ((n = read(STDIN_FILENO, line, MAXLINE)) > 0) {

        line[n] = 0;/* null terminate */
        
        // scan two integers
        if (sscanf(line, "%d%d", &int1, &int2) == 2) {
            sprintf(line, "%d\n", int1 + int2);
            n = strlen(line);

            if (write(STDOUT_FILENO, line, n) != n){
                // err_sys("write error");
                DIE("write error")
            }
        }
        
        // invalid args
        else {
            if (write(STDOUT_FILENO, "invalid args\n", 13) != 13){
                // err_sys("write error");
                DIE("write error")
            }
        }
    }

    exit(0);
}