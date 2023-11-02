#define _POSIX_SOURCE 1
#include <stdio.h>
#include <signal.h>

typedef void (*sighandler_t)(int);

sighandler_t sigset(int signum, sighandler_t handler) {
    struct sigaction new_action, old_action;
    
    new_action.sa_handler = handler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;
    
    if (signum == SIGALRM || signum == SIGCHLD) {
        new_action.sa_flags |= SA_RESTART;
    }
    
    if (sigaction(signum, &new_action, &old_action) < 0) {
        return SIG_ERR;
    }
    
    return old_action.sa_handler;
}

int sighold(int signum) {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, signum);
    return sigprocmask(SIG_BLOCK, &mask, NULL);
}


int sigrelse(int signum) {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, signum);
    return sigprocmask(SIG_UNBLOCK, &mask, NULL);
}

sighandler_t sigignore(int signum) {
    return sigset(signum, SIG_IGN);
}

void sigpause(int signum) {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, signum);
    sigsuspend(&mask);
}
