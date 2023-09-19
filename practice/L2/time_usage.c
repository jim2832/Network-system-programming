#include <sys/types.h>
#include <sys/times.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(){
    int m;
    time_t t;
    struct tms tms1, tms2;
    clock_t time1, time2;

    /* Num ticks per sec */
    double tick = sysconf(_SC_CLK_TCK);

    if( (time1 = times( &tms1 )) == -1 ){
        perror("times");
        exit(1);
    }

    for(m=0; m<999999; m++){
        getpid();
    }

    if( (time2 = times( &tms2 )) == -1 ){
        perror("times");
        exit(1);
    }
    
    printf("My Real time is: %f sec \n", ( time2 - time1 ) / tick );
    printf("My User time is: %f sec \n", (( tms2.tms_utime - tms1.tms_utime) / tick ));
    printf("My Sys time is: %f sec \n", (( tms2.tms_stime - tms1.tms_stime) / tick ));

    return 0;
}