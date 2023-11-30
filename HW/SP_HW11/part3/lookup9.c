/*
 * lookup9 : does no looking up locally, but instead asks
 * a server for the answer. Communication is by Internet UDP Sockets
 * The name of the server is passed as resource. PORT is defined in dict.h
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
#define h_addr h_addr_list[0] /* for backward compatibility */

int lookup(Dictrec * sought, const char * resource) {
	static int sockfd;
	static struct sockaddr_in server;
	struct hostent *host;
	static int first_time = 1;

	if (first_time) {  /* Set up server address & create local UDP socket */
		first_time = 0;

		/* Set up destination address.
		 * Fill in code. */
		host = gethostbyname(resource);
		if (host == NULL) {
			fprintf(stderr,"Lookup : cannot find host %s\n",resource);
			exit(errno);
		}
		server.sin_family = AF_INET; // set family
		server.sin_port = htons(PORT); // host to network short
		memcpy(&server.sin_addr, host->h_addr, host->h_length); // copy address to server.sin_addr
		

		/* Allocate a socket.
		 * Fill in code. */
		if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			DIE("socket");
		}
	}

	/* Send a datagram & await reply
	 * Fill in code. */
	if(sendto(sockfd, sought->word, WORD, 0, (struct sockaddr *)&server, sizeof(server)) < 0) {
		DIE("sendto");
	}
	if(recvfrom(sockfd, sought->text, TEXT, 0, NULL, NULL) < 0) {
		DIE("recvfrom");
	}

	if (strcmp(sought->text,"XXXX") != 0) {
		return FOUND;
	}

	return NOTFOUND;
}
