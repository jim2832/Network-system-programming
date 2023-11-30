/*
 * udp_server : listen on a UDP socket ;reply immediately
 * argv[1] is the name of the local datafile
 * PORT is defined in dict.h
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "dict.h"

int main(int argc, char **argv) {
	static struct sockaddr_in server,client;
	int sockfd,siz;
	Dictrec dr, *tryit = &dr;

	if (argc != 2) {
		fprintf(stderr,"Usage : %s <datafile>\n",argv[0]);
		exit(errno);
	}

	/* Create a UDP socket.
	 * Fill in code. */
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		DIE("socket");
	}

	/* Initialize address.
	 * Fill in code. */
	server.sin_family = AF_INET; // set family
	server.sin_port = htons(PORT); // host to network short
	server.sin_addr.s_addr = htonl(INADDR_ANY); // host to network long

	/* Name and activate the socket.
	 * Fill in code. */
	if(bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
		DIE("bind");
	}

	for (;;) { /* await client packet; respond immediately */

		siz = sizeof(client); /* siz must be non-zero */

		/* Wait for a request.
		 * Fill in code. */

		while (recvfrom(sockfd, tryit->word, WORD, 0, (struct sockaddr *)&client, &siz) > 0) {
			/* Lookup request and respond to user. */
			switch(lookup(tryit,argv[1]) ) {
				case FOUND: 
					/* Send response.
					 * Fill in code. */
					if(sendto(sockfd, tryit->text, TEXT, 0, (struct sockaddr *)&client, sizeof(client)) < 0) {
						DIE("sendto");
					}
					break;
				case NOTFOUND : 
					/* Send response.
					 * Fill in code. */
					if(sendto(sockfd, "XXXX", TEXT, 0, (struct sockaddr *)&client, sizeof(client)) < 0) {
						DIE("sendto");
					}
					break;
				case UNAVAIL:
					DIE(argv[1]);
			} /* end lookup switch */
		} /* end while */
	} /* end forever loop */
} /* end main */
