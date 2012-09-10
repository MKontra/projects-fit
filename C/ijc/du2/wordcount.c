/*
	filename: wordcount.c
	meno: Kontra Matus	
	fakulta: FIT
	uloha: 2 - B
	17.4.2008
	compiler: GCC-4.2.3
*/
	


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "htable.h"
#include "io.h"


int
main ( int argc , char *argv[] )
{
	htable_ptr table;
	
/*	
	velkost tabulky sa vo vseobecnosti urcuje podla toho aky velky vstup ocakavame
	volanie hasovacie funkcie( zaleziod jej zlozitosti) je efektivnejsie ako prechadzanie linearnim
	zoznamom a volanie strcmp na polia znakov -> pre casto vyuzivanu tabulku na velke mnozstvo dat zvolime vecsiu velkost
	dalej je vhodne pouzit prvocislo nakolko to zmensuje clusterizaciu ( pripad ked sa mnoho klucov nahasuje na ten isty index )
	+ velkost ako prvocislo sa pouziva pri tabulkach s variabilnou velkostou 
*/

	if ( ( table = htable_init( 1031 ) ) == NULL )
	{
		printf("Nedostatok pamate\n");
		exit(1);
	};

	char slovo[101];

	while ( read_word( slovo , 100 , stdin) != 0 )
	{
		htable_listitem_ptr new = htable_lookup ( table , slovo );
		if ( new == NULL )
		{
			printf("Nedostatok pamate\n");
			exit(1);
		}
		
		(new->count)++;		

	}
	

	for (htable_iterator iter = htable_begin( table ); !htable_it_eq ( iter, htable_end ( table ) ) ;iter = htable_next ( iter )){
	printf( "%s\t%d\n" , htable_it_dereference ( iter )->key,htable_it_dereference ( iter )->count);
}
	htable_free ( table ); 

return 0;
}


