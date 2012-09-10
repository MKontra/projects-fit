/*
* Projekt IPK3
* Autor: Matus Kontra, xkontr00@stud.fit.vutbr.cz
* Datum: 5.5.2009
*/

#ifndef SEGMENT_H
#define SEGMENT_H

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

#define MAX_DATA 80
#define HEADER_SIZE 64
#define MAX_WINDOW_SIZE 128

typedef enum
{
	HS = 1,
 	HS_SCK = 2,
  	SND = 3,
   	TERM = 4,
    	ACK = 5,
	ACK_TERM = 6
} opcodes;

struct rdt_segment
{
	uint16_t opcode;
	uint16_t checksum;
	uint32_t seq_ack_num;
	//uint32_t window_size;
	
	char data[MAX_DATA + 1];
};

struct qentry
{
	struct rdt_segment seg;	//segment
	int sent;
	int ack;
	unsigned long sent_time;
	int delay;
	
	struct qentry * next;
	struct qentry * prev;
};

struct queue
{
	int size;
	
	struct qentry * head;
	struct qentry * tail;
};

uint16_t calc_checksum( struct rdt_segment * segment );
int wait_for_one( int udt, struct rdt_segment * dseg, int maxtime, in_addr_t *addr, in_port_t *port);
int process_more( int udt, int (*segment_handler) (struct rdt_segment seg) );

//fronta
int qinit( struct queue * q );
int qpush_back( struct queue * q, struct rdt_segment * seg );
int qpop_front( struct queue * q );

//FUNKCIE DEFINOVANE V udt.h
int udt_init(in_port_t local_port);
int udt_recv(int udt, void *buff, size_t nbytes, in_addr_t *addr, in_port_t *port);
int udt_send(int udt, in_addr_t addr, in_port_t port, void *buff, size_t nbytes);
#endif
