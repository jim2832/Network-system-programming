#include <stdio.h>
#include "consumer.h"

int receive_data(struct Consumer *consumer){
    Shared_memory *const shm = consumer->buf; // shared memory
    int count = 0; // count received data
    int prev_data = -1; // previous data

    int i = 0;
    while(1){
        int current = shm[i].data; // current data

        // if data is changed
        if(current > prev_data){
            prev_data = current; // update previous data
            count++; // update count
        }

        // if data is not changed for 1 second
        else if(current == -1){
            break;
        }

        // update index
        i++;

        // if index is out of range
        if(i == consumer->buffer_size){
            i = 0;
        }
    }

    return consumer->num_data - count; // return number of lost data
}