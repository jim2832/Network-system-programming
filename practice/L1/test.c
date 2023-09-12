#include <sys/types.h>
#include <time.h>
#include <stdio.h>

int main() {
    /* Declare apointer variable only*/
    
    time_t tptr;
    time(&tptr);
    printf("Machine time in seconds = %ld\n", tptr);

    return 0;
}