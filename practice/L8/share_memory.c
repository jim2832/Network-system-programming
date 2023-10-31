struct something *shmaddr; // 共享記憶體位址
int shmid; // 共享記憶體識別碼
key_t key = ftok("/etc/passwd", 'J'); // 產生key

.....

// shmget() returns the identifier of the shared memory segment associated with the value of the argument key.
shmid = shmget(key, sizeof(struct something), IPC_CREAT | 0666); // 建立共享記憶體

// shmget() returns the identifier of the shared memory segment associated with the value of the argument key.
if (shmid == -1)
    /* Handle error. */

// Attach the segment to our data space. (掛載共享記憶體)
shmaddr = (struct something *) shmat(shmid, NULL, 0);

// shmat() attaches the shared memory segment identified by shmid to the address space of the calling process.
if (shmaddr == (struct something *) - 1)
    /* Handle error. */

.....

/* Use the shared memory here */

.....

// Detach the shared memory segment. (解除掛載共享記憶體)
shmdt(shmaddr); // 解除掛載共享記憶體

// Deallocate the shared memory segment. (刪除共享記憶體)
shmctl(shmid, IPC_RMID, (struct shmid_ds *)NULL); // 刪除共享記憶體