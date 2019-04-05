#define BUFSIZE 64
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define OK 0

int main(){
	//execl("./anillo", "anillo", "4", NULL);
	execlp("ls", "ls", "-l", NULL);
	exit(0);
}
