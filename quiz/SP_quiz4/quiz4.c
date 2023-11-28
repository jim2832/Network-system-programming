#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUF_SIZE 1024
#define WRITE_SEM_NAME "/write_sem"
#define READ_SEM_NAME "/read_sem"
#define MUTEX_SEM_NAME "/mutex_sem"

// Thread data structure
struct ThreadData {
    sem_t *write_sem; // Writer semaphore
    sem_t *read_sem;  // Reader semaphore
    sem_t *mutex_sem; // Mutex semaphore
    char buffer[BUF_SIZE]; // Globally shared buffer
    int bytes; // Number of bytes transferred
    int xfrs; // Number of transfers
    int eof;  // Nonzero if end-of-file on input
};

void *writerThread(void *arg) {
    struct ThreadData *data = (struct ThreadData *)arg;

    while (1) {
        // Wait for reader to finish
        if (sem_wait(data->write_sem) == -1) {
            perror("sem_wait");
            exit(EXIT_FAILURE);
        }

        // Check if reader has finished
        if (sem_wait(data->mutex_sem) == -1) {
            perror("sem_wait");
            exit(EXIT_FAILURE);
        }

        // Read from stdin
        data->bytes = read(STDIN_FILENO, data->buffer, BUF_SIZE);
        if (data->bytes == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        if (data->bytes == 0) {
            data->eof = 1;
        }

        // Release mutex
        if (sem_post(data->mutex_sem) == -1) {
            perror("sem_post");
            exit(EXIT_FAILURE);
        }

        // Release reader
        if (sem_post(data->read_sem) == -1) {
            perror("sem_post");
            exit(EXIT_FAILURE);
        }

        if (data->bytes == 0) {
            break;
        }
    }

    pthread_exit(NULL);
}

void *readerThread(void *arg) {
    struct ThreadData *data = (struct ThreadData *)arg;

    while (1) {
        // Wait for writer to finish
        if (sem_wait(data->read_sem) == -1) {
            perror("sem_wait");
            exit(EXIT_FAILURE);
        }

        if (data->eof) {
            break;
        }

        // Check if writer has finished
        if (sem_wait(data->mutex_sem) == -1) {
            perror("sem_wait");
            exit(EXIT_FAILURE);
        }

        // Write to stdout
        if (write(STDOUT_FILENO, data->buffer, data->bytes) != data->bytes) {
            perror("write");
            exit(EXIT_FAILURE);
        }

        data->xfrs++;

        // Release mutex
        if (sem_post(data->mutex_sem) == -1) {
            perror("sem_post");
            exit(EXIT_FAILURE);
        }

        // Release writer
        if (sem_post(data->write_sem) == -1) {
            perror("sem_post");
            exit(EXIT_FAILURE);
        }
    }

    pthread_exit(NULL);
}

int main() {
    sem_t *write_sem, *read_sem, *mutex_sem;
    pthread_t writer_thread, reader_thread;

    // Create semaphores
    write_sem = sem_open(WRITE_SEM_NAME, O_CREAT | O_EXCL, 0644, 1);
    read_sem = sem_open(READ_SEM_NAME, O_CREAT | O_EXCL, 0644, 0);
    mutex_sem = sem_open(MUTEX_SEM_NAME, O_CREAT | O_EXCL, 0644, 1);

    if (write_sem == SEM_FAILED || read_sem == SEM_FAILED || mutex_sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    struct ThreadData data = {.write_sem = write_sem, .read_sem = read_sem, .mutex_sem = mutex_sem, .bytes = 0, .xfrs = 0, .eof = 0};

    // Create threads
    if (pthread_create(&writer_thread, NULL, writerThread, (void *)&data) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&reader_thread, NULL, readerThread, (void *)&data) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    // Wait for threads to terminate
    if (pthread_join(writer_thread, NULL) != 0 || pthread_join(reader_thread, NULL) != 0) {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }

    // Close named semaphores
    if (sem_close(write_sem) == -1 || sem_close(read_sem) == -1 || sem_close(mutex_sem) == -1) {
        perror("sem_close");
        exit(EXIT_FAILURE);
    }

    // Unlink named semaphores
    if (sem_unlink(WRITE_SEM_NAME) == -1 || sem_unlink(READ_SEM_NAME) == -1 || sem_unlink(MUTEX_SEM_NAME) == -1) {
        perror("sem_unlink");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
