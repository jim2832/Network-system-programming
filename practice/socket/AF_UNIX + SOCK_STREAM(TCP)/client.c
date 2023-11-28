/* clnt1.c - AF_UNIX, SOCK_STREAM */
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

int main() {
    int namesize = sizeof(struct sockaddr_un);
    int sd;
    struct sockaddr_un srvr;
    double dub = 2.3, dub2;

    /* Create client's socket sd */
    if ((sd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("clnt1.c:main:socket");
        exit(1);
    }

    /* Fill in server's address and connect to server */
    srvr.sun_family = AF_UNIX;
    strcpy(srvr.sun_path, "/tmp/socket1");
    if (connect(sd, (struct sockaddr *)&srvr, namesize) == -1) {
        perror("clnt1.c:main:connect");
        exit(1);
    }
    
    /* Communicate with server */
    write(sd, &dub, sizeof(dub));
    read(sd, &dub2, sizeof(dub2));
    close(sd);
    printf("dub: %.2f, dub2: %.2f\n", dub, dub2);

    return 0;
}