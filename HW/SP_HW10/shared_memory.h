#ifndef _SHARED_MEMORY_H_
#define _SHARED_MEMORY_H_

// shared memory structure
typedef struct{
    int data;
    char buffer[128];
}Shared_memory;

#endif