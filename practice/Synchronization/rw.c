/************************************************
This the main for the writer thread. Get the
appropriate lock, do the read or write, and then
release the lock. Note that trying to get either
lock can force the thread to block if the lock is
unavailable.
/*************************************************/

void *writerMain(void *arg) {

    int loopCount;

    for (loopCount = 0; loopCount < LOOP_MAX; loopCount++) {
        /* Get either the rwlock or the mutex, depending on the mode. */
        if (userwlock) {
            pthread_rwlock_wrlock(&rwlock);
        }
        else {
            pthread_mutex_lock(&mutex);
        }

        doWrite();

        /* Release the lock. */

        if (userwlock) {
            pthread_rwlock_unlock(&rwlock);
        }
        else {
            pthread_mutex_unlock(&mutex);
        }
    }
}

/************************************************
This is the main for the reader threads. Get the
locks as for the writer threads, except specify
that this is a reader and not a writer when getting
the multiple reader single writer lock.
/***************************************************/

void *readerMain(void *idArg) {
    int id = (int)idArg; /* Arg is thread ID number. */
    int loopCount;

    for (loopCount = 0; loopCount < LOOP_MAX; loopCount++) {
        /* Get either the rwlock or the mutex, depending on the mode. */
        if (userwlock) {
            pthread_rwlock_rdlock(&rwlock);
        }
        else {
            pthread_mutex_lock(&mutex);
        }

        doRead();

        /* Release the lock. */
        if (userwlock) {
            pthread_rwlock_unlock(&rwlock);
        }
        else {
            pthread_mutex_unlock(&mutex);
        }
    }
}