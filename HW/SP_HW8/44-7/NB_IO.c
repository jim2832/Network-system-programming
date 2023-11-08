#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main() {
    char *fifo_path = "fifo_file";
    char buffer[1024];
    mkfifo(fifo_path, S_IRUSR | S_IWUSR | S_IWGRP); // Create FIFO file
    int fd = open(fifo_path, O_RDONLY); // Open FIFO file
    if(fd == -1) {
        perror("open");
        exit(1);
    }

    int flags = fcntl(fd, F_GETFL); // Get file flags
    flags |= O_NONBLOCK; // Set non-blocking flag
    fcntl(fd, F_SETFL, flags); // Set file flags

    while(read(fd, buffer, sizeof(buffer)) > 0) {
        printf("%s\n", buffer);
    }

    close(fd);
    return 0;
}