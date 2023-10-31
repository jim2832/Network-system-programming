#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int fd;
    caddr_t addr;
    struct stat statbuf;

    // check command line args
    if (argc != 2) {
        fprintf(stderr, "Usage: mycat2 filename\n");
        exit(1);
    }

    // stat() stats the file pointed to by path and fills in buf.
    if (stat(argv[1], &statbuf) == -1) {
        perror("stat");
        exit(1);
    }

    // open() returns a file descriptor for the named file that is the lowest file descriptor not currently open for that process.
    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    // mmap() creates a new mapping in the virtual address space of the calling process.
    addr = mmap((caddr_t)NULL, statbuf.st_size, PROT_READ, MAP_SHARED, fd, (off_t)0);

    if (addr == MAP_FAILED) {
            perror("mmap");
            exit(1);
    }

    /* no longer need fd */
    close(fd);

    // write() writes up to count bytes from the buffer pointed buf to the file referred to by the file descriptor fd.
    write(1, addr, statbuf.st_size);

    return(0);
}