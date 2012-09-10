#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define CLEANUP_PROC_PROC do {	free (filebuf1); \
				free (filebuf2); \
				close (fd1); \
				close (fd2); } while ( 0 );

typedef struct transact {
	int src_id;
	int dst_id;
	double amount;

	int success;
	pid_t process_pid;
} transaction_t, *transaction_ptr;

typedef struct acc {
	double cash;
} account_t, *account_ptr;

typedef struct trans_item {
	transaction_t action;
	struct trans_item * next;
} transaction_item_t, *transaction_item_ptr;

typedef struct trans_list {
	int num;
	transaction_item_ptr head;
	transaction_item_ptr tail;
} transaction_list_t, *transaction_list_ptr;

/* file manipulation methods
*/

int
writeIntFile ( int fd , int num )
{
	int len;
	char buffer[16] = { 0 };
	snprintf( buffer ,16, "%d",num );
	
	int status = 0;
	status = write ( fd, buffer, ( len = strlen ( buffer )) );



	if ( status == -1 ) 
	{
		perror("Chyba pri write(): ");

	}

	return ( status == -1) ? status : len;
}

int
writeDoubleFile ( int fd , double num )
{
	int len;
	char buffer[16] = { 0 };
	snprintf( buffer ,16, "%.2lf",num );

	int status = 0;
	status = write ( fd, buffer, (len = strlen ( buffer )) );

	if ( status == -1 ) 
	{
		perror("Chyba pri write(): ");

	}

	return ( status == -1) ? status : len;
}

int
writeStringFile ( int fd , char * str )
{
	int len;
	int status = 0;
	status = write ( fd, str, len = strlen ( str ) );

	if ( status == -1 ) 
	{
		perror("Chyba pri write(): ");
	}

	return ( status == -1) ? status : len;
}

int 
readIntStream ( char * stream , int * num )
{
	char * c;
	int i = ( int ) strtol ( stream	, &c , 10 );
	*num = i;
	return c - stream;
}

int 
readDoubleStream ( char * stream , double * num )
{
	char * c;
	float i = ( int ) strtod ( stream , &c);
	*num = i;
	return c - stream;
}

//pokusime sa zamknut cely subor blokujucim volanim fcntl
//vraciame 0 pri uspechu
int 
try_lock_file ( int fd )
{
struct flock lock;

lock.l_type = F_WRLCK;
lock.l_whence = SEEK_SET;
lock.l_start = 0;
lock.l_len = 0;		//zamykame cely subor...


int status = 0;

status = fcntl(fd, F_SETLKW , &lock);

if ( status == -1 && !(errno == EACCES || errno == EAGAIN) )
	perror("Chyba pri zamykani fcntl():");

if ( status == -1 && (errno == EACCES || errno == EAGAIN) )
	status = 1;

return status;
}

int 
unlock_file ( int fd )
{
struct flock lock;

lock.l_type = F_UNLCK;
lock.l_whence = SEEK_SET;
lock.l_start = 0;
lock.l_len = 0;		//zamykame cely subor...


int status = 0;

status = fcntl(fd, F_SETLK  , &lock);

if (status==-1)
	perror("Chyba pri fcntl():");

return status;
}

//nacita cely subor do ukazatela (aj naalokuje)
int
readAllocFile( int fd , char ** buffer )
{
	int i = 512;
	int sum = 0;

	*buffer = malloc ( 513 * sizeof ( char ) );
	if ( buffer == NULL )
		return -1;

	while ( i == 512 && ( i = read ( fd , (*buffer) + sum ,512 ) ) > 0 )
	{
		
		sum += i;

		if ( sum % 512 == 0 ) 
		{
			*( buffer ) = realloc ( *buffer , ( sum + 513 ) * sizeof ( char ) );	
			if ( *(buffer) == NULL ) 
			{
				return -1;
				perror("Chyba pri realloc():");
			}
		}

	}
	if ( i == -1 )
	{
		perror("Chyba pri read():");
		return -1;
	}
	*( *(buffer) + sum ) = '\0';
return sum;
}

transaction_list_ptr
create_transaction_list ( void )
{
	transaction_list_ptr tmp = malloc ( sizeof ( transaction_list_t) );

	if ( tmp == NULL ) 
	{
		perror("Chyba pri volani malloc():");
		return NULL;
	}

	tmp->num = 0;
	tmp->head = NULL;
	tmp->tail = NULL;

	return tmp;	

}

transaction_item_ptr
put_transaction_to_list ( transaction_list_ptr list , int src, int dst, float amount )
{

	transaction_item_ptr tmp = malloc ( sizeof ( transaction_item_t ) );
	
	if ( tmp == NULL ) return NULL;

	tmp->action.src_id = src;
	tmp->action.dst_id = dst;
	tmp->action.amount = amount;

	tmp->action.success = 0;
	tmp->action.process_pid = 0;

	tmp->next = NULL;

	if ( list-> head == NULL )
	{	
		list->num=1;
		list->head = tmp;
		list->tail = tmp;

		return tmp;
	}

	list->tail->next = tmp;
	list->tail = tmp;
	list->num += 1;

	return tmp;

}	

transaction_item_ptr
assign_pid_transaction ( transaction_item_ptr transaction , pid_t pid )
{
	if ( transaction == NULL ) return NULL;

	transaction->action.process_pid = pid;
	
	return transaction;
}

transaction_item_ptr
assign_stat_transaction ( transaction_item_ptr transaction , int success)
{
	if ( transaction == NULL ) return NULL;

	transaction->action.success = success;
	
	return transaction;
}

transaction_item_ptr
get_next_transaction_list_item ( transaction_list_ptr list, transaction_item_ptr transaction )
{
	if ( list == NULL || transaction == NULL )
		return NULL;

	return transaction->next;

}

transaction_item_ptr
get_first_transaction_item ( transaction_list_ptr list )
{ 
	if ( list == NULL )
		return NULL;

	return list->head;
}



void
free_first_transaction ( transaction_list_ptr list )
{
	if ( list == NULL )
		return ;

	transaction_item_ptr tmp = list->head;
	list->head = list->head->next;
	list->num -=1;

	free ( tmp );
}
	
void 
clear_list ( transaction_list_ptr list )
{

	if ( list == NULL ) return;

	transaction_item_ptr tmp = get_first_transaction_item ( list ) ;
	transaction_item_ptr tmp2 = NULL;

	while ( tmp != NULL ) {
		tmp2=tmp;
		tmp = get_next_transaction_list_item ( list , tmp );
		free ( tmp2 );
	}

	list->num = 0;
	list->head = NULL;
	list->tail = NULL;

}

void 
free_list ( transaction_list_ptr list )
{
	if ( list == NULL ) return;

	clear_list ( list );

	free (list); 

}

transaction_list_ptr
init_transaction_list_from_buffer( char * buffer , int bufsiz)
{
	transaction_list_ptr new = create_transaction_list ();
	
	if ( new == NULL ) return NULL;

	transaction_item_ptr new_item;
	int buff_pos = 0;
	int stat_src = 1;
	int stat_dst = 1;
	int stat_amn = 1;
	int tmp_src , tmp_dst ;
	double tmp_amn;

	while ( *( buffer + buff_pos ) != '\0' )
	{
		stat_src = readIntStream( buffer + buff_pos, &tmp_src );
		if ( stat_src == 0 || 0) break;
		buff_pos += stat_src;

		stat_dst = readIntStream( buffer + buff_pos, &tmp_dst );
		if ( stat_dst == 0 || 0) break;
		buff_pos += stat_dst;

		stat_amn = readDoubleStream( buffer + buff_pos , &tmp_amn );
		if ( stat_amn == 0 || 0) break;
		buff_pos += stat_amn;

		new_item = put_transaction_to_list ( new , tmp_src, tmp_dst, tmp_amn );
	}

	return new;

}

int
write_transaction_list_to_file( int fd, transaction_list_ptr list )
{
	if ( fd == -1 || list == NULL )
		return -1;
	
	int i = 0;

	for ( transaction_item_ptr tmp = get_first_transaction_item ( list ); tmp != NULL ; tmp = get_next_transaction_list_item ( list , tmp ) )
	{
		writeIntFile( fd, tmp->action.src_id );
		writeStringFile ( fd, " ");
		writeIntFile( fd, tmp->action.dst_id );
		writeStringFile ( fd, " ");
		writeDoubleFile ( fd, tmp->action.amount );
		writeStringFile ( fd, ( tmp->action.success == 0 ) ? " ok\n" : " failed\n" );	
		i++;
	}

return i;
}

transaction_item_ptr
pid_lookup_item ( transaction_list_ptr list, transaction_item_ptr since , pid_t pid )
{

	for ( transaction_item_ptr tmp = ( since == NULL ) ? get_first_transaction_item ( list ) : since ; tmp != NULL ; tmp = get_next_transaction_list_item ( list , tmp ) )
	{
		if ( tmp->action.process_pid == pid ) return tmp;
	}

return NULL;
}

int
process_transaction ( transaction_item_ptr item )
{
int status = 0;
char filename1[16] = {0};
char filename2[16] = {0};
int fd1;
int fd2;
char * filebuf1;
char * filebuf2;
int len1;
int len2;

account_t acc1;
account_t acc2;

snprintf( filename1 ,16, "%d",item->action.src_id );
snprintf( filename2 ,16, "%d",item->action.dst_id );

if ( strcmp (filename1, filename2) == 0 ) return -1;

fd1 = open ( filename1, O_RDWR );
if ( fd1 == -1 )
{	
 	fprintf(stderr,"Nemozem otvorit subor %s: ",filename1);
	perror(NULL);
	return ( status = -1 );
}

fd2 = open ( filename2, O_RDWR );
if ( fd2 == -1 )
{	
 	fprintf(stderr,"Nemozem otvorit subor %s: ",filename2);
	perror(NULL);
	return ( status = -1 );
}

// deadloku predchadzame tym ze sa najskor kazdy proces pokusi zamknut subor uctu s nizsim cislom


while ( 1 )
{	
	int lock1;
	int lock2;
	if ( item->action.src_id < item->action.dst_id ) 
	{
		lock1 = try_lock_file ( fd1 );
		lock2 = try_lock_file ( fd2 );
	}
	else
	{
		lock2 = try_lock_file ( fd2 );
		lock1 = try_lock_file ( fd1 );
	}

	if ( (lock1 == -1) || (lock2 == -1) )	//chyba systemoveho volania fcnt ina ako eint a eacces
	{
		close (fd1);
		close (fd2);
		return -1;
	}

	if ( (lock1 == 0) && (lock2 == 0 ))	//podaril sa nam zamknut oba subory
		break;
	
	//ak fcntl skoncil z ineho dovodu ako nadobudnutie zamku odomkneme a pokusime sa zamknutie zopakovat
	unlock_file ( fd1 );
	unlock_file ( fd2 );
}

len1 = readAllocFile( fd1 , &filebuf1 );
if ( len1 == -1)
{
 	fprintf(stderr,"Chyba pri nacitavani do bufferu\n");
	return -1;
}
len2 = readAllocFile( fd2 , &filebuf2 );
if ( len2 == -1)
{
 	fprintf(stderr,"Chyba pri nacitavani do bufferu\n");

	//cleanup
	free (filebuf1);
	close (fd1);
	close (fd2);
	return -1;
}

if ( readDoubleStream ( filebuf1, &acc1.cash ) == -1 )
{
 	fprintf(stderr,"Chyba pri nacitavani cisla zo suboru\n");

	//cleanup
	CLEANUP_PROC_PROC
	return -1;
}

if ( readDoubleStream ( filebuf2, &acc2.cash ) == -1 )
{
 	fprintf(stderr,"Chyba pri nacitavani cisla zo suboru\n");

	//cleanup
	CLEANUP_PROC_PROC
	return -1;
}	

if ( (ftruncate ( fd1, 0 ) == -1 ) || ( lseek( fd1, 0,SEEK_SET) == -1) )
{
	perror("Chyba pri zmene velkosti suboru alebo posunuti na zaciatok");

	CLEANUP_PROC_PROC
	return -1;
}

if ( (ftruncate ( fd2, 0 ) == -1) || (lseek( fd2, 0,SEEK_SET) == -1))
{
	perror("Chyba pri zmene velkosti suboru alebo posunuti na zaciatok");

	CLEANUP_PROC_PROC
	return -1;
}

if ( acc1.cash < item->action.amount)
{
	status = -1;
}
else
{
	acc1.cash -= item->action.amount;
	acc2.cash += item->action.amount;
}

writeDoubleFile ( fd1 , acc1.cash );
writeDoubleFile ( fd2 , acc2.cash );

unlock_file ( fd1 );
unlock_file ( fd2 );
free (filebuf1);
free (filebuf2);
close (fd1);
close (fd2);

return status;
}


int main(int argc, char *argv[])
{

char * filebuf = NULL;
int buff_len;
int proc_stat = 0;

int file;
if ( ( file = open ( "transakce" , O_RDWR ) ) == -1 ){
	perror("Chyba pri volani open():");
	exit( 1 );
}




if ( (buff_len = readAllocFile( file , &filebuf )) == -1 )
{
	perror("Chyba pri volani readAndAlloc():");
	
	close( file );
	 exit (1);
}

if ( close ( file ) == -1 )
{
	free ( filebuf );
	perror("Chyba pri volani close():");
	exit (1);
}

transaction_list_ptr zoznam = init_transaction_list_from_buffer( filebuf , buff_len );

free ( filebuf );

if ( zoznam == NULL )
{
	printf("Chyba pri vytvarani zoznamu\n");
	exit (1);
}

pid_t tmpid; 
pid_t main_id = getpid();

for ( transaction_item_ptr tmp = get_first_transaction_item ( zoznam ); tmp != NULL ; tmp = get_next_transaction_list_item ( zoznam , tmp ) )
{
 	tmpid = fork();

	if ( tmpid == -1 )
	{
		perror ("Chyba pri forku() :");
		proc_stat = 1;
		break;
	}
	else if ( tmpid == 0 )
	{
		proc_stat = process_transaction (tmp);
		break;
	}
	else if ( tmpid > 0 )
	{
		assign_pid_transaction( tmp , tmpid );
	}
}


//len rodic -> cakaj a vypis
if ( (getpid() == main_id) && (proc_stat == 0) )
{
	int pocet = zoznam->num;
	while ( pocet > 0 )
	{
		int ret_sts = 0;
		int ret_pid = waitpid ( -1, &ret_sts , 0) ;

		transaction_item_ptr tmp = pid_lookup_item ( zoznam, NULL , ret_pid );
		assign_stat_transaction ( tmp, WEXITSTATUS(ret_sts) );
		
		pocet--;
	}

	
	if ( (file = open ( "transakce"  ,O_RDWR | O_TRUNC ) ) != -1 ){
		write_transaction_list_to_file ( file, zoznam );
		close(file);
	}
	else
	{
	perror("Chyba pri open():");
	}

}

if ( (getpid() == main_id) && (proc_stat == 1) )
{
	kill (0, SIGKILL);	//vycistenie procesov pri chybe forku

}

// vsetci -> uvolnite zdroje a vratte
free_list( zoznam);
return ( proc_stat );

}

