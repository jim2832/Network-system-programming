 #include <signal.h>
 #include <unistd.h>
 #include <stdlib.h>

void handler(int signo) {
    write(1, "Hi, I'm the signal handler!\n", 29);
    /* If absent, execution resumes where interrupted */
    exit(0);
}

main() {
    void (*ohand)(int);
    
    /* Install handler() */
    ohand = sigset(SIGINT, handler);

    if (ohand == SIG_ERR) {
        perror("sigset");
    }

    /* do some stuff */

    /* Reinstall ohand */
    (void)sigset(SIGINT, ohand);
}