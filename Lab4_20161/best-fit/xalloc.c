
/*
 * ssoo/xalloc.98/xalloc.c
 *
 * CONTENIDO INICIAL:
 *	Codigo correspondiente a la Seccion 8.7 del libro:
 *	"The C Programing Language", de B. Kernigham y D. Ritchie.
 *
 * En este fichero se incluiran las rutinas pedidas 
 *
 */

#include <unistd.h>
#include <stdio.h>
#include "xalloc.h"

/*
 * Definicion de la cabecera para huecos y bloques. 
 * La union con un campo de tipo Align fuerza que el tama~no
 * de la cabecera sea multiplo del tama~no de este tipo.
 */
typedef long Align;    /* for alignment to long boundary */

union header {   /* block header: */
	struct {
		union header *ptr;  /* next block if on free list */
		size_t size;     /* size of this block */
	} s;
	Align x;             /* force alignment of blocks */
};

typedef union header Header;

/*
 * La lista de huecos esta ordenada por direcciones y es circular.
 * base es el "falso" hueco de tama~no cero que asegura que la lista
 * nunca esta vacia 
 */
static Header base;   /* empty list to get started */

/*
 * freep apuntara al hueco situado en la lista antes del hueco
 * por el que comenzara la busqueda.
 * Necesario para implementar la estrategia next-fit
 */
static Header  *freep = NULL;  /* start of the free list */


#define NALLOC 1024

/*
 * morecore: ask system for more memory 
 *
 * Esta funcion se llama desde xmalloc cuando no hay espacio.
 * Aumenta el tama~no de la zona de datos como minimo en NALLOC*sizeof(Header)
 * y a~nade esta nueva zona a la lista de huecos usando xfree.
 *
 */

 void imprimirHuecos(){
	printf("- Imprimiendo espacios libres:\n");
	Header* p;
	printf("Freep : %x\n", freep);
	char c;
	//scanf("%c",&c);
	for(p=freep->s.ptr;;p=p->s.ptr){
		printf("Address: %x - Size:%d\n",p,p->s.size);
		if(p==freep) break;
	}
	printf("\n");
}

void copiarDatos(void* ptr1, void* ptr2, int nbytes){

	//printf("Destino: %x - Fuente: %x\n", ptr1, ptr2);

	char* dest = (char*)ptr1; 
	char* src = (char*)ptr2;
	int i;

	for(i = 0;i< nbytes;i++){
		dest[i] = src[i];
	}
}

static Header *morecore(size_t nu)
{
	printf("Llamo morecore\n");
	char *cp;
	Header *up;

	if (nu < NALLOC)	
		nu = NALLOC;
	cp= sbrk(nu * sizeof(Align)); // 8k papu :v
	if (cp == (char *) -1)
		return NULL;
	up = (Header *) cp;
	up ->s.size = nu;
	printf("- Imprimiendo espacio libre nuevo:\n");
	printf("Address: %x, size: %d\n\n",up, up->s.size);
	xfree((void *)(up+1));
	return freep;
}

/* xmalloc: general-purpose storage allocator */
void *xmalloc (size_t nbytes)
{
	int start = 0; //flag
	Header  *p, *prevp, *best = NULL;
	size_t nunits;
	
	nunits = (nbytes+sizeof(Align)-1)/sizeof(Align) + (sizeof(Header)-1)/sizeof(Align) + 1; // Los ultimos elementos de la suma son para incluir el header
	printf("Numero de units que se necesita: %d\n",nunits);
	if(nunits == 0) printf("Pediste cero :( \n");

	if (( prevp = freep) == NULL ) { 
		base.s.ptr = freep = prevp = & base; 
		base.s.size = 0;
		start = 1;
	}

	printf("Antes de malloc\n");
	imprimirHuecos();

	if (start == 1){
		if ((p = morecore(nunits)) == NULL)
			return NULL;  /* none left */
	}
	
	for (p= prevp->s.ptr;; prevp = p, p = p->s.ptr) {
		printf("Hueco actual: %x, size: %d\n", p, p->s.size);
		if (p->s.size >= nunits) {
			if (best == NULL){					
				best = p;
			} else {
				if (best->s.size > p->s.size)
					best = p;
			}				
		}
		if (p > p->s.ptr || p == p->s.ptr) break;
		
	}	
	
	printf("best: %x\n\n", best);
	
	if (best != NULL){
		if (best->s.size == nunits || best->s.size == (nunits + 1)){ // Si es exacto o queda espacio de uno no genera hueco
			printf("best tiene espacio exacto\n");
			p->s.ptr = best->s.ptr;
		} else {
			printf("best es mas grande que el espacio requerido\n");
			printf("best: %x - size: %d - siguiente: %x\n", best, best->s.size, best->s.ptr);
			printf("Units del nuevo hueco: %d\n", best->s.size - nunits);
			best->s.size -= nunits;
			Align* p2 = (Align*)best; // Esta parte es importante, como p es un puntero de Head y size esta en unidades de Align, casteamos el puntero como Align para poder sumarle el size.
		    p2 += best->s.size;
			p = (Header*)p2; // El puntero en la posicion correcta se vuelve a castear como Header
			p->s.size = nunits;
			printf("Nuevo espacio asignado, address:%x - size: %d\n", p, p->s.size);
			
			freep = prevp; 

			printf("\nDespues de malloc\n");
			imprimirHuecos();

			printf("Direccion de inicio del nuevo bloque (sin contar la cabecera):%x\n", p+1);
			return (void *)(p + 1);
		}
	} else {
		// si se ha dado toda la vuelta y no se encontro ningun hueco
		// disponible para ese tamanio		
		return NULL;
	}
}



/* xfree: put block ap in the free list */
void xfree(void *ap)
{
	Header *bp, *p;

	bp = (Header *)ap - 1;

	for (p= freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
		if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
			break;  /* freed block at start or end of arena */ 

	/* Comprueba compactacion con hueco posterior */
		
	if ((Header*)((Align*)bp + bp->s.size) == p->s.ptr) { // La misma logica de incremento del puntero de la linea 96
		bp->s.size += p->s.ptr->s.size;
		bp->s.ptr = p->s.ptr->s.ptr;
	} else
		 bp->s.ptr = p->s.ptr;

	/* Comprueba compactacion con hueco anterior */
	if ((Header*)((Align*)p + p->s.size) == bp) { // La misma logica de incremento del puntero de la linea 96
		p->s.size += bp->s.size;
		p->s.ptr = bp->s.ptr;
	} else
		p->s.ptr = bp;
		
	// Como la estrategia es best-fit no se asigna a freep el hueco actual
	// sino que se empezara la busqueda desde el primer hueco	
	//freep = p; /* estrategia next-fit */
}

void *xrealloc(void * ptr, size_t size)
{
	printf("Antes de realloc\n");
	imprimirHuecos();

	Header *bp, *p;
	
	bp = (Header*)ptr - 1;

	size_t nunits = (size+sizeof(Align)-1)/sizeof(Align) + (sizeof(Header)-1)/sizeof(Align) + 1;

	printf("Me has pedido %d unidades, %d bytes\n",nunits, size);
	printf("Bp, dir: %x, size: %d\n",bp, bp->s.size);

	for (p= freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr){
		if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
			break;
	}
	printf("p, dir: %x, size: %d\n\n",p, p->s.size);	
	//printf("Pase el for\n");
	
	if(nunits < bp->s.size){ // Caso 1: Piden menos de lo que hay
		printf("Caso 1: reducir tamaño\n");

		if(bp->s.size - nunits > 1 || (Header*)((Align*)bp + bp->s.size) == p->s.ptr){
			Header *generado = (Header*)( (Align*)bp + nunits);
			generado->s.size = bp->s.size - nunits; 
			if((Header*)((Align*)bp + bp->s.size) == p->s.ptr){ // compactar a la derecha
				generado->s.size += p->s.ptr->s.size;
				generado->s.ptr = p->s.ptr->s.ptr;
			}
			else{
				generado->s.ptr = p->s.ptr;	
			}
			p->s.ptr = generado;
			bp->s.size = nunits;
			freep = p;
		}
	} else if(nunits > bp->s.size){ // piden mas de lo que hay
		
		int required_units = nunits - bp->s.size;
		int hay_posterior_adyacente = ((Header*)((Align*)bp + bp->s.size) == p->s.ptr);
		int hay_anterior_adyacente = ((Header*)((Align*)p + p->s.size) == bp);
		Header* next_hole = p->s.ptr;
		int prev_size = next_hole->s.size;
		int units_adyacentes_disponibles = 0;
		
		if(hay_anterior_adyacente) units_adyacentes_disponibles+=p->s.size;
		if(hay_posterior_adyacente) units_adyacentes_disponibles+=next_hole->s.size;

		if(hay_posterior_adyacente && next_hole->s.size >= required_units){  // Caso 2: Piden mas y hay un hueco posterior adyacente con espacio suficiente
			printf("Caso 2: aumentar tamaño y existe hueco parte posterior del bloque, necesito %d\n",required_units);
			if (next_hole->s.size == required_units || next_hole->s.size == (required_units + 1)){ // Si es exacto o queda espacio de uno no genera hueco
				p->s.ptr = next_hole->s.ptr;
			}
			else {
				next_hole->s.size -= required_units;
				Align* p2 = (Align*)next_hole;
				p2 += required_units;
				next_hole = (Header*)p2; 
				//p->s.size = required_units;
			}

			bp->s.size += required_units;
			freep = p; 
		}
		else if(hay_anterior_adyacente && units_adyacentes_disponibles >= required_units){ // Caso 3: Piden mas y hay un hueco anterior adyacente con espacio suficiente
			//printf("Base -> Direccion: %x - Size: %d\n", bp, bp->s.size);
			//printf("Hueco Anterior -> Direccion: %x - Size: %d\n", p, p->s.size);
			printf("Caso 3: aumentar tamaño y existe hueco parte anterior del bloque, necesito %d\n",required_units);
			Header *siguiente, *prev, *new_bp;
			int bp_size = bp->s.size, i;

			if(hay_posterior_adyacente){ 
				//printf("H\n");
				required_units -= next_hole->s.size; 
				siguiente = next_hole->s.ptr;

				p->s.ptr = next_hole->s.ptr;
				bp->s.size += next_hole->s.size;
			}
			else siguiente = next_hole;

			for(prev = p->s.ptr; prev!=p; prev = prev->s.ptr);

			//printf("Prev founded\n");

			if (p->s.size == required_units || p->s.size == (required_units + 1)){ // Si es exacto o queda espacio de uno no genera hueco
				prev->s.ptr = siguiente;
				new_bp = p;
				//printf("Nu nuevo\n");
			}
			else {
				//printf("Required: %d - PSize: %d - Adyacentes: %d\n", required_units, p->s.size,units_adyacentes_disponibles);
				p->s.size -= required_units;
				new_bp = (Header*)((Align*)p + p->s.size);
				new_bp->s.size = bp->s.size + required_units;
				//printf("Nuevo\n");
			}

			copiarDatos(new_bp + 1, bp + 1, sizeof(Align)*(bp_size - (sizeof(Header)-1)/sizeof(Align) - 1));
			//printf("Copio\n");
			bp = new_bp;			
			freep = prev;
		}
		else{ // Caso 4: no hay memoria por ningun lado, hay q buscar o pedir (gracias malloc :) )

			printf("Caso 4: No hay espacio\n");
			printf("BP es: %x\n", bp);
			Header *new_bp;

			printf("Pide malloc\n");
			new_bp =  (Header*)((Header*)xmalloc(size) - 1);
			printf("Sale malloc\n");
			if(new_bp == (NULL )) return NULL;
			printf("Copia\n");
			copiarDatos(new_bp + 1, bp+1, sizeof(Align)*(bp->s.size - (sizeof(Header)-1)/sizeof(Align) - 1));
			printf("Limpia\n");
			xfree(bp +1);
			//freep = bp;
			printf("Asigna\n");
			
			bp = new_bp;



			printf("Despues de realloc\n");
			imprimirHuecos();
			printf("New bp: %x - size: %x\n",bp,bp->s.size);
			char c;
			scanf("%c",&c);

		}
		
	} else {
		printf("Me pidieron el mismo tamanio :p\n");
	}
	printf("Despues de realloc\n");
	imprimirHuecos();
	printf("Bp, dir: %x, size: %d\n",bp, bp->s.size);

	

	return (void*)(bp + 1);
}

//para compilar gcc main.c xalloc.c -o main -w
