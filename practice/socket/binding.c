#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int main() {
    struct sockaddr_in sin; // Internet domain socket address structure

    int sd;
    sd = socket(AF_INET, SOCK_STREAM, 0); // AF_INET: Internet domain socket, SOCK_STREAM: TCP
    memset((char *)&sin, '\0', sizeof(sin)); // clear sin

    sin.sin_family = AF_INET; // AF_INET: Internet domain socket
    sin.sin_port = htons(7000); // htons: host to network short
    sin.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY: any address, htonl: host to network long

    // bind() assigns the address specified by addr to the socket referred to by the file descriptor sd
    if (bind(sd, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
        perror("bind");
        exit(1);
    }

    // listen() marks the socket referred to by sd as a passive socket, that is,
    // as a socket that will be used to accept incoming connection requests using accept()
    if (listen(sd, 5) == -1) { // 5: backlog (the maximum length to which the queue of pending connections for sd may grow)
        perror("listen?");
        exit(1);
    }
}
