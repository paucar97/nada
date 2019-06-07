/*
 * pagetable.c -  Modela la tabla de páginas virtuales; se utiliza en cada referencia 
 *                para buscar el pte_t y descubrir si está en la memoria o no.
 *                Sorprendentemente, la tabla de 2 niveles y una tabla optimizada de 1
 *                nivel parecen tener el mismo rendimiento.
 */

//#include <config.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <vmsim.h>
#include <util.h>
#include <options.h>
#include <pagetable.h>
#include <stats.h>

typedef struct _pagetable_level {
  uint size; // tamaño del nivel de tabla de pagina
  uint log_size; //# of bits del tamaño del nivel de tabla de pagina
  bool_t is_leaf;
} pagetable_level_t;

/* 
  Define una tabla de páginas multinivel. Esta es la definición 
  más grande que puede tener cada nivel. pagetable_init puede optar 
  por reducir los tamaños, si se necesitan menos bits 
  (porque el tamaño de la página es más grande)
*/

pagetable_level_t levels[3] = {
  { 4096, 12, FALSE },// el nivel 0 tendra 12 bits
  { 4096, 12, FALSE },// el nivel 1 tendra 12 bits
  { 256, 8, TRUE } /* if log_2(pagesize) < 8, need 3 level pagetable , 
                      el maximo numero de bits en el virtual frame number es 32*/
};

/* vfn_bits is number of bits in the virtual frame number *
 * vfn_bits should be sum of log_size fields of all levels */
uint vfn_bits;

/* Structure representing our multi-level pagetable */
typedef struct _pagetable {
  void **table; /* If lowest-level, array of pte_t pointers;
		               otherwise array of pagetable_t pointers. */
  int level;
} pagetable_t;

/*

root_table->table es la actual tabla de paginas 
Use pte_t *pte=(pte_t *) root_table->table[i] to access each pte entry

*/
static pagetable_t *root_table;


pte_t *pagetable_lookup_helper(uint vfn, uint bits, uint masked_vfn,
			                          pagetable_t *pages, ref_kind_t type);

pte_t *pagetable_new_pte(uint vfn);

pagetable_t *pagetable_new_table (int level);

inline uint getbits(uint x, int p, int n);

void pagetable_test_entry(uint vfn, int l1, int l2);

///////////////////////////////////////////////////////////////////////////////////

void pagetable_init() {
  int level;
  uint page_bits, bits;
  
  //numero de bits para representar el size o el offset de lapagina
  page_bits = log_2(opts.pagesize);
  
  if (page_bits == -1) {
    fprintf(stderr, "vmsim: Pagesize must be a power of 2\n");
    abort();
  }
  //numero de bits para representar el numero de pagina virtual
  vfn_bits = addr_space_bits - page_bits;

  bits = 0;
  level = 0;
  while (1) {
    bits += levels[level].log_size;
    //si con el primer nivel de la tabla es suficiente para contener el numero de paginas virtuales
    if (bits >= vfn_bits)
      break;
    level++;
  }
 
  levels[level].log_size = levels[level].log_size - (bits - vfn_bits);
  levels[level].size = pow_2(levels[level].log_size);
  levels[level].is_leaf = TRUE;
  
  if (opts.test) {
    int i;
    printf("vmsim: vfn_bits %d, %d level table\n", vfn_bits, level+1);
    for (i=0; i<=level; i++) {
      printf(" level %d: %u bits (%u entries)\n", i, levels[i].log_size,
      levels[i].size);
    }
  }  
  root_table = pagetable_new_table(0);
}

pagetable_t *pagetable_new_table(int level) {
  pagetable_t *table;
  pagetable_level_t *config;
  config = &levels[level];
  assert(config);
  
  table = malloc(sizeof(struct _pagetable));
  assert(table);

  table->table = calloc(config->size, sizeof(void*));
  assert(table->table);

  table->level = level;

  return table;
}

/* Busca la entrada de la tabla de paginas para la pagina virtual dada.
 * If the page is not in memory, will have valid==0.
 * If the vfn has never been seen before, will create a new pte_t with the given vfn and valid==0.
 * type es para el seguimiento estadistico.
 */
pte_t *pagetable_lookup_vaddr(uint vfn, ref_kind_t type) {
  return pagetable_lookup_helper(vfn, 0, vfn, root_table, type);
}

/* Recursively search the pagetables. Crea cualquier entrada (ya sea los niveles de
 * la tabla de la página o el propio pte_t) que faltan en la búsqueda.
 * Returns the pte_t at the given vfn (or a new one if none was there
 * previously)
 * vfn - Virtual frame number
 * bits - Number of (high) bits already considered by higher levels.
 * pages - The pagetable for this level.
 */
pte_t *pagetable_lookup_helper(uint vfn, uint bits, uint masked_vfn, pagetable_t *pages, ref_kind_t type) {
  uint index;
  int log_size;
  /**/
  log_size = levels[pages->level].log_size;
  index = getbits(masked_vfn, vfn_bits - (1+bits), log_size);
  bits += log_size;
  masked_vfn = getbits(masked_vfn, vfn_bits - (1+bits), vfn_bits - bits);
  
  if (levels[pages->level].is_leaf) {
    if (pages->table[index] == NULL) {
      /* falla Obligatoria - first access */
      stats_compulsory(type);
      //se crea una nueva entrada en ese indice
      pages->table[index] = (void*)pagetable_new_pte(vfn);
    }
    return (pte_t*)(pages->table[index]);
  } else {
    if (pages->table[index] == NULL) {
      pages->table[index] = pagetable_new_table(pages->level+1);
    }
    return pagetable_lookup_helper(vfn, bits, masked_vfn, pages->table[index],type);
  }
}

/*genera un nuevo page table entry*/
pte_t *pagetable_new_pte(uint vfn) {
  pte_t *pte;
  pte = (pte_t*)(malloc(sizeof(pte_t)));
  assert(pte);

  pte->vfn = vfn;
  pte->pfn = -1;
  pte->valid = FALSE;
  pte->modified = FALSE;
  pte->reference = 0;
  pte->nfu_counter = 0;
  
  return pte;
}

void pagetable_test() {
  printf("Testing pagetables\n");
  pagetable_init();
  assert(root_table);

  if (vfn_bits == 22) {
    pagetable_test_entry(0, 0, 0);
    pagetable_test_entry(1023, 0, 1023);
    pagetable_test_entry(1024, 1, 0);
    pagetable_test_entry((1 << vfn_bits) - 1, levels[0].size-1, levels[1].size-1);
    pagetable_test_entry((1 << vfn_bits) - 2, levels[0].size-1, levels[1].size-2);
    pagetable_test_entry((1 << vfn_bits) - 1024, levels[0].size-1, 0);
    pagetable_test_entry((1 << vfn_bits) - 1025, levels[0].size-2, levels[1].size-1);
  }
}

void pagetable_test_entry(uint vfn, int l1, int l2) {
  pte_t *pte;
  printf("Looking up %u\n", vfn);
  pte = pagetable_lookup_vaddr(vfn, REF_KIND_CODE);
  assert(pte && pte->vfn == vfn);
  assert(root_table->table[l1]);
  assert(((pagetable_t*)root_table->table[l1])->table[l2]);
  assert(((pte_t*)((pagetable_t*)root_table->table[l1])->table[l2])->vfn == vfn);
}


/*
*/
void pagetable_dump() {
  //assert(root_table);
  //assert(root_table->level==0);
  uint vfn_bits=addr_space_bits-log_2(opts.pagesize);
  uint pt_size=pow_2(vfn_bits);
  printf("\nCampos del page table entry. valid : vfn : pfn : modified : reference\n");
  uint i;
  for(i=0; i< pt_size;i++) {
	  if(root_table->table[i]) {
		  pte_t *pte=(pte_t *) root_table->table[i];
		  printf("pagetable[0x%x] -> %d : 0x%x : 0x%x : %d : %d\n",  
			  i,
			  pte->valid,
			  pte->vfn,
			  pte->pfn,
			  pte->modified,
			  pte->reference);
	  }
  }
}