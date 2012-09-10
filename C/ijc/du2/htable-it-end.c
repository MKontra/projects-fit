/*
	filename: htable-it-end.c
	meno: Kontra Matus	
	fakulta: FIT
	uloha: 2 - B
	17.4.2008
	compiler: GCC-4.2.3
*/

#include <stdlib.h>
#include "htable.h"

/* htable_end
	-kvazi ukazatel za posledny element
	-obsah struktury iteratora je jednoznacny t.j. htable_next ho vrati az ked narazi na koniec 
	a nemoze sa stat ze by ho vratil aj pri iteracii cez platne prvky tabulky 
*/

htable_iterator
htable_end ( htable_ptr table )
{
	htable_iterator tmp =
	{
		.dest_table = table,
		.index = table->size,
		.curr_ptr = NULL
	};

	return tmp;
}

