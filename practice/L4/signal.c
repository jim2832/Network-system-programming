#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>

int main(){
    sigset_t set1;
    /* Clear set1 (all bits = 0) */
    sigemptyset( &set1 );

    /* Express interest in SIGINT */
    sigaddset( &set1, SIGINT );

    /* Express interest in SIGQUIT */
    sigaddset( &set1, SIGQUIT );

    return 0;
}