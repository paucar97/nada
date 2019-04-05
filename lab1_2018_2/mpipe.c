#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#define BUFSIZE 64

int main(int argc, char const *argv[])
{
	char message[] = "Enviando message";
	int fd[2];
	pipe(fd);
	if(fork()){
		write(fd[1],message,sizeof(message));
		//read(fd[0],message,sizeof(message));
		printf("%s\n",message );
		wait(NULL);
	}else{
		read(fd[0],message,sizeof(message));
		if(!strcmp(message,"Enviando message")){
			strcpy(message,"OK");
			printf("%s\n",message );
		}else{
			printf("ERROR EN EL ENVIO\n");
			exit(1);
		}
	}

	return 0;
}