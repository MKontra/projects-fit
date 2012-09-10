#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#include <pthread.h>

#include <regex.h>

#define STATIC_ARRAY_SIZE(myArray) (sizeof(myArray)/sizeof(*myArray))
#define MAX_WORKERS 30
#define allocMul 32

#define OP_REQUEST 1
#define RESP_MATCH 1
#define RESP_NOMATCH 2
#define PROT_VER 1

//databza samotna
static struct simpledb db;

typedef struct string
{
	char * str;
	int len;
	int memsize;
} *String;

String
newStr(void)
{
	String retstr = malloc( sizeof( struct string) );
	if (retstr == NULL)
		return NULL;
		
	retstr->str = malloc ( allocMul );
	retstr->str[0] = '\0';
	retstr->memsize = allocMul;
	retstr->len = 0;
	return retstr;
}

String
newFromConstChar(char * uia)
{
	if (uia == NULL)
		return NULL;
	
	String retstr = malloc( sizeof( struct string) );
	if (retstr == NULL)
		return NULL;
	
	int uialen = strlen( uia );
	int allSiz = ((uialen / allocMul) + 1)* allocMul;
	retstr->str = malloc ( allSiz );
	if ( retstr->str == NULL )
		return NULL;
	strcpy( retstr->str, uia );
	retstr->memsize = allSiz;
	retstr->len = uialen;
	return retstr;	
}

String
newStrFromConstStartEnd( char * uia, int start, int end )
{
	
	if (uia == NULL)
		return NULL;	
	
	int uialen = end - start;
		
	if ( uialen < 0 )
		return NULL;	
	
	String retstr = malloc( sizeof( struct string) );
	if (retstr == NULL)
		return NULL;

	int allSiz = ((uialen / allocMul) + 1)* allocMul;
	retstr->str = malloc ( allSiz );
	if ( retstr->str == NULL )
		return NULL;
	memcpy( retstr->str, uia + start , uialen );
	retstr->str[end - start ] = '\0';
	retstr->memsize = allSiz;
	retstr->len = uialen;
	return retstr;
}

String 
newStrFromStr( String str )
{
	if ( str == NULL )
		return NULL;
	
	String retstr = malloc( sizeof( struct string) );
	if (retstr == NULL)
		return NULL;	
	
	int allSiz = ((str->len / allocMul) + 1)* allocMul;
	retstr->str = malloc ( allSiz );
	if ( retstr->str == NULL )
		return NULL;
	memcpy( retstr->str, str->str, str->len + 1 );
	retstr->memsize = allSiz;
	retstr->len = str->len;
	return retstr;	
}

int
emptyStr( String str )
{
	if ( str == NULL )
		return -1;
	
	if ( str->str == NULL )
		return -1;
	
	free( str->str );
	str->len = 0;
	str->memsize = 0;
	return 0;
}

int
freeString( String * strptr )
{
	if (strptr == NULL)
		return -1;
	
	emptyStr(*strptr);
	free( *strptr );
	*strptr = NULL;
	return 0;
}

String
appendTo(String s1, String s2)
{
	int sumlen = (s1->len+s2->len) ;
	int allSiz = (( sumlen / allocMul) + 1)* allocMul;
	s1->str = realloc ( s1->str, allSiz );
	if ( s1->str == NULL )
		return NULL;
	strcat( s1->str, s2->str );
	s1->memsize = allSiz;
	s1->len = sumlen;
	return s1;
}

String
appendChar( String s1, int chval )
{
	int newlen = s1->len+1;
	int allSiz = (( newlen / allocMul) + 1)* allocMul;
	if ( allSiz != s1->memsize )
	{
		s1->str = realloc ( s1->str, allSiz );
		if ( s1->str == NULL )
			return NULL;
	}
	*(s1->str+s1->len) = chval;
	*(s1->str+newlen) = '\0';
	s1->memsize = allSiz;
	s1->len = newlen;	
	return s1;
}

String
mergeStrings( String s1, String s2 )
{
	String retstr = malloc( sizeof( struct string) );
	if (retstr == NULL)
		return NULL;	
	
	int sumlen = (s1->len+s2->len) ;
	int allSiz = (( sumlen / allocMul) + 1)* allocMul;
	retstr->str = malloc ( allSiz );
	if ( retstr->str == NULL )
		return NULL;
	strcpy( retstr->str, s1->str );
	strcat( retstr->str, s2->str );
	retstr->memsize = allSiz;
	retstr->len = sumlen;
	return retstr;
}

int
loadStringConst( String s1, char * uia )
{
	emptyStr( s1 );
	int uialen = strlen(uia);
	int allSiz = ((uialen / allocMul) + 1)* allocMul;
	s1->str = malloc ( allSiz );
	if ( s1->str == NULL )
		return -1;
	strcpy( s1->str, uia );
	s1->memsize = allSiz;
	s1->len = uialen;
	return 0;
}

int
loadString( String s1, String s2 )
{
	emptyStr( s1 );
	
	int allSiz = ((s2->len / allocMul) + 1)* allocMul;
	s1->str = malloc ( allSiz );
	if ( s1->str == NULL )
		return -1;
	strcpy( s1->str, s2->str );
	s1->memsize = allSiz;
	s1->len = s2->len;
	return 0;	
}

String
num_out_rec( int val, int neg )
{
	if ( val == 0 )
	{
		if ( neg )
			return appendChar(newStr(), '-');
		else
			return newStr();
	}
	else if ( val > 0 )
	{
		return appendChar(num_out_rec( val / 10, neg ), (val % 10) + '0' );
	} else
		return num_out_rec(-val, 1);
}

String
num_out( int val )
{
	return num_out_rec(val, 0);
}

// 1;xadamc00;Adamcik;Lukas;FIT
struct simpledb_entry
{
	int id;
	String login;
	String username;
	String surname;
	String focus;
	
	struct simpledb_entry * next;
};

struct simpledb
{
	struct simpledb_entry * head;
};

void
init_db( target )
	struct simpledb * target;
{
	target->head = NULL;		
}

int
insert_entry( target, id, login, username, surname, focus)
	struct simpledb * target;
	int id;
	String login;
	String username;
	String surname;
	String focus;
{
	struct simpledb_entry * ne = malloc ( sizeof( struct simpledb_entry) );
	if ( ne == NULL )
		return -1;
	ne->id = id;
	ne->login = newStrFromStr(login);
	ne->username = newStrFromStr(username);
	ne->surname = newStrFromStr(surname);
	ne->focus = newStrFromStr(focus);
	
	ne->next = target->head;
	target->head = ne;
			
	return 0;
}

String
read_line( src )
	FILE * src;
{
	if ( feof(src) )
		return NULL;
	
	String retval = newStr();
		
	int inchar = 0;
	while ( (inchar = fgetc(src)) != -1 )
	{
		if ( inchar == '\r' )
			break;
		appendChar( retval, inchar );
	}
	
	return retval;
}
	
String *
tokenize_line( line )
	String line;
{
	
	if ( line == NULL || line->len == 0 )
		return NULL;
	
	String * retval = malloc ( 5 * sizeof (String) );
	if ( retval == NULL )
		return NULL;
	
	for ( int i = 0; i < 5 ; i++)
		retval[i] = newStr();
	
	int perline = 5;
	int pos = 0;
	int last = 0;
	
	while ( (perline != 0)  )
	{
		if (line->str[pos] == ';')
		{
			freeString(&retval[ 5 - perline ]);
			retval[ 5 - perline ] = newStrFromConstStartEnd( line->str, last, pos );
			perline--;
			pos++;
			last = pos;

		} else
		if ( pos == line->len )
		{
			freeString(&retval[ 5 - perline ]);
			retval[ 5 - perline ] = newStrFromConstStartEnd( line->str, last, pos );
			break;
		} else 
			pos++;
			
	}
	
	return retval;
}

int
populate_db( target, src)
	struct simpledb * target;
	FILE * src;
{
	
	if ( src == NULL )
		return -1;
	
	String line = NULL;
	while ( (line = read_line(src)) != NULL)
	{
		String * tkns = tokenize_line(line);
		freeString(&line);
		
		if ( tkns == NULL )
			continue;
		
		int id = -1;
		
		if ( tkns[0] != NULL )
		{
			if ( sscanf(tkns[0]->str, "%d", &id) != 1 )
				id = -1;
		}
		
		insert_entry(target, id, tkns[1], tkns[2], tkns[3], tkns[4]);
				
		for (int i = 0; i < 5 ; i++)
			freeString(&tkns[i]);
	}
	
	fclose(src);
	return 0;
}

void
dump_db( target, dest)
	struct simpledb * target;
	FILE * dest;
{
	struct simpledb_entry * nx = target->head;
	while( nx != NULL )
	{
		fprintf(  dest, 				   "%d;%s;%s;%s;%s\n", nx->id, nx->login == NULL ? "NULL" : nx->login->str,
										       nx->username == NULL ? "NULL" : nx->username->str,
										       nx->surname == NULL ? "NULL" : nx->surname->str,
										       nx->focus == NULL ? "NULL" : nx->focus->str );
		nx = nx->next;
	}
	
}

void
dispose_db( target )
	struct simpledb * target;
{
	struct simpledb_entry * nx = target->head;
	struct simpledb_entry * tmp = NULL;
		
	while( nx != NULL )
	{
		tmp = nx;
		freeString( &nx->login );
		freeString( &nx->username );
		freeString( &nx->surname );
		freeString( &nx->focus );
		nx = nx->next;
		free(tmp);
	}
}

struct simpledb_entry **
find_entry( target, id, login, name, surname, focus, count)
	struct simpledb * target;
	int id;
	String login;
	String name;
	String surname;
	String focus;
	int * count;
{
	int cnt = 0;
	struct simpledb_entry ** retlist = NULL;
	
	struct simpledb_entry * nx = target->head;	
	while ( nx != NULL )
	{
		int entry_pass = 1;
		entry_pass = entry_pass && ( (id == -1) || (nx->id == id) );
		entry_pass = entry_pass && ( (login == NULL) || login->len == 0 || (strcmp(nx->login->str, login->str) == 0) );
		entry_pass = entry_pass && ( (name == NULL) || name->len == 0 || (strcmp(nx->username->str, name->str) == 0) );
		entry_pass = entry_pass && ( (surname == NULL) || surname->len == 0 || (strcmp(nx->surname->str, surname->str) == 0) );
		entry_pass = entry_pass && ( (focus == NULL) || focus->len == 0 || (strcmp(nx->focus->str, focus->str) == 0) );
		if ( entry_pass )
		{
			retlist = realloc( retlist, ++cnt*sizeof(struct simpledb_entry *) );
			retlist[cnt - 1] = nx;
		}
		
		nx = nx->next;
	}
	
	*count = cnt;
	return retlist;
}

int
create_bind_listen( port )
	int port;
{
	struct sockaddr_in stSockAddr;
	int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if( SocketFD == -1 )
	{
		perror("Nemozem vytvorit socket");
		exit(EXIT_FAILURE);
	}
	
	memset(&stSockAddr, 0, sizeof(stSockAddr));
	
	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_port = htons(port);
	stSockAddr.sin_addr.s_addr = INADDR_ANY;
	
	if( bind(SocketFD,(const void *)&stSockAddr, sizeof(stSockAddr)) == -1 )
	{
		perror("Nemohol som nabindovat socket. Obsadeny");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}
	
	if( listen(SocketFD, MAX_WORKERS ) == -1 )
	{
		perror("Listen zlyhal");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}
	
	return SocketFD;
}

void
send_no_match( connFD )
	int connFD;
{

	int32_t resp = RESP_NOMATCH;
	int16_t ver = PROT_VER;
	int32_t len = 0;	
	send(connFD, &resp, 4, 0);
	send(connFD, &ver, 2, 0);
	send(connFD, &len, 4, 0);

	return;
}

void
send_matches( connFD, matches, count )
	int connFD;
	struct simpledb_entry ** matches;
	int count;
{

	int32_t resp = RESP_MATCH;
	int16_t ver = PROT_VER;
	String minone = newFromConstChar("-1");
	
	String outstr = newStr();
	for ( int i = 0; i < count; i++ )
	{
		appendTo(outstr, minone );
		appendChar(outstr, ';');		
		appendTo(outstr, matches[i]->login );
		appendChar(outstr, ';');	
		appendTo(outstr, matches[i]->username );
		appendChar(outstr, ';');
		appendTo(outstr, matches[i]->surname );
		appendChar(outstr, ';');
		appendTo(outstr, matches[i]->focus );
		appendChar(outstr, '\n');		
	}
	freeString(&minone);
	
	int32_t len = strlen(outstr->str);	
	send(connFD, &resp, 4, 0);
	send(connFD, &ver, 2, 0);
	send(connFD, &len, 4, 0);
	send(connFD, outstr->str, len, 0);
	freeString(&outstr);
}

void
handle_request( pckbody, connectionFD )
	String pckbody;
	int connectionFD;
{
	String * requested = tokenize_line(pckbody);
	if ( requested == NULL )
	{
		send_no_match( connectionFD );
		return;
	}
	int found = 0;
	struct simpledb_entry ** matches = find_entry(&db, -1, requested[1], requested[2], requested[3], requested[4], &found);
	if ( matches == NULL )
	{

		for (int i = 0; i < 5 ; i++)
			freeString(&requested[i]);	
		send_no_match( connectionFD );
		return;
	}
	send_matches(connectionFD, matches, found);
	for (int i = 0; i < 5 ; i++)
		freeString(&requested[i]);	
	
	free( matches );
	return;
}

void
handle_connection( connectionFD )
	int connectionFD;
{
	char bopcode[4];
	char bprver[2];
	char pcklen[4];	
	
	//nacitaj opkod
	int pos = 0;
	while ( pos < 4 )
	{
		int ret = recv(connectionFD, bopcode+pos, 4-pos, 0);
		if ( ret <= 0 )
		{
			shutdown(connectionFD, SHUT_RDWR);	
			close(connectionFD);
			return;
		}
		pos += ret;
	}
	
	pos = 0;
	while ( pos < 2 )
	{
		int ret = recv(connectionFD, bprver+pos, 2-pos, 0);
		if ( ret <= 0 )
		{
			shutdown(connectionFD, SHUT_RDWR);	
			close(connectionFD);
			return;
		}
		pos += ret;
	}
	
	pos = 0;
	while ( pos < 4 )
	{
		int ret = recv(connectionFD, pcklen+pos, 4-pos, 0);
		if ( ret <= 0 )
		{
			shutdown(connectionFD, SHUT_RDWR);	
			close(connectionFD);
			return;
		}
		pos += ret;
	}
	
	int inlen = (int) (*((int32_t *) &pcklen));	
	int opcode = (int) (*((int32_t *) &bopcode));	
	int ver = (int) (*((int16_t *) &bprver));	
		
	if ( ver != PROT_VER )
	{
		return;
	}
	
	//nacitaj telo	
	int remainding = inlen;
	String in = newStr();
	char inbuffer[4097];
	while ( 1 )
	{
		if (remainding <= 0)
			break;
		
		int todo = inlen < 4096 ? remainding : 4096;
		int amn = 0;
		if ( 0 > (amn= recv(connectionFD, inbuffer, todo, 0)) )
		{
			shutdown(connectionFD, SHUT_RDWR);	
			close(connectionFD);
			return;
		}
		inbuffer[amn] = '\0';
		String tmp = newFromConstChar(inbuffer);
		appendTo(in, tmp);
		freeString(&tmp);
		remainding -= todo;
	}

	switch ( opcode )
	{
		case OP_REQUEST: 
			;
			handle_request( in, connectionFD ); 
			break;
	}
	
	freeString(&in);
}

static pthread_mutex_t pass_lock = PTHREAD_MUTEX_INITIALIZER; // mutex pouzity na zamykanie pri predavani parametrov

void *
handle_thread_func( arg )
	void * arg;
{
	int connectFD = *((int*) arg);
	pthread_mutex_unlock(&pass_lock);
	
	handle_connection(connectFD);
		
	shutdown(connectFD, SHUT_RDWR);	
	close(connectFD);
	pthread_exit(NULL);
}

int
main_loop( sockFD )
	int sockFD;
{
	pthread_mutex_lock (&pass_lock); //pociatocne uzamknutie
	for(;;)
	{
		int connectFD = accept(sockFD, NULL, NULL);
		
		if( connectFD < 0 )
		{
			perror("Chyba primania pripojenia");
			close(connectFD);
			exit(EXIT_FAILURE);
		}
		
		
		pthread_t handle;
		int rc = pthread_create(&handle, NULL, handle_thread_func, (void *) &connectFD);	
		if ( rc < 0 )
		{
			perror("Error crating thread");
			pthread_mutex_unlock(&pass_lock);	//odomkni pri chybe a pokracuj
		}	
		pthread_mutex_lock(&pass_lock);	//pusti nas dalej az ked sme precitali parameter
	
	}
}

int
main( argc, argv )
	int argc;
	char * argv[];
{

	int pnum = 8086;

	if ( argc == 3 )
	{
		if ( strcmp(argv[1], "-p") == 0 )
		{
			if ( sscanf(argv[2], "%d", &pnum) != 1 )
			{
				printf("Bad numeric parameter\n");
				exit(1);
			}
		} else
		{
			printf("Bad parameter\n");
			exit(1);
		}
	}			
	

	FILE * src = fopen("ipk_database.txt", "rb");
	if ( src == NULL )
	{
		perror("main: file error");
		return -1;
	}
	
	int newsock = create_bind_listen(pnum);

	init_db( &db );
	populate_db(&db, src);
	main_loop(newsock);
	dispose_db( &db );
	close( newsock ); 
	return 0;
}

