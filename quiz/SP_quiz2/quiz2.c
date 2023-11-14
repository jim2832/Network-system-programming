#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <email_address>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *email = argv[1]; // Email address to send to
    srand(time(NULL));           // Seed the random number generator
    char buffer[1024];           // Buffer to store the message

    // Create the pipe
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("Error creating pipe");
        exit(EXIT_FAILURE);
    }

    // Fork the process
    pid_t pid = fork();
    switch (pid) {
    case -1:
        perror("Error forking");
        exit(EXIT_FAILURE);
        break;

    case 0:               // Child process
        close(pipefd[1]); // Close the write end of the pipe

        while(1){
            read(pipefd[0], buffer, sizeof(buffer));
            char command[2048];

            // printf("Receiver received: %s\n", buffer);

            close(pipefd[0]); // Close the read end of the pipe
            strcat(buffer, " error"); // Add " error" to the end of the message
            sprintf(command, "mail -s \"%s\" %s", buffer, email); // Create the command to send the email

            system(command); // Send the email
        }

        exit(0);
        break;

    default:
        close(pipefd[0]); // Close the read end of the pipe

        while(1){
            int random_sleep_time = rand() % 9 + 1;
            int random_number = rand() % 3 + 1;
            char *error_message;
            switch (random_number) {
            case 1:
                error_message = "type1";
                break;
            case 2:
                error_message = "type2";
                break;
            case 3:
                error_message = "type3";
                break;
            default:
                break;
            }
            write(pipefd[1], error_message, strlen(error_message) + 1);

            sleep(random_sleep_time);
        }
        
        close(pipefd[1]); // Close the write end of the pipe

        break;
    }

    return 0;
}