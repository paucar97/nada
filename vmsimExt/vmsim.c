//#include <config.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <vmsim.h>
#include <util.h>
#include <options.h>
#include <pagetable.h>
#include <physmem.h>
#include <stats.h>
#include <fault.h>

void init();
void test();
void simulate();

/* refs per '.' printed */
const static uint dot_interval = 100;
const static uint dots_per_line = 64;


/*
En el archivo vmsim.c la función principal ( main ) simula el trabajo de la unidad de memoria virtual:
lee referencias de memoria desde un archivo de trazo (trace file) y chequea si esta referencia 
se encuentra en la memoria. 
Si está en memoria, algunas estadísticas se actualizan. 
Si no se encuentra en memoria, ocurre un fallo de página y el manejador de fallos de páginas es invocado.
El manejador de fallos de página usa el algoritmo para desalojar una página de la memoria 
y hace espacio para la página solicitada. 
Muchos algoritmos de reemplazo de página pueden ser usados con el simulador.
Usted debe especificar el algoritmo de reemplazo a ser usado cuando ejecuta el simulador 

(observe la salida de ./vmsim -h ).

The data is in the format [Process ID], [Mode], [Virtual Address]

Ejemplos de ejecucion: 
./vmsim random example_trace.txt

Where, ’3’ represents the number of memory frames. ’32’ represents
the page size. The page size should always be a power of 2.
exampletrace.txt is the input trace file and random stands for the al-
gorithm we choose to run.
*/

int main(int argc, char **argv) {
  options_process(argc, argv);
  if (opts.test) {
    test();
    printf("Tests done.\n");
    exit(0);
  }

  init();
  simulate();
  stats_output();

  return 0;
}

void init() {   
  pagetable_init();
  physmem_init();
  stats_init();
  fault_init();
}

void test() {
  printf("Running vmtrace tests...\n");
  util_test();
  stats_init();
  pagetable_test();
}

/*Devuelve el tipo de referencia especificado*/
ref_kind_t get_type(char c)
{
	if (c == 'R') return REF_KIND_LOAD;
	if (c == 'W') return REF_KIND_STORE; 
	return REF_KIND_CODE;
}

void print_Line(){
  for (size_t i = 0; i < 90; i++)
    printf("=");  
  printf("\n");  
}

void simulate() {
  uint pid;
  char ch;
  vaddr_t vaddr;
  ref_kind_t type;
  pte_t *pte;
  fault_handler_t handler; //para el manejador de fallos de pagina
  uint count = 0; //cuenta las referencias que se hacen
  FILE *fin = NULL;	
  
  int posfifo = 0; //guarda la posicion en la lista fifo

  handler = opts.fault_handler->handler;
  

  //#ifdef DEBUG
  char response[20];
  uint pgfault=FALSE;
  //#endif

  if ((fin=fopen(opts.input_file, "r")) == NULL) {
	  fprintf(stderr, "Could not open input file %s.\n", opts.input_file);
    exit(1);
  }

  //BORRAR ESTO
  printf("\nvaddr (Virtual Address) tiene %d bits, los primeros %d bits para vfn (Virtual Frame Number), y los %d bits siguientes para el offset (log_2(pagesize=%d))\n\n",
	addr_space_bits, vfn_bits, log_2(opts.pagesize), opts.pagesize);

  while (fscanf(fin, "%d, %c, %x", &pid, &ch, &vaddr) != EOF) {
    type = get_type(ch); 
	  stats_reference(type);
    
	  count++;
    if (opts.verbose && (count % dot_interval) == 0) {
      /* this process has no measurable effect on running time */
      printf(".");
      fflush(stdout); 
      if ((count % (dots_per_line * dot_interval)) == 0) { 
	      printf("\n");
	      fflush(stdout);
      }
    }


    //se realiza la busqueda de la direccion virtual en la pagetable
    pte = pagetable_lookup_vaddr(vaddr_to_vfn(vaddr), type);
    
    //#ifdef DEBUG
    print_Line();
    printf("Referencia nro %d con pid:%d  mode:%c  vaddr:0x%x vfn:0x%x(=primeros %d bits del %d-bit vaddr)\n",
    count, pid, ch, vaddr, vaddr_to_vfn(vaddr), vfn_bits, addr_space_bits);
    print_Line();
    //printf("Got the count=%dth memory ref with pid:%d mode:%c vaddr:0x%x vfn:0x%x\n",
    //count, pid, ch, vaddr, vaddr_to_vfn(vaddr));
    
    pgfault = !pte->valid;
    
    printf("Ocurrio un page %s. ver memoria luego de manejar el fallo? y or n: ",
    pgfault ? "fault":"hit");
    
    scanf("%s", response);
    //#endif

    //si la entrada no es valida (no esta en memoria real, se llama a handler)
    if (!pte->valid) {
      /* Fault
        Invocando a handler() se transfiere el control al manejador de 
        fallos de página definido en ese momento.
      */
      stats_miss(type);
      handler(pte, type);

      //se agrega a la memoria principal, por ende se pguarda la posicion fifo
      pte->pos = posfifo++;
    } 
    //si la entrada se encuentra en memoria principal 
    //else {
      
    //}

    pte->reference = 1; //se actualiza la referencia a la entrada de la tabla de paginas
    
    if(type == REF_KIND_STORE) 
      pte->modified = 1; 

    //se actualiza el contador para LRU
    pte->cont_ref = count; //se usa el contador de referencias 
    

    //nfu actualiza los contadores
	  for(int i=0;physmem[i] != NULL && i < opts.phys_pages;i++){
      physmem[i]->nfu_counter += physmem[i]->reference;
    }

    if (type == REF_KIND_STORE)
      pte->modified = TRUE;

    //#ifdef DEBUG
    //printf("Page %s", pgfault? "Fault!\n": "Hit!\n");
    if (response[0]=='Y' || response[0]=='y') {
	    pagetable_dump();
	    physmem_dump();
	    response[0]='N';
    }
    //#endif


    if (opts.limit && count >= opts.limit) {
      if (opts.verbose)
	      printf("\nvmsim: reached %d references\n", count);
      break;
    }

  }
}

