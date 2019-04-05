#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void){
	int i,status;
	pid_t child,firstFather = getpid();
	char buff[256];
	for(i=1;i<4;++i){
		if((child=fork())) break;
	}
	if(i==4){
		snprintf(buff,sizeof(buff),"pstree %d > chainprocesses.txt",firstFather);
		system(buff);
	}	
	fprintf(stderr,"Esta es la vuelta Nro %d\n",i);
	fprintf(stderr,"Recibi de fork el valor de %d\n",child);
	fprintf(stderr,"Soy el proceso %d con padre %d\n\n",getpid(),getppid());
	wait(&status);
	return 0;
}
