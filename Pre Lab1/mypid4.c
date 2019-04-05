#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(void){
	pid_t child;
	int status;
	char buff[256];
	if((child=fork())){
		fprintf(stderr,"\nFork() devolvió %d por tanto soy el padre con ID = %d", child, getpid());
		wait(&status);
	}
	else{
		fprintf(stderr,"\nFork() devolvió %d por tanto soy el hijo con ID = %d", child, getpid());
		//pid_t parent = getppid();
		//system("pstree parent > aprocesos3.txt");
		snprintf(buff,sizeof(buff),"pstree %d > aprocesos4.txt",getppid());
		system(buff);
	}
	return 0;
}
