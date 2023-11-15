#define _GNU_SOURCE
#include "posix_msg_file.h"
#include <mqueue.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>

/* SIGCHLD handler */
static void handler(int sig){
    int savedErrno = errno;                 /* waitpid() might change 'errno' */
    while (waitpid(-1, NULL, WNOHANG) > 0) continue;
    errno = savedErrno;
}

/* Executed in child process: serve a single client */
static void serveRequest(const struct requestMsg *req){
    struct responseMsg resp;
    
    mqd_t clientMQ = mq_open(req->pathname, O_RDWR); // open client MQ
    int fd = open(req->pathname, O_RDONLY);
    if (fd == -1) {                     /* Open failed: send error text */
        resp.mtype = RESP_MT_FAILURE;
        snprintf(resp.data, sizeof(resp.data), "%s", "Couldn't open");
        mq_send(clientMQ, (char *) &resp, strlen(resp.data) + 1, 0);
        mq_close(clientMQ);
        exit(EXIT_FAILURE);             /* and terminate */
    }

    /* Transmit file contents in messages with type RESP_MT_DATA. We don't
       diagnose read() and msgsnd() errors since we can't notify client. */

    resp.mtype = RESP_MT_DATA;
    ssize_t numRead;
    while ((numRead = read(fd, resp.data, RESP_MSG_SIZE)) > 0)
        if (mq_send(clientMQ, (char *) &resp, numRead, 0) == -1)
            break;                      /* Exit loop on error */

    /* Send a message of type RESP_MT_END to signify end-of-file */

    resp.mtype = RESP_MT_END;
    mq_send(clientMQ, (char *) &resp, 0, 0);    /* Zero-length mtext */
    mq_close(clientMQ);                         /* Close client message queue */        
}

int main(int argc, char *argv[]){
    struct sigaction sa;

    /* Establish SIGCHLD handler to reap terminated children */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
        errExit("sigaction");


    /* Create server message queue */
    
    mqd_t serverMQ = mq_open(SERVER_KEY, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR, NULL);
    if(EEXIST == errno && serverMQ == -1) // server already exists
        serverMQ = mq_open(SERVER_KEY, O_RDWR, S_IRUSR | S_IWUSR, NULL);
    else if (serverMQ == -1) // error
        errExit("mq_open");


    /* Read requests, handle each in a separate child process */

    while(1) {
        struct requestMsg req; // Request message received from client
        ssize_t msgLen = mq_receive(serverMQ, (char *) &req, sizeof(struct requestMsg), 0);
        if (msgLen == -1) {
            if (errno == EINTR)         /* Interrupted by SIGCHLD handler? */
                continue;               /* ... then restart msgrcv() */
            errMsg("mq_receive");           /* Some other error */
            break;                      /* ... so terminate loop */
        }

        pid_t pid = fork();                   /* Create child process */
        if (pid == -1) {
            errMsg("fork");
            break;
        }

        if (pid == 0) {                 /* Child handles request */
            serveRequest(&req);
            _exit(EXIT_SUCCESS);
        }

        /* Parent loops to receive next client request */
    }

    /* If msgrcv() or fork() fails, remove server MQ and exit */

    // close server MQ
    if (mq_close(serverMQ) == -1)
        errExit("mq_close");

    // remove server MQ
    mq_unlink(SERVER_KEY);
    
    return 0;
}