#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <sys/wait.h>

#define SHM_SIZE 1024

typedef struct{
    int seg; // serial number of data
    int data; // data
} shared_mem;

int main(int argc, char *argv[]){
    if(argc != 4){
        printf("Usage: %s <consumer number> <data generation rate> <total data number>\n", argv[0]);
        exit(1);
    }

    int consumer_num = atoi(argv[1]); // number of consumers
    int gen_rate = atoi(argv[2]); // data generation rate
    int total_data_num = atoi(argv[3]); // total number of data

    // Create shared memory
    key_t key = ftok(".", 'x');
    int shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if(shmid < 0){
        perror("shmget");
        exit(1);
    }

    // Attach shared memory
    shared_mem *shm =(shared_mem*) shmat(shmid, NULL, 0);
    if(shm == (shared_mem*) - 1){
        perror("shmat");
        exit(1);
    }

    // Initialize shared memory
    shm->seg = 0;
    shm->data = 0;

    // Fork consumers
    for(int i=0; i<consumer_num; i++){
        pid_t pid = fork();

        // Error handling
        if(pid < 0){
            perror("fork");
            exit(1);
        }

        // Child process (consumer)
        else if(pid == 0){
            int data_count = 0;
            while(data_count < total_data_num){
                if(shm->seg > 0){
                    printf("Consumer %d received data %d with seg %d\n", i, shm->data, shm->seg);
                    shm->seg = 0;
                    data_count++;
                }
            }
            exit(0);
        }
    }

    // Parent process(producer)
    srand(time(NULL)); // seed for random number generator
    int data_count = 0;
    while(data_count < total_data_num){
        if(shm->seg == 0){
            shm->data = rand() % 9 + 1; // generate random number between 1 and 9
            shm->seg = data_count + 1; // set seg to data_count + 1
            printf("Producer generated data %d with seg %d\n", shm->data, shm->seg);
            data_count++;
        }
        usleep(1000000 / gen_rate);
    }

    printf("The total number of data received by consumers: %d\n", data_count);

    // Detach and remove shared memory
    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}