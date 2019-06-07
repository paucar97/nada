/*
 * fault.c - Defines the available fault handlers. One example,
 *           fault_random, is provided; be sure to add your handlers 
 *	         to fault_handlers[].
 *            
 */

#include <vmsim.h>
#include <fault.h>
#include <options.h>
#include <physmem.h>
#include <stdlib.h>
#include <stdio.h>

static void fault_random(pte_t *pte, ref_kind_t type);
static void fault_init_random();

static void fault_lru(pte_t *pte, ref_kind_t type);
static void fault_fifo(pte_t *pte, ref_kind_t type);
static void fault_second(pte_t *pte, ref_kind_t type);
static void fault_clock(pte_t *pte, ref_kind_t type);

static void fault_nfu(pte_t *pte, ref_kind_t type);

static void fault_nru(pte_t *pte, ref_kind_t type);

//aca se tienen que definir los algoritmos de reemplazo de paginas
fault_handler_info_t fault_handlers[8] = {
  { "random", fault_random },
  { "lru", fault_lru },
  { "fifo", fault_fifo},
  { "second", fault_second},
  { "clock", fault_clock},
  { "nfu", fault_nfu},
  { "nru", fault_nru},
  { NULL, NULL } /* last entry must always be NULL/NULL */
};

/* Initialize any state needed by the fault handlers here.
 * For example, the random number generator must be initialized for
 * fault_random. 
 * */
void fault_init() {
  fault_init_random();
}

void fault_init_random() {
  long seed = 1234567;
  srandom(seed);
}

/*
Random

*/
void fault_random(pte_t *pte, ref_kind_t type) {
  int pfn;
  pfn = random() % opts.phys_pages; //se elige una pagina random a desalojar
  physmem_evict(pfn, type); // se desaloja la pagina
  physmem_load(pfn, pte, type); // se carga la nueva pagina en ese lugar y cambia valid a 1
}

/*
Least Recently Used
se debe desalojar aquella pagina que no haya sido referenciada durante la 
mayor cantidad de tiempo.

se va a simular el algoritmo mediante un contador, el cual se incrementa despues
de cada referencia. Cada entrada de la tabla de paginas tendra un espacio para
almacenar este contador, despues de cada referencia a memoria, el valor actual 
del contador se almacena en la tabla de paginas para la pagina que se acaba de 
referenciar. Cuando ocurra un fallo de pagina ocurren dos sotuaciones: 

1. si aun no se llena la memoria fisica, hay espacio en algun marco de pagina para 
la entrada que viene
2. si se llena la memoria fisica se examinará todos los contadores 
en la tabla de paginas para encontrar el menor, ese sera el de menor uso reciente
*/
void fault_lru(pte_t *pte, ref_kind_t type) {
  int fframe = 0; //el freeframe mas cercano
  int nfeval = 1; //numero de frames evaluados
  //se busca el fframe 
  while(nfeval <= opts.phys_pages){ 
    nfeval++; 
    if(physmem[fframe] == NULL) break;
    else fframe++;
  }
  
  //si hay un free frame en la memoria fisica
  if(fframe < opts.phys_pages)
    physmem_load(fframe, pte , type);
  //si no hay frames libres se necesita reemplazar
  else{
    int loc = 0;
    int min = physmem[loc]->cont_ref; 
    //se recorre la memoria en busqueda del menor contador
    for (size_t i = 0; i < opts.phys_pages; i++)
    {
      if(min > physmem[i]->cont_ref){
        min = physmem[i]->cont_ref;
        loc = i; 
      }
    }
    //una vez que se tiene el minimo solo se desaloja y se rellena 
    physmem_evict(loc, type); 
    physmem_load(loc, pte, type);
  }
}

/*
First In First Out 
se necesita mantener una lista de todas las paginas actualmente en memoria, en donde 
la llegada mas reciente esta en la parte final y la menos reciente en la parte frontal 
En un fallo de pagina, se elimina la pagina que esta en la parte frontal y la nueva 
pagina se agrega a la parte final de la lista. 

se necesitara que cada entrada en la page table almacene su posicion en la lista la cual
se mide de acuerdo al tiempo en el que llegó y que ademas no se debe actualizar si se 
referencia nuevamente, se guarda solo la primera vez que llega a memoria principal, esto se logra 
guardando una poscicion en la entrada de la tabla de paginas

*/
void fault_fifo(pte_t *pte, ref_kind_t type){
  int fframe = 0; //el freeframe mas cercano
  int nfeval = 1; //numero de frames evaluados
  //se busca el fframe 
  while(nfeval <= opts.phys_pages){ 
    nfeval++; 
    if(physmem[fframe] == NULL) break;
    else fframe++;
  }
  
  //si hay un free frame en la memoria fisica
  if(fframe < opts.phys_pages)
    physmem_load(fframe, pte , type);
  //si no hay frames libres se necesita reemplazar
  else{
    int loc = 0;
    int head = physmem[loc]->pos; 
    //se recorre la memoria en busqueda del menor pos
    for (size_t i = 0; i < opts.phys_pages; i++)
    {
      if(head > physmem[i]->pos){
        head = physmem[i]->pos;
        loc = i; 
      }
    }
    //una vez que se tiene el minimo solo se desaloja y se rellena 
    physmem_evict(loc, type); 
    physmem_load(loc, pte, type);
  }
}

/*
FIFO second chance 
parecido al algoritmo fifo solo ue ahora se tiene que inspeccionar el bit R 
de la pagina mas antigua, o sea la de menor posicion. si es 0 se sustituye la pagina 
si es 1 el bit R se setea a 0 y la pagina se pone al final de la lista, se actualiza 
su posicion o 'tiempo de llegada' a la del tiempo actual. 

se debe tener en cuenta que cuando una pagina es llevada al final de la lista y 
se actualiza su R y su tiempo de llegada, como se esta simulando la lista sobre 
las entradas de la memoria fisica, se debe reordenar la tabla para que la busqueda 
en la memoria principal se realice por orden en la lista fifo. 
 
ademas que el bit R (reference) sera simulado con bitr el cual se setea cada vez que se haga referencia
a una pagina!  
*/
static void fault_second(pte_t *pte, ref_kind_t type){
  int fframe = 0; //el freeframe mas cercano
  int nfeval = 1; //numero de frames evaluados
  //se busca el fframe 
  while(nfeval <= opts.phys_pages){ 
    nfeval++; 
    if(physmem[fframe] == NULL) break;
    else fframe++;
  }  

  //si hay un free frame en la memoria fisica
  if(fframe < opts.phys_pages)
    physmem_load(fframe, pte , type);
  //si no hay frames libres se necesita reemplazar
  else{
    int loc;
    pte_t* temp;

    //a lo más va a dar dos vueltas a la lista 
    while(TRUE){
      // se reordena la lista fifo de paginas en memoria principal 
      for(int k=0;k<opts.phys_pages;k++){
        for(int l=0; l<(opts.phys_pages-k-1);l++){
          if(physmem[l+1]->pos < physmem[l]->pos){
            temp = physmem[l];
            physmem[l] = physmem[l+1];
            physmem[l+1] = temp;
          }	
        }
      }

		  // se recorre la lista inspeccionando el bit R
      for(int i=0;i<opts.phys_pages;i++){
        //si el bit es 1 se setea a 0 y se actualiza la posicion en la lista con el tiempo actual
        if(physmem[i]->reference == 1){
          physmem[i]->reference = 0;		          
          physmem[i]->pos = physmem[i]->cont_ref;
        }
        //si es que encuentra una entrada antigua la saca
        else{
          loc = i;          
          physmem_evict(loc, type);  
          physmem_load(loc, pte, type);
          return;
        }
      }
    }

  }
}


/*
CLOCK 
debido a que el algoritmo de segunda oportunidad es innecesariamente ineficiente. 
el algoritmo consiste en una lista circular de marcos de pagina de la memoria 
principal, con la 'manecilla' apuntando a la pagina mas antigua. 

cuando ocurre un fallo de pagina, la pagina a la que apunta la manecilla se 
inspecciona, si el bit R es 0 la pagina se dejaloja, se inserta la nueva 
pagina en el reloj en su  lugar y la manecilla se avanza una posicion. 

si R es 1 se borra y la manecilla avanza a la siguiente pagina

el proceso se va a repetir hasta encontrar una pagina con R = 0

*/
void fault_clock(pte_t *pte, ref_kind_t type){
  int fframe = 0; //el freeframe mas cercano
  int nfeval = 1; //numero de frames evaluados
  
  static int manecilla = 0;
  
  //se busca el fframe 
  while(nfeval <= opts.phys_pages){ 
    nfeval++; 
    if(physmem[fframe] == NULL) break;
    else fframe++;
  }  

  //si hay un free frame en la memoria fisica
  if(fframe < opts.phys_pages)
    physmem_load(fframe, pte , type);
  //si no hay frames libres se necesita reemplazar
  else{
    int loc; 
		int aguja = manecilla;
    //partiendo de la manecilla se va a buscar un marco con R = 0 
    while(TRUE){
      //si se encuentra un pf physical frame con R = 0 se puede salir guardando su ubicacion
			if(physmem[aguja]->reference == 0){
				loc = aguja;
				break;
			}			
      //si el marco tiene R en 1
			else{
				physmem[aguja]->reference = 0;
			}
      //se repite hasta que se encuentre
			if((aguja+1) == opts.phys_pages )
				aguja = -1;	

      //se actualiza la aguja 
      aguja++; 
    }	

    //se desaloja la pagina en el marco loc 
		physmem_evict(loc, type);  
    //se coloca la pte en el marco loc
		physmem_load(loc, pte, type);
    //se avanza la manecilla 
		manecilla++;
    //si se llega al final la manecilla debe dar la vuelta
		if(manecilla == opts.phys_pages)
			manecilla = 0;	
  }

}

/*
nfu 

se suma R en cada referencia
*/
void fault_nfu(pte_t *pte, ref_kind_t type){ 

  int fframe = 0; //el freeframe mas cercano
  int nfeval = 1; //numero de frames evaluados
  //se busca el fframe 
  while(nfeval <= opts.phys_pages){ 
    nfeval++; 
    if(physmem[fframe] == NULL) break;
    else fframe++;
  }  

  //si hay un free frame en la memoria fisica
  if(fframe < opts.phys_pages)
    physmem_load(fframe, pte , type);
  //si no hay frames libres se necesita reemplazar
  else{
    int loc; 
    int min = physmem[0]->nfu_counter; 
    for(int i=0;i<opts.phys_pages;i++) {
      if(min > physmem[i]->nfu_counter) {
        min = physmem[i]->nfu_counter;
        loc = i;
      }		
    }
    physmem_evict(loc, type);
    physmem_load(loc, pte, type);
  }
}


void fault_nru(pte_t *pte, ref_kind_t type){
  int fframe = 0; //el freeframe mas cercano
  int nfeval = 1; //numero de frames evaluados
  //se busca el fframe 
  while(nfeval <= opts.phys_pages){ 
    nfeval++; 
    if(physmem[fframe] == NULL) break;
    else fframe++;
  }  

  //si hay un free frame en la memoria fisica
  if(fframe < opts.phys_pages)
    physmem_load(fframe, pte , type);
  //si no hay frames libres se necesita reemplazar
  else{
    int loc0 ,loc1, loc2 , loc3 ;
    loc0 = loc1 = loc2 = loc3 = -1;

    for(int i=0;i<opts.phys_pages;i++) {
      if(loc0 == -1 && !physmem[i]->modified  && !physmem[i]->reference) loc0 = i;
      else if(loc1 == -1 && physmem[i]->modified  && !physmem[i]->reference) loc1 = i; 
      else if(loc2 == -1 && !physmem[i]->modified  && physmem[i]->reference) loc2 = i; 
      else if(loc3 == -1 && physmem[i]->modified  && physmem[i]->reference) loc3 = i;       	
    }

    int loc;
    if(loc0 != -1) loc = loc0;
    else if(loc1 != -1) loc = loc1;
    else if(loc2 != -1) loc = loc2;
    else if(loc3 != -1) loc = loc3;

    physmem_evict(loc, type);
    physmem_load(loc, pte, type);
  }

}