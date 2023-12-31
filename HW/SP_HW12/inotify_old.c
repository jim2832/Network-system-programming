/*
Write a program that logs all file creations, deletions, and renames under the
directory named in its command-line argument. The program should monitor
events in all of the subdirectories under the specified directory. To obtain a list of
all of these subdirectories, you will need to make use of nftw() (Section 18.9). When
a new subdirectory is added under the tree or a directory is deleted, the set of
monitored subdirectories should be updated accordingly.
*/

#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <limits.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <ftw.h>
#include <time.h>

#define DIE(x) perror(x), exit(errno)
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

// global variables
int inotify_fd;
int *directories;
int num_directories = 0;


// callback function for nftw
int callback(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){
    if(typeflag == FTW_D){ // if it is a directory
        // add one more element
        directories = realloc(directories, (num_directories + 1) * sizeof(int));
        if(directories == NULL){
            DIE("realloc");
        }

        // add watch
        directories[num_directories] = inotify_add_watch(inotify_fd, fpath, IN_CREATE | IN_DELETE | IN_MOVE);
        if(directories[num_directories] == -1){
            DIE("inotify_add_watch");
        }

        num_directories++;
    }

    return 0;
}


// monitor the events
void monitor(char *path){
    char buffer[BUF_LEN];

    // read the events
    ssize_t numRead = read(inotify_fd, buffer, BUF_LEN);
    if(numRead < 0){
        DIE("read");
    }

    // iterate over the events
    FILE *FileLog = fopen("FileLogger", "a");
    for(char *ptr=buffer; ptr<buffer+numRead; ptr+= EVENT_SIZE + ((struct inotify_event *)ptr)->len){
        // get the event
        struct inotify_event *event = (struct inotify_event *) ptr;

        // get the current time
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        char TimeInfo[64];
        strftime(TimeInfo, sizeof(TimeInfo), "%c", tm); // format: Sun Sep 16 01:03:52 1973

        // get the path of the file
        strcat(path, "/");
        strcat(path, event->name);

        // create event
        if(event->mask & IN_CREATE){
            printf("File created: %s/%s\n", path, event->name);
            fprintf(FileLog, "%s -> File created: %s/%s\n", TimeInfo, path, event->name);
        }

        // delete event
        else if(event->mask & IN_DELETE){
            printf("File deleted: %s/%s\n", path, event->name);
            fprintf(FileLog, "%s -> File deleted: %s/%s\n", TimeInfo, path, event->name);
        }

        // move event
        else if(event->mask & IN_MOVE){
            printf("File moved/renamed: %s/%s\n", path, event->name);
            fprintf(FileLog, "%s -> File moved/renamed: %s/%s\n", TimeInfo, path, event->name);
        }
    }

    // close the file
    if(fclose(FileLog) == EOF){
        DIE("fclose");
    }
}


int main(int argc, char *argv[]){
    // error check
    if(argc != 2){
        printf("Usage: %s <directory>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // create inotify instance
    inotify_fd = inotify_init();
    if(inotify_fd == -1){
        DIE("inotify_init");
    }

    // add watch
    int wd = inotify_add_watch(inotify_fd, argv[1], IN_CREATE | IN_DELETE | IN_MOVE);
    if(wd == -1){
        DIE("inotify_add_watch");
    }

    // add directory to the list
    if(nftw(argv[1], callback, 20, 0) == -1){
        DIE("nftw");
    }

    // monitor the events
    for(;;){
        if(nftw(argv[1], callback, 20, 0) == -1){
            DIE("nftw");
        }
        monitor(argv[1]);
    }

    // close inotify instance
    if(close(inotify_fd) == -1){
        DIE("close");
    }

    return 0;
}