// #include <grp.h>
#include <dirent.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

// Modified from TLPI page 159 by SCC
// Return UID corresponding to 'name', or -1 on error
static uid_t userIdFromName(const char *name)
{
    if (!name || !(*name))
        return -1;

    char *endptr = NULL;
    uid_t u = strtol(name, &endptr, 10);
    if (endptr && *endptr == '\0')
        return u; /* As a convenience to caller */

    /* allow a numeric string */
    struct passwd *pwd = getpwnam(name);
    if (!pwd)
        return -1;
    return pwd->pw_uid;
}

static inline int __attribute__((pure)) is_numeric(const char *str)
{
    char *endptr = NULL;
    strtol(str, &endptr, 10);
    return (endptr && !(*endptr));
}

static inline char *gen_subdir(const char *PATH, const char *name)
{
    char *subdir = (char *) calloc(strlen(PATH) + strlen(name) + 1, 1);
    memcpy(subdir, PATH, strlen(PATH));
    strcat(subdir, name);
    return subdir;
}

static inline void free_subdir(char *subdir)
{
    free(subdir);
}

static void show_name_id(const char *ps, const char *uname)
{
    char filename[256] = {0};
    memcpy(filename, ps, strlen(ps));
    strncat(filename, "/status", 8);
    FILE *f = fopen(filename, "r");

    char *line = NULL;
    size_t len = 0;
    int counter = 0;
    char buf[256] = {0};
    //? getline has a memory leakage issue?
    while (getline(&line, &len, f) != EOF && ++counter < 10) {
        if (counter == 1) {
            memcpy(buf, line, strlen(line));
        } else if (counter == 9) {
            static const char delim[] = " \t\n"; /* SPACE or TAB or NL */
            strtok(line, delim);
            strtok(NULL, delim);
            uid_t uid = atoi(strtok(NULL, delim));

            if (uid == userIdFromName(uname))
                printf("%sUid:\t%d\n", buf, uid);
        }
    }
    fclose(f);
}


int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Error usage!");
        exit(1);
    }

    struct dirent *direntp = NULL;
    static const char *PATH = "/proc/";
    DIR *dirp = opendir(PATH);

    while (!!(direntp = readdir(dirp))) {
        const char *name = direntp->d_name;
        if (!is_numeric(name))
            continue;  // Skip non-pid*endptr

        char *subdir = gen_subdir(PATH, name);
        show_name_id(subdir, argv[1]);
        free_subdir(subdir);
    }

    closedir(dirp);
    return 0;
}