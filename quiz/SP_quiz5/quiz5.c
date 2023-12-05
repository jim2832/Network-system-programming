#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define DIE(x){ perror(x); exit(1); }
#define MAXLINE 4096

static void sig_pipe(int signo){
	DIE("SIGPIPE caught");
}

int main(void){
    int n;
	int fd1[2], fd2[2]; //pipes
	pid_t pid;
	char line[MAXLINE];

    // set line buffering for stdin and stdout
	if(signal(SIGPIPE, sig_pipe) == SIG_ERR){
		DIE("signal error");
	}

    // fd1: parent -> child, fd2: child -> parent
	if(pipe(fd1) < 0 || pipe(fd2) < 0){
		DIE("pipe error");
	}

    // fork processes
	if((pid = fork()) < 0){
		DIE("fork error");
	}
    
    // parent
    else if(pid > 0){
        // close the unused ends of the pipes
		close(fd1[0]);
		close(fd2[1]);

        // set line buffering for stdin and stdout
		FILE *fpin = fdopen(fd1[1], "w");
		FILE *fpout = fdopen(fd2[0], "r");

		while(fgets(line, MAXLINE, stdin) != NULL){
			n = strlen(line);

            // write to pipe
			if(fwrite(line, sizeof(char), n, fpin) != n){
				DIE("write error to pipe");
			}

            // flush the buffer
			fflush(fpin);

            // read from pipe
			if(fgets(line, MAXLINE, fpout) == NULL){
				DIE("read error from pipe");
			}

            // print to stdout
			if(fputs(line, stdout) == EOF){
				DIE("fputs error");
			}
		}

        // close pipes
		if(ferror(stdin)){
			DIE("fgets error on stdin");
		}

		exit(0);
	}

    // child
    else{
		close(fd1[1]);
		close(fd2[0]);

		FILE *fpin = fdopen(fd1[0], "r");
		FILE *fpout = fdopen(fd2[1], "w");

        // set line buffering for stdin and stdout
		if(fpin == NULL || fpout == NULL){
			DIE("fdopen error");
		}

        // redirect stdin and stdout
		if(dup2(fileno(fpin), STDIN_FILENO) == -1){
			DIE("dup2 error to stdin");
		}
		if(dup2(fileno(fpout), STDOUT_FILENO) == -1){
			DIE("dup2 error to stdout");
		}

		fclose(fpin);
		fclose(fpout);

        // execute add2
		execl("./add2", "add2",(char *)NULL);
		DIE("execl error");
	}

    return 0;
}