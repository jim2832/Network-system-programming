/* This is the semaphore which regulates the bank
tellers. It operates like a mutex but can manage
more than one resource. */

sem_t bankLine;
pthread_mutex_t semMutex = THREAD_MUTEX_INITIALIZER;
pthread_cond_t semCond = PTHREAD_COND_INITIALIZER;

/* Initialize semaphore to manage NUM_TELLERS
quantity of a resource. Set up so that only
threads of this process can use it. */
if (sem_init(&bankLine, THREADS_OF_THIS_PROCESS, NUM_TELLERS)) {
    perror("Error initializing semaphore");
    exit(errno);
}

/* This is one of those rushed clients. */
if (inaHurry) {
    /* See if a teller is available. If not,
    sem_trywait returns EAGAIN. */
    if ((sem_trywait(&bankLine) == -1) && (errno == EAGAIN)) {
        /* If get here, there is a teller available right away. Drop thru and do business. */
    }
    else {

#if REPORT_NUM_SEM_FREE

        sem_getvalue(&bankLine, &availableTellers);
        pthread_mutex_unlock(&semMutex);
        sprintf(string, "Client %d is in a hurry and got a teller.Count is now %d\n ", whoAmI, availableTellers);

        printWithTime(string);
#else
        sprintf(string, "Client %d%c is in a hurry and got a eller.\n", whoAmI);
        printWithTime(string);

#endif
    }

    /* Not in a hurry. */

}

else {

    /* Wait for next available teller. */

#if REPORT_NUM_SEM_FREE
    /* Calling sem_trywait() in a loop until we get
    the lock, and sleeping on the condition
    variable if the lock is unavailable is
    necessary to prevent deadlock, since we have
    the mutex. */

    while ((sem_trywait(&bankLine) == -1) && (errno == EAGAIN)) {

        pthread_cond_wait(&semCond, &semMutex);
    }

    sem_getvalue(&bankLine, &availableTellers);

    sprintf(string, "Client %d got a teller. Count is now %d\n ",whoAmI, availableTellers);
    printWithTime(string);
    pthread_mutex_unlock(&semMutex);
    
#else

    sem_wait(&bankLine);

    sprintf(string, "Client %d got a teller.\n", whoAmI);

    printWithTime(string);

#endif
}

/* We've got a teller. Do our business. */

doBusiness(whoAmI);

#if REPORT_NUM_SEM_FREE

pthread_mutex_lock(&semMutex);

/* Done doing business. Teller is available. */

sem_post(&bankLine);

/* Get value of the semaphore to report back as a
diagnostic. */
sem_getvalue(&bankLine, &availableTellers);
pthread_cond_signal(&semCond);

pthread_mutex_unlock(&semMutex);

sprintf (string,

"Client %d done doing business. Available”

“teller count: %d\n", whoAmI, availableTellers);

printWithTime(string);
#else

/* Done doing business. Teller is available. */

sem_post(&bankLine);