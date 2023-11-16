/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2023.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

#include "posix_shm_xfr.h"
#include <sys/mman.h>
#define _GNU_SOURCE

int
main(int argc, char *argv[])
{
    int semid, shmid, xfrs, bytes;
    struct shmseg *shmp;

    /* Get IDs for semaphore set and shared memory created by writer */

    semid = semget(SEM_KEY, 0, 0);
    if (semid == -1)
        errExit("semget");

    shmid = shm_open(SHM_KEY, 0, 0); // open shared memory object
    if (shmid == -1)
        errExit("shm_open");

    shmp = mmap(NULL, sizeof(struct shmseg), PROT_READ, MAP_SHARED, shmid, 0); // attach at address chosen by system

    /* Transfer blocks of data from shared memory to stdout */

    for (xfrs = 0, bytes = 0; ; xfrs++) {
        if (reserveSem(semid, READ_SEM) == -1)          /* Wait for our turn */
            errExit("reserveSem");

        if (shmp->cnt == 0)                     /* Writer encountered EOF */
            break;
        bytes += shmp->cnt;

        if (write(STDOUT_FILENO, shmp->buf, shmp->cnt) != shmp->cnt)
            fatal("partial/failed write");

        if (releaseSem(semid, WRITE_SEM) == -1)         /* Give writer a turn */
            errExit("releaseSem");
    }

    /* Give writer one more turn, so it can clean up */

    // wait for writer to finish
    if (releaseSem(semid, WRITE_SEM) == -1)
        errExit("releaseSem");
    
    // unmap shared memory
    if(munmap(shmp, sizeof(struct shmseg)) == -1)
        errExit("munmap");

    fprintf(stderr, "Received %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}