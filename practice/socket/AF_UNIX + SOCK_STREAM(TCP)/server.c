/* srvr1.c - AF_UNIX, SOCK_STREAM
 * Server reads a value from client and
 * sends back twice that value */
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    int sd, comm;
    struct sockaddr_un myname, client;
    int namesize = sizeof(struct sockaddr_un);
    int clientsize = sizeof(struct sockaddr_un);
    double dub;

    /* Create server's rendezvous socket sd */
    if ((sd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("srvr1.c:main:socket");
        exit(1);
    }

    /* Fill in server's address and bind it to sd */
    myname.sun_family = AF_UNIX;
    strcpy(myname.sun_path, "/tmp/socket1");
    unlink(myname.sun_path);


    if (bind(sd, (struct sockaddr *)&myname, namesize) == -1) {
        perror("srvr1.c:main:bind");
        exit(1);
    }
    
    /* Prepare to receive multiple connect requests */
    if (listen(sd, 128) == -1) {
        perror("srvr1.c:main:listen");
        exit(1);
    }

    /* Infinite loop to accept client requests */
    while (1) {
        comm = accept(sd, (struct sockaddr *)&client, &clientsize);
        if (comm == -1) {
            perror("srvr1.c:main:accept");
            exit(1);
        }
        read(comm, &dub, sizeof(dub));
        dub += dub;
        write(comm, &dub, sizeof(dub));
        close(comm);
    }

    /* Install a signal handler for cleanup including:
     * close(sd);
     * unlink( myname.sun_path ); */

    return 0;
}