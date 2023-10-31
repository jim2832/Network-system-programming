/* pos_shm_play : set up an area of shared memory,
or attach to an existing one. Simply report what
is there, and write the process id. Wait a random
period and repeat. */

#define _POSIX_SHARED_MEMORY_OBJECTS
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#define DIE(x) perror(x), exit(1)
#define MEMSIZ 256

int main(int argc, char **argv) {
    int shmid, delay;
    char *path = "/SHM_DEMO";
    char *shm;

    /* Delete segment if -u on commandline. */
    if (argc == 2 && strcmp(argv[1], "-u") == 0) {
        if (shm_unlink(path) == -1)
            DIE("Failed to unlink shared memory");
        exit(0);
    }

    /* Create or attach segment. */
    if ((shmid = shm_open(path, O_RDWR)) == -1) {
        printf("Creating new shared memory segment. \n");
        if ((shmid = shm_open(path, O_RDWR | O_CREAT, 0666)) == -1)
            DIE("Failed to create shm");
        if (ftruncate(shmid, MEMSIZ) == -1)
            DIE("Setting the size failed");
    }
    
    /* Attach to the segment to get a pointer to it. */
    if ((shm = mmap(NULL, MEMSIZ, PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0)) == MAP_FAILED)
        DIE("mmap failed");

    /* Write the process id into the segment. */
    srand(getpid());

    /* Wait a random period and repeat. */
    delay = rand() % 5;

    /* Write the process id into the segment. */
    for (;;) {
        printf("Shared memory contains : %s\n", shm);
        sprintf(shm, "Process %d was here\n", getpid());
        sleep(delay);
    }
}