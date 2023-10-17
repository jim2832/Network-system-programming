#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

int count;

void handler(int signo) {
    count++;
    write(1, " OUCH!!\n", 10 );
}

main() {
    int x;
    struct sigaction act;
    time_t start;

    /* Install handler() for SIGINT and SIGQUIT */
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGINT);
    sigaddset(&act.sa_mask, SIGQUIT);
    act.sa_flags = 0;
    sigaction(SIGQUIT, &act, (struct sigaction *)NULL);
    sigaction(SIGINT, &act, (struct sigaction *)NULL);
    
    printf("Do not press control-C or control-\\.\n");

    /* Waste time while user may or may not ^C or ^\ */
    start = time((time_t*)0);

    while (difftime(time((time_t*)0), start) < 10);
    if (count) {
        printf("\nYou pressed ^C or ^\\ %d times!\n", count);
    }
    else{
        printf ("\nYou follow directions well.\n");
    }
}