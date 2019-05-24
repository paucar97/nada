#include <unistd.h>
#include <stdio.h>
#include "buddy.h"

typedef long Align; 

struct hole{
	struct hole *buddy;
	size_t size;
};

typedef struct hole Hole;

struct nodo{
	struct nodo *sig;
	Hole *hueco;
};

struct lista{
	struct nodo *init;
	int size;
};


typedef struct lista Lista;
	
int MAX = 20;
Lista listas[20];

#define NALLOC 1024

static Hole *morecore(size_t nu)
{
	printf("Llamo morecore\n");
	char *cp;
	Hole *up;

	if (nu < NALLOC)	
		nu = NALLOC;
	cp= sbrk(nu * sizeof(Align)); // 8k papu :v
	if (cp == (char *) -1)
		return NULL;
	up = (Hole *) cp;
	up ->size = nu;
	return up;
}

// creo las listas de inodos vacias en un principio, menos la ultima
// en esta se guardara todo el espacio inicial que luego se ira dividiendo
// conforme pidan memoria

void initListas(void){
	int i;
	for(i=0; i<MAX; i++){
		listas[i].size = 0;
		listas[i].init = NULL;
	}
	struct nodo *my_node;
	my_node->sig = NULL;
	my_node->hueco= morecore(MAX);
	listas[MAX-1].init = my_node;
}

// agrega un hueco a la lista

void add(Hole *my_hole, Lista list){
	struct nodo *my_node;
	my_node->hueco = my_hole;
	my_node->sig = NULL;
	
	if (list.init == NULL){
		list.init = my_node;
	} else {
		struct nodo *currNode = list.init;
		struct nodo *prevNode;
		while (currNode !=NULL){
			prevNode = currNode;
			currNode = currNode->sig;
		}
		
		prevNode->sig = my_node;
	}
	
	list.size += 1;	
}

// divide un hueco en 2 huecos, asignandole a cada uno su buddy(el otro hueco)
// y su size
void split(Hole* my_hole,int i){

	Align* p2 = (Align*)my_hole;
	p2 += (my_hole->size)/2;
		
	Hole* new_hole = (Hole*)p2;	
	new_hole->size = (my_hole->size)/2;	
	my_hole->size = (my_hole->size)/2;
	
	new_hole->buddy = my_hole;
	my_hole->buddy = new_hole;
	
	add(my_hole,listas[i]);
	add(new_hole,listas[i]);
}

void* gethole(int i){
	if (i == MAX+1){
		return NULL;
	}
	
	if (listas[i].init == NULL){
		Hole *new_hole = (Hole*)get_hole(i+1);
		split(new_hole,i);
	}
	
	struct nodo *currNode = listas[i].init;
	listas[i].init = currNode->sig;
	
	Hole *first_hole = currNode->hueco;
	
	return (void*)first_hole;
}

void *xmalloc (size_t nbytes)
{
	int i = log(nbytes);
	
	Hole *my_hole = (Hole*)get_hole(i);
	
	return (void*)my_hole;
}

