#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static inline int __attribute__((pure)) is_numeric(const char *str) {
    char *endptr = NULL;
    strtol(str, &endptr, 10);
    return (endptr && !(*endptr));
}

static inline char *gen_subdir(const char *PATH, const char *name) {
    char *subdir = (char *)calloc(strlen(PATH) + strlen(name) + 1, 1);
    memcpy(subdir, PATH, strlen(PATH));
    strcat(subdir, name);
    return subdir;
}

static inline void free_subdir(char *subdir) { free(subdir); }

static void show_fd_link(const char *ps) {
    char filename[256] = {0};
    memcpy(filename, ps, strlen(ps));
    strncat(filename, "/fd", 5);
    DIR *dir = opendir(filename);
    if (!dir) {
        perror(filename);
        return;
    }

    struct dirent *direntp = NULL;
    while (!!(direntp = readdir(dir))) {
        char buf[256] = {0};

        if (!is_numeric(direntp->d_name))
            continue;

        char fd_path[512] = {0};
        snprintf(fd_path, sizeof(fd_path), "%s/%s", filename, direntp->d_name);
        if (readlink(fd_path, buf, 256) != -1){
            printf("%s -> %s\n", fd_path, buf);
        }
    }

    closedir(dir);
}

int main(void) {
    struct dirent *direntp = NULL;
    static const char *PATH = "/proc/";
    DIR *dirp = opendir(PATH);

    while (!!(direntp = readdir(dirp))) {
        const char *name = direntp->d_name;
        if (!is_numeric(name))
            continue; // Skip non-pid*endptr

        char *subdir = gen_subdir(PATH, name);
        show_fd_link(subdir);
        free_subdir(subdir);
    }

    closedir(dirp);
    return 0;
}