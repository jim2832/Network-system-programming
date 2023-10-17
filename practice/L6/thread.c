#include <pthread.h>
#define NUM_THREADS 5
#define SLEEP_TIME 10

pthread_t tid[NUM_THREADS]; /* array of thread IDs */

void *sleeping(void *); /* thread routine */

void start() {
    int i;
    for(i=0; i<NUM_THREADS; i++){
        pthread_create(&tid[i], NULL, sleeping, (void *)SLEEP_TIME);
    }
}