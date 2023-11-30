/*
 * isock_server : listen on an internet socket ; fork ;
 *                child does lookup ; replies down same socket
 * argv[1] is the name of the local datafile
 * PORT is defined in dict.h
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dict.h"

int main(int argc, char **argv) {
	static struct sockaddr_in server;
	int sd,cd,n;
	Dictrec try_it;

	if (argc != 2) {
		fprintf(stderr,"Usage : %s <datafile>\n",argv[0]);
		exit(1);
	}

	/* Create the socket.
	 * Fill in code. */
	if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		DIE("socket");
	}

	/* Initialize address.
	 * Fill in code. */
	server.sin_family = AF_INET; // set family
	server.sin_port = htons(PORT); // host to network short
	server.sin_addr.s_addr = htonl(INADDR_ANY); // set address

	/* Name and activate the socket.
	 * Fill in code. */
	if(bind(sd, (struct sockaddr *)&server, sizeof(server)) < 0) {
		DIE("bind");
	}
	if(listen(sd, 5) < 0) {
		DIE("listen");
	}

	/* main loop : accept connection; fork a child to have dialogue */
	for (;;) {

		/* Wait for a connection.
		 * Fill in code. */
		if((cd = accept(sd, NULL, NULL)) < 0) {
			DIE("accept");
		}

		/* Handle new client in a subprocess. */
		switch (fork()) {
			case -1 : 
				DIE("fork");

			case 0 :
				close (sd);	/* Rendezvous socket is for parent only. */
				/* Get next request.
				 * Fill in code. */
				while ((n = read(cd, try_it.word, WORD)) > 0) {
					/* Lookup the word , handling the different cases appropriately */
					switch(lookup(&try_it,argv[1]) ) {
						/* Write response back to the client. */
						case FOUND:
							/* Fill in code. */
							if(write(cd, try_it.text, strlen(try_it.text)+1) < 0) {
								DIE("write");
							}
							break;
						case NOTFOUND:
							/* Fill in code. */
							if(write(cd, "XXXX", strlen("XXXX")+1) < 0) {
								DIE("write");
							}
							break;
						case UNAVAIL:
							DIE(argv[1]);
					} /* end lookup switch */
				} /* end of client dialog */
				exit(0); /* child does not want to be a parent */

			default :
				close(cd);
				break;
		} /* end fork switch */
	} /* end forever loop */
} /* end main */
