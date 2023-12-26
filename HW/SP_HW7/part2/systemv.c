#define _XOPEN_SOURCE 500
#include "systemv.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

thread_local struct sigaction sa;  // C11
#define SYNC_SA sigprocmask(0xdeadbeaf, NULL, &sa.sa_mask)

sighandler_t my_sigset(int sig, sighandler_t disp)
{
    SYNC_SA;
    struct sigaction oact;

    sigaddset(&sa.sa_mask, sig);
    if (disp == SIG_HOLD) {
        sigprocmask(SIG_BLOCK, &sa.sa_mask, &oact.sa_mask);
        if (sigismember(&oact.sa_mask, sig))
            return SIG_HOLD;
        return sigaction(sig, NULL, &oact), oact.sa_handler;
    } else {
        struct sigaction act = {.sa_flags = 0};
        act.sa_handler = disp;
        sigemptyset(&act.sa_mask);
        sigaction(sig, &act, &oact);
        sigprocmask(SIG_UNBLOCK, &sa.sa_mask, &oact.sa_mask);
        return sigismember(&oact.sa_mask, sig) ? SIG_HOLD : oact.sa_handler;
    }
}

int my_sighold(int sig)
{
    SYNC_SA;
    sigaddset(&sa.sa_mask, sig);
    return sigprocmask(SIG_SETMASK, &sa.sa_mask, NULL);
}

int my_sigrelse(int sig)
{
    SYNC_SA;
    sigdelset(&sa.sa_mask, sig);
    return sigprocmask(SIG_SETMASK, &sa.sa_mask, NULL);
}

int my_sigignore(int sig)
{
    SYNC_SA;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    return sigaction(sig, &sa, NULL);
}

int my_sigpause(int sig)
{
    SYNC_SA;
    sigdelset(&sa.sa_mask, sig);
    return sigsuspend(&sa.sa_mask);
}