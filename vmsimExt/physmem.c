
/*
 Es la estructura para representar la memoria física junto con algunas funciones, por ejemplo
 para cargar y desalojar páginas.
*/


//#include <config.h>
#include <assert.h>
#include <stdlib.h>

#include <options.h>
#include <pagetable.h>
#include <physmem.h>
#include <stats.h>

pte_t **physmem;

/*Inicializa a null la memoria fisica de acuerdo al numero de paginas
 dadas como options o por default 
 */
void physmem_init() {
  physmem = (pte_t**)(calloc(opts.phys_pages, sizeof(pte_t*)));
  assert(physmem);
}

/* Get an array of pte_ts representing physical memory.
 * Do not modify this array directly; do not modify elements of it directly.
 * Use physmem_evict/physmem_load.
 * There are opts.phys_pages elements in the array.
 * Empty elements are NULL. */
pte_t **physmem_array() {
  return physmem;
}

/* Evict(desaloja) the page at the given pfn from memory. type should specify
 * the type of reference causing the eviction (i.e., the type passed
 * to the fault handler). Will mark the pfn as empty (adecuado for
 * physmem_load). */
void physmem_evict(uint pfn, ref_kind_t type) {
  assert(0 <= pfn && pfn < opts.phys_pages);

  /* No page here - nothing to do */
  if(physmem[pfn] == NULL || !physmem[pfn]->valid) {
    physmem[pfn] = NULL;
    return;
  }

  stats_evict(type);
  if (physmem[pfn]->modified) {
    stats_evict_dirty(type);
  }

  physmem[pfn]->modified = 0;
  physmem[pfn]->valid = 0;
  physmem[pfn] = NULL;
}

/* Load the given page (pte) into the given physical memory slot (pfn).
 * That slot should be empty (ya sea because it has never been used, or
 * because the page there has been evicted). type should specify what
 * kind of reference casused the load. */
void physmem_load(uint pfn, pte_t *new_page, ref_kind_t type) {
  assert(0 <= pfn && pfn < opts.phys_pages);
  assert(new_page && !new_page->valid);
  assert(physmem[pfn] == NULL);

  physmem[pfn] = new_page;

  physmem[pfn]->pfn = pfn;
  physmem[pfn]->reference = 0;
  physmem[pfn]->modified = 0;
  physmem[pfn]->valid = 1;
}


/*
*/

void physmem_dump() {
  uint i;
 // printf("physmem fields pfn:valid:reference:modified\n");
  printf("\nCampos del physmem page table entry. valid : vfn : pfn : modified : reference: nfCount\n");

  for(i = 0 ; i < opts.phys_pages; i++) {
    pte_t *pte=(pte_t *) physmem[i];
    if (pte) {
      printf("physmem[0x%x] -> %d : 0x%x : 0x%x : %d : %d : %d\n",
            i,
            pte->valid,
            pte->vfn,
            pte->pfn,
            pte->modified,
            pte->reference, 
            pte->nfu_counter);
  	}
  }
  printf("\n");
}

