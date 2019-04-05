#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

double final;
pid_t pid_padre;
int n,tam;
void crea_arbol(int);
char cadenaPs[40];

int main(int narg,char*argv[]) {
	int n;
	n = atoi(argv[1]);
	final=pow(2,(n-1));
	tam = final;
	pid_padre = getpid();
	crea_arbol(1);
	return 0;
}

void crea_arbol(int x){
	char cadena[60];
	sprintf(cadena,"Soy el proceso %d con pid %d y ppide %d\n",x,getpid(),getppid());
	write(1, cadena, strlen(cadena));
	if(x >= final){
		if(x == 2*final - 1){
			sprintf(cadenaPs,"pstree %d -p > binarytree.txt",pid_padre);
			system(cadenaPs);
		}	
		return;
	}	 
	if(!fork()) { crea_arbol(2*x); sleep(tam/x); exit(0);}
	if(!fork()) { crea_arbol(2*x+1); sleep(tam/x); exit(0);}
	wait(NULL);	
	wait(NULL);
}
