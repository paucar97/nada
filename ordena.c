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


int main ( int argc, char *argv[]){
	int pipe_fd[2];
	int papa = getpid();
	int n = atoi(argv[1]); // EL N 
	int nu[n];
	int numero[n];
	
	pipe(pipe_fd);
	if (!fork()){
		dup2(pipe_fd[1],1);
		close(pipe_fd[1]);
		for(int i =0;i<n;i++){
			numero[i] = rand()%20;
		}
		write(1,numero,sizeof(numero));
	}else{
		dup2(pipe_fd[0],0);
		close(pipe_fd[0]);
		read(0,nu,sizeof(nu));
		//ordenar(nu,n);
		int temporal;
		for (int i=0; i< n;i++)
			printf("El numero %d \n",nu[i]);
		printf("----DESPUES DE ORDENAR----\n");		
		for( int i=0; i<n; i++){
        	for( int j=0; j<n-1; j++){
	            if(nu[j]>nu[j+1]){
	                temporal = nu[j];
	                nu[j] = nu[j+1];
	                nu[j+1] = temporal;
	            }
	        }
	    }

		for (int i=0; i< n;i++)
			printf("El numero %d \n",nu[i]);
	}

	return 0;
}

