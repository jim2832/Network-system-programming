#include <sys/ipc.h>  /* System 5 IPC defs. */
#include <sys/sem.h>  /* System 5 IPC semaphore defs.*/
#include <sys/types.h>/* For general. */
#include <pthread.h>  /* Posix threads. */
#include <stdlib.h>   /* Needed for delay to work properly. */
#include "utils.h"    /* For fractSleep() and printWithTime(). */

/* Shared data is created in this mmapped file. */
#define SHARED_DATA_FILE "/tmp/proc_mutex.dat"

/* Global stuff. */
typedef struct globalStuff {
    pthread_mutex_t sharedMutex;
    int count;
} globalStuff;

/* Handle to shared area. */
struct globalStuff *globalArea;

/****************************************************/
void cleanup(int x) {
    /*****************************************************
    Cleanup handler, called as a signal handler for ^C.
    Unlock the mutex before terminating.
    /****************************************************/
    printf("Cleaning up...\n");
    pthread_mutex_unlock(&globalArea->sharedMutex);
    exit(0);
}

/* For interprocess mutex init. */
pthread_mutexattr_t mutex_attributes;

/* True if we set up shared area. */
boolean_t createdHere = B_FALSE;

/* Open the shared area. Err out if it alread exists (which means we are
not the first process to use it, and thus not to be the creator if it. */
shared_fd = open(SHARED_DATA_FILE, (O_RDWR | O_CREAT | O_EXCL), 0777);

/* Flag that we are the creator if we are, or just open the file otherwise. */
if ((shared_fd == -1) && (errno == EEXIST)) {
    shared_fd = open(SHARED_DATA_FILE, O_RDWR, 0777);
}
else {
    createdHere = B_TRUE;
}

/* Some other error opening the file. */
if (shared_fd < 0) {
    perror("Error opening shared mutex area");
    exit(errno);
}

/* Expand the file to hold the shared data, before mmapping it. */
if (createdHere) {
    if (ftruncate(shared_fd, sizeof(struct globalStuff))) {
        perror("ftruncate");
        exit(errno);
    }
}

/* Mmap the file so all processes needing access to it can see it. */
globalArea = (struct globalStuff *)mmap(NULL, sizeof(struct globalStuff), PROT_READ | PROT_WRITE, MAP_SHARED, shared_fd, 0);

if (globalArea == (struct globalStuff *)-1) {
    perror ("Error getting shared Mutex virtual addr");
    exit (errno);
}

/* Set up cleanup handler for ^C. */
signal(SIGINT, cleanup);

/* Initialize mutex in shared region if we are the shared region creator. */
if (createdHere) {

    /* Initialize mutex attributes list. */
    if (pthread_mutexattr_init(&mutex_attributes)) {
        perror("pthread_mutexattr_init");
        exit(errno);
    }

    /* Reflect in the attributes that this is an interprocess shared mutex. */
    if (pthread_mutexattr_setpshared(&mutex_attributes,
                                     PTHREAD_PROCESS_SHARED)) {
        perror("pthread_mutexattr_setpshared");
        exit(errno);
    }

    /* Initialize the mutex in the mmapped area, with the attributes above. */
    if (pthread_mutex_init(&globalArea - > sharedMutex, &mutex_attributes)) {
        perror("pthread_mutex_init");
        exit(errno);
    }
}