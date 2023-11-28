#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>

int main() {
    struct sockaddr_un client;
    int len = sizeof(client), comm;

    int sd;
    sd = socket(AF_INET, SOCK_STREAM, 0);
    memset((char *)&client, '\0', sizeof(client));

    loop :
    while ((comm = accept(sd, &client, &len)) != -1) { // accept() returns a new socket descriptor to use for this single connection
        if (fork() == 0) {
            /* child to handle session */
            close(sd);        /* do not need rendezvous */
            do_service(comm); /* talk to client using comm */
            close(comm);
            exit(0);
        }
        close(comm); /* do not need communication */
    }

    /* If a signal arrives during accept(), it fails
    with -1. So, if catching signals, use the following
    2 lines */
    if (errno == EINTR) {
        goto loop;
    }
}