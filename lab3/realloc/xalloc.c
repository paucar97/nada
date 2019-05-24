/*
 *
 * CONTENIDO INICIAL:
 *	Codigo correspondiente a la Seccion 8.7 del libro:
 *	"The C Programing Language", de B. Kernigham y D. Ritchie.
 *
 * En este fichero se incluiran las rutinas pedidas 
 *
 */
#include <stdio.h>
#include <unistd.h>
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

static Header *morecore(size_t nu)
{
	char *cp;
  //, *sbrk(int);
	Header *up;

	if (nu < NALLOC)	
		nu = NALLOC;
	cp= sbrk(nu * sizeof(Header));
	if (cp == (char *) -1) /* no space at all */
		return NULL;
	up = (Header *) cp;
	up ->s.size = nu;
	xfree((void *)(up+1));
	return freep;
}

/* xmalloc: general-purpose storage allocator */
void *xmalloc (size_t nbytes)
{
	Header  *p, *prevp;
	size_t nunits;

	/* 
	   Calcula cuanto ocupara la peticion medido en tama~nos de
	   cabecera (incluyendo la propia cabecera). 
	   El termino "sizeof(Header)-1" provoca un redondeo por exceso.
	   El termino "+ 1" es para incluir la propia cabecera.
	*/
	nunits = (nbytes+sizeof(Header)-1)/sizeof(Header) + 1;


	/* En la primera llamada se construye una lista de huecos con un
	   unico elemento de tama~no cero (base) que se apunta a si mismo */
	if (( prevp = freep) == NULL ) { /* no free list yet */
		base.s.ptr = freep = prevp = & base; 
		base.s.size = 0;
	}

	/*
	   Recorre la lista circular de huecos, empezando por el siguiente al
	   que apunta freep, hasta que encuentra uno que satisface la peticion
	   o da toda una vuelta a la lista (no hay espacio suficiente)
	*/
	for (p= prevp->s.ptr; ; prevp = p, p = p->s.ptr) {
		if (p->s.size >= nunits) {  /* big enough */
			if (p->s.size == nunits)  /* exactly */
			prevp->s.ptr = p->s.ptr;
			else {  /* allocate tail end */
				p->s.size -= nunits;
				p+= p->s.size;
				p->s.size = nunits;
			}
			freep = prevp; /* estrategia next-fit */
			return (void *)(p+1); /* devuelve un puntero a la
						 zona de datos del bloque */
		}
		/* Si ha dado toda la vuelta pide mas memoria y vuelve
		   a empezar */
		if (p == freep) /* wrapped around free list */
			if ((p = morecore(nunits)) == NULL)
				return NULL;  /* none left */
	}
}


/* xfree: put block ap in the free list */
void xfree(void *ap)
{
	Header *bp, *p;

	bp = (Header *)ap - 1;  /* point to block header */

	/*
	   Bucle que recorre la lista de huecos para buscar el hueco
	   anterior al bloque que se va a liberar.
	   Del bucle se sale cuando se encuentran los dos huecos
	   de la lista (el apuntado por p y el apuntado por p->s.ptr)
	   entre los que se incluira el nuevo hueco (el apuntado por bp).
	   Hay dos casos:
		- La direccion del nuevo hueco es mayor (bp > p) o
		menor (bp < p->s.ptr) que la de ningun otro hueco de la
		lista (corresponde al break)
		- La direccion del nuevo hueco esta comprendida entre
		dos huecos de la lista (corresponde a la salida normal
		del for)
	*/

	for (p= freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
		if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
			break;  /* freed block at start or end of arena */ 


	/* Comprueba compactacion con hueco posterior */
	if (bp + bp->s.size == p->s.ptr) {  /* join to upper nbr */
		bp->s.size += p->s.ptr->s.size;
		bp->s.ptr = p->s.ptr->s.ptr;
	} else
		 bp->s.ptr = p->s.ptr;

	/* Comprueba compactacion con hueco anterior */
	if (p + p->s.size == bp) {         /* join to lower nbr */
		p->s.size += bp->s.size;
		p->s.ptr = bp->s.ptr;
	} else
		p->s.ptr = bp;

	freep = p; /* estrategia next-fit */
}


Header* busca_adyacente_der(Header* h) {
      
      Header *aux, *ini;
 
      aux = freep;
      ini = aux;
      while (1) {                    
          /* si encuentra un nodo libre */
          if (aux->s.ptr == h) {
             break; 
          } 
          if (aux == ini) return NULL;
          aux = aux->s.ptr;
      }      
      return aux;
}

Header* busca_adyacente_izq(Header* h) {
      
      Header *aux, *ini;
 
      aux = freep;
      ini = aux;
      while (1) {                    
          /* si encuentra un nodo libre */
          if ((aux->s.ptr + aux->s.ptr->s.size) == h) {
             break; 
          } 
          if (aux == ini) return NULL;
          aux = aux->s.ptr;
      }      
      return aux;
}

Header *aumenta_buffer(void *ptr, size_t size) {
   
   Header *bp, *p, *h, *aux, *y, *nuevo;
   char *ptraux, *ptr2;
  
   int i,aux1;

  
   bp= (Header*)ptr-1;

   
   /* Se desplaza al final del bloque de datos */
   ptr2 = (char*)ptr;  

   /* muevo puntero a una direccion adyacente */  
   aux= bp+bp->s.size;

   h=busca_adyacente_der(aux);
   
   if (h!=NULL)
   {
	/*
	si el tama¤o se suficiente al lado derecho
	se libera el espacio de memoria y se cambia el 
	tama¤o de ptr
	*/
     printf("Entro al buscar derecha");

     if ((h->s.ptr->s.size+ bp->s.size) >= size){    
	h= h->s.ptr->s.ptr;
	bp->s.size= size;	
	return bp;
     }
     else{ /*No hay espacio suficiente en la derecha*/
	y=busca_adyacente_izq(bp);
        if (y!=NULL ){
           /*
		si la suma de los 3 bloques es suficiente
         	*/  		
           if ((y->s.ptr->s.size + h->s.ptr->s.size + bp->s.size) >= size){
		/* copia de datos */
                ptraux = (char*)(y->s.ptr +  1);
                for (i=0; i<bp->s.size - 1; i++) {
                     *ptraux++ = *ptr2++; /* Por confirmar */
		     
                } 
		h=h->s.ptr->s.ptr;
		bp->s.size=size;                
		return bp;		
	   }	  
	}	
     }
   }


   ptr2 = (char*)ptr;
   printf("Entro al final \n");
   /* Cuando no hay espacio suficiente en los adyacentes 
     del bloque */

   nuevo = (Header*)xmalloc(size);
   /* copio datos */

  
   ptraux = (char*)(nuevo+1) ;

   for (i=0; i < (bp->s.size -1)*sizeof(Header) ; i++) {
	printf("i=%d Ptr2 %d  \n",i, *ptr2 );
       *ptraux++ = *ptr2++; /* Por confirmar */
		
   }
   nuevo->s.size=size/sizeof(Header); 
   xfree(ptr);
   
   return nuevo;
}

Header *disminuye_buffer(void *ptr, size_t size) {

  Header *bp, *p, *h,* nuevo;
  char *auxizq, *auxder, *auxini;
  size_t tam;
  int i;

  bp=(Header*)ptr-1;
  p=bp;
  p+=bp->s.size;
  h=busca_adyacente_der(p); /* Te devuelve el anterior al hueco adyacente */
  if (h!=NULL)
  {        
    printf("Entro disminuir  con H \n");

    auxizq  = (char *)p;
    auxizq  -= (bp->s.size*sizeof(Header) - size); 
    auxini  = auxizq;   
    auxder  = (char*)p;
    printf("%p \n", h->s.ptr);
    printf("%p \n", p);
    
    for (i=0; i<sizeof(Header); i++) {
        *auxizq++ = *auxder++; 
    }
    nuevo = (Header*)auxini;
      
    tam=  h->s.ptr->s.size + bp->s.size - size/sizeof(Header);
    
    printf("nuevo tamaño %d tamaño anterior %d tamaño de bloque adyacente %d ",tam,bp->s.size, h->s.ptr->s.size);
  
    /* p->s.ptr=h->s.ptr->s.ptr;*/
    /*nuevo->s.ptr= p->s.ptr;*/
    nuevo->s.size+= tam;
    h->s.ptr=nuevo;
    freep=h;

  } 
  
  bp->s.size=size/sizeof(Header);
  
  return bp;       
}

void *xrealloc(void * ptr, size_t size) {

  Header *bp;
  Header *p = NULL;
  int *aux;
  
  /* Obtiene el encabezador del bloque de datos apuntado
     por ptr */

  bp = (Header*)ptr-1; 

  
 if (sizeof(Header)*(bp->s.size-1) < size) {
      printf("aumenta\n");
      p = aumenta_buffer(ptr,size);
  } 
  else if ( sizeof(Header)*(bp->s.size-1) > size) {
      printf("disminuye\n");    
      p = disminuye_buffer(ptr,size); 
  }
  else {
      printf("iguales\n"); 
      p = (Header*)ptr-1;
  }

  /* Devuelve un puntero al bloque de datos reasignado */
  aux = (int *)(p+1);
 
  return (void*)(p+1);

}


void main(void) {
  int i;
  int *p;

  p = (int*)xmalloc(2*sizeof(int));
  printf("Reservo con xmalloc : %d headers\n", 2*sizeof(int) / sizeof(Header)); 
 
  p[0] = 99;
  p[1] = 99; 
 
  printf(" --> INICIO:\n"); 
  printf(" %d \n",p[0]); 
  printf(" %d \n",p[1]); 

  p = (int*)xrealloc(p,16*sizeof(int));
  printf("Reservo con xrealloc : %d headers\n", 16*sizeof(int) / sizeof(Header)); 

  printf(" --> DESPUES DEL XREALLOC:\n"); 

  for(i=0;i<16;i++) {
        if (i>1) p[i]=i;
	   printf(" %d \n",p[i]); 
  }
 
 
 if (p!=NULL) {   
    xfree(p);
  }
  printf(" --> DESPUES DEL XFREE:\n"); 
  for(i=0;i<16;i++) {
     printf(" %d \n",p[i]); 
  }

}
