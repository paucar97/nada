#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <math.h>

void *xmalloc (size_t nbytes);
void xfree(void *ap);

int main(void)
{ unsigned long long int x,unidad,base;
  unsigned long long int *pt[10], *qt[10];

  unidad=1024; 
  x=0;
  base=pow(2,5)+.5;
  do {
    if((pt[x]=(unsigned long long int *)xmalloc(base*unidad))) 
       fprintf(stdout,"Se solicitaron %lld bytes y estan ubicados en %p\n ",base*unidad,pt[x]);
    else
       fprintf(stderr,"No hay suficiente memoria\n");       
    x++; }
  while(x<=10);

  xfree(pt[3]);
  xfree(pt[6]);

  x=0;
  fprintf(stdout,"\n\n");
  do {
    if((qt[x]=(unsigned long long int *)xmalloc(base*unidad))) 
       fprintf(stdout,"Se solicitaron %lld bytes y estan ubicados en %p\n ",base*unidad,qt[x]);
    else
       fprintf(stderr,"No hay suficiente memoria\n");       
    x++; }
  while(x<=10);

  exit(0);                  
  
}


