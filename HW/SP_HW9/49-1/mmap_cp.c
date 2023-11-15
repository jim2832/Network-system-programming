#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

void cp(const char* source, const char* target){
    int source_fd, target_fd; // file descriptors
    char *source_addr, *target_addr; // file addresses
    struct stat sb; // file status
    
    // open the source file
    source_fd = open(source, O_RDONLY);
    if(source_fd == -1){
        perror("open");
        exit(EXIT_FAILURE);
    }

    // get the size of the source file
    if(fstat(source_fd, &sb) == -1){
        perror("fstat");
        exit(EXIT_FAILURE);
    }

    // open the target file
    target_fd = open(target, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if(target_fd == -1){
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Truncate the destination file to the size of the source file
    if(ftruncate(target_fd, sb.st_size) == -1){
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    // mmap the source file
    source_addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, source_fd, 0);
    if(source_addr == MAP_FAILED){
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // mmap the target file
    target_addr = mmap(NULL, sb.st_size, PROT_WRITE, MAP_SHARED, target_fd, 0);
    if(target_addr == MAP_FAILED){
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // copy the source file to the target file
    for(int i = 0; i < sb.st_size; i++){
        target_addr[i] = source_addr[i];
    }

    // unmap the source file
    if(munmap(source_addr, sb.st_size) == -1){
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    // unmap the target file
    if(munmap(target_addr, sb.st_size) == -1){
        perror("munmap");
        exit(EXIT_FAILURE);
    }

    // close the source file
    if(close(source_fd) == -1){
        perror("close");
        exit(EXIT_FAILURE);
    }

    // close the target file
    if(close(target_fd) == -1){
        perror("close");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]){
    if(argc != 3){
        fprintf(stderr, "Usage: %s <source> <target>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *source_path = argv[1];
    char *target_path = argv[2];

    cp(source_path, target_path);

    return 0;
}