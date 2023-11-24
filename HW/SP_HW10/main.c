#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/shm.h>
#include <math.h>
#include "producer.h"
#include "consumer.h"
#include "shared_memory.h"
#define _GUN_SOURCE

int calculate_lost(int, int, int);

int main(int argc, char *argv[]){
    // Check arguments
    if(argc != 5){
        fprintf(stderr, "Usage: %s <data number> <transmission rate> <consumer number> <buffer size>\num_consumer", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Get arguments
    int num_data = atoi(argv[1]); // number of data
    int rate = atoi(argv[2]); // transmission rate
    int num_consumer = atoi(argv[3]); // number of consumers
    int buffer_size = atoi(argv[4]); // buffer size

    // create shared memory (system v)
    key_t shm_key = ftok(".", 'a');

    int shmid;
    if((shmid = shmget(shm_key, (buffer_size+1) * sizeof(Shared_memory), IPC_CREAT | 0666)) < 0){ // +1 for producer
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // attach shared memory
    Shared_memory *shm = (Shared_memory*)shmat(shmid, NULL, 0);
    if(shm == (Shared_memory*)-1){
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // initialize shared memory
    memset(shm, 0, (buffer_size+1) * sizeof(Shared_memory));

    // create consumers
    int temp = num_consumer;
    while(temp--){
        pid_t pid = fork();

        // fork error
        if(pid == -1){
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if(pid == 0){
            // create consumer
            struct Consumer consumer;
            consumer.ID = temp;
            consumer.num_data = num_data;
            consumer.buffer_size = buffer_size;
            consumer.buf = shm;
            consumer.job = receive_data;

            // receive data from shared memory
            int fail = consumer.job(&consumer);
            union sigval value;
            value.sival_int = fail;
            sigqueue(getppid(), SIGUSR1, value); // send signal to parent process

            exit(EXIT_SUCCESS);
        }
    }

    // create producer
    pid_t pid = fork();
    if(pid == -1){
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if(pid == 0){
        // create producer
        struct Producer producer;
        producer.num_data = num_data;
        producer.delay = rate;
        producer.buffer_size = buffer_size;
        producer.buf = shm;
        producer.job = send_data;

        // send data to shared memory
        producer.job(&producer);

        exit(EXIT_SUCCESS);
    }

    // calculate the number of total data and lost data
    int total = num_data * num_consumer;
    int lost = calculate_lost(num_data, rate, num_consumer);
    int received = total - lost;

    // detach shared memory
    if(shmdt(shm) == -1){
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
    // destroy shared memory
    if(shmctl(shmid, IPC_RMID, NULL) == -1){
        perror("shmctl");
        exit(EXIT_FAILURE);
    }

    // print the result
    printf("M = %d  R = %d  N = %d\n", num_data, rate, num_consumer);
    printf("---------------------------------------------\n");
    printf("Total messages: %d\n", total);
    printf("Sum of received messages by all consumers: %d\n", received);
    printf("Loss rate: %.2f%%\n", 100 * (1.0 - (double)received / total));
    printf("---------------------------------------------\n");

    return 0;
}


// calculate the number of lost data    
int calculate_lost(int num_data, int rate, int num_consumer){
    int lost = 0;
    siginfo_t info = {._sifields._rt.si_sigval.sival_int = 0};
    sigset_t set;
    sigemptyset(&set); // initialize set
    sigaddset(&set, SIGUSR1); // add SIGUSR1 to set
    sigprocmask(SIG_BLOCK, &set, NULL); // block SIGUSR1

    const struct timespec timeout = {.tv_sec = log10(num_consumer) * num_data * rate / 1000}; // timeout for sigtimedwait
    for(int i=0; i<num_consumer; i++){
        while(sigtimedwait(&set, &info, &timeout) == -1){
            perror("sigwait");
            goto end;
        }
        lost += info.si_value.sival_int;
    }
end:
    return lost;
}