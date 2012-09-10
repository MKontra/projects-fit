// enumtest.c
// Reseni IJC-DU1, priklad a), 14.3.2008
// Autor: Matus Kontra, FIT
// Verzia prekladaca: 4.2.3
// popis - testovacia aplikacia pre modul enumdny

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#include "error.h"
#include "enumdny.h"

int
main ( int argc, char ** argv )
{
    enum dny den;
    
    char * l = setlocale ( LC_ALL , "cs_CZ.ISO-8859-2" );
    if ( l == NULL )
	{
	    Error( "setlocale: Nelze nastavit ceskou lokalizaci\n" );
	}
    den = ReadDay ();

    PrintDayShort ( den );
    printf ( "\n" );
    
    PrintDayShortASCII ( den );
    printf ( "\n" );
    
    PrintDayLong ( den );
    printf ( "\n" );
    
    PrintDayLongASCII ( den );
    printf ( "\n\n" );
    
    for ( den = Po ; den < 10 ; den++ )
	{
	    PrintDayShortASCII ( den );
	    printf ( "\n" );
	}     
        
    return 0;
}







    

