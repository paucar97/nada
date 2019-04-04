#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void){
	int i,status;
	pid_t child,papa =getpid();
	char buff[200];
	for(i=1;i<4;++i) if((child=fork())) break;
	fprintf(stderr, "Esta es la vuelta Nro %d\n",i);
	fprintf(stderr, "Recibi de fork el valor de %d\n",child);
	fprintf(stderr, "Soy el proceso %d con el padre %d\n", getpid(),getppid());
	wait(&status);
	if(i == 4){
		snprintf(buff,sizeof(buff),"pstree %d > carajo.txt", papa);
		system(buff);
	}
	return 0;
}