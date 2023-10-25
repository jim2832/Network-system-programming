/*
Write a program that uses two pipes to enable bidirectional communication between a parent and child process.

The parent process should loop reading a block of text from standard input and use one of the pipes to send the text to the child,
which converts it to uppercase and sends it back to the parent via the other pipe.

The parent reads the data coming back from the child and echoes it on standard output before continuing around the loop once more.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h> 

#define BUFFER_SIZE 1024

int main(){
    printf("Enter 'q' to quit.\n\n");

    // pipe descriptors
    int parent2child[2]; // parent -> child
    int child2parent[2]; // child -> parent
    char buffer[BUFFER_SIZE];

    if(pipe(parent2child) == -1){
        fprintf(stderr, "Pipe failed");
        return 1;
    }

    if(pipe(child2parent) == -1){
        fprintf(stderr, "Pipe failed");
        return 1;
    }

    pid_t pid = fork();

    switch(pid){
        // failed
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
            break;
        
        // child process
        case 0:
            // child writes to parent and parent reads from child
            close(parent2child[1]); // close wrtie of parent
            close(child2parent[0]); // close read of child
            
            // loop reading a block of text from standard input
            while(1){
                read(parent2child[0], buffer, BUFFER_SIZE); // read from parent process

                if(!strcmp(buffer, "q\n")) break;

                for(int i=0; i<strlen(buffer); i++){
                    buffer[i] = toupper(buffer[i]); // convert to uppercase
                }

                write(child2parent[1], buffer, strlen(buffer)+1); // write to parent process
            }

            close(parent2child[0]); // close read of parent
            close(child2parent[1]); // close write of child
            break;
        
        // parent process
        default:
            // parent writes to child and child reads from parent
            close(parent2child[0]); // close read of parent
            close(child2parent[1]); // close write of child

            // loop reading a block of text from standard input
            while(1){
                printf("Please enter a string: ");
                fgets(buffer, BUFFER_SIZE, stdin);

                write(parent2child[1], buffer, strlen(buffer)+1); // write to child process

                if(!strcmp(buffer, "q\n")) break;

                read(child2parent[0], buffer, BUFFER_SIZE); // read from child process
                printf("Received from child: %s\n", buffer);
            }

            close(parent2child[1]); // close wrtie of parent
            close(child2parent[0]); // close read of child
            break;
    }

    return 0;
}