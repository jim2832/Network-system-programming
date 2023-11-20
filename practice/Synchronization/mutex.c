/****************************************************
This program demonstrates the use of a POSIX thread
mutex to coordinate access to a shared resource.
Two threads are created, both of which access a
common counter. Delays are used to cause contention to
prove that the mutexes really work.
This program operates in two modes. With no
commandline arguments, the program operates without
synchronization and the threads fall all over
themselves. With argument “sync” passed on the
commandline, synchronization is used and the
program operates correctly.
/**************************************************/

#include "utils.h"    /* for fractSleep() and printWithTime(). */
#include <errno.h>    /* Error definitions. */
#include <pthread.h>  /* Posix threads. */
#include <stdio.h>    /* Standard IO Library. */
#include <sys/types.h>/* For general. */
#define LOOP_MAX 20     /* Each thread counts up this many iterations.*/

/* Global stuff. */

/* Common counter incremented by all threads. */
int commonCounter = 0;

/* mutex lock. Note the initializer can be used onlyin this form of declaration.*/
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/* Delay to cause contention. */
double delay = 0.2;

/* Synchronized flag. */
boolean_t synchronized = B_FALSE;

/* Forward references. */
void *threadMain(void *);

/**************************************************/
main(int argc, char *argv[]) {
    /***************************************************
    Main program. Get commandline arguments and spawn threads.
    /**************************************************/

    /* Define two threads. */
    pthread_t threadID_1;
    pthread_t threadID_2;

    /* We have an argument and it is "sync": run in synchronized mode.*/

    if ((argc > 1) && (!(strcmp(argv[1], "sync")))) {
        synchronized = B_TRUE;
    }

    printf("\n%s running with sync %s\n\n", argv[0], (synchronized) ? "Enabled" : "Disabled");

    /* Create threads. */
    if ((pthread_create(&threadID_1, NULL, threadMain, NULL)) ||
        (pthread_create(&threadID_2, NULL, threadMain, NULL))) {
        perror("Error starting thread(s)");
        exit(errno);
    }
    /* Wait for threads to finish. */
    pthread_join(threadID_1, (void **)NULL);
    pthread_join(threadID_2, (void **)NULL);
}


/**************************************************/
void *threadMain(void *dummy) {
    /**************************************************
    This is the main for each thread. Get counter
    value from global, hold it for a while to cause
    contention, then replace it with an incremented
    value. While it is held, another thread tries to get it and increment it as
    well.

    When running in synchronized mode, get the mutex
    lock before accessing the global, and release it
    only after we are done with the global. This causes
    other threads to wait for the global until we are
    finished with it.
    /**************************************************/

    int loopCount;   /* Loop counter. */
    int temp;        /* Used in modification delay. */
    char buffer[80]; /* For writing status. */

    for (loopCount = 0; loopCount < LOOP_MAX; loopCount++) {

        /* Get mutex lock if running in synchronized mode. */

        if (synchronized) {
            pthread_mutex_lock(&mutex);
        }

        /* Now that we have the lock, we can safely
        increment the counter without worry that the
        other thread accesses it at the same time. */

        /* Print out initial value. */
        sprintf(buffer, "Common counter incremented from %d to ", commonCounter);
        write(STDOUT_FILENO, buffer, strlen(buffer));

        /* Modify common counter. Take a long time to do it to cause contention. */
        temp = commonCounter;
        fractSleep(delay);
        commonCounter = temp + 1;

        /* Update status output. */
        sprintf(buffer, "%d\n", commonCounter);
        write(STDOUT_FILENO, buffer, strlen(buffer));

        /* Release mutex lock if running in synchronized mode. */
        if (synchronized) {
            pthread_mutex_unlock(&mutex);
        }

        /* Delay for half as long as the incrementdelay, to cause contention. */
        fractSleep(delay / 2.0);
    }
}


/*********************************************/
void fractSleep(float time) {
    /*********************************************
    Routine to delay for fractions of a second.
    *********************************************/

    static struct timespec timeSpec;

    /* Truncate fraction in int conversion. */
    timeSpec.tv_sec = (time_t)time;

    /* Load fraction. */
    timeSpec.tv_nsec = (int)((time - timeSpec.tv_sec) * 1000000000);

    /* Delay. */
    nanosleep(&timeSpec, NULL);
}