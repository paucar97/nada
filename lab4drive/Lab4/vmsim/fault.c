/*
 * fault.c - Defines the available fault handlers. One example,
 *           fault_random, is provided; be sure to add your handlers 
 	     to fault_handlers[].
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
static void fault_fifo(pte_t *pte, ref_kind_t type);
static void fault_lru(pte_t *pte, ref_kind_t type);
static void fault_second(pte_t *pte, ref_kind_t type);
static void fault_clock(pte_t *pte, ref_kind_t type);
static void fault_mfu(pte_t *pte, ref_kind_t type);
static void fault_lfu(pte_t *pte, ref_kind_t type);

fault_handler_info_t fault_handlers[8] = {
  { "random", fault_random },
  { "fifo", fault_fifo },
  { "lru", fault_lru},
  { "second", fault_second},
  { "clock", fault_clock},
  {	"mfu", fault_mfu},
  {	"lfu", fault_lfu},
  { NULL, NULL } /* last entry must always be NULL/NULL */
};

/* Initialize any state needed by the fault handlers here.
 * For example, the random number generator must be initialized for
 * fault_random. */
void fault_init() {
  fault_init_random();
}

void fault_init_random() {
  long seed = 1234567;
  srandom(seed);
}

void fault_random(pte_t *pte, ref_kind_t type) {
  int page;
  page = random() % opts.phys_pages;
  physmem_evict(page, type);
  physmem_load(page, pte, type);
}

void fault_fifo(pte_t *pte, ref_kind_t type) {
	//printf("FIFO not implemented yet!\n");

    static int page=0;
    physmem_evict(page, type);
    physmem_load(page, pte, type);
    page = (page+1) % opts.phys_pages;
}


// LRU replacement 
void fault_lru(pte_t *pte, ref_kind_t type) {
	//printf("LRU not implemented yet!\n");

	static int frame = 0;
	int i,location=0;
	int minimum=0;
	static int check = 1;

	if(check <= opts.phys_pages)
	{
		physmem_load(frame, pte, type);
		frame = frame + 1;
		check = check + 1;			
	}
	else
	{	
		minimum = physmem[0]->counter;
		for(i=0;i<opts.phys_pages;i++)
		{
			if(minimum > physmem[i]->counter)
			{
				minimum = physmem[i]->counter;
				location = i;
			} 
		}

		physmem_evict(location, type);		
		physmem_load(location, pte, type);
	}
}


//least frequetly used algorithm - LFU
void fault_lfu(pte_t *pte, ref_kind_t type) {

	//printf(" \n\n lfu not implemented yet \n\n");

	static int frame = 0;
	
	int i;

	pte_t *test=(pte_t*)malloc(sizeof(pte_t));

	//int min_frequency = 0;
	int loc = 0;

	static int check = 1;
	
	//for FAULT
	if(check <= opts.phys_pages)
	{
		physmem_load(frame, pte, type);
		
		frame = frame + 1;
		check = check + 1;			

	}
	else
	{

		//min_frequency = physmem[0]->frequency;
		test->frequency = physmem[0]->frequency;
		for(i=0;i<opts.phys_pages;i++)
		{

			if(test->frequency > physmem[i]->frequency)
			{
				test->frequency = physmem[i]->frequency;
				loc = i;
				//min_counter = physmem[i]->frequency;
				test->c = physmem[i]->c;
			}

			
		}

		//to maintain FIFO order
		for(i=0;i<opts.phys_pages;i++)
		{
			if((physmem[i]->frequency == test->frequency) && (physmem[i]->c < test->c))
				loc = i;
		}

		physmem_evict(loc, type);
  		physmem_load(loc, pte, type);
		
	
	}	


}


// LRU replacement 
void fault_mfu(pte_t *pte, ref_kind_t type) {
	//printf("LRU not implemented yet!\n");

	static int frame = 0;
	int i,location=0;
	int maximum=0;
	static int check = 1;

	if(check <= opts.phys_pages)
	{
		physmem_load(frame, pte, type);
		frame = frame + 1;
		check = check + 1;			
	}
	else
	{	
		maximum = physmem[0]->counter;
		for(i=0;i<opts.phys_pages;i++)
		{
			if(maximum < physmem[i]->counter)
			{
				maximum = physmem[i]->counter;
				location = i;
			} 
		}

		physmem_evict(location, type);		
		physmem_load(location, pte, type);
	}
}

void fault_second(pte_t *pte, ref_kind_t type){
    static int page=0;
    while(1){
    	//printf("Bit de referencia: %d\n", physmem[page]->reference);
        if(physmem[page]!=NULL && physmem[page]->reference==1){
            printf("Page =  %d , referencia = %d\n",page,physmem[page]->reference);
            printf("Referencia cambiada a 0\n");
            physmem[page]->reference=0;
            page = (page+1) % opts.phys_pages;
            continue;
        }
        else{
        	printf("Marco escogido: %d\n", page);
            physmem_evict(page, type);
            physmem_load(page, pte, type);
            page = (page+1) % opts.phys_pages;
            break;
        }
    }
    //page++;
	//if(page == opts.phys_pages)
	//	page = 0;
    //page = (page+1) % opts.phys_pages;
}

static void fault_clock(pte_t *pte, ref_kind_t type) {

	static int clock_hand=0;
    while(1){
    	//printf("Bit de referencia: %d\n", physmem[page]->reference);
        if(physmem[clock_hand]!=NULL && physmem[clock_hand]->used==1){
            printf("Page =  %d , referencia = %d\n",clock_hand,physmem[clock_hand]->used);
            printf("Referencia cambiada a 0\n");
            physmem[clock_hand]->used=0;
            continue;
        }
        else{
        	printf("Marco escogido: %d\n", clock_hand);
            physmem_evict(clock_hand, type);
            physmem_load(clock_hand, pte, type);
            break;
        }
    }
    clock_hand = (clock_hand+1) % opts.phys_pages;
}

