// Encoding: ISO 8859-2

// enumdny.c
// Reseni IJC-DU1, priklad a), 14.3.2008
// Autor: Matus Kontra, FIT
// Verzia prekladaca: 4.2.3
// popis - implementacia funkcii modulu enumdny...

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <ctype.h>
#include <string.h>

#include "enumdny.h"
#include "error.h"

#define ALLOC_BLOCK 16

const static char * daysASCII[] = {
		               "Po","Ut","St","Ct","Pa","So","Ne"
		             };
		      
const static char * daysCS[] = {
		             "Po","Út","St","Èt","Pá","So","Ne"
		          };
		      
const static char * daysCSLong[] = {
		                "Pondìlí",
				"Úterí",
				"Støeda",
				"Ètvrtek",
				"Pátek",
				"Sobota",
				"Nedìle"
		    	      };
		       
const static char * daysASCIILong[] = {
				    "Pondeli",
				    "Uteri",
				    "Streda",
				    "Ctvrtek",
				    "Patek",
				    "Sobota",
				    "Nedele"
				 };
				 
void
PrintDayShort ( enum dny den )
{
    if ( den < 1 || den > 7 )
	{
	    Error ( "PrintDay: hodnota %d mimo rozsah\n" , den );
	} else 
	{
	    printf ( "%s" , daysCS[ den-1 ] );
	}
}		
		
void
PrintDayShortASCII ( enum dny den )
{
    if ( den < 1 || den > 7 )
	{
	    Error ( "PrintDay: hodnota %d mimo rozsah\n" , den );
	} else 
	{
	    printf ( "%s" , daysASCII[ den-1 ] );
	}
}	

void
PrintDayLong ( enum dny den )
{
    if ( den < 1 || den > 7 )
	{
	    Error ( "PrintDay: hodnota %d mimo rozsah\n" , den );
	} else 
	{
	    printf ( "%s" , daysCSLong[ den-1 ] );
	}
}

void
PrintDayLongASCII ( enum dny den )
{
    if ( den < 1 || den > 7 )
	{
	    Error ( "PrintDay: hodnota %d mimo rozsah\n" , den );
	} else 
	{
	    printf ( "%s" , daysASCIILong[ den-1 ] );
	}
}
			
enum dny
ReadDay (void)
{
    enum dny den;
    char * userInput;
    int i, c, found;    

    char *l = setlocale (LC_ALL, "cs_CZ.ISO-8859-2");
    if (l == NULL)
	Error("setlocale: Nelze nastavit ceskou lokalizaci\n");
	
    while ( isspace ( c = getc (stdin) ) );
    
    ungetc ( c, stdin );
    
    i = 0;
    userInput = NULL;
    while ( isalpha ( c = getc (stdin) ) && !feof (stdin) )
	{
	 if ( i % ALLOC_BLOCK == 0 )
	    {
		userInput = (char *) realloc ( userInput, sizeof(char) * ( i + ALLOC_BLOCK ) );
	    }
	 if ( i == 0 ) {
		*(userInput + i) = toupper ( (char) c );
	    }
	    else
	    {
		*(userInput + i) = tolower ( (char) c );
	    }	    
	i++;
	}

    if ( userInput == NULL )
	return 0;
    	
    *(userInput + i) = '\0';
    
    den = 0;
    found = 0;
    if ( strlen ( userInput ) > 2 ) 
	{
	    for ( i = 0; i < 7; i++ )
		if ( strcmp ( userInput, daysCSLong[i] ) == 0 )
		    {
		    den = i + 1;
		    found = 1;
		    }
	    if ( found == 0 )
	    {
		for ( i = 0; i < 7; i++ )
		    if ( strcmp ( userInput, daysASCIILong[i] ) == 0 )
			{
			den = i + 1;
			}
	    }
	} else
	{
	    for ( i = 0; i < 7; i++ )
		if ( strcmp ( userInput, daysCS[i] ) == 0 )
		    {
		    den = i + 1;
		    found = 1;
		    }
	    if ( found == 0 )
	    {
		for ( i = 0; i < 7; i++ )
		    if ( strcmp ( userInput, daysASCII[i] ) == 0 )
			{
			den = i + 1;
			}
	    }
	}
        
    free ( userInput );
    
    return den;
    
}   

 

