#ifndef __NEW_VERSION_H__
#define __NEW_VERSION_H__
#include <signal.h>

#define sigsuspend my_sigwaitinfo

int my_sigwaitinfo(const sigset_t *__set);

#endif