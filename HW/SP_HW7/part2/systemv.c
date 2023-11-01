#include <signal.h>
#include <unistd.h>

// Define the System V signal handler type
typedef void (*sighandler_t)(int);

// Define the System V signal set type
typedef unsigned long sigset_t;

// Define the System V signal mask type
typedef unsigned long sigmask_t;

// Define the System V signal action type
typedef struct sigaction sigaction_t;

// Define the System V signal handler table type
typedef struct {
    sighandler_t sh_handlers[NSIG];
} sh_table_t;

// Define the System V signal handler table
static sh_table_t sh_table = {0};

// Define the System V signal mask
static sigmask_t sh_mask = 0;

// Define the System V signal action
static sigaction_t sh_action = {0};

// Define the System V signal set
static sigset_t sh_set = 0;

// Define the System V signal handler
static void sh_handler(int signo) {
    sighandler_t handler = sh_table.sh_handlers[signo];
    if (handler != SIG_DFL && handler != SIG_IGN) {
        handler(signo);
    }
}

// Define the System V sigset() function
int sigset(int signo, sighandler_t handler) {
    if (signo < 1 || signo >= NSIG) {
        return -1;
    }
    sh_table.sh_handlers[signo] = handler;
    return 0;
}

// Define the System V sighold() function
int sighold(int signo) {
    if (signo < 1 || signo >= NSIG) {
        return -1;
    }
    sigaddset(&sh_mask, signo);
    return sigprocmask(SIG_BLOCK, &sh_mask, NULL);
}

// Define the System V sigrelse() function
int sigrelse(int signo) {
    if (signo < 1 || signo >= NSIG) {
        return -1;
    }
    sigdelset(&sh_mask, signo);
    return sigprocmask(SIG_UNBLOCK, &sh_mask, NULL);
}

// Define the System V sigignore() function
int sigignore(int signo) {
    if (signo < 1 || signo >= NSIG) {
        return -1;
    }
    return sigset(signo, SIG_IGN);
}

// Define the System V sigpause() function
int sigpause(int signo) {
    if (signo < 1 || signo >= NSIG) {
        return -1;
    }
    sh_action.sa_handler = sh_handler;
    sh_action.sa_mask = sh_set;
    sh_action.sa_flags = 0;
    sigaction(signo, &sh_action, NULL);
    sigdelset(&sh_set, signo);
    return sigsuspend(&sh_set);
}
