#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(){
    int sd;
    struct sockaddr_un name; // UNIX domain socket address structure

    sd = socket(AF_UNIX, SOCK_STREAM, 0); // AF_UNIX: UNIX domain socket, SOCK_STREAM: TCP
    name.sun_family = AF_UNIX;

    strcpy(name.sun_path, "/tmp/socket1");
    unlink(name.sun_path);

    // bind() assigns the address specified by addr to the socket referred to by the file descriptor sd
    if (bind(sd, (struct sockaddr *)&name, sizeof(name)) == -1){
        perror("bind");
        exit(1);
    }
}
