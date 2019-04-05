#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc,char* argv[]){
	int i,status;
	int n = atoi(argv[1]);
	int h;
	pid_t child,pid_padre;
	char buff[256];
	pid_padre = getpid();
	for(i=0; i<n;++i)
		if((child=fork())<=0) {
			for( h=0;h<i;h++){
				if((child=fork())) break;
			}
			if((i == n-1) && (h == i)){
				snprintf(buff,sizeof(buff),"pstree %d > arbolpodado2.txt",pid_padre);
				system(buff);				
			}
			if(h==i){
				sleep(30);
			}			
			fprintf(stderr,"Esta es la vuelta Nro %d\n",i);
			fprintf(stderr,"Recibi de fork el valor de %d\n",child);
			fprintf(stderr,"Soy el proceso %d con padre %d\n\n",getpid(),getppid());
			wait(&status);				
			break;
			}
		else fprintf(stderr,"Este es el ciclo Nro %d y se esta creando el proceso %d\n",i,child);
	if(pid_padre==getpid()){
		//snprintf(buff,sizeof(buff),"pstree %d > arbolpodado.txt",pid_padre);
		//system(buff);
		for(i=0;i<n;++i) 
			wait(&status);
	}
	fprintf(stderr,"Este es el proceso %d con padre %d\n",getpid(),getppid());
	return 0;
}
