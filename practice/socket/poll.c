#include <errno.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
/* Number of file descriptors to poll. */
#define NUM_FDS_TO_POLL 2
main() {
    int udpsd1, udpsd2;
    struct sockaddr_in sin1;
    struct sockaddr_in sin2;
    struct sockaddr_in returnAddr;
    int returnAddrSize;

    /* This array of struct pollfd defines fully
    what poll() is to wait on, and provides a place
    for poll() to respond. */
    struct pollfd fds[NUM_FDS_TO_POLL];

    /* Set up two UDP sockets to wait on. */
    memset((char *)&sin1, '\0', sizeof(sin1));
    
    sin1.sin_family = AF_INET;
    sin1.sin_port = htons(7000);
    sin1.sin_addr.s_addr = htonl(INADDR_ANY);
    
    /* create one udp sd */
    udpsd1 = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpsd1 == -1) {
        perror("udp socket");
        exit(errno);
    }
    if (bind(udpsd1, (struct sockaddr *)&sin1, sizeof(sin1)) == -1) {
        perror("tcp bind");
        exit(errno);
    }

    /* Set up two UDP sockets to wait on. */
    memset((char *)&sin2, '\0', sizeof(sin2));
    
    sin2.sin_family = AF_INET;
    sin2.sin_port = htons(7001);
    sin2.sin_addr.s_addr = htonl(INADDR_ANY);

    /* create one udp sd */
    udpsd2 = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpsd2 == -1) {
        perror("udp socket");
        exit(errno);
    }
    if (bind(udpsd2, (struct sockaddr *)&sin2, sizeof(sin2)) == -1) {
        perror("udp bind");
        exit(errno);
    }

    /*
    Initialize each element of the fds array with:
    - fd: the file descriptor of interest.
    - events: what type of event to wait on.
    Poll returns with the revents field set with the
    events which are ready to do. E.G. if it
    returns POLLIN, a read can be done without
    blocking.
    */
    fds[0].fd = udpsd1;
    fds[0].events = POLLIN;
    fds[1].fd = udpsd2;
    fds[1].events = POLLIN;

    /*
    Timeout value (poll’s third argument) can be:
    0: Immediate return whether or not fd is ready.
    -1 (INFTIM): Blocks until a fd is ready.
    <other value>: time in mS to wait for a fd
    before returning.
    */
    for (;;) {
        int numfds;
        if ((numfds = poll(fds, NUM_FDS_TO_POLL, -1)) < 0) {
            perror("poll");
            exit(errno);
        } else {
            printf("Poll returned % d fds to read.\n", numfds);
        }
        /* if udp sd ready, use it */
        if (fds[0].revents == POLLIN) {
            char buffer[80];
            if (recvfrom(udpsd1, buffer, 80, 0, (struct sockaddr *)&returnAddr,
                         &returnAddrSize) >= 0) {
                puts(buffer);
            } else {
                perror("recvfrom");
            }
        }
        /* if udp sd ready, use it */

        if (fds[1].revents == POLLIN) {

            char buffer[80];

            if (recvfrom(udpsd2, buffer, 80, 0,

                         (struct sockaddr *)&returnAddr,

                         &returnAddrSize) >= 0) {

                puts(buffer);

            } else {

                perror("recvfrom");
            }
        }
    }
}