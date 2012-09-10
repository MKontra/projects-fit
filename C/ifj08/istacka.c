/*****************************
 *  Prekladac jazyka IF2008
 *  Matus Kontra (xkontr00) (veduci)
 *  Jozef Kobrtek (xkobrt00)
 *  Patrik Seben (xseben00)
 *  Matej Smetana (xsmeta02)
******************************/ 

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "istacka.h"


//vytvorenie a znicenie
ptr_IStackA 
newStack( int unit_size )
{
  ptr_IStackA newtmp = NULL;
  if ( ( newtmp = ( ptr_IStackA ) malloc ( sizeof (IStackA) ) ) == NULL )
  {
    return NULL;
  }
  
  newtmp -> elem_size = unit_size;
  newtmp -> top_index = 0;
  newtmp -> alloc_size = ALLOC_BLOCK_MULT * unit_size;
  
  if ( (newtmp -> data = malloc (newtmp ->alloc_size)) == NULL )
  {
    free (newtmp);
    return NULL;
  }
  
  return newtmp;
}

int
StackInit ( ptr_IStackA stck, int unit_size)
{
  stck -> elem_size = unit_size;
  stck -> top_index = 0;
  stck -> alloc_size = ALLOC_BLOCK_MULT * unit_size;
  
  if ( (stck -> data = malloc (stck -> alloc_size)) == NULL )
  {
    return STACK_ERROR;
  }
  
  return STACK_OK;
}

int
destroyStack( ptr_IStackA stck )
{

  free(stck -> data);
  return STACK_OK;

}

//push, pop, top ..
int 
Spush( ptr_IStackA stck, void * data )
{
  if ( (stck-> top_index +1)*(stck->elem_size) >= (stck->alloc_size) )
    if( (stck->data = realloc ( stck->data , stck->alloc_size + (ALLOC_BLOCK_MULT * stck->elem_size)) ) == NULL )
      return STACK_ERROR;
  
  stck->alloc_size = stck->alloc_size + (ALLOC_BLOCK_MULT * stck->elem_size);
  memmove( ((char *) stck->data ) + (stck -> top_index)*(stck->elem_size), data, stck->elem_size);

  //printf("ukladal som hodnotu %p\n", (void *) *( (int *)data) );
  //printf("data su na adrese %p\n", stck->data);
  //printf("nova polozka sa uklada na miesto %p\n", ((char *) stck->data ) + (stck -> top_index)*(stck->elem_size));
  //printf("hodnota topu je %d pouzita differencia %d\n",stck -> top_index, (stck -> top_index)*(stck->elem_size));
  (stck -> top_index)++;  
  return STACK_OK;
  
}

void
Spop ( ptr_IStackA stck )
{

  if ( SEmpty (stck) != 1 )
  {
 
  (stck->top_index)--;
  
  if ( stck->top_index*stck->elem_size < (stck->elem_size -32 ))
  {
    //realokacia ?
  }
  }
  
}

void * 
Stop ( ptr_IStackA stck )
{
  //printf("Top bude brat z adresy %p\n",((char *) stck->data ) + (stck -> top_index -1)*(stck->elem_size));
  //printf("diff voci dataptr %d\n",(stck -> top_index -1)*(stck->elem_size) );
  return ((char *) stck->data ) + (stck -> top_index -1)*(stck->elem_size);
}

int
SEmpty ( ptr_IStackA stck )
{
  if ( stck -> top_index != 0)
    return 0;
    
  return 1;

}

//"dalsie nestandardne operacie"
int
Sdepth ( ptr_IStackA stck )
{
  return stck->top_index;
}

void *
SgetTopPtr ( ptr_IStackA stck )
{
  return stck->data;
}

void * 
StopPopCpy ( ptr_IStackA stck ) //okopiruje polozku vrati ukazatel a odstrani zo zasobnika
{
  void * retptr = malloc ( stck->elem_size );
  if ( retptr == NULL )
    return NULL;
  
  memmove( retptr, ((char *) stck->data ) + (stck -> top_index)*(stck->elem_size), stck->elem_size);
  
  Spop(stck);
  return retptr;

}
