/*
 * convert.c : take a file in the form 
 *  word1
 *  multiline definition of word1
 *  stretching over several lines, 
 * followed by a blank line
 * word2....etc
 * convert into a file of fixed-length records
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "dict.h"
#define BIGLINE 512

int main(int argc, char **argv){
	FILE *in;
	FILE *out;        /* defaults */
	char line[BIGLINE];
	static Dictrec dr, blank;
	
	/* If args are supplied, argv[1] is for input, argv[2] for output */
	if(argc==3){
		if((in =fopen(argv[1],"r")) == NULL){
			DIE(argv[1]);
		}
		if((out =fopen(argv[2],"w")) == NULL){
			DIE(argv[2]);
		}	
	}
	else{
		printf("Usage: convert [input file] [output file].\n");
		return -1;
	}

	// init
    memset(&blank, 0, sizeof(Dictrec));

	/* Main reading loop : read word first, then definition into dr */

	/* Loop through the whole file. */
	while(1){
		
		/* Create and fill in a new blank record.
		 * First get a word and put it in the word field, then get the definition
		 * and put it in the text field at the right offset.  Pad the unused chars
		 * in both fields with nulls.
		 */

		/* Read word and put in record.  Truncate at the end of the "word" field.
		 *
		 * Fill in code. */
		
		// EOF
        if(fgets(dr.word, WORD, in) == NULL) break;
        dr.word[strlen(dr.word) - 1] = '\0';

		/* Read definition, line by line, and put in record.
		 *
		 * Fill in code. */
        size_t text_len = 0;
        dr.text[0] = '\0';

        while(fgets(line, BIGLINE, in) != NULL){
            if(line[0] == '\n') break;
            strncat(dr.text, line, TEXT-text_len);
            text_len = strlen(dr.text);
        }

		/* Write record out to file.
		 *
		 * Fill in code. */
		fwrite(&dr, sizeof(Dictrec), 1, out);
	}

	fclose(in);
	fclose(out);
	return 0;
}
