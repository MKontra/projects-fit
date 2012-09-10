/*
* Projekt IPK3
* Autor: Matus Kontra, xkontr00@stud.fit.vutbr.cz
* Datum: 5.5.2009
*/

#define _POSIX_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>

#include "rdtsegment.h"

static in_addr_t remote_addr = 0x7f000001;	//localhost
static in_port_t default_lport = 30000;
static in_port_t default_sport = 20000;

static int dsock = -1;
static int winsize = 4;

static int lastsendseq = 1;
static int lastseqnum = 0;


static struct queue window;

int
qpush_prior_back( struct queue * q, struct rdt_segment * seg )
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
		struct qentry * it;
		for ( it = q->head; it != NULL; it=it->next )	//najdi poziciu
		{
			if ( it->seg.seq_ack_num == seg->seq_ack_num ) { free (tmp); return -1; } //paket tam uz je
			if ( it->seg.seq_ack_num > seg->seq_ack_num ) break;
		}
		//it obsahuje prvok na ktoreho mieste by ma byt 
		if ( it == q->head ) //na miesto prveho
		{
			q->head->prev = tmp;
			tmp->prev = NULL;
			tmp->next = q->head;
			q->head = tmp;
		} else if ( it == NULL )
		{
			//posledne miesto
			q->tail->next = tmp;
			tmp->prev = q->tail;
			tmp->next = NULL;
			q->tail = tmp;
			
		} else  //niekde v strede 
		{
			tmp->prev = it->prev;
			tmp->next = it;
			it->prev->next = tmp;
			it->prev = tmp;			
		}
		
		q->size += 1;
		return 0;
	}
}

int
main_loop()
{
	
	dsock = udt_init(default_sport);	
	
	fd_set readfds;	
	char inbuffer[HEADER_SIZE + MAX_DATA + 1];
		
	while ( 1 )
	{
		FD_ZERO(&readfds);
		FD_SET(dsock, &readfds);
		
		int retval = select(dsock+1, &readfds, NULL, NULL, NULL);
		
		if ( retval <= 0 ) continue;		
		
		if (FD_ISSET(dsock, &readfds)) {	// Prisla sprava - acknowledge alebo exit no
			udt_recv(dsock, inbuffer, HEADER_SIZE + MAX_DATA + 1, NULL, NULL);
			struct rdt_segment rc = *((struct rdt_segment *) inbuffer);
			
			uint16_t chck = calc_checksum(&rc);
			if ( chck != rc.checksum ) {continue;}
			
			//printf("Got %d %d", rc.opcode, rc.seq_ack_num);
			
			switch( rc.opcode )
			{
				case SND:
					;
					//prisiel paket
					
					//printf("received valid %d, data:%s\n", rc.seq_ack_num, rc.data);
					
					if ( rc.seq_ack_num < lastsendseq )
					{
						//prisiel paket ktory sme uz potvrdili a vypisali
						//posleme znova potvrdenie a neriesime
						//printf("already acknowledged and printed\n");
						//printf("----------------------------\n\n");
						struct rdt_segment tst = { ACK, 0, rc.seq_ack_num, "" };
						tst.checksum = calc_checksum(&tst);
						udt_send(dsock, remote_addr, default_lport, &tst, sizeof(tst));
					} else 
					if ( rc.seq_ack_num >= lastsendseq + winsize )
					{
						//printf("Cannot receive window too small\n");
						//printf("----------------------------\n\n");
						//prilis vysoka sekvencia este sme neprijali dost paketov a v okne neni miesto
						; //nerobime nic
					} else
					{
						//paket je z oblasti ktoru mozeme prijat
						//printf("In WIN flushing:\n");
						qpush_prior_back(&window, &rc);
						
						struct qentry * tmp;
						

						for ( tmp = window.head; tmp != NULL; tmp = tmp->next ) //cekneme ci mozeme pisat
						{
							if ( tmp->seg.seq_ack_num == lastsendseq ) //ok mozme vypiat a popnut
							{		
								printf("%s", tmp->seg.data);
								qpop_front(&window);
								lastsendseq++;
							} else
								break;
						}
						//printf("Till seq: %d\n", lastsendseq);
						//printf("----------------------------\n\n");
						//posli ack
						
						struct rdt_segment tst = { ACK, 0, rc.seq_ack_num, "" };
						tst.checksum = calc_checksum(&tst);
						udt_send(dsock, remote_addr, default_lport, &tst, sizeof(tst));
						
						if ( lastsendseq == lastseqnum )	//ukoncenie spojenia
						{
							//fprintf(stderr,"koniec\n");
							exit(0);
						}
					}
					break;
					
				case TERM:
					;
					//potvrd term pockaj na vsetky sequencie a skonci
					lastseqnum = rc.seq_ack_num;
					struct rdt_segment tst = { ACK_TERM, 0, rc.seq_ack_num, "" };
					tst.checksum = calc_checksum(&tst);
					udt_send(dsock, remote_addr, default_lport, &tst, sizeof(tst));
						if ( lastsendseq == lastseqnum )	//ukoncenie spojenia
						{
							//fprintf(stderr,"koniec\n");
							exit(0);
						}
					break;
					
				default: break;
			}
			
		}		
		
	}
}

int
main( argc, argv )
	int argc;
	char * argv[];
{
	qinit(&window);

	char ch;

	while ((ch = getopt(argc,argv,"d:s:")) != -1) {
		switch(ch) {
		case 'd':
				default_lport = atol(optarg);
			break;
		case 's':
				default_sport = atol(optarg);
			break;
		}
	}
	
	main_loop();	
	
	return 0;
}