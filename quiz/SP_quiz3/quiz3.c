#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <sys/wait.h>

#define STACK_SIZE 3

int *stack;
pthread_mutex_t mutex;

void push(int value){
    pthread_mutex_lock(&mutex); // Lock the mutex
    if(stack[STACK_SIZE - 1] != 0){
        printf("Stack is full. Cannot push %d.\n", value);
    }
    else{
        // Find the first available slot
        for(int i=0; i<STACK_SIZE; i++){
            if(stack[i] == 0){
                stack[i] = value;
                printf("Process %d pushed %d to stack.\n", getpid(), value);
                break;
            }
        }
    }
    pthread_mutex_unlock(&mutex); // Unlock the mutex
}

void pop(){
    pthread_mutex_lock(&mutex); // Lock the mutex
    if(stack[0] == 0){
        printf("Stack is empty. Cannot pop.\n");
    }
    else{
        // Find the topmost element and pop it
        for(int i=STACK_SIZE-1; i>=0; i--){
            if(stack[i] != 0){
                printf("Process %d popped %d from stack.\n", getpid(), stack[i]);
                stack[i] = 0;
                break;
            }
        }
    }
    pthread_mutex_unlock(&mutex); // Unlock the mutex
}

int main(int argc, char *argv[]){
    if(argc != 2){
        fprintf(stderr, "Usage: %s <process number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int num_processes = atoi(argv[1]); // number of processes to create

    // Create shared memory for the stack
    key_t key = ftok("/tmp", 'A');
    int shmid = shmget(key, STACK_SIZE * sizeof(int), IPC_CREAT | 0666);
    if(shmid == -1){
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Attach to shared memory
    stack =(int *)shmat(shmid, NULL, 0);
    if(stack == (int *)-1){
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Initialize the stack
    for(int i=0; i<STACK_SIZE; i++){
        stack[i] = 0;
    }

    // Initialize mutex
    pthread_mutex_init(&mutex, NULL);

    // Fork processes
    for(int i=0; i<num_processes; i++){
        pid_t pid = fork(); // Fork a child process

        // Fork failed
        if(pid < 0){
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }

        // Child process
        else if(pid == 0){
            srand(getpid()); // Seed for random numbers

            // Push operation
            if(rand() % 2 == 0){
                int value = rand() % num_processes + 1;
                push(value);
            }

            // Pop operation
            else{
                pop();
            }

            exit(EXIT_SUCCESS);
        }
        usleep(500000);
    }

    // Wait for all processes to finish
    for(int i=0; i<num_processes; i++){
        wait(NULL);
    }

    // Clean up
    pthread_mutex_destroy(&mutex);
    shmdt(stack);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
