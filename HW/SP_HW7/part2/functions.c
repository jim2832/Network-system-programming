#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

// Set the signal handler for the given signal
void _sigset(int signum, void(*handler)(int)){
    struct sigaction sa;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(signum, &sa, NULL);
}

// Block the given signal
void _sighold(int signum){
    sigset_t blocked;
    sigemptyset(&blocked);
    sigaddset(&blocked, signum);
    sigprocmask(SIG_BLOCK, &blocked, NULL);
}

// Unblock the given signal
void _sigrelse(int signum){
    sigset_t unblocked;
    sigemptyset(&unblocked);
    sigaddset(&unblocked, signum);
    sigprocmask(SIG_UNBLOCK, &unblocked, NULL);
}

// Ignore the given signal
void _sigignore(int signum){
    _sigset(signum, SIG_IGN);
}

// Suspend the process until a signal is received
void _sigpause(){
    sigset_t mask;
    sigemptyset(&mask);
    sigsuspend(&mask);
}

// Send the given signal to the given process
void _signal_handler(int signum){
    printf("Received signal %d\n", signum);
}

int main(int argc, char *argv[]){
    _sigset(SIGUSR1, _signal_handler); // Set the signal handler for SIGUSR1
    _sighold(SIGUSR1); // Block SIGUSR1
    _sigrelse(SIGUSR1); // Unblock SIGUSR1

    pid_t pid = fork(); // Fork a child process

    if(pid == 0){ // Child process
        _sigpause(); // Suspend the process until a signal is received
        printf("Child process received signal\n");
    }
    else{ // Parent process
        printf("Parent process sleep for 3 second then send signal to child\n");
        sleep(3); // Sleep for 3 seconds
        kill(pid, SIGUSR1); // Send SIGINT to the child process
        printf("Parent process sent signal\n");
    }

    return 0;
}
