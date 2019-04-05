#define BUFSIZE 64
#define WLOOP 5
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(void) {
	char buffer[BUFSIZE];
	char message[]="Hello, what's up?\n";
	int ii, pipe_fd[2];
	pipe(pipe_fd);
	if(fork() == 0) {
		close(pipe_fd[0]);
		printf("I am PID[%d] (child).\n", (int) getpid());
		for(ii=0;ii<WLOOP;ii++)
			write(pipe_fd[1], message, sizeof(message)-1);
		close(pipe_fd[1]);
	} else {
		close(pipe_fd[1]);
		printf("I am PID[%d] (parent).\n", (int) getpid());
		memset(buffer, 0, sizeof(buffer));
		while((ii=read(pipe_fd[0], buffer, BUFSIZE-1)) != 0){
			printf("PARENT READ[%d]:\n%s\n", (int) ii, buffer);
			memset(buffer, 0, sizeof(buffer));
		}
		close(pipe_fd[0]);
	}
	return 0;
}
