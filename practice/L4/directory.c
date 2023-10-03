#include <sys/stat.h>
#include <stdio.h>

int main(int argc, char *argv[]){
    struct stat buf;
    if (stat(argv[1], &buf) != -1){
        if(S_ISDIR(buf.st_mode)){
            printf("%s is a directory.\n", argv[1]);
        }
    }

    return 0;
}