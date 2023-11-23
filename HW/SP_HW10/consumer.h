#ifndef __COMSUMER_H__
#define __COMSUMER_H__
#include "shared_memory.h"

// consumer structure
struct Consumer{
    int ID; // consumer ID
    int num_data; // number of data
    int buffer_size; // buffer size
    Shared_memory *buf; // shared memory
    int (*job)(struct Consumer*);
};

// receive data from shared memory
int receive_data(struct Consumer *consumer);

#endif