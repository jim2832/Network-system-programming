#include "new.h"
#define NULL ((void *) 0)

int my_sigwaitinfo(const sigset_t *__mask)
{
    sigset_t _local_set = *__mask;
    sigaddset(&_local_set, SIGUSR1);
    return sigwaitinfo(&_local_set, NULL);
}