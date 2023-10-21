/*
 * lookup1 : straight linear search through a local file
 * 	         of fixed length records. The file name is passed
 *	         as resource.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dict.h"

int lookup(Dictrec *sought, const char *resource)
{
    if (strlen(sought->word) == 1) {
        return UNAVAIL;
    }

    FILE *res_rec = fopen(resource, "r");

    // Traverse the whole resource file.
    while (!feof(res_rec)) {
        char *buf = NULL;
        size_t __size = 0;
        size_t len = getline(&buf, &__size, res_rec);
        if (len == -1)  // empty line
            continue;
        const char *word = strtok(buf, " ");
        if (!strcmp(word, sought->word)) {  // found
            char *text = buf + strlen(word);
            while (!(*text)) {
                ++text;
            }
            strcpy(sought->text, ++text);
            fclose(res_rec);
            return FOUND;
        }
    }
    fclose(res_rec);
    return NOTFOUND;
}