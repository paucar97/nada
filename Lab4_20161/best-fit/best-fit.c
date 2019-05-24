
#include <stdio.h>
#include "xalloc.h"

int main(int argc,char** argv){
	//codigo = 20121825, Ultimo digito = 5
	long *direccion;
	long *dir1, *dir2;
	int n;
	if (argc <2){
		n = 5;
	} else {
		n = atoi(argv[1]);
	}
	int i = 1; 
	if(n== 0) 
		printf("Se pidio memoria para 0kb se deberia reservar el hueco por defecto cuando se llama a xmalloc");
	else{
		for(;;){
			printf("******** Pidiendo memoria para %d bytes...\n\n",i*n);
			direccion = (long*)xmalloc(i*n); // con el flag se evita usar morecore
			if (direccion == NULL) break;
			
			// Bloques a los que se hara free
			if (i == 2) dir1 = direccion;
			if (i == 6) dir2 = direccion;
			printf("Bloque de %d bytes , Direccion: %lx\n\n", n*i, direccion);			
			i++;
		}
		printf("Bloque de %d bytes , Direccion: no se pudo asignar\n", n*i);
		
		printf("*****************************PRUEBA***************************\n");
		printf("Liberando dos bloques de 10 bytes y de 30 bytes...\n\n");
		xfree(dir1);
		xfree(dir2);
		printf("\n******** Pidiendo memoria para 10 bytes...\n\n");
		printf("Deberia escoger el espacio menor...\n\n");
		direccion = (long*)xmalloc(10);
		printf("Bloque de 10 bytes , Direccion: %lx\n\n", direccion);		
	}
	return 0;
}
