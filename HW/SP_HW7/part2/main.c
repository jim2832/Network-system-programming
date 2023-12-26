#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include "systemv.h"

static void handler(int sig)
{
    (void) sig;
}

int main(int argc, char *argv[])
{
    (void) argv;
    sigset(SIGUSR1, handler);
    sighold(SIGUSR1);
    sigrelse(SIGUSR1);
    if (argc > 1)
        sigignore(SIGUSR1);

    pid_t pid = fork();
    if (!pid) {
        sigpause(SIGUSR1);
        puts("Got it!");
    } else {
        puts("Sleep 1 second to send SIG to child");
        sleep(1);
        kill(pid, SIGUSR1);
    }
    return 0;
}