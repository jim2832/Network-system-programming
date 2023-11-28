#include <signal.h>
#include <stdio.h>
#include <sys/filio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    int char_present;
    void handler() { char_present++; }
    int fd, flags;

    /* Install handler for SIGPOLL, see Signals module*/
    install_disp(SIGPOLL, handler);

    /* Register fd to have SIGPOLL sent when IO is
     * possible. Usually fd is a pipe or socket. */
    ioctl(fd, I_SETSIG, S_RDNORM);

    while (1) {
        /* Block SIGPOLL to prevent char_present being changed */
        if (char_present) {
            /* Can use poll(2) to determine which
            descriptor sent the SIGPOLL, if more than one. */
            do_io();
            char_present = 0;
        }

        /* Unblock SIGPOLL */
        /* Main program work is in this loop */
    }
}