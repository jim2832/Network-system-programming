#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    char *fifo_path = "fifo_file";
    char buffer[1024];
    int fd = open(fifo_path, O_RDONLY | O_NONBLOCK);
    if(fd == -1) {
        perror("open");
        exit(1);
    }

    while(read(fd, buffer, sizeof(buffer)) > 0) {
        printf("%s\n", buffer);
    }

    close(fd);
    return 0;
}