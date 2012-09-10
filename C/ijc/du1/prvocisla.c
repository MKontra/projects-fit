// prvocisla.c
// Reseni IJC-DU1, priklad a), 14.3.2008
// Autor: Matus Kontra, FIT
// Verzia prekladaca: 4.2.3
// popis - program na testovanie vyuzitia makier a unline funkcii, implementacia jednoducheho algoritmu erastenovho sita

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "error.h"

#define buLongSize (sizeof( unsigned long )*8) 
#define N 100000000


#define BitArray( array_name, size ) \
        unsigned long array_name[size/buLongSize + 2] = {size}



#ifndef USE_INLINE
// MAKRA

#define GetBit( array_name, index ) \
	( index > array_name[0] ) ? ( (Error("Index %ld mimo rozsah 0..%lu", (long) index , (unsigned long) array_name[0])), 0) \
				  : ( (array_name[index/buLongSize + 1] >> (index % buLongSize)) & (unsigned long) 1) 
	

#define SetBit( array_name, index , expr) \
	( index > array_name[0] ) ? ( (Error("Index %ld mimo rozsah 0..%lu", (long) index , (unsigned long) array_name[0])), 0) \
				  : ( ((expr) == 0) ? (array_name[index/buLongSize + 1] &= ~( (unsigned long) 1 << (index % buLongSize))) \
						    : (array_name[index/buLongSize + 1] |= ( (unsigned long) 1 << (index % buLongSize) )) \
						    )


#else
//INLINOVKY

inline unsigned int
GetBit( unsigned long array[] , unsigned long index )
{
return ( index > array[0] ) ? ( (Error("Index %ld mimo rozsah 0..%lu", (long) index , (unsigned long) array[0])), 0)
			    : ( (array[index/buLongSize + 1] >> (index % buLongSize)) & (unsigned long) 1); 
}

inline void
SetBit( unsigned long array[] , unsigned long index , int expr )
{
( index > array[0] ) ? ( (Error("Index %ld mimo rozsah 0..%lu", (long) index , (unsigned long) array[0])), 0) 
		     : ( ((expr) == 0) ? (array[index/buLongSize + 1] &= ~( (unsigned long) 1 << (index % buLongSize))) 
				       : (array[index/buLongSize + 1] |= ( (unsigned long) 1 << (index % buLongSize) ))
				       );
}

#endif


int
main ( int argc , char * argv [] )
{

BitArray ( pole , N );

unsigned long i = 2;
unsigned long sqrtN = sqrt(N);

for ( ; i < sqrtN ; i++ )
{
    if ( GetBit (pole,i) == 0 )
    {
	for ( unsigned long n = 2*i ; n < N ; n = n + i ) SetBit ( pole , n , 1 );
    }
}

unsigned long lastprim[10]; 
int cnt = 0;
i = N - 1;
while ( cnt < 10 )
{
    if ( GetBit ( pole, i ) == 0 ) 
    {
	lastprim[cnt++] = i;
    }
i--;
}    
    
for ( i = 10; i > 0; i-- )
    {
	printf("%lu\n",lastprim[i-1]);
    }

return 0;

}

