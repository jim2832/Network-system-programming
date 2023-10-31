#define NUM_ELEMENTS 50

.....

struct something *shmaddr; // 共享記憶體位址
int shmid; // 共享記憶體識別碼
shmid = shmget(key, NUM_ELEMENTS * sizeof(struct something), IPC_CREAT | 0666); // 建立共享記憶體

// shmget() returns the identifier of the shared memory segment associated with the value of the argument key.
if (shmid == -1)
    /* Handle error */

// Attach the segment to our data space.
shmaddr = (struct something *) shmat(shmid, (char *)NULL, 0); // 這裡的NULL可以改成一個位址，但是這個位址必須是page-aligned

// shmat() attaches the shared memory segment identified by shmid to the address space of the calling process.
if (shmaddr == (struct something *)-1)
    /* Handle error */

.....

/* May now use shmaddr[0], shmaddr[1],
.... shmaddr[49] */

.....

// Detach the shared memory segment.
shmdt(shmaddr); // 解除掛載共享記憶體

// Deallocate the shared memory segment.
shmctl(shmid, IPC_RMID, (struct shmid_ds *)NULL); // 刪除共享記憶體