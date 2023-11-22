#include <stdio.h>
#include "producer.h"

void delay(useconds_t us){
    usleep(us * 1000);
}

void write_data(Shared_memory *shm, int data){
    shm->data = data;
    sprintf(shm->buffer, "data %d", data); // write data to buffer
}

void send_data(struct Producer *producer){
    Shared_memory *shm = producer->buf; // shared memory

    for(int i=0; i<producer->num_data; i++){
        delay(producer->delay); // delay for transmission rate
        write_data(&shm[i % producer->buffer_size], i);
    }
}
