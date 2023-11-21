/*******************************************************
This program demonstrates the use of an IPC semaphore
in coordinating tasks which require simultaneous
multiple resource allocations, each requesting more
than one of the resource.
Allocate a semaphore to represent the resources of a
moving company with five trucks, 12 human movers,
and an insurance policy of $1 Million to be spread
across all concurrent jobs.
Jobs are done on a first come, first served basis.
A job goes only if there are an adequate number of
movers, trucks and insurance to cover the job,
after all other jobs are accounted for. If the job
is a go, decrement the resource count, so that all
resources in use are accounted for when the next
job is requested.
/**************************************************/

#include <sys/ipc.h>  /* System 5 IPC defs. */
#include <sys/sem.h>  /* System 5 IPC semaphore defs.*/
#include <sys/types.h>/* For general. */
#include <pthread.h>  /* Posix threads. */
#include <stdlib.h>   /* Needed for delay to work properly. */
#include "utils.h"    /* For fractSleep() and printWithTime(). */
#define NUM_THREADS 10 /* Number of simultaneous requests. */
#define TIME_BTWN_NEW_THREADS 0.5 /* Time between intro of new request. */
#define RUNTIME_RANGE 5.0 /* Time of longest job. */

/* These define a single semaphore group consisting of three semaphores. All the semaphores in a
group can be modified together in a single atomic operation. The first semaphore represents the
number of trucks available; the second: the number of movers available; the third: the amount
of insurance in $1000 units available. */

#define NUM_SEMS_IN_GROUP 3
#define TRUCK_SEM 0
#define MOVER_SEM 1
#define INSUR_SEM 2

/* These are used to initialize semaphores with total resources managed. */
#define NUM_TRUCKS 5
#define NUM_MOVERS 12
#define AMT_INSUR 1000

/* Flags passed thru sembuf structure; wait for resource. */
#define WAIT 0
#define STRING_SIZE 80
#define FALSE 0
#define TRUE (!FALSE)

/* Define some jobs here. This table specifies how
many of each resource is requested per each job.
The values in this table cause contention: for
example, the first job uses 4 trucks out of 5
available, making some of the others wait for a
truck. */

struct job {
    int numTrucks;
    int numMovers;
    int amtInsurance;
} jobTable[] = {
    {4, 5, 250},  /* Use many trucks */
    {1, 2, 500},  /* Normal amts. */
    {3, 5, 1000}, /* Lots of insur */
    {2, 8, 250},  /* Many movers. */
};

/* Number of jobs in the table. */
int numJobs = sizeof(jobTable) / sizeof(struct job);
int semid; /* IPC semaphore identifier (semaphore accessed through this).*/
extern int errno;
void *threadMain(void *);

/**************************************************/
main() {
    /**************************************************
    Main function. Allocate and initialize resources,
    then spawn threads.
    /**************************************************/
    /* Array of threads, one per request. */
    pthread_t threads[NUM_THREADS];

    /* Needed to convert constant for semctl call.*/
    int numTrucks = NUM_TRUCKS;
    int numMovers = NUM_MOVERS;
    int amtInsur = AMT_INSUR;
    int count;

    /* Allocate a single semaphore group with three
    semaphores in it. */
    if ((semid = semget(IPC_PRIVATE, NUM_SEMS_IN_GROUP, IPC_CREAT | 0600)) ==
        -1) {
        perror("semget");
        exit(errno);
    }

    /* Initialize each semaphore in the group. Could also have used SETALL. */
    if ((semctl(semid, TRUCK_SEM, SETVAL, &numTrucks)) ||
        (semctl(semid, MOVER_SEM, SETVAL, &numMovers)) ||
        (semctl(semid, INSUR_SEM, SETVAL, &amtInsur))) {
        perror("Error initializing semaphores");
        goto cleanup;
    }

    /* Initialize random number generator used for time delays. */
    srand48(time(NULL));

    /* Spawn the threads. The argument passed to threadMain is a job table index, so multiple
    requests can be made using the same table entry when the index wraps. Delay to model jobs being
    staggered instead of coming all at once. */
    for (count = 0; count < NUM_THREADS; count++) {
        if (pthread_create(&threads[count], NULL, threadMain, (void *)(count % numJobs))) {
            perror("Error starting reader threads");
            goto cleanup;
        }
        fractSleep(TIME_BTWN_NEW_THREADS);
    }

    /* Wait for threads to finish. */
    for (count = 0; count < NUM_THREADS; count++) {
        pthread_join(threads[count], (void **)NULL);
    }

cleanup:

    /* Delete the semaphore. This is not doneautomatically by the system. */
    if (semctl(semid, 0, IPC_RMID, NULL)) {
        perror("semctl IPC_RMID:");
    }
}

/*************************************************/

void *threadMain(void *arg) {

    /*************************************************
    Here is where a thread starts executing. A
    thread in executing this function represents a
    task for the moving company, and requests the
    resources it needs.
    /*************************************************/

    /* The argument passed in is a table index. */
    int jobNum = (int)arg;

    /* Local string for message composition. */
    char string[STRING_SIZE];

    sprintf(string, "Job # %d requesting %d trucks, %d people, % d000 insurance...\n ", jobNum,
            jobTable[jobNum].numTrucks, jobTable[jobNum].numMovers,
            jobTable[jobNum].amtInsurance);

    printWithTime(string);

    /* Get the resources needed. Wait for them if necessary. */
    if (reserve(semid, jobTable[jobNum])) {
        perror("reserve");
        return (NULL);
    }

    sprintf(string, "Job # %d got %d trucks, %d people, %d000 insurance and is running...\n",
            jobNum, jobTable[jobNum].numTrucks, jobTable[jobNum].numMovers, jobTable[jobNum].amtInsurance);
    printWithTime(string);

    /* Delay to simulate the time the resources are in use. */
    fractSleep(drand48() * RUNTIME_RANGE);

    sprintf(string, "Job # %d done; returning %d trucks, %d people, %d000 insurance...\n",
            jobNum, jobTable[jobNum].numTrucks, jobTable[jobNum].numMovers, jobTable[jobNum].amtInsurance);
    printWithTime(string);

    /* Release resources. */
    if (release(semid, jobTable[jobNum])) {

        perror("release");
    }
}

/*************************************************/
int reserve(int semid, struct job thisJob) {
    /************************************************
    Reserve resources required for a job. This wrapper function passes negative values into
    playWithSemaphores(), since negative values represent resource allocations.
    /***************************************************/

    // reserver the resouce by minus the free resource
    return (playWithSemaphores(semid, -thisJob.numTrucks, -thisJob.numMovers, -thisJob.amtInsurance));
}

/*************************************************/
int release(int semid, struct job thisJob) {
    /*************************************************
    Release resources of a job completed. This
    wrapper function passes positive values into
    playWithSemaphores(), since positive values
    represent resource deallocations.
    /***************************************************/

    // release the resource by add the free resource
    return (playWithSemaphores(semid, thisJob.numTrucks, thisJob.numMovers, thisJob.amtInsurance));
}

/*************************************************/
static int playWithSemaphores(int semid, int numTrucks, int numMovers, int amtInsurance) {
    /**************************************************
    This is the workhorse function that allocates deallocates resources from
    the semaphore group.
    /****************************************************/

    /* There is one operation per semaphore for this
    example. This allocates an array of semaphore
    operations, all of which is carried out with a
    single atomic operation (system call to
    semop()).*/

    struct sembuf ops[NUM_SEMS_IN_GROUP];

    /* One operation per semaphore. Note that a
    negative value to ops[x].sem_op allocates a
    resource, while a positive value releases it. */

    ops[0].sem_num = TRUCK_SEM;
    ops[0].sem_op = numTrucks;
    ops[1].sem_num = MOVER_SEM;
    ops[1].sem_op = numMovers;
    ops[2].sem_num = INSUR_SEM;
    ops[2].sem_op = amtInsurance;

    /* All semaphore operations are to be handled in the same way. */
    ops[0].sem_flg = ops[1].sem_flg = ops[2].sem_flg = WAIT;

    /* "The call" that does the work. */
    return (semop(semid, ops, NUM_SEMS_IN_GROUP));
}