#ifndef __PRODUCER_H__
#define __PRODUCER_H__
#include <unistd.h>
#include <sys/types.h>
#include "shared_memory.h"

// producer structure
struct Producer{
    int num_data; // number of data
    useconds_t delay; // transmission rate
    int buffer_size; // buffer size
    Shared_memory *buf; // shared memory
    void (*job)(struct Producer *);
};

// send data to shared memory
void send_data(struct Producer *produer);

#endif