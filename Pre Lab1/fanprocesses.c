#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void){
	int i,status;
	pid_t child,pid_padre;
	char buff[256];
	pid_padre = getpid();
	for(i=1; i<4;++i)
		if((child=fork())<=0) break;
		else fprintf(stderr,"Este es el ciclo Nro %d y se esta creando el proceso %d\n",i,child);
	if(pid_padre==getpid()){
		snprintf(buff,sizeof(buff),"pstree %d > fanprocesses.txt",pid_padre);
		system(buff);
		for(i=1;i<4;++i) 
			wait(&status);
	}
	fprintf(stderr,"Este es el proceso %d con padre %d\n",getpid(),getppid());
	return 0;
}
