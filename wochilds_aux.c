#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX 1000000

int main(void){

	//printf("%ld",time(NULL));
	//srand(time(NULL));
	//printf("%d",rand());


	int j, status;
	pid_t child;

	long i=0, *arreglo;

	arreglo = (long *)malloc(MAX*sizeof(long));

	srand(time(NULL)); //el time(NULL) devuelve la cantidad de segundos desde el 1 de enero de 1970


	for(j=1;j<5;++j) 
		if((child=fork())) break;
		else if(child==0){
			if(j==1){
				for(i=0; i<MAX; i++)
				arreglo[i] = rand() % 20;
			}
			else if(j==2){
				for(i=0; i<MAX; i++)
					printf("%li ",arreglo[i]);
				printf("\n\n");
			}
				else if(j==3){
					for(i=0; i<MAX; i++)
						if(arreglo[i] % 2)
							arreglo[i]=1;
				|		else
							arreglo[i]=0;
				}
					else{
						for(i=0; i<MAX ; i++)
							printf("%li ", arreglo[i]);
					}
		}
		wait(&status);
		return 0;
}

	

	/*long i=0, *arreglo;

	arreglo = (long *)malloc(MAX*sizeof(long));

	srand(time(NULL)); //el time(NULL) devuelve la cantidad de segundos desde el 1 de enero de 1970

	for(i=0; i<MAX; i++)
		arreglo[i] = rand() % 20;

	for(i=0; i<MAX; i++)
		printf("%li ",arreglo[i]);
	printf("\n\n");

	for(i=0; i<MAX; i++)
		if(arreglo[i] % 2)
			arreglo[i]=1;
		else
			arreglo[i]=0;

	for(i=0; i<MAX ; i++)
		printf("%li ", arreglo[i]);
	exit(0);*/
	


