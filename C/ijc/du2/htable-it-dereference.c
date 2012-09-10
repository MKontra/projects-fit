/*
	filename: htable-it-dereference.c
	meno: Kontra Matus	
	fakulta: FIT
	uloha: 2 - B
	17.4.2008
	compiler: GCC-4.2.3
*/

#include "htable.h"

/* htable_it_dereference
	-vrati ukazatel zo struktury iteratora
*/

inline
htable_listitem_ptr
htable_it_dereference ( htable_iterator i )
{
	return i.curr_ptr;
}

