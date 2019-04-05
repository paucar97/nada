#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#define OK 0

int main(){
	int fd[2],id;
	pipe(fd);
	if((id = fork())){
		system("ps -l");
		kill(id,SIGTERM);
		system("ps -l");
		exit(OK);
	}	
	read(fd[0],&id,sizeof(id));
	exit(OK);
}	
