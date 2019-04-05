#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void){
	pid_t child;
	if((child=fork()))
		fprintf(stderr,"\nFork() devolvi� %d por tanto soy el padre con ID = %d", child, getpid());
	else
		fprintf(stderr,"\nFork() devolvi� %d por tanto soy el hijo con ID = %d", child, getpid());
	return 0;
}
