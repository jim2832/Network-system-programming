#define _XOPEN_SOURCE 500 // For nftw
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/inotify.h>

#define BUF_LEN (1024 * (sizeof(struct inotify_event) + 16))

static int inotify_fd;

static void handle_event(const char *path, const struct inotify_event *event) {
    if (event->mask & IN_CREATE) {
        printf("File created: %s/%s\n", path, event->name);
    } else if (event->mask & IN_DELETE) {
        printf("File deleted: %s/%s\n", path, event->name);
    } else if (event->mask & IN_MOVED_FROM) {
        printf("File moved from: %s/%s\n", path, event->name);
    } else if (event->mask & IN_MOVED_TO) {
        printf("File moved to: %s/%s\n", path, event->name);
    }
}

static int event_handler(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    // Add the directory to the inotify watch list
    if (typeflag == FTW_D || typeflag == FTW_DP) {
        int wd = inotify_add_watch(inotify_fd, fpath, IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO);
        if (wd == -1) {
            perror("inotify_add_watch");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Set up inotify
    inotify_fd = inotify_init();
    if (inotify_fd == -1) {
        perror("inotify_init");
        exit(EXIT_FAILURE);
    }

    // Set up nftw to traverse the directory tree
    if (nftw(argv[1], event_handler, 20, FTW_PHYS) == -1) {
        perror("nftw");
        exit(EXIT_FAILURE);
    }

    // Monitor inotify events
    char buf[BUF_LEN];
    ssize_t numRead;
    struct inotify_event *event;
    while (1) {
        if (nftw(argv[1], event_handler, 20, FTW_PHYS) == -1) {
            perror("nftw");
            exit(EXIT_FAILURE);
        }

        numRead = read(inotify_fd, buf, BUF_LEN);
        if (numRead == 0) {
            fprintf(stderr, "read() from inotify fd returned 0!");
            exit(EXIT_FAILURE);
        }

        for (char *p = buf; p < buf + numRead;) {
            event = (struct inotify_event *)p;
            handle_event(argv[1], event);
            p += sizeof(struct inotify_event) + event->len;
        }
    }

    // Close inotify
    close(inotify_fd);

    return 0;
}
