#ifndef __SYSTEMV_H__
#define __SYSTEMV_H__

#include <signal.h>
typedef void (*sighandler_t)(int);

sighandler_t my_sigset(int sig, sighandler_t disp);
int my_sighold(int sig);
int my_sigrelse(int sig);
int my_sigignore(int sig);
int my_sigpause(int sig);

#define sigset my_sigset
#define sighold my_sighold
#define sigrelse my_sigrelse
#define sigignore my_sigignore
#define sigpause my_sigpause

#endif