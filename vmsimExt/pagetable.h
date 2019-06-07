#ifndef PAGETABLE_H
#define PAGETABLE_H

#include <vmsim.h>

//estas dos constantes no son utilizadas en el programa {ignorar}
//const static int pagesize = 4096; 
const static int log_pagesize = 12;

/*Estructura que representa una page table entry*/
typedef struct _pte {
  uint           vfn; /* Virtual frame number */
  uint           pfn; /* Physical frame number si y solo si valid=1 */
  int           reference;  //para simular FIFO second chance 
  bool_t        valid; /* es True si la entrada esta en uno de los marcos de la memoria fisica */
  bool_t        modified;

  //contador para LRU 
  int           cont_ref; //almacena el contador que se actualiza con cada referencia

  //posicion para FIFO 
  int           pos;

  //contador de referencias nfu
  int            nfu_counter;

} pte_t;

void pagetable_init();

pte_t *pagetable_lookup_vaddr(uint vfn, ref_kind_t type);

void pagetable_test();

void pagetable_dump();

#endif /* PAGETABLE_H */
