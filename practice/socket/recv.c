#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int main() {
    char message[80];
    int length = sizeof(message);
    int rval, i;
    int sd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in sin; // Internet domain socket address structure
    memset((char *)&sin, '\0', sizeof(sin));
    sin.sin_family = AF_INET;

    if ((rval = recv(sd, message, length, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    
    for (i = 0; i < rval; i++) {
        putchar(message[i]);
    }
}