/*
 * physmem.h - The physmem module models the physical memory pages.
 *
 */

#ifndef PHYSMEM_H
#define PHYSMEM_H

#include <vmsim.h>
#include <pagetable.h>


void physmem_init();

pte_t** physmem_array();

void physmem_evict(uint pfn, ref_kind_t type);

void physmem_load(uint pfn, pte_t *pte, ref_kind_t type);

void physmem_dump();

extern pte_t **physmem;



#endif /* PHYSMEM_H */
