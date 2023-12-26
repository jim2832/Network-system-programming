/*
Implement popen() and pclose().

Although these functions are simplified by not requiring the signal handling employed in the implementation of system() (Section 27.7),
you will need to be careful to correctly bind the pipe ends to file streams in each process,
and to ensure that all unused descriptors referring to the pipe ends are closed.

Since children created by multiple calls to popen() may be running at one time,
you will need to maintain a data structure that associates the file stream pointers allocated by popen() with the corresponding child process IDs.
(If using an array for this purpose, the value returned by the fileno() function,
which obtains the file descriptor corresponding to a file stream, can be used to index the array.)

Obtaining the correct process ID from this structure will allow pclose() to select the child upon which to wait.

This structure will also assist with the SUSv3 requirement that any still-open file streams created by earlier calls to popen() must be closed in the new child process.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


// customized structure to store the file stream and the pid of the child process
struct Process{
    FILE* file;
    pid_t pid;
};


// popen function
struct Process my_popen(char* command, char* type){
    int pipe_fd[2];
    FILE* file;

    if(pipe(pipe_fd) == -1){
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // create child process
    pid_t pid = fork();

    switch (pid){
        // failed fork
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
            break;

        // child process
        case 0:
            if(!strcmp(type, "r")){
                close(pipe_fd[0]); // close read
                dup2(pipe_fd[1], STDOUT_FILENO); // redirect stdout to the pipe
            }
            else if(!strcmp(type, "w")){
                close(pipe_fd[1]); // close the write end of the pipe
                dup2(pipe_fd[0], STDIN_FILENO); // redirect stdin from the pipe
            }
            
            execl("/bin/sh", "sh", "-c", command, NULL); // execute the command
            perror("execl");
            exit(EXIT_FAILURE);
            break;
        
        // parent process
        default:
            if(!strcmp(type, "r")){
                close(pipe_fd[1]); // close write
                file = fdopen(pipe_fd[0], "r");
            }
            else if(!strcmp(type, "w")){
                close(pipe_fd[0]); // close read
                file = fdopen(pipe_fd[1], "w");
            }
            break;
    }

    struct Process child = {file, pid};
    
    return child;
}


// pclose function
int my_pclose(struct Process* child){
    int status;
    fclose(child->file); // Close the file stream

    if(waitpid(child->pid, &status, 0) == -1){
        perror("waitpid");
        return -1;
    }

    return status;
}


int main(){
    // Test my_popen with read mode
    struct Process child = my_popen("ls -l", "r");

    if (child.file == NULL) {
        perror("my_popen");
        exit(EXIT_FAILURE);
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), child.file)) {
        printf("Output of child process: %s", buffer);
    }

    int status = my_pclose(&child);
    if (status == -1) {
        perror("my_pclose");
        exit(EXIT_FAILURE);
    }

    printf("Child process exited with status %d\n", status);

    // Test my_popen with write mode
    struct Process writeChild = my_popen("cat > output.txt", "w");
    if (writeChild.file == NULL) {
        perror("my_popen");
        exit(EXIT_FAILURE);
    }

    const char* message = "test message 1226";
    fwrite(message, sizeof(char), strlen(message), writeChild.file);

    status = my_pclose(&writeChild);
    if (status == -1) {
        perror("my_pclose");
        exit(EXIT_FAILURE);
    }

    printf("Child process exited with status %d\n", status);

    return 0;
}