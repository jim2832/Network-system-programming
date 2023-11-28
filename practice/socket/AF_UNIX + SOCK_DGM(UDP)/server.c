/* srvr2.c - AF_UNIX, SOCK_DGRAM
 * Server reads a string from client and
 * sends back a greeting appended to the string */
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#define BUFSZ 256

int main() {
    int sd, comm;
    struct sockaddr_un myname, client;
    int namesize = sizeof(struct sockaddr_un);
    char buf[BUFSZ];

    /* Create server's socket sd */
    if ((sd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
        perror("srvr2.c:main:socket");
        exit(1);
    }

    /* Fill in server's address and bind it to sd */
    myname.sun_family = AF_UNIX;
    strcpy(myname.sun_path, "/tmp/socket2srvr");
    unlink(myname.sun_path);
    if (bind(sd, (struct sockaddr *)&myname, namesize) == -1) {
        perror("srvr2.c:main:bind");
        exit(1);
    }

    /* Infinite loop to accept client requests */
    while (1) {
        if (recvfrom(sd, (char *)&buf, BUFSZ, 0, (struct sockaddr *)&client, &namesize) == -1) {
            perror("srvr2.c:main:recvfrom");
            exit(1);
        }
        strcat(buf, ", Hello from the server");
        if (sendto(sd, buf, BUFSZ, 0, (struct sockaddr *)&client, namesize) == -1) {
            perror("srvr2.c:main:sendto");
            exit(1);
        }
    }

    /* Install a signal handler for cleanup including:
     * close(sd);
     * unlink( myname.sun_path ); */

    return 0;
}