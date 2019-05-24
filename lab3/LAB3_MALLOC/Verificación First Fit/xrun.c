#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <math.h>

void *xmalloc (size_t nbytes);
void xfree(void * ptr);

int main(void)
{ unsigned int x,unidad,base;
  /*unsigned int *pt;*/

  unidad=1024; 
  x=0;
  unsigned int* pt[8];
  unsigned int* pt1;
  do {
    base=pow(2,x)+.5;
    if((pt[x]=(unsigned int*)xmalloc(base*unidad))) 
       fprintf(stdout,"Se solicitaron %d bytes y estan ubicados en %p\n",base*unidad,pt[x]);
    else
       fprintf(stderr,"No hay suficiente memoria\n");       
    x++; }
  while(x<=7);
  //pt1 = (unsigned int*)xmalloc(unidad);
  fprintf(stdout,"Liberare 2K = %p\n",pt[5]);
  xfree(pt[1]);
  //fprintf(stdout,"Liberare 2K = %p\n",pt[1]);
  //fprintf(stdout,"ME libere\n");
  xfree(pt[3]);
  xfree(pt[5]);
  pt1 = (unsigned int*)xmalloc(32*unidad);
  fprintf(stdout,"Se solicitaron %d bytes y estan ubicados en %p\n",unidad*2,pt1);
  /*fprintf(stdout,"Se solicitaron %d bytes y estan ubicados en %p\n",unidad,pt[5]);*/
  //pt1 = (unsigned int*)xmalloc(unidad);
  //fprintf(stdout,"Se solicitaron %d bytes y estan ubicados en %p\n",unidad,pt1); 
  //pt1 = (unsigned int*)xmalloc(16*63); 
  //fprintf(stdout,"Se solicitaron %d bytes y estan ubicados en %p\n",unidad,pt1); 
  //pt1 = (unsigned int*)xmalloc(16*240);
  //fprintf(stdout,"Se solicitaron %d bytes y estan ubicados en %p\n",unidad,pt1);
  exit(0);                  
  
}


