#ifndef __NEW_H__
#define __NEW_H__
#include <signal.h>

#define sigsuspend my_sigwaitinfo

int my_sigwaitinfo(const sigset_t *__set);

#endif