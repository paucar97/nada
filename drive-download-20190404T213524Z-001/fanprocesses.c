#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main (void){
	int i,status;
	pid_t child,pid_padre;
	char buff[200];
	pid_padre=getpid();
	for(i=1;i<4;++i)
		if ((child=fork())<=0) break;
		else fprintf(stderr, "Este es el ciclo Nro %d y se esta creando el proceso %d\n",i,child );
	if (i == 4){
		snprintf(buff,sizeof(buff),"pstree %d > carajo2.txt", pid_padre);
		system(buff);
	}	
	if (pid_padre == getpid()) for (int i=1;i<4;++i) wait(&status);
	fprintf(stderr, "Este es el presente %d con el padre %d\n",getpid(),getppid());
	return 0;
}