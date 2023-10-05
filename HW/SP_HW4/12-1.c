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

void removeFirstNCharacters(char *str, int n){
    int length = strlen(str);
    if(n >= length){
        // If n is greater than or equal to the length of the string,
        // remove the entire string(set it to an empty string).
        str[0] = '\0';
    } else{
        // Move characters to the left to remove the first n characters
        for(int i = 0; i <= length - n; i++){
            str[i] = str[i + n];
        }
        str[length - n] = '\0';  // Null-terminate the new string
    }
}

void removeTrailingNewline(char *str) {
    int length = strlen(str);

    // Check if the last character is a newline
    if(length > 0 && str[length - 1] == '\n'){
        // Replace the newline with a null terminator
        str[length - 1] = '\0';
    }
}

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

    /* open the directory */
    DIR *dir = opendir("/proc");
    if(!dir){
        perror("Unable to open /proc directory");
        return;
    }

    struct dirent *entity;
    char path[1024];
    int count = 1;

    // DT_DIR(directory) -> 4
    // DT_REG(file) -> 8
    while((entity = readdir(dir)) != NULL){

        /* Check if the entity is a directory and represents a process ID */
        if(entity->d_type == 4 && atoi(entity->d_name) != 0){ // It's a directory
            sprintf(path, "/proc/%s/status", entity->d_name);

            /* parse the status file */
            FILE *file = fopen(path, "r");
            char *line = NULL;
            size_t len = 0;
            int line_number = 0;
            char pid[256] = {0};
            char process[256] = {0};

            while(getline(&line, &len, file) != EOF && line_number++ < 10){

                // process name
                if(line_number == 1){
                    strcpy(process, line);
                    removeFirstNCharacters(process, 6);
                    removeTrailingNewline(process);
                }
                // pid
                else if(line_number == 6){
                    strcpy(pid, line);
                    removeFirstNCharacters(pid, 5);
                    removeTrailingNewline(pid);
                }
                // uid
                else if(line_number == 9){
                    char delim[] = " \t\n"; /* SPACE or TAB or NL */
                    strtok(line, delim);
                    strtok(NULL, delim);
                    uid_t uid = atoi(strtok(NULL, delim));

                    // Only list the processes that are run by <username>
                    if(uid == userIdFromName(username)){
                        printf("%d. Proess ID: %s, Process: %s\n", count, pid, process);
                        count++;
                    }
                }

            }
            fclose(file);
        }
    }
    closedir(dir);
}

int main(int argc, char *argv[]){
    if(argc != 2){
        fprintf(stderr, "Usage: %s <username>\n", argv[0]);
        return 1;
    }

    char *user = argv[1];
    printf("\nThe processes that are run by user '%s' are as followed:\n\n", user);
    listProcessesForUser(user);

    return 0;
}