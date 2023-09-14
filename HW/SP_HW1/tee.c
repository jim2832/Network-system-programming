/*
The tee command reads its standard input until end-of-file,
writing a copy of the input to standard output and to the file named in its command-line argument.
(We show an example of the use of this command when we discuss FIFOs in Section 44.7.)
Implement tee using I/O system calls. By default, tee overwrites any existing file with the given name.
Implement the –a command-line option (tee –a file),
which causes tee to append text to the end of a file if it already exists.
(Refer to Appendix B for a description of the getopt() function, which can be used to parse command-line options.)
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
    printf("Usage: ./tee [-a] file\n");
}

int main(int argc, char *argv[]){
    int opt;
    int append = 0; // 0 -> false

    // using opt to parse the command line
    while((opt = getopt(argc, argv, "a")) != -1){
        if(opt == 'a'){
            append = 1;
            break;
        }
        else{
            print_usage();
            exit(EXIT_FAILURE);
        }
    }
    
    // command line arguments error
    if(optind >= argc){
        print_usage();
        exit(EXIT_FAILURE);
    }

    // open func flag and mode
    int flag = O_WRONLY | O_CREAT;
    int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH; /*rw-rw-r--*/

    // append or not
    if(append)
        flag |= O_APPEND;
    else
        flag |= O_TRUNC;

    // file descriptor
    int fd = open(argv[optind], flag, mode);

    // open error
    if(fd == -1){
        perror("open");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    ssize_t read_bytes;

    while((read_bytes = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0){
        // standard output
        if(write(STDOUT_FILENO, buffer, read_bytes) == -1){
            perror("write to stdout");
            exit(EXIT_FAILURE);
        }

        // file
        if(write(fd, buffer, read_bytes) == -1){
            perror("write to file");
            exit(EXIT_FAILURE);
        }
    }

    // close the file
    if(close(fd) == -1)
        perror("close");
        exit(EXIT_FAILURE);

    return 0;
}