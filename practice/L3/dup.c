//這支程式是在做 redirect，將原本的 stdout 寫入一個檔案裡

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

main() {
    int fd;
    fd = open("/tmp/foo", O_CREAT|O_WRONLY|O_TRUNC, 0660);
    
    if( fd == -1 ) {
    perror("open");
    exit(1);
    }
    close(stdout); //fd = 1被釋放掉

    /* If succeeds fd 1 is "/tmp/foo" */
    if( dup(fd) != stdout ) {
        fprintf(stderr,"dup failed to return 1!\n");
        exit(1);
    }

    /* Don't need fd any more */
    close(fd);

    /* stdout output goes to "/tmp/foo" */
    printf("Hi Folks!\n");
}