/*
	filename: htable-hash.c
	meno: Kontra Matus	
	fakulta: FIT
	uloha: 2 - B
	17.4.2008
	compiler: GCC-4.2.3
*/

#include "htable.h"

/*  htable_hash
	-vypocita index do tabulky
*/

unsigned int
htable_hash( const char * key , unsigned int htable_size )
{
          unsigned int h=0;
          unsigned char *p;

          for(p=(unsigned char*)key; *p!='\0'; p++) 
              h = 31*h + *p;

          return h % htable_size;

}

