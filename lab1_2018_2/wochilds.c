#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>	
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX 8

long k=0;
int main(void) {
    long i=0, *arreglo,*aux;
    arreglo = (long *)malloc(MAX*sizeof(long));
    aux = (long *)malloc(MAX*sizeof(long));
    int fd[2],ppadre;

    srand(time(NULL));
    /*for(i=0;i< MAX; i++) 
       arreglo[i] = rand() % 20;*/
  
    pipe(fd); 
    ppadre=getpid();
    
    for(int j=0;j<4;j++){ // para crear los 4 hijos
        if(fork()) { // es padre
            break;
        }else{ // es hijo

            for(i=k;i<(MAX/4);i++) arreglo[i] = rand() % 20;
            write(fd[1],arreglo,sizeof(arreglo));
            for(i=k;i<(MAX/4);i++){ //convierto a 0 o 1 
                if(arreglo[i] % 2)
                    arreglo[i]=1;
                else
                    arreglo[i]=0;
            } 
            int indAux=k;
            for(i=0;i<MAX/4;i++) k++;
            read(fd[0],aux,sizeof(aux));
        	for(long ind=indAux;ind<(MAX/4)*j;ind++) aux[ind] = arreglo[ind];
            write(fd[1],aux,sizeof(aux));		
            /*if(k==MAX){ // TERMINO DE LLENAR EL ARREGLO
                for(i=0;i< MAX; i++) 
                    printf("%li ",arreglo[i]);
            }*/
        }
    }
if(ppadre==getpid()){ // si es padre espero a que mis 4 hijos mueras
  for(i=0;i<4;i++) 
    waitpid(-1,NULL,0);
 for(i=0;i< MAX; i++) // luego de esperar imprimo el arregloc on ceros y unos
 	read(fd[1],aux,sizeof(aux));
 	for(long ti=0;ti<MAX;ti++)
    	printf("%li ",aux[i]);
}
  
  return 0;
}