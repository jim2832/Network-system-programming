#include "new_version.h"
#define NULL ((void *) 0)

int my_sigwaitinfo(const sigset_t *__mask){
    sigset_t set = *__mask;
    sigaddset(&set, SIGUSR1);
    return sigwaitinfo(&set, NULL);
}