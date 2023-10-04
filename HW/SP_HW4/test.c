#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

int main(){
    DIR * dir;
    struct dirent * entry;
    int i;
    dir = opendir("/home/jim/Desktop/Network_system_programming/HW/");
    while((entry = readdir(dir)) != NULL){
        printf("d_name : %s\n", entry->d_name);
        printf("d_type : %d\n", entry->d_type);
        printf("\n");
    }
    closedir(dir);

    return 0;
}