#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
int main(void){
	pid_t child;
	pid_t idPadre =getpid();
	char buff[200];
	if((child = fork()))
		fprintf(stderr, "\nFork() devolvio %d por tanto soy el padre con ID = %d \n",child,getpid() );

	else{
		fprintf(stderr, "\nFork() devolvio %d devolvio por tanto soy el hijo con ID= %d \n",child,getpid());
		snprintf(buff, sizeof(buff), "pstree %d > hola/hprocess.txt", idPadre);
		system(buff);
	}
	if(idPadre == getpid()) wait(0);
	return 0;	
}