#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

void ListProcessWithPath(char *filepath){
    DIR *dir;
    struct dirent *entity;

    /* open /proc/ directory */
    const char *path = "/proc";
    dir = opendir(path);
    if(dir == NULL){
        perror("Fail to open /proc/");
        exit(EXIT_FAILURE);
    }

    while((entity = readdir(dir)) != NULL){
        if(atoi(entity->d_name) != 0){
            char fd_path[1024] = {0};
            sprintf(fd_path, "/proc/%s/fd", entity->d_name);

            DIR *fd_dir = opendir(fd_path);
            if(fd_dir == NULL){
                perror("Fail to open /proc/PID/fd/");
                closedir(dir);
                exit(EXIT_FAILURE);
            }

            struct dirent *fd_entity;
            while((fd_entity = readdir(fd_dir)) != NULL){
                char Target[1024] = {0};

                char Source[2048];
                sprintf(Source, "%s/%s", fd_path, fd_entity->d_name);
                if(readlink(Source, Target, sizeof(Target)) != -1){
                    if(!(strcmp(filepath, Target))){
                        printf("%s -> %s\n", Source, Target);
                    }
                }
            }
        }
    }
}

int main(int argc, char* argv[]){
    if(argc != 2){
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return -1;
    }

    ListProcessWithPath(argv[1]);

    return 0;
}