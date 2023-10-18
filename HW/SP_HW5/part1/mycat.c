#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Uasge: ./mycat <filename>\n");
        exit(EXIT_SUCCESS);
    }

    FILE *file = fopen(argv[1], "r");

    if(file == NULL){
        perror("Unable to open the file");
        exit(EXIT_FAILURE);
    }

    char c;
    while((c = fgetc(file)) != EOF){
        putchar(c);
    }

    fclose(file);

    return 0;
}