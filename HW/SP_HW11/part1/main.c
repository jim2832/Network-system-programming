/*
 * The same main will do to test all the versions  of  lookup.   The
 * argument  to  main  is  always  passed  as the second argument to
 * lookup. It identifies the resource needed by lookup to  find  its
 * answers.  It  might be a file name or a named pipe, or the key to
 * some shared memory or a message queue, etc. The point is,  it  is
 * just  a  string;  main does not need to understand how the string
 * will be used. The work involved in each  exercise  is  to  define
 * lookup in different ways. What is the same throughout is that the
 * FIRST time lookup is called, it may have to open some resource. A
 * static internal flag is used to determine whether or not this was
 * the first call.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dict.h"

int main(int argc, char **argv) {
	FILE *in;
	char word[WORD];
	Dictrec try_it;

	if (argc != 2) {
		fprintf(stderr,"Usage : %s <resource>\n",argv[0]);
		exit(errno);
	}

	while(fputs("What word do you want : ",stderr),fgets(try_it.word, WORD, stdin)) {
		try_it.word[strlen(try_it.word)-1] = '\0'; // remove "\n"
		switch(lookup(&try_it, argv[1]) ) {
			case FOUND:
				printf("%s : %s",try_it.word, try_it.text);
				break;
			case NOTFOUND:
				printf("%s : Not Found!\n", try_it.word);
				break;
			case UNAVAIL:
				DIE(argv[1]);
		}
	}
}