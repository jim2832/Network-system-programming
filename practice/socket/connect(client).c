#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netdb.h>

// unix domain socket
int main() {
    struct sockaddr_un name;

    int sd;
    sd = socket(AF_UNIX, SOCK_STREAM, 0);

    name.sun_family = AF_UNIX;
    strcpy(name.sun_path, "/tmp/socket1");

    // connect() is used by the client to establish a connection to the server
    if (connect(sd, (struct sockaddr *)&name, sizeof(name)) == -1) {
        perror("connect");
        exit(1);
    }
}

// internet domain socket
int main() {
    int sd; // socket descriptor
    sd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in sin; // Internet domain socket address structure
    memset((char *)&sin, '\0', sizeof(sin));
    sin.sin_family = AF_INET;

    /* match server */
    struct hostent *he;
    he = gethostbyname("Server");
    sin.sin_port = htons(7000);
    memcpy((char *)&sin.sin_addr, he->h_addr_list[0], he->h_length);

    // connect() is used by the client to establish a connection to the server
    if (connect(sd, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
        perror("connect");
        exit(1);
    }
}