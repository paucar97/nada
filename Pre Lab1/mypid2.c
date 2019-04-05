#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main(void){
	pid_t child;
	int status;
	if((child=fork())){
		fprintf(stderr,"\nFork() devolvió %d por tanto soy el padre con ID = %d", child, getpid());
		wait(&status);
	}
	else{
		fprintf(stderr,"\nFork() devolvió %d por tanto soy el hijo con ID = %d", child, getpid());
		system("pstree > aprocesos2.txt");
	}
	return 0;
}
