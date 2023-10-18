#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
    char buffer[512];
    getcwd(buffer, sizeof(buffer));
    puts(buffer);

    return 0;
}