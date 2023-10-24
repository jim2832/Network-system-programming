#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

void print_tree(int pid, int level) {
    DIR *dir;
    struct dirent *entry;
    char path[256];

    // Open the /proc directory
    dir = opendir("/proc");
    if (dir == NULL) {
        perror("opendir");
        exit(1);
    }

    // Iterate through each entry in the /proc directory
    while ((entry = readdir(dir)) != NULL) {
        // Check if the entry is a directory and represents a process
        if (entry->d_type == 4 && atoi(entry->d_name) != 0) {
            int parent_pid;
            char cmdline[256];

            // Get the parent process ID and command line
            snprintf(path, sizeof(path), "/proc/%s/stat", entry->d_name);
            FILE *stat_file = fopen(path, "r");
            if (stat_file) {
                fscanf(stat_file, "%*d %*s %*c %d", &parent_pid);
                fclose(stat_file);
            }

            snprintf(path, sizeof(path), "/proc/%s/cmdline", entry->d_name);
            FILE *cmdline_file = fopen(path, "r");
            if (cmdline_file) {
                fgets(cmdline, sizeof(cmdline), cmdline_file);
                fclose(cmdline_file);
            } else {
                strcpy(cmdline, "N/A");
            }

            if (parent_pid == pid) {
                for (int i = 0; i < level; i++) {
                    printf("  ");
                }
                printf("%s (%s)\n", entry->d_name, cmdline);
                print_tree(atoi(entry->d_name), level + 1);
            }
        }
    }

    closedir(dir);
}

int main() {
    int init_pid = 1; // You can change this to the desired initial process ID
    printf("Init (%d)\n", init_pid);
    print_tree(init_pid, 1);

    return 0;
}
