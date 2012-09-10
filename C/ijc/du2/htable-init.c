/*
	filename: htable-in-init.c
	meno: Kontra Matus	
	fakulta: FIT
	uloha: 2 - B
	17.4.2008
	compiler: GCC-4.2.3
*/

#include "htable.h"
#include <stdlib.h>


/* htable_init
	-alokuje do pomocneho ukazatela strukturu tabulky
	-alokuje pole ukazatelov na linearne zoznamy a nastavy velkost
*/
	
htable_ptr
htable_init ( int size )
{
	htable_ptr newtab = malloc ( sizeof ( htable_t ) );

	if ( newtab == NULL ) return NULL;

	newtab->size = size;
	newtab->headers = ( htable_listitem_ptr * ) calloc ( size , sizeof ( htable_listitem_ptr ) );
	
	if ( newtab->headers == NULL ) return NULL;

	return newtab;

}

