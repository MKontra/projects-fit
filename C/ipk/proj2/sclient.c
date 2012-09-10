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

struct fields
{
	int id_on;
	int login_on;
	int username_on;
	int surname_on;
	int focus_on;
};

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
appendTo(String s1, String s2)
{
	if ( s2 == NULL )
		return s1;
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
read_line( src )
	FILE * src;
{
	if ( feof(src) )
		return NULL;
	
	String retval = newStr();
		
	int inchar = 0;
	while ( (inchar = fgetc(src)) != -1 )
	{
		if ( inchar == '\n' )
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
connect_to_host(char * hostname, int port)
{
	//returns valid sfd or -1 on error
	
	struct hostent *hp;
	
	if ((hp= gethostbyname(hostname)) == NULL) 
		return -1;
	
	struct sockaddr_in stSockAddr;
	memset(&stSockAddr, 0, sizeof(stSockAddr));
	memcpy((char *) &stSockAddr.sin_addr,hp->h_addr,hp->h_length);
	
	stSockAddr.sin_family = hp->h_addrtype;
	stSockAddr.sin_port = htons( (unsigned short) port);

	int retSocketFD = socket(hp->h_addrtype, SOCK_STREAM, IPPROTO_TCP);
	
	if (retSocketFD == -1)
	{
		printf("Could not create socket...\n");
		return -1;
	}	
	
	if (connect(retSocketFD,(struct sockaddr *)&stSockAddr,sizeof (stSockAddr)) < 0) 
	{ 
		printf("Connection failure...\n");
		close(retSocketFD);
		return(-1);
	} 

	return retSocketFD;
}

String *
separate_lines(String in, int * count)
{
	String * retval = NULL;
	int retcount = 0;
	int last = 0;
	int pos = 0;
	while( pos < strlen(in->str) )
	{
		if ( in->str[pos] == '\n' )
		{
			retval = realloc( retval, ++retcount*sizeof(String) );
			retval[retcount-1] = newStrFromConstStartEnd(in->str, last, pos);
			pos++;
			last = pos;
		}
		pos++;
	}
	*count = retcount;
	return retval;
}

void
print_formatted(String str, struct fields flds)
{
	int count = 0;
	String * res = separate_lines(str, &count );
	
	for ( int c = 0; c < count ; c++)
	{
		String * resfields = tokenize_line(res[c]);
		
		for ( int i = 0; i < 4; i++ )
		{
			if ( flds.login_on == i )
				printf("%s;", resfields[1]->str);
			
			if ( flds.username_on == i )
				printf("%s;", resfields[3]->str);
			
			if ( flds.surname_on == i )
				printf("%s;", resfields[2]->str);
			
			if ( flds.focus_on == i )
				printf("%s;", resfields[4]->str);
		}
		printf("\n");
		for (int i = 0; i < 5 ; i++)
			freeString(&resfields[i]);	
	}
	
	for (int i = 0; i<count; i++)
		freeString(&res[i]);
	
}

int main(int argc, char * argv[])
{

	struct fields posi = { -1, -1, -1, -1, -1 };
	char * hostname = NULL;
	int port = 8086;
	char * req_login = NULL;
	char * req_username = NULL;
	char * req_surname = NULL;
	char * req_focus = NULL;
	
	int cnt = 1;
	while ( cnt < argc )
	{
		if ( argv[cnt][0] != '-' )
		{
			fprintf (stderr,"bad argument\n");
			exit(1);
		}	
	
		switch (argv[cnt][1])
		{
			case 'h': if (cnt+1 < argc)
					hostname = argv[cnt + 1];cnt = cnt + 2;
				break;
			case 'p':if (cnt+1 < argc)
					if ( sscanf(argv[cnt+1],"%d",&port) != 1)
					{
						fprintf (stderr,"port is nan\n");
						exit(1);
					}
				cnt = cnt + 2;	
				break;
			case 'n': if (cnt+1 < argc)
					req_surname = argv[cnt + 1]; cnt = cnt + 2;
				break;	
			case 's': if (cnt+1 < argc)
					req_username = argv[cnt + 1];cnt = cnt + 2;
				break;
			case 'l': if (cnt+1 < argc)
					req_login = argv[cnt + 1];cnt = cnt + 2;
				break;
			case 'f': if (cnt+1 < argc)
					req_focus = argv[cnt + 1];cnt = cnt + 2;
				break;
				
			default:
				;
				int i = 1;
				while ( i < strlen (argv[cnt]) )
				{
					switch( argv[cnt][i] )
					{
						case 'n':
						case 'N':
							posi.username_on = i-1;
							break;
						case 's':
						case 'S':
							posi.surname_on = i-1;
							break;
						case 'l':
						case 'L':
							posi.login_on = i-1;
							break;
						case 'f':
						case 'F':
							posi.focus_on = i-1;
							break;	
						default:
							fprintf(stderr,"Unknown field requested\n");exit(1);
					}
					i++;
				}
				cnt ++;			
				break;
		}
	}
	
	if ( hostname == NULL )
	{
		fprintf(stderr,"No hostname\n");exit(1);
	}
	
	if ( (req_login == NULL ) && (req_username==NULL)&&(req_surname==NULL)&&(req_focus==NULL))
	{
		fprintf(stderr,"No informations provided\n");exit(1);
	}
	
	String req = newStr();
	
	String mone = newFromConstChar("-1;");
	appendTo(req, mone);
	freeString(&mone);
	
	String rlstr = newFromConstChar(req_login);
	appendTo(req, rlstr);
	appendChar(req,';');
	freeString(&rlstr);
			
	String rustr = newFromConstChar(req_username);
	appendTo(req, rustr);
	appendChar(req,';');
	freeString(&rustr);
		
	String rsstr = newFromConstChar(req_surname);
	appendTo(req, rsstr);
	appendChar(req,';');
	freeString(&rsstr);
	
	String rfstr = newFromConstChar(req_focus);
	appendTo(req, rfstr);
	freeString(&rfstr);	
	
	int sock = connect_to_host( hostname, port );
	if ( sock < 0 )
		exit(1);
	int32_t op = OP_REQUEST;
	int16_t vr = PROT_VER;
	send(sock, &op, 4, 0);
	send(sock, &vr, 2, 0);
	char * tp = req->str;
	int32_t sval = (int) strlen(tp);
	send(sock, &sval, 4 , 0);
	send(sock, tp, sval, 0);

	freeString(&req);	
	
	char bopcode[4];
	char bprver[2];
	char pcklen[4];	
	
	//nacitaj opkod
	int pos = 0;
	while ( pos < 4 )
	{
		int ret = recv(sock, bopcode+pos, 4-pos, 0);
		if ( ret <= 0 )
		{
			fprintf(stderr, "Error reading data from server\n");
			shutdown(sock, SHUT_RDWR);	
			close(sock);
			exit(1);
		}
		pos += ret;
	}
	
	pos = 0;
	while ( pos < 2 )
	{
		int ret = recv(sock, bprver+pos, 2-pos, 0);
		if ( ret <= 0 )
		{
			fprintf(stderr, "Error reading data from server\n");
			shutdown(sock, SHUT_RDWR);	
			close(sock);
			exit(1);
		}
		pos += ret;
	}
	
	pos = 0;
	while ( pos < 4 )
	{
		int ret = recv(sock, pcklen+pos, 4-pos, 0);
		if ( ret <= 0 )
		{
			fprintf(stderr, "Error reading data from server\n");
			shutdown(sock, SHUT_RDWR);	
			close(sock);
			exit(1);
		}
		pos += ret;
	}
	
	int inlen = (int) (*((int32_t *) &pcklen));	
	int opcode = (int) (*((int32_t *) &bopcode));	
	int ver = (int) (*((int16_t *) &bprver));	
		
	if ( ver != PROT_VER )
	{
			fprintf(stderr, "Wrong server version\n");
			shutdown(sock, SHUT_RDWR);	
			close(sock);
			exit(1);
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
		if ( 0 > (amn= recv(sock, inbuffer, todo, 0)) )
		{
			fprintf(stderr, "Error reading data from server\n");
			shutdown(sock, SHUT_RDWR);	
			close(sock);
			exit(1);
		}
		inbuffer[amn] = '\0';
		String tmp = newFromConstChar(inbuffer);
		appendTo(in, tmp);
		freeString(&tmp);
		remainding -= todo;
	}

	switch ( opcode )
	{
		case RESP_MATCH: 
			;
			print_formatted(in, posi);
			break;
			
		case RESP_NOMATCH:
			;
			printf("No data matched in db...\n");
			break;
	}
	
	freeString(&in);
	shutdown(sock, SHUT_RDWR);	
	close(sock);
}

