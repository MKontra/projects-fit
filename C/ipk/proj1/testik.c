#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#include <regex.h>

#define STATIC_ARRAY_SIZE(myArray) (sizeof(myArray)/sizeof(*myArray))

struct byte_buffer
{
	char * data;
	long size;
};


struct HTTP_headers
{
	char * key;
	char * value;
	
	struct HTTP_headers * next;
};

struct HTTP_response
{
	int code;
	struct HTTP_headers * headers;
	struct byte_buffer body;
	char * requestUrl;
};

/**
struct HTTP_headers *
create_headers()
{
	struct HTTP_headers * newheds = malloc( sizeof(struct HTTP_headers) );
	if ( newheds == NULL )
		return NULL;
	
	newheds->key = NULL;
	newheds->value = NULL;
	newheds->next = NULL;
	
	return newheds;
}
**/

//appends header to to list
//if address of null pointer is used inicialize it with new value
int
add_header(struct HTTP_headers ** hh, char * key, char * val)
{
	if ( hh == NULL )
		return -1;
	
	struct HTTP_headers * newheds = malloc( sizeof(struct HTTP_headers) );
	if ( newheds == NULL )
		return -1;	
	
	newheds->key = malloc( strlen(key)+1 );
	strcpy ( newheds->key, key);
	newheds->value = malloc( strlen(val)+1 );
	strcpy ( newheds->value, val);	
	newheds->next = NULL;
	
	if ( *hh == NULL )
	{
		*hh = newheds;
	} else
	{
		struct HTTP_headers * itr = *hh;
		while ( itr->next != NULL )
			itr = itr->next;
		itr->next = newheds;
	}
	

	
	return 0;
}

//returns list of pointers to  value strings ( not copies)
char **
getValue( struct HTTP_headers * hh, char * key )
{
	int mtchs = 0;
	char ** retval = NULL;
	while ( hh != NULL )
	{
		if ( strcasecmp( hh->key, key) == 0 )
		{
			retval = realloc( retval, (++mtchs) * sizeof( char *) );
			retval[mtchs-1] = hh->value;
		}
		hh = hh-> next;
	}
	
	return retval;
}

//returns char containing text representation of headers (hhtp protocol compilant)
char *
sprintf_headers( struct HTTP_headers * hh)
{
	char * defsep = ": ";
	char * newstr = NULL;
	int pos = 0;
	
	while ( hh != NULL )
	{	
		int keylen = (hh->key == NULL) ? 0 : strlen(hh->key);
		int vallen = (hh->value == NULL) ? 0 : strlen(hh->value);
		newstr = realloc( newstr, pos + keylen + vallen + 3 );
		memcpy( newstr + pos, hh->key, keylen );
		pos += keylen;
		memcpy( newstr + pos, defsep, 2 );
		pos += 2;
		memcpy( newstr + pos, hh->value, vallen );
		pos += vallen;	
		*(newstr + pos) = '\n';
		pos++;
		hh = hh->next;
	}
	 
	newstr = realloc( newstr, pos + 1 );
	newstr[pos] = '\0';

	return newstr;
}

//frees allocated space
int
disposeHeaders(struct HTTP_headers * hh)
{
	if ( hh == NULL )
		return -1;
	
	struct HTTP_headers * next = NULL;
	do
	{
		free(hh->key); free(hh->value);
		next = hh->next;
		free(hh);
		hh = next;
	} while  (hh != NULL);
	
	return 0;
}

void
freeResponse( struct HTTP_response * res )
{
	free(res->requestUrl);
	disposeHeaders(res->headers);	
	free(res->body.data);
}

//make copy of array
char * 
copyArray( char * u1 )
{
	if ( u1 == NULL )
		return NULL;
	
	char * newarr = ( char * ) malloc( strlen( u1 ) + 1 );
	if ( newarr == NULL || u1 == NULL )
	{
		free(newarr);
		return NULL;
	}
	
	strcpy( newarr, u1 );
	return newarr;
}	
	
//appends char to *str, may change old value of pointer
int
appendChar(char ** str, char c)
{
	int stlen = ((*str == NULL) ? 0 : strlen(*str));
	*str = realloc( *str, stlen + 2);
	if ( *str == NULL )
		return -1;
	(*str)[stlen] = c;
	(*str)[stlen+1] = '\0';
	return 0;
}

//appends an array, same as above
int
appendArr( char ** dst, int oldlen, char * src, int len )
{
	int dstlen = oldlen;
	*dst = realloc( *dst, dstlen + len + 1 );
	if ( *dst == NULL )
		return -1;
	 
	memcpy( (*dst) + dstlen, src, len );
	(*dst) [dstlen + len] = '\0';

	return 0;
}

int
appendStr( char ** dst, char * src )
{
	int dstlen = (*dst == NULL) ? 0 : strlen(*dst);
	int srclen = (src == NULL) ? 0 : strlen(src);
	*dst = realloc( *dst, dstlen + srclen + 1 );
	if ( *dst == NULL )
		return -1;
	 
	memcpy( (*dst) + dstlen, src, srclen );
	(*dst) [dstlen + srclen] = '\0';

	return 0;
}

//return new string containing values of u1 concatenated with u2
char * 
concatByteArrays( char * u1, char * u2 )
{
	int u1len = strlen( (char *) u1);
	int u2len = strlen( (char *) u2);
	char * newarr = malloc( u1len + u2len + 1 );
	if ( newarr == NULL )
		return NULL;
	strcpy( (char *) newarr, (char *) u1 );
	strcat( (char *) newarr, (char *) u2 );
	return newarr;
}

//returns position of first occurence of char val
int
contains(char * array, int len, char val)
{
	for (int i = 0; i < len ; i++)
		if ( array[i] == val ) return i;
	return -1; 
}

//shifts array cnt positions to right
int
shiftRChars(char * str, int cnt)
{
	//posunie specifikovany retazec doprava o n pozicii (zaplni * )
	//miesto musi byt naalokovane....
	if ( cnt == 0 || str == NULL )
		return -1;

	int newlen = strlen(str) + cnt;

	for ( int i = newlen; i >= cnt; i-- )
		str[i] = str[i-cnt];

	for ( int i = 0; i < cnt; i++)
		str[i] = '*';
	
	return 0;
}

//shifts array cnt positions to left
int
shiftLChars(char * str, int cnt)
{
	//posunie specifikovany retazec dolava o n pozicii ? uvolnit pamac alebo nie ?
	if ( cnt == 0 || str == NULL )
		return -1;

	int newlen = strlen(str);

	for ( int i = 0; i <= newlen-cnt+1; i++ )
		str[i] = str[i+cnt];
	
	return 0;
}

void
freeUrl(char ** arg)
{
	if (arg == NULL)
		return;
	
	for ( int i = 0; i < 8; i++ )
	{
		if ( arg[i] != NULL )
			free(arg[i]);
	}
	
	free (arg);
}

//parse url in format schema://username:password@host:port/relpath/file.ext?query=value&query2=value2#anchor
//order of parts returned	
char **
parseUrl(char * urlstring)
{
	if ( urlstring == NULL )
		return NULL;
	
	char ** retval = malloc ( 8*sizeof(char*) );
	for (int i = 0; i < 8; i++)
	  retval[i] = NULL;
	
	regex_t url_pattern_com;
	regmatch_t matchpos[12];
	char * urlpattern = "^(([^:]*)://)?(([^:@]+)(:([^@]+))?@)?([^/:$]+)?(:([0-9]+))?(/.*)?$";
	
	if ( 0 != regcomp( &url_pattern_com, urlpattern, REG_EXTENDED | REG_ICASE ) )
	{
	  free(retval);
	  regfree(&url_pattern_com);
	  return NULL;
	}

	if ( 0 != regexec( &url_pattern_com, urlstring, 11, matchpos, 0 ) )
	{
	  free(retval);
	  regfree(&url_pattern_com);
	  return NULL;
	}

	regfree(&url_pattern_com);
	
	char * scheme = NULL;
	char * hostname = NULL;
	char * port = NULL;
	char * username = NULL;
	char * password = NULL;
	
	char * rest = NULL;

	if ( matchpos[2].rm_so != -1 )
	{	
		int newlen = matchpos[2].rm_eo - matchpos[2].rm_so + 1;
		scheme = malloc ( newlen );
		strncpy ( scheme, urlstring + matchpos[2].rm_so, newlen - 1);
		scheme[newlen - 1] = 0;
	}

	retval[0] = scheme;

	if ( matchpos[4].rm_so != -1 )
	{	
		int newlen = matchpos[4].rm_eo - matchpos[4].rm_so + 1;
		username = malloc ( newlen );
		strncpy ( username, urlstring + matchpos[4].rm_so, newlen - 1);
		username[newlen - 1] = 0;
	}

	retval[1] = username;

	if ( matchpos[6].rm_so != -1 )
	{	
		int newlen = matchpos[6].rm_eo - matchpos[6].rm_so + 1;
		password = malloc ( newlen );
		strncpy ( password, urlstring + matchpos[6].rm_so, newlen - 1);
		password[newlen - 1] = 0;
	}

	retval[2] = password;

	if ( matchpos[7].rm_so != -1 )
	{	
		int newlen = matchpos[7].rm_eo - matchpos[7].rm_so + 1;
		hostname = malloc ( newlen );
		strncpy ( hostname, urlstring + matchpos[7].rm_so, newlen - 1);
		hostname[newlen - 1] = 0;
	}

	retval[3] = hostname;

	if ( matchpos[9].rm_so != -1 )
	{	
		int newlen = matchpos[9].rm_eo - matchpos[9].rm_so + 1;
		port = malloc ( newlen );
		strncpy ( port, urlstring + matchpos[9].rm_so, newlen - 1);
		port[newlen - 1] = 0;
	}

	retval[4] = port;

	if ( matchpos[10].rm_so == -1 )
	  return retval;
	
	

	rest = urlstring + matchpos[10].rm_so;
	//printf("parsing rest %s\n", rest);

	regex_t rest_pattern_com;
	char * restpattern = "^(/[^?#$]*)([?]([^#$]*))?([#]([^$]*))?$";

	if ( 0 != regcomp( &rest_pattern_com, restpattern, REG_EXTENDED | REG_ICASE ))
	{
		for (int i = 0; i < 8; i++)
		free(retval[i]);
		free ( retval );
		return NULL;
	}


	if ( 0 != regexec( &rest_pattern_com, rest, 7, matchpos, 0 ) )
	  return retval;
	
	regfree(&rest_pattern_com);
	
	char * path = NULL;
	char * query = NULL;
	char * anchor = NULL;

	if ( matchpos[1].rm_so != -1 )
	{
		int newlen = matchpos[1].rm_eo - matchpos[1].rm_so + 1;
		path = malloc ( newlen );
		strncpy ( path, rest + matchpos[1].rm_so, newlen - 1);
		path[newlen - 1] = 0;
	}

	retval[5] = path;

	if ( matchpos[3].rm_so != -1 )
	{
		int newlen = matchpos[3].rm_eo - matchpos[3].rm_so + 1;
		query = malloc ( newlen );
		strncpy ( query, rest + matchpos[3].rm_so, newlen - 1);
		query[newlen - 1] = 0;
	}

	retval[6] = query;

	if ( matchpos[5].rm_so != -1 )
	{
		int newlen = matchpos[5].rm_eo - matchpos[5].rm_so + 1;
		anchor = malloc ( newlen );
		strncpy ( anchor, rest + matchpos[5].rm_so, newlen - 1);
		anchor[newlen - 1] = 0;
	}

	retval[7] = anchor;
	return retval;
	
}

static char syntaxchars[] = { ';', '/', '?', ':', '@', '=', '#' ,'&' };
static int synsize = STATIC_ARRAY_SIZE(syntaxchars);

int
escapeUrl(char * urlstring)
{
	
	//escapes non-ascii and special characters
	char sbuf[3];
	int newlendelta = 0;
	int urlstrlen = strlen (urlstring);	
	
	if ( urlstring == NULL )
		return -1;

	for ( int i = 0; i < urlstrlen; i++ )
	{
		char current = urlstring[i];
		if ( contains(syntaxchars, synsize, current) != -1)
			continue;

		if ( (current >= 48) && (current <= 57) )
			continue;

		if ( (current >= 65) && (current <= 90) )
			continue;

		if ( (current >= 97) && (current <= 122) )
			continue;

		if ( current == '-' || current == '_' || current == '.' || current == '~' )
			continue;

		newlendelta += 2;
	}

	urlstrlen += newlendelta;

	urlstring = (char *) realloc ( urlstring, urlstrlen*sizeof(char) );

	for ( int i = 0; i < urlstrlen; i++ )
	{
		char current = urlstring[i];

		if ( contains(syntaxchars, synsize, current) != -1)
			continue;

		if ( (current >= 48) && (current <= 57) )
			continue;

		if ( (current >= 65) && (current <= 90) )
			continue;

		if ( (current >= 97) && (current <= 122) )
			continue;

		if ( current == '-' || current == '_' || current == '.' || current == '~' )
			continue;
		
		urlstring[i] = '%';
		shiftRChars( urlstring + i + 1 , 2);
		snprintf( sbuf, 3, "%02X", current);
		strncpy( urlstring + i + 1, sbuf, 2);
		i+=2;
	}
	return newlendelta / 2;
}

void
unescapeUrl(char * escapedurlstring)
{
  for (int i = 0; i < strlen(escapedurlstring); i++)
    if (escapedurlstring[i] == '%')
    {
      int chval = 0;
      //printf("processing char: %c\n",escapedurlstring[i]); 
      if ( sscanf(escapedurlstring + i + 1, "%2x", (unsigned int *) &chval) == 1 )
      {
	//printf("first if\n");
	escapedurlstring[i] = (char) chval;
	shiftLChars(escapedurlstring + i + 1, 2);
      } else
	      if ( sscanf(escapedurlstring + i + 1, "%2X", (unsigned int *) &chval) == 1 )
      {
	//printf("second if\n");
	escapedurlstring[i] = (char) chval;
	shiftLChars(escapedurlstring + i + 1, 2);
      } else 
	continue;
    }

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

int
makeHTPPRequest(int sockFD, char * type, char * path, struct HTTP_headers * hh, void * postdata, int pdsize)
{
	int pos = 0;
	int tlen = strlen(type);
	if ( path == NULL )
		path = "/";
	int plen = strlen(path);	
	
	char * htpver  = "HTTP/1.1";
	int htpverlen = strlen(htpver);
	char * restr = malloc ( tlen + 1 + plen + 1 + htpverlen + 3 );
	memcpy( restr+pos, type, tlen );
	pos += tlen;
	*(restr + pos) = ' ';
	pos++;
	memcpy( restr+pos, path, plen );
	pos += plen;
	*(restr + pos) = ' ';
	pos++;
	memcpy( restr+pos, htpver, htpverlen );
	pos += htpverlen;
	*(restr + pos) = '\r';
	pos++;	
	*(restr + pos) = '\n';
	pos++;	
	*(restr + pos) = '\0';
	pos++;	

	
	if ( hh != NULL )
	{
		char * hpart = sprintf_headers(hh);
		restr = realloc( restr, pos + strlen(hpart) + 1 );
		strcat( restr, hpart); 
		free(hpart);
	}
	
	int spos = strlen(restr);
	restr = realloc( restr, spos+3 );
	
	restr [spos] = '\r';
	restr [spos+1] = '\n';
	restr [spos+2] = '\0';		
	
	send(sockFD, restr, strlen(restr) , 0);
	free(restr);
	return -1;
}

char *
readHTTPResponse(int sockFD)
{
	if ( sockFD < 0 )
		return NULL;
	
	char rebuf[2];
	char * upnov = NULL;
	int cnt = 0;
	int res;
	while ( (cnt = recv(sockFD, rebuf, 1, 0)) > 0)
	{
		if ( (res = appendChar( &upnov, rebuf[0] )) < 0 )
		{
			//try again
			if ( (res = appendChar( &upnov, rebuf[0] )) < 0 )
				exit(1);
		}
		
		if ( strstr ( upnov, "\r\n\r\n" ) != NULL ) break;
	}
	
	return upnov;

}



int
getHTTPResponseCode( char * input, char ** endshere )
{
	if ( (*endshere = strstr(input, "\r\n")) == NULL )
		return -1;
	*endshere = (*endshere + 2);
	int i = 0;
	while ( input[i++] != ' ' );
	char * tstptr = NULL;
	int rescode = (int) strtol (input + i, &tstptr, 10 );
	
	if ( (input + i) == tstptr )
		return -1;
	
	return rescode;
}

int
readByte( int sockFD )
{
	char rebuf[2];
	if ( recv(sockFD, rebuf, 1, 0) > 0 ) 
	{
		return rebuf[0];
	}else
		return -1;
}

struct byte_buffer
readToEnd( int sockFD )
{
	int amn = 0;
	int total = 0;
	char buff[4097];
	char * rcvdData = NULL;
	while ( 0 < (amn = recv( sockFD, buff, 4096, 0)) ) 
	{
		appendArr( &rcvdData, total, buff, amn );
		total += amn;
	}
	struct byte_buffer retval = { rcvdData, total };
	return retval;
}

struct byte_buffer
readLen( int sockFD, int length )
{
	int amn = 0;
	int total = 0;
	char buff[4097];
	char * rcvdData = NULL;	
	
	while ( 0 < ( amn = recv ( sockFD, buff, length < 4096 ? length : 4096, 0 ) ) )
	{
		appendArr( &rcvdData, total, buff, amn );
		total += amn;
		length -= amn;
	}
	
	struct byte_buffer retval = { rcvdData, total };
	return retval;
	
}

struct byte_buffer
readRaw( int sockFD, int length )
{

	if ( length < 0 )
		return readToEnd(sockFD);
	
	return readLen( sockFD, length );
	
}	

int
readChunkLen( int sockFD )
{
	
	char * strform = NULL;
	int c = 0;
	
	while ( ( c = readByte(sockFD)) != -1 )
	{
		if ( (c != '\r') && (c != '\n') && (c != ' ') && (c != '\t') )
			break;
	}
	do
	{
		appendChar(&strform, (char) c );
		if ( NULL != strstr ( strform, "\r\n" ) )
			break;
	} while ( ( c = readByte(sockFD)) != -1 );
	
	int retval = -1;
	if  (sscanf(strform, "%x", &retval) <= 0 )
	{
		free(strform);
		return -1;
	}
	else 
	{
		free(strform);
		return retval;
	}
}

struct byte_buffer
readChunked( int sockFD )
{

	int chunksize = 0;
	char buff[4097];	
	struct byte_buffer retval = { NULL, 0 };
	struct byte_buffer getval = { NULL, 0 };
	while ( 0 < ( chunksize = readChunkLen(sockFD) ) )
	{
		getval = readLen( sockFD, chunksize);
		appendArr( &retval.data, retval.size, getval.data, getval.size);
		retval.size += getval.size;
		free(getval.data);
	}
	return retval;
}

struct byte_buffer
readHTTPBody( int sockFD, int length , int chunked )
{
	
	if ( chunked == 0 )
	{
		return readRaw( sockFD, length );
	} else
	{
		return readChunked( sockFD );
	}

}
			
	
	

struct HTTP_headers *
parseHTTPHeaders( char * input, char ** endsHere )
{
	struct HTTP_headers * resset = NULL;
	int pos = 0;
	regex_t pair;
	regmatch_t matchpos[3];
	char * hpattern = "^([^:]+)[ ]*:[ ]*([^\r]+)\r\n";
	int retcompval = regcomp( &pair, hpattern, REG_EXTENDED | REG_ICASE );
	while ( 0 == regexec( &pair, input+pos, 3, matchpos, 0 ) )
	{
		//printf("match\n");
		//for ( int i =0; i < 3; i++)
		//	printf("Match %d: start: %d stop: %d\n", i, matchpos[i].rm_so, matchpos[i].rm_eo);
		char o1 = (input+pos)[matchpos[1].rm_eo];
		(input+pos)[matchpos[1].rm_eo] = '\0';
		char o2 = (input+pos)[matchpos[2].rm_eo];
		(input+pos)[matchpos[2].rm_eo] = '\0';
		add_header( &resset, (input+pos+matchpos[1].rm_so), (input+pos+matchpos[2].rm_so) );
		(input+pos)[matchpos[1].rm_eo] = o1;
		(input+pos)[matchpos[2].rm_eo] = o2;
		pos += matchpos[0].rm_eo;
		//printf("continuing %s\n", input+pos );
	}
	
	regfree(&pair);
	
	return resset;
		
}

struct HTTP_response
open_url_nohandlers( char * location )
{

	struct HTTP_response retval = { 0, NULL, {NULL, 0} };
		
	if ( location == NULL )
		return retval;
		
	char ** prsd = parseUrl(location);	
	
	
	if ( prsd == NULL )
	{
		printf("Wrong URL\n");
		exit(1);
	}	
	
	if ( prsd[3] == NULL )
	{
		printf("Missing hostname\n");
		exit(1);
	}		
	
	char * hostport = NULL;
	if ( prsd[4] != NULL )
	{	
		hostport = concatByteArrays ( prsd[3], prsd[4] );
	} else
		hostport = copyArray(prsd[3]);	
	
	struct HTTP_headers * defheds = NULL;		
		
	add_header(&defheds, "Host", hostport);free(hostport);
	add_header(&defheds, "Connection", "close");
	add_header(&defheds, "User-Agent", "Mozilla/5.0 (X11; U; Linux x86_64; en-US; rv:1.9.0.2) Gecko/2008092313 Ubuntu/8.04 (hardy) Firefox/3.1" );
	
	int portnum = prsd[4] == NULL ? 80 : atoi(prsd[4]);
	
	int newsock = connect_to_host(prsd[3], portnum);
	
	if ( newsock < 0 )
	{
		disposeHeaders(defheds);
		freeUrl(prsd);
		printf("Couldnt create socket\n");
		exit(1);
	}		
	
	
	//for ( int i = 0; i < 8; i++)
	//	printf("cast:%s\n", prsd[i]);
	
	char *rpath = NULL;
	if ( prsd[6] != NULL )
	{
		char * fstmp = copyArray(prsd[5]);
		appendChar( &fstmp, '?' );
		rpath = concatByteArrays( fstmp, prsd[6] );
		free(fstmp);
	}
	else
		rpath = copyArray(prsd[5]);		
	makeHTPPRequest(newsock, "GET", rpath, defheds, NULL, 0);free(rpath);
	
	char * resheds = readHTTPResponse(newsock);
	
	if ( resheds == NULL )
	{
		disposeHeaders(defheds);
		freeUrl(prsd);
		printf("Error reading response\n");
		close(newsock);
		exit(1);
	}
	
	char * tstptr = NULL;	
	
	int code = getHTTPResponseCode(resheds, &tstptr);
	
	if ( code < 0 )
	{
		disposeHeaders(defheds);
		freeUrl(prsd);
		printf("Error reading response code\n");
		free(resheds);
		close(newsock);
		exit(1);
	}
	
	struct HTTP_headers * res = parseHTTPHeaders( tstptr, NULL);
	
	if ( res == NULL )
	{
		disposeHeaders(defheds);
		freeUrl(prsd);
		printf("Error parsing headers\n");
		free(resheds);
		close(newsock);
		exit(1);
	}
	
	char ** tmp = NULL;
	int toRead = ( ( tmp = getValue(res, "Content-Length")) == NULL ) ? -1 : atoi(tmp[0]); free(tmp);
	int chunked = ( ( tmp = getValue(res, "Transfer-Encoding")) == NULL ) ? 0 : ( strcmp(tmp[0], "chunked") == 0  );free(tmp);	
	
	struct byte_buffer retbody = readHTTPBody(newsock, toRead,  chunked);
	
	disposeHeaders(defheds);
	freeUrl(prsd);
	free(resheds);
	close(newsock);
	
	retval.code = code;
	retval.headers = res;
	retval.body = retbody;
	retval.requestUrl = copyArray(location);
	return retval;
}

void
saveresfile(struct HTTP_response * res, char * url)
{

	int useDef = 1;
	char defname[] = "index.html";
	char ** prsd = parseUrl(url);
	char * relpath = prsd[5];
	
	int relLen = 0;
	if ( strcmp(relpath,"/") == 0 )
	{
		free(relpath);
		prsd[5] = NULL;
		relpath = NULL;
	}
	if ( relpath != NULL )
	{	
		useDef = 0;
		relLen = strlen(relpath);
		while( (*(relpath + (relLen)) != '/') && (relLen != 0) )
			relLen--;	
		relLen++;
	}
	
	FILE * out = fopen( useDef==1 ? defname : (relpath + relLen) , "wb");	
	fwrite( res->body.data, res->body.size, 1 , out );
	freeUrl(prsd);
	return;
}

int
main( int argc, char* argv[] )
{
	
	char ran[] = "http://zip.4chan.org/a/imgboard.html";
	char * toopen = copyArray(ran);
	struct HTTP_response f1;
	while (1)
	{
		printf("Requesting url: %s\n", toopen );
		f1 = open_url_nohandlers(toopen);
		char * oldurl = ran;
		if ( f1.code/100 == 2 )	//OK
		{
			saveresfile(&f1, ran);
			freeResponse(&f1);
			free(toopen);
			return f1.code;
		}
		
		if ( f1.code/100 == 3 )	//Redirects
		{
			char ** hlp = NULL;
			char * newurl = ( ((hlp = getValue(f1.headers, "Location")) == NULL) ? NULL : hlp[0] );free(hlp);

			if ( newurl == NULL )
			{
				printf("Response code: %d, missing \"Location\" header field", f1.code);
				freeResponse(&f1);
				exit(1);
			}
			hlp = parseUrl(newurl);
			char ** oldurl = parseUrl(toopen);
			char * newtarget = NULL;
			if ( hlp[3] == NULL ) //partial url
			{
				appendStr(&newtarget, oldurl[0]); 	//nalep na zaciatok schemu
				appendStr(&newtarget, oldurl[1]);	
				appendStr(&newtarget, oldurl[2]);	//login a heslo
				appendStr(&newtarget, oldurl[3]);	//hostitel
				appendStr(&newtarget, oldurl[4]);	//port
				appendStr(&newtarget, newurl);
			} else
				newtarget = copyArray(newurl);	
							
			free(toopen);
			toopen = newtarget;
			freeUrl(oldurl);
			freeUrl(hlp);
			freeResponse(&f1);
			continue;
		}
		free(toopen);		
		freeResponse(&f1);
		printf("HTTP response code %d\n", f1.code);
		return(f1.code);
		
	}			

} 
