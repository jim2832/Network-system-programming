#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFSIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s source_file destination_file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *source_file = argv[1];
    char *destination_file = argv[2];

    int src_fd, dest_fd;
    char buffer[BUFSIZE];
    ssize_t nread;

    src_fd = open(source_file, O_RDONLY);
    if (src_fd == -1) {
        perror("open source file");
        exit(EXIT_FAILURE);
    }

    dest_fd = open(destination_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (dest_fd == -1) {
        perror("open destination file");
        close(src_fd);
        exit(EXIT_FAILURE);
    }

    while ((nread = read(src_fd, buffer, BUFSIZE)) > 0) {
        ssize_t nwritten = write(dest_fd, buffer, nread);
        if (nwritten == -1) {
            perror("write");
            close(src_fd);
            close(dest_fd);
            exit(EXIT_FAILURE);
        }
    }

    if (nread == -1) {
        perror("read");
        close(src_fd);
        close(dest_fd);
        exit(EXIT_FAILURE);
    }

    close(src_fd);
    close(dest_fd);

    return 0;
}
