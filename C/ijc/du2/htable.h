#ifndef HTABLE_H
#define HTABLE_H

typedef struct listitem * htable_listitem_ptr;

typedef struct listitem {
	char * key;
	int count;
	htable_listitem_ptr next;
} htable_listitem;

typedef struct htable {
	unsigned int size;
	htable_listitem_ptr * headers;
} htable_t, * htable_ptr;

typedef struct iterator {
	htable_ptr dest_table;
	unsigned int index;
	htable_listitem_ptr curr_ptr; 
} htable_iterator, *htable_iterator_ptr;

/* funkcie na pracu s tabulkou */

htable_ptr htable_init ( int size );
inline htable_listitem_ptr htable_lookup ( htable_ptr table , char * key );
void htable_clear ( htable_ptr table );
void htable_free ( htable_ptr table );
htable_iterator htable_begin ( htable_ptr table );
htable_iterator htable_end ( htable_ptr table );
inline htable_iterator htable_next ( htable_iterator );
inline int htable_it_eq ( htable_iterator i1 , htable_iterator i2 );
inline htable_listitem_ptr htable_it_dereference ( htable_iterator i );

/*interne funkcie tabulky */

unsigned int htable_hash ( const char * key , unsigned int htable_size );

#endif // HTABLE_H

