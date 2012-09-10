/*
	filename: htable-it-next.c
	meno: Kontra Matus	
	fakulta: FIT
	uloha: 2 - B
	17.4.2008
	compiler: GCC-4.2.3
*/

#include <stdlib.h>
#include "htable.h"

/* htable_next
	-vracia novy platny prvok alebo iterator zhodny s htable_end ak presiel za posledny zaznam
	-najskor prehlada sucasny index a vrat novy iterator
	-ak nenajde v sucasnom indexe platny zaznam prechadza kazdym nasledujucim indexom
*/

inline
htable_iterator
htable_next ( htable_iterator iter)
{
	unsigned int i = iter.index;
	i++;
	
	htable_iterator tmp = 
	{ 
		.dest_table = iter.dest_table ,
		.index = iter.index
	};

	if ( iter.curr_ptr->next != NULL )
	{
		tmp.curr_ptr = iter.curr_ptr->next;
	}

	else
	{
		if ( iter.index >= iter.dest_table->size )
			return (tmp = htable_end ( iter.dest_table ) );

		while ( i <= iter.dest_table->size ) 
		{

		if ( i == iter.dest_table->size ) 
		{
			return ( tmp = htable_end ( iter.dest_table ) );
		} else 
		{
			if ( *(iter.dest_table->headers + i) != NULL ) break;
		}

		i++;
		}
	
		if ( i < iter.dest_table->size )
		{
			tmp.index = i;
			tmp.curr_ptr = *(iter.dest_table->headers + i);
		}
						
	}

	return tmp;
}

