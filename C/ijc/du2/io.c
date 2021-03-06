/*
	filename: io.c
	meno: Kontra Matus	
	fakulta: FIT
	uloha: 2 - B
	17.4.2008
	compiler: GCC-4.2.3
*/

#include <stdio.h>
#include <ctype.h>

#include "io.h"

int
read_word (char *str, int MAX, FILE * istream)
{

  static int flag = 0;

  int i, c;

  if (str == NULL || istream == NULL)
    return 0;

  if (feof (istream) != 0)
    return 0;

  while (isspace (c = fgetc (istream)));

  i = 0;

while ( !isspace ( c ) && !feof ( istream ) && i < MAX )
{
	* ( str + i ) = ( char ) c;

	c = fgetc ( istream ) ;
	i++;	
}

if ( flag == 0 && i > MAX )
{
	flag = 1;
	fprintf( stderr, "Varovanie: Bol prekroceny limit nacitavania slovo bude skratene" );
}

* ( str + i ) = '\0';

while ( i==MAX && !feof ( istream ) && !isspace ( c = fgetc ( istream ) ) );

return i;
}


