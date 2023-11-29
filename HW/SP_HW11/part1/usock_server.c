/*
 * usock_server : listen on a Unix socket ; fork ;
 *                child does lookup ; replies down same socket
 * argv[1] is the name of the local datafile
 * PORT is defined in dict.h
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "dict.h"

int main(int argc, char **argv) {
    struct sockaddr_un server;
    int sd,cd,n;
    Dictrec try_it;

    if (argc != 3) {
      fprintf(stderr,"Usage : %s <dictionary source>"
          "<Socket name>\n",argv[0]);
      exit(errno);
    }

    /* Setup socket.
     * Fill in code. */
	server.sun_family = AF_UNIX;
	strcpy(server.sun_path, argv[2]);
    
    /* Initialize address.
     * Fill in code. */
	sd = socket(AF_UNIX, SOCK_STREAM, 0);

    /* Name and activate the socket.
     * Fill in code. */
	if(bind(sd, (struct sockaddr *)&server, sizeof(server)) < 0){
		DIE("bind");
	}
	if(listen(sd, 5) < 0){
		DIE("listen");
	}

    /* main loop : accept connection; fork a child to have dialogue */
    for (;;) {
		/* Wait for a connection.
		 * Fill in code. */
		if((cd = accept(sd, NULL, NULL)) < 0){
			DIE("accept");
		}

		/* Handle new client in a subprocess. */
		switch (fork()) {
			case -1 : 
				DIE("fork");

			/* Child : serve the client. */
			case 0 :
				close (sd);	/* Rendezvous socket is for parent only. */
				/* Get next request.
				 * Fill in code. */
				while(n = read(cd, try_it.word, WORD) > 0) {
					/* Lookup request. */
					switch(lookup(&try_it,argv[1]) ) {
						/* Write response back to client. */
						case FOUND: 
							/* Fill in code. */
							if(write(cd, try_it.text, strlen(try_it.text) + 1) < 0){
								DIE("write");
							}
							break;
						case NOTFOUND: 
							/* Fill in code. */
							if(write(cd, "XXXX", 5) < 0){
								DIE("write");
							}
							break;
						case UNAVAIL:
							DIE(argv[1]);
					} /* end lookup switch */

				} /* end of client dialog */

				/* Terminate child process.  It is done. */
				exit(0);

			/* Parent continues here. */
			default :
				close(cd);
				break;
		} /* end fork switch */
    } /* end forever loop */
} /* end main */