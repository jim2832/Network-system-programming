#define _GNU_SOURCE
#include <mqueue.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "posix_msg_file.h"
#define CLIENT_KEY "/client_key"

mqd_t clientMQ; // ID of client's queue

static void removeQueue(void){
    if(mq_close(clientMQ) == -1)
        errExit("mq_close");
}

/* SIGCHLD handler */
static void handler(int sig){
    int savedErrno = errno;                 /* waitpid() might change 'errno' */
    while (waitpid(-1, NULL, WNOHANG) > 0) continue;
    errno = savedErrno;
}

int main(int argc, char *argv[]){
    struct sigaction sa;
    /* Establish SIGCHLD handler to reap terminated children */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
        errExit("sigaction");

    // struct requestMsg req; // Request message to send to server
    // struct responseMsg resp; // Response message received from server
    mqd_t serverMQ; // ID of server's queue
    int numMsgs; // number of messages received

    // error check
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        usageErr("%s pathname\n", argv[0]);

    if (strlen(argv[1]) > 4095)
        cmdLineErr("pathname too long (max: %ld bytes)\n", 4095);

    /* Get server's queue identifier; create queue for response */

    serverMQ = mq_open(SERVER_KEY, O_RDWR);
    if (serverMQ == -1)
        errExit("msgget - server message queue");

    clientMQ = mq_open(CLIENT_KEY, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR, NULL);
    if(EEXIST == errno && clientMQ == -1){ // if client queue already exists
        clientMQ = mq_open(CLIENT_KEY, O_RDWR, S_IRUSR | S_IWUSR, NULL);
    }
    else if (clientMQ == -1)
        errExit("msgget - client message queue");

    if (atexit(removeQueue) != 0) // register exit function
        errExit("atexit");
    /* Send message asking for file named in argv[1] */

    // strcpy(req.clientId, CLIENT_KEY);
    // strncpy(req.pathname, argv[1], sizeof(req.pathname) - 1);
    // req.pathname[sizeof(req.pathname) - 1] = '\0';
    //                                     /* Ensure string is terminated */

    struct requestMsg req = { .clientId = CLIENT_KEY, .pathname = {0}};
    strcpy(req.pathname, argv[1]);

    if(mq_send(serverMQ, (char *) &req, sizeof(struct requestMsg), 0) == -1) // send request to server
        errExit("mq_send");
    mq_close(serverMQ); // close server MQ

    /* Get first response, which may be failure notification */

    struct responseMsg resp = {.mtype = RESP_MT_DATA, .data = {0}};
    ssize_t msgLen = mq_receive(clientMQ, (char *) &resp, sizeof(struct responseMsg), 0);
    if (msgLen == -1)
        errExit("mq_receive");

    if (resp.mtype == RESP_MT_FAILURE) {
        printf("%s\n", resp.data);      /* Display msg from server */
        exit(EXIT_FAILURE);
    }


    /* File was opened successfully by server; process messages
       (including the one already received) containing file data */

    ssize_t totBytes = msgLen;                  /* Count first message */
    for (numMsgs = 1; resp.mtype == RESP_MT_DATA; numMsgs++) {
        msgLen = msgrcv(clientMQ, &resp, sizeof(struct responseMsg), 0, 0);
        if (msgLen == -1)
            errExit("msgrcv");

        totBytes += msgLen;
    }

    printf("Received %ld bytes (%d messages)\n", (long) totBytes, numMsgs);

    mq_close(clientMQ); // close client MQ
    mq_unlink(CLIENT_KEY); // unlink client MQ

    exit(EXIT_SUCCESS);
}