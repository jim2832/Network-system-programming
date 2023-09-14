/*
Write a program like cp that,
when used to copy a regular file that contains holes(sequences of null bytes),
also creates corresponding holes in the target file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUFFER_SIZE 2048

void print_usage(){
    printf("Error: Invalid command\n");
    printf("Usage: ./cp source_file target_file\n");
}

int main(int argc, char *argv[]){
    if(argc != 3){
        print_usage();
        exit(EXIT_FAILURE);
    }

    /* get source and target file from command line arguments */
    char *source_file = argv[1];
    char *target_file = argv[2];

    int source_fd, target_fd; /* file descriptor*/

    /* open source file */
    source_fd = open(source_file, O_RDONLY);
    
    if(source_fd == -1){
        perror("source open");
        exit(EXIT_FAILURE);
    }

    /* open target file */
    int target_flag = O_WRONLY | O_CREAT | O_TRUNC;
    int target_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; /*rw-rw-rw-*/
    target_fd = open(target_file, target_flag, target_mode);
    
    if(target_fd == -1){
        perror("target open");
        exit(EXIT_FAILURE);
    }

    /* read from source file and write into target file */
    ssize_t read_bytes;
    char buffer[BUFFER_SIZE];
    
    while((read_bytes = read(source_fd, buffer, BUFFER_SIZE)) > 0){
        if(write(target_fd, buffer, read_bytes) == -1){
            perror("write");
            close(source_fd);
            close(target_fd);
            exit(EXIT_FAILURE);
        }
    }

    /* close all files */
    if(close(source_fd) == -1 || close(target_fd) == -1){
        perror("close");
        exit(EXIT_FAILURE);
    }

    return 0;
}