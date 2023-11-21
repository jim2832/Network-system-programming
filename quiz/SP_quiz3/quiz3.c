#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include "semun.h"
#include "svshm_xfr.h"

#define STACK_SIZE 3

typedef struct{
    int stack[STACK_SIZE];
    int top;
} Stack;

void push(Stack* stack, int value){
    stack->stack[stack->top] = value;
    stack->top++;
}

int pop(Stack* stack){
    stack->top--;
    return stack->stack[stack->top];
}

int main(int argc, char* argv[]){
    if(argc != 2){
        printf("Usage: %s <process_count>\n", argv[0]);
        return 1;
    }

    int process_count = atoi(argv[1]); // number of processes to create

    // // semaphores
    // int semid = semget(SEM_KEY, 2, IPC_CREAT | OBJ_PERMS);
    // if (semid == -1){
    //     perror("semget");
    //     return 1;
    // }

    // // initialize semaphore
    // if (initSemAvailable(semid, WRITE_SEM) == -1){
    //     perror("initSemAvailable");
    //     return 1;
    // }
    // if (initSemInUse(semid, READ_SEM) == -1){
    //     perror("initSemInUse");
    //     return 1;
    // }

    // Create shared memory for the stack
    key_t key = ftok("/tmp", 'A');
    int shmid = shmget(key, sizeof(Stack), IPC_CREAT | 0666);
    if(shmid == -1){
        perror("shmget");
        return 1;
    }

    // Attach to shared memory
    Stack* stack = (Stack*)shmat(shmid, NULL, 0);
    if(stack == (Stack*)-1){
        perror("shmat");
        return 1;
    }

    // Initialize stack
    stack->top = 0;

    // Fork child processes
    for(int i=0; i<process_count; i++){
        pid_t pid = fork();

        // fork error
        if(pid == -1){
            perror("fork");
            return 1;
        }
        
        // child process
        else if(pid == 0){
            srand(getpid());

            // Push or pop from the stack
            int operation = rand() % 2;

            // Push
            if(operation == 0){
                int value = rand() % 100;

                // // Wait for our turn
                // if(reserveSem(semid, WRITE_SEM) == -1){
                //     perror("reserveSem");
                //     return 1;
                // }

                if(stack->top < STACK_SIZE){
                    printf("Child %d pushing %d\n", getpid(), value);
                    push(stack, value);
                }
                else{
                    printf("Child %d can't push because stack is full\n", getpid());
                }

                // // Give popper a turn
                // if(releaseSem(semid, READ_SEM) == -1){
                //     perror("releaseSem");
                //     return 1;
                // }
            }

            // Pop
            else{
                // // Wait for our turn
                // if(reserveSem(semid, READ_SEM) == -1){
                //     perror("reserveSem");
                //     return 1;
                // }

                if(stack->top > 0){
                    int value = pop(stack);
                    printf("Child %d popping %d\n", getpid(), value);
                }
                else{
                    printf("Child %d can't pop because stack is empty\n", getpid());
                }

                // // Give pusher a turn
                // if(releaseSem(semid, WRITE_SEM) == -1){
                //     perror("releaseSem");
                //     return 1;
                // }
            }

            return 0;
        }
    }

    // Wait for all child processes to finish
    for(int i = 0; i < process_count; i++){
        wait(NULL);
    }

    // Detach and remove shared memory
    shmdt(stack);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
