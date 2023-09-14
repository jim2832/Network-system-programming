// An implementation of the `tee` command
// Author: SCC

#include <stdio.h>
#include <string.h>
#define APPEND 1
#define OVERWRITE 0

FILE *direction(const char *filename, int option)
{
    const char *__OPT__[2] = {"w+", "a+"};
    FILE *f = fopen(filename, __OPT__[option]);

#ifdef __DEBUG__
    printf("filename: %s, opt: %d, FILE HEX: %p\n", filename, option, f);
#endif
    return f;
}

int main(int argc, char **argv)
{
    FILE *sec_out = NULL;
    if (argc == 3 && !strcmp(argv[1], "-a"))  // Append option
        sec_out = direction(argv[2], APPEND);
    else if (argc == 2)
        sec_out = direction(argv[1], OVERWRITE);

    int c;
    while ((c = getc(stdin)) != EOF) {
        putc(c, stdout);
        if (sec_out)
            putc(c, sec_out);
    }

    if (sec_out)
        fclose(sec_out);

    return 0;
}