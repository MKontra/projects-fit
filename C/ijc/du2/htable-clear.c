/*
	filename: htable-clear.c
	meno: Kontra Matus	
	fakulta: FIT
	uloha: 2 - B
	17.4.2008
	compiler: GCC-4.2.3
*/

#include "htable.h"
#include <stdlib.h>

/* htable_clear
	- "vycisti" tabulku
	- postupne vymazava vsetky linearne zoznamy a uvolnuje kluce
*/

void
htable_clear ( htable_ptr table )
{
	unsigned int i;
	htable_listitem_ptr item;
	htable_listitem_ptr tmp;
	for ( i = 0 ; i < table->size ; i++ )
	{
		item =*( table->headers + i );
		while ( item != NULL )
		{
			tmp = item->next;

			free (item->key);
			free (item); 
			
			item = tmp;
		}
	}

}

