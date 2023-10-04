#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <errno.h>

/* Return UID corresponding to 'name', or -1 on error */
uid_t userIdFromName(const char *name){
    struct passwd *pwd;
    uid_t u;
    char *endptr;

    /* On NULL or empty string */
    if(name == NULL || *name == '\0'){
        return -1;
    }

    /* strtol -> string to long int */
    u = strtol(name, &endptr, 10);

    /* allow a numeric string */
    if(*endptr == '\0'){
        return u;
    }

    pwd = getpwnam(name);
    if(pwd == NULL){
        return -1;
    }

    return pwd->pw_uid;
}

void listProcessesForUser(const char *username){
    /* get the user ID from name */
    uid_t uid = userIdFromName(username);
    printf("%d", uid);
    if(uid == -1){
        fprintf(stderr, "Error: Unable to find user %s\n", username);
        return;
    }

    /* open the directory */
    DIR *dir = opendir("/proc");
    if(!dir){
        perror("Unable to open /proc directory");
        return;
    }

    struct dirent *entity;
    char path[1024];

    // DT_DIR(directory) -> 4
    // DT_REG(file) -> 8
    while((entity = readdir(dir)) != NULL){
        // Check if the entity is a directory and represents a process ID
        if(entity->d_type == 4 && atoi(entity->d_name) != 0){ // It's a directory
            sprintf(path, "/proc/%s/status", entity->d_name);
            // printf("%s\n", path);

            // Open the status file for the process
            int fd = open(path, O_RDONLY);
            if(fd == -1){
                // Process directory may have disappeared, skip it
                continue;
            }

            // Read the contents of the status file
            char buffer[4096];
            ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);
            if(bytesRead > 0){
                buffer[bytesRead] = '\0';
                char *token = strtok(buffer, "\n");

                // Parse and extract process ID and command name
                char *pid = NULL;
                char *command = NULL;
                while(token != NULL){
                    if(strncmp(token, "Pid:", 4) == 0){
                        pid = strtok(token + 4, " \t");
                    }
                    else if(strncmp(token, "Name:", 5) == 0){
                        command = strtok(token + 5, " \t");
                    }

                    if(pid != NULL && command != NULL)
                        break;

                    token = strtok(NULL, "\n");
                }

                if(pid != NULL && command != NULL && atoi(pid) != 0){
                    printf("Process ID: %s, Command: %s\n", pid, command);
                }
            }

            close(fd);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]){
    if(argc != 2){
        fprintf(stderr, "Usage: %s <username>\n", argv[0]);
        return 1;
    }

    listProcessesForUser(argv[1]);

    return 0;
}