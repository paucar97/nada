#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#define OK 0

int main(){
	int id1,fd[2];
	
	id1 = getpid();
	pipe(fd);
	if(!fork()){
		int id2;
		char cad[20];
		
		id2 = getpid();
		sprintf(cad,"pstree -sl %d",id2);
		system(cad);
		kill(id1,SIGTERM);
		system(cad);
		exit(OK);
	}
	read(fd[0],&id1,sizeof(id1));
	exit(OK);	
}	
