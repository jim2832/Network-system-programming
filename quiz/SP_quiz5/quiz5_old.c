// Manually include the C standard library.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#define DIE(x) perror(x); exit(1);

#define	MAXLINE	4096			/* max line length */

static void	sig_pipe(int);		/* our signal handler */

int main(void) {
	int n, fd1[2], fd2[2];
	pid_t pid;
	char line[MAXLINE];

	if (signal(SIGPIPE, sig_pipe) == SIG_ERR){
		// err_sys("signal error");
		DIE("signal error")
	}

	// fd1: parent -> child
	// fd2: child -> parent
	if (pipe(fd1) < 0 || pipe(fd2) < 0){
		// err_sys("pipe error");
		DIE("pipe error")
	}

	// fork processes
	if ((pid = fork()) < 0) {
		// err_sys("fork error");
		DIE("fork error")
	}

	/* parent */
	else if (pid > 0) {
		close(fd1[0]); // close read end of pipe 1
		close(fd2[1]); // close write end of pipe 2

		while (fgets(line, MAXLINE, stdin) != NULL) {
			n = strlen(line);
			
			if (write(fd1[1], line, n) != n){
				// err_sys("write error to pipe");
				DIE("write error to pipe")
			}

			if ((n = read(fd2[0], line, MAXLINE)) < 0){
				// err_sys("read error from pipe");
				DIE("read error from pipe")
			}

			if (n == 0) {
				// err_msg("child closed pipe");
				DIE("child closed pipe")
				break;
			}

			line[n] = 0;	/* null terminate */
			if (fputs(line, stdout) == EOF)
				// err_sys("fputs error");
				DIE("fputs error")
		}

		// close pipes
		if (ferror(stdin)){
			// err_sys("fgets error on stdin");
			DIE("fgets error on stdin")
		}
		
		exit(0);
	}

	/* child */
	else {
		close(fd1[1]); // close write end of pipe 1
		close(fd2[0]); // close read end of pipe 2

		// redirect stdin
		if (fd1[0] != STDIN_FILENO) {
			if (dup2(fd1[0], STDIN_FILENO) != STDIN_FILENO){
				// err_sys("dup2 error to stdin");
				DIE("dup2 error to stdin")
			}
			close(fd1[0]);
		}

		// redirect stdout
		if (fd2[1] != STDOUT_FILENO) {
			if (dup2(fd2[1], STDOUT_FILENO) != STDOUT_FILENO){
				// err_sys("dup2 error to stdout");
				DIE("dup2 error to stdout")
			}
			close(fd2[1]);
		}

		// execute add2
		if (execl("./add2", "add2", (char *)0) < 0){
			// err_sys("execl error");
			DIE("execl error")
		}
	}
	exit(0);
}

static void sig_pipe(int signo){
	printf("SIGPIPE caught\n");
	exit(1);
}
