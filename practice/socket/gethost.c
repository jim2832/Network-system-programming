/* gethost.c */
#include <netdb.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>

main(int argc, char *argv[]) {
    struct hostent *he; // host information
    char **tmp;

    // error checking
    if (argc != 2) {
        fprintf(stderr, "Usage: %s hostname\n", argv[0]);
        exit(1);
    }

    // gethostbyname() returns a structure of type hostent for the given host name
    he = gethostbyname(argv[1]);
    if (he == NULL) {
        fprintf(stderr, "No such host.\n");
        exit(1);
    }

    // h_name is the official name of the host
    printf("Official name of %s is %s.\n", argv[1], he->h_name);

    // h_aliases is a NULL-terminated array of alternate names for the host
    for (tmp = he->h_aliases; *tmp != NULL; tmp++) {
        printf("\tAlias: %s\n", *tmp);
    }

    // h_addrtype is the type of address being returned; usually AF_INET
    tmp = he->h_addr_list;
    printf("IP address: %d.%d.%d.%d\n", (unsigned char)(*tmp)[0],
           (unsigned char)(*tmp)[1], (unsigned char)(*tmp)[2],
           (unsigned char)(*tmp)[3]);
           
    return 0;
}