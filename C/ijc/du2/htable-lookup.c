/*
	filename: htable-lookup.c
	meno: Kontra Matus	
	fakulta: FIT
	uloha: 2 - B
	17.4.2008
	compiler: GCC-4.2.3
*/

#include "htable.h"
#include <string.h>
#include <stdlib.h>

/*   htable_lookup
	-inline = casto volana, i ked je relativne dlha je vhodnejsie ju pouzit ako inline pretoze
	 sa da predpokladat vela slov na vstupe
	-vyhladava v tabulke prvok podla kluca ak nenajde vytvori
	-prechadza sucasnym indexom = hashom kluca  a porovnava kluce
	-ak nenajde naalokuje novy zaznam a prida ho do zoznamu ( na zaciatok )
*/

inline
htable_listitem_ptr
htable_lookup ( htable_ptr table , char * key )
{

	if ( key == NULL )
		return NULL;	

	unsigned int hash = htable_hash ( key , table->size );
	
	const htable_listitem_ptr data = * (table->headers + hash);

	htable_listitem_ptr tmp = data; 
	
	while ( tmp != NULL )
	{
		if ( strcmp ( tmp->key , key ) == 0 )
			return tmp;
		
		tmp = tmp->next;
	}
	
	htable_listitem_ptr newentry = malloc ( sizeof (htable_listitem) );

	if ( newentry == NULL ) 
		return NULL;

	newentry->key = malloc ( ( strlen ( key ) + 1 ) * sizeof ( char ) ); 

	if ( newentry->key == NULL )
	{
		free ( newentry );
		return NULL;
	}

	strcpy ( newentry->key , key );

	newentry->count = 0;
	newentry->next = data;

	* (table->headers + hash) = newentry;

	return newentry;

}	

