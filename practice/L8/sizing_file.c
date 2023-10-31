#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
int main() {
    int fd;
    int pagesize = sysconf(_SC_PAGESIZE);
    caddr_t addr;

    // sysconf() returns the value of a system variable.
    printf("Page size is %d bytes.\n", pagesize);

    // open() returns a file descriptor for the named file that is the lowest file descriptor not currently open for that process.
    fd = open("mapfile", O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    // ftruncate() causes the regular file named by path or referenced by fd to be truncated to a size of precisely length bytes.
    if (ftruncate(fd, (off_t)(6 * pagesize)) == -1) {
        perror("ftruncate");
        exit(1);
    }

    // system() executes a command specified in command by calling /bin/sh -c command, and returns after the command has been completed.
    system("ls -l mapfile");
    addr = mmap(NULL, 6 * pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (off_t)0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    /* no longer need fd */
    close(fd);

    /* Write into file! */
    (void)strcpy(addr, "Test string.\n");

    /* Display 1 line of mapfile */
    system("head -1 mapfile");

    return (0);
}