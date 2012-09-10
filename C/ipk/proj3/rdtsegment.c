/*
* Projekt IPK3
* Autor: Matus Kontra, xkontr00@stud.fit.vutbr.cz
* Datum: 5.5.2009
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <signal.h>

#include "rdtsegment.h"


/**
struct rdt_segment
{
	uint16_t opcode;
	uint16_t checksum;
	uint32_t seq_ack_num;
	
	char data[MAX_DATA + 1];
};
**/

uint16_t
calc_checksum( segment )
	struct rdt_segment * segment;
{
	//Internet chceksum - as defined in rfc1071	
	long sum = 0;
	
	sum += segment->opcode;
	sum += *((unsigned short *) &(segment->seq_ack_num));
	sum += *((unsigned short *) &(segment->seq_ack_num) + 1 );
	//sum += *((unsigned short *) &(segment->window_size));
	//sum += *((unsigned short *) &(segment->window_size) + 1 );
			
	char * addr = (char *) segment->data;
	int count = strlen(segment->data);
	if ( count > 80 ) count = 80;
	
	while( count > 1 )  {
		/*  this is the inner loop */
		sum += * (unsigned short *) addr++;
		count -= 2;
	}
	
		/*  add left-over byte, if any */
	if( count > 0 )
		sum += * (unsigned char *) addr;
	
		/*  fold 32-bit sum to 16 bits */
	while (sum>>16)
		sum = (sum & 0xffff) + (sum >> 16);
	
	return (uint16_t) ~sum;
	
}

int wait_for_one( int udt, struct rdt_segment * dseg, int maxtime, in_addr_t *addr, in_port_t *port)
{
	
	struct timeval tv;
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(udt, &readfds);
	
        tv.tv_sec = maxtime;
        tv.tv_usec = 0;
	
	struct timeval * tvp = &tv;
	
	if ( maxtime == 0 )
		tvp = NULL;
	
        int sval = select(udt+1, &readfds, NULL, NULL, tvp);

           if (sval == -1)
               perror("select()");
           else if (sval) 
		return udt_recv(udt, dseg, sizeof(struct rdt_segment), addr, port);
           else
               return 0;
	return 0;
}

int
qinit( struct queue * q )
{
	q->size = 0;
	q->head = q->tail = NULL;
	return 0;
}

int
qpush_back( struct queue * q, struct rdt_segment * seg )
{
	struct qentry * tmp = malloc( sizeof (struct qentry) );
	if ( tmp == NULL )
		return -1;
	
	memcpy(&tmp->seg, seg, sizeof(struct rdt_segment));
	
	if ( q->tail == NULL )
	{
		tmp->prev = tmp->next = NULL;
		q->size = 1;
		q->head = q->tail = tmp;
		return 0;
	} else
	{
		tmp->prev = q->tail;
		tmp->next = NULL;
		q->tail->next = tmp;
		
		q->size += 1;
		q->tail = tmp;
		return 0;
	}
}

int
qpop_front( struct queue * q )
{
	if ( q->size == 0 )
		return 0;
	else 
	if ( q->size == 1 )
	{
		q->size = 0;
		free(q->head);
		q->head = q->tail = NULL;	
		return 0;	
	} else
	if ( q->size > 1 )
	{
		struct qentry * tmp = q->head;
		q->head = q->head->next;
		free(tmp);
		q->size -= 1;
		return 0;
	} else
		return -1;	
		
}


int udt_init(in_port_t local_port)
{
	int udt = socket(AF_INET, SOCK_DGRAM, 0);
	fcntl(udt, F_SETFL, O_NONBLOCK);
	if (udt <= 0) {
		fprintf(stderr, "UDT: Cannot create UDT descriptor.");
		exit(EXIT_FAILURE);
	}
	struct sockaddr_in sa;
	bzero(&sa, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(0);
	sa.sin_port = htons(local_port);
	int err = bind(udt, (const struct sockaddr *) &sa, sizeof(sa));
	if (err == -1) {
		fprintf(stderr, "UDT: Cannot bind to the specified port.");
		exit(EXIT_FAILURE);
	}
	return udt;
}

int udt_recv(int udt, void *buff, size_t nbytes, in_addr_t *addr, in_port_t *port)
{
	struct stat info;
	if (fstat(udt, &info) != 0) {
	      fprintf(stderr, "UDT: descriptor is invalid, probably udt_init not called.\n");
	      exit(EXIT_FAILURE);
	}
	struct sockaddr_in sa;
	bzero(&sa, sizeof(sa));
	socklen_t salen = sizeof(sa);
	ssize_t nrecv = recvfrom(udt, buff, nbytes, MSG_DONTWAIT, (struct sockaddr *) &sa, &salen);
	if(addr != NULL) (*addr) = ntohl(sa.sin_addr.s_addr);
	if(port!=NULL) (*port) = ntohs(sa.sin_port);
	if (nrecv < 0 ) nrecv = 0;
	return nrecv;
}

int udt_send(int udt, in_addr_t addr, in_port_t port, void *buff, size_t nbytes)
{
	struct stat info;
	if (fstat(udt, &info) != 0) {
	      fprintf(stderr, "UDT: descriptor is invalid, probably udt_init not called.\n");
	      exit(EXIT_FAILURE);
	}

	struct sockaddr_in sa;
	bzero(&sa, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(addr);
	sa.sin_port = htons(port);
	ssize_t nsend = sendto(udt, buff, nbytes, 0, (const struct sockaddr *) &sa, sizeof(sa));
	return nsend == nbytes;
}
