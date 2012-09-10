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
static int lastackseq = 1;

static int lastsendseq = 1;

static int requested_term = 0;
static long requested_term_time = 0;
static int ack_term = 0;

static struct queue window;

/**
struct rdt_segment
{
	uint16_t opcode;
	uint16_t checksum;
	uint32_t seq_ack_num;
	uint32_t window_size;
	
	char data[MAX_DATA + 1];
};
**/
/**
int udt_init(in_port_t local_port);
int udt_recv(int udt, void *buff, size_t nbytes, in_addr_t *addr, in_port_t *port);
int udt_send(int udt, in_addr_t addr, in_port_t port, void *buff, size_t nbytes);
**/

/**
int wait_for_one( int udt, struct rdt_segment * dseg, int maxtime, in_addr_t *addr, in_port_t *port);
**/

//funkcia zodpovedna za znovuodoslanie
void sigalrm_handler(int sig) {	
	struct qentry * tmp;
	unsigned long now = (unsigned long) time (NULL);
	
	for ( tmp = window.head; tmp != NULL; tmp = tmp->next )
	{
		//cekujeme znovuodoslanie
		if ( now - tmp->sent_time > tmp->delay )
		{
			//resetuj cas
			tmp->sent_time = now;
			udt_send(dsock, remote_addr, default_sport, tmp, sizeof(*tmp));	
			//printf("Resending sequence: %d\n", tmp->seg.seq_ack_num);
		}			
	}

	
	if ( (requested_term == 1) && ( ack_term == 0) )
		if ( now - requested_term_time > 5 )
			{
				struct rdt_segment tst = { TERM, 0, lastsendseq, "" };
				tst.checksum = calc_checksum(&tst);
				requested_term = 1;
				requested_term_time = (unsigned long) time (NULL);
				udt_send(dsock, remote_addr, default_sport, &tst, sizeof(tst));
			}
			
	
	signal(SIGALRM, sigalrm_handler);
}

/**
int
do_hs( int dsock )
{
	int retries = 3;
	
	struct rdt_segment out;
	struct rdt_segment in = { HS, 0, 1, 0, "" };
	in.checksum = calc_checksum( &in );
	
	while ( retries )
	{
		udt_send( dsock, remote_addr, default_sport, &in, sizeof(in) );
		
		if ( wait_for_one (dsock, &out, 5, &remote_addr, &default_sport) > 0 )
			if ( out.checksum == calc_checksum(&out) )
				break;
		
		retries--;
	}
	
	if ( retries == 0 )
		fprintf(stderr,"Handshake failed, host offline or broken line\n");
	
	wininitsize = winsize = out.window_size;
	printf("window size: %d", out.window_size);
		
}
**/

int
main_op_cycle()
{
	//prve naplnenie
	
	//kazdych 200 milisekund sa prebehne fronta a zisti sa ci netreba znovuodoslat data
	signal(SIGALRM, sigalrm_handler);
	struct itimerval itv;
	itv.it_interval.tv_sec = 0; 	// sets an interval of the timer
	itv.it_interval.tv_usec = 200000;	
	itv.it_value.tv_sec = 0;		// sets an initial value
	itv.it_value.tv_usec = 200000;
	setitimer(ITIMER_REAL, &itv, NULL);

	// we may need to mask timer later
	sigset_t sigmask;
	sigemptyset(&sigmask);
	sigaddset(&sigmask, SIGALRM);
	
	dsock = udt_init(default_lport);
	
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);	// make stdin reading non-clocking

	fd_set readfds;	
	
	char inbuffer[HEADER_SIZE + MAX_DATA + 1];
	//cakame na stdin a socket
	//ak su data na vstupe pozreme sa na okno a ak sa posunulo odosleme dalsie
	//ak prisli data na socket potvrdime nalezite segmenty a pripadne slidujeme
	while ( 1 )
	{
		FD_ZERO(&readfds);
		FD_SET(dsock, &readfds);
		FD_SET(STDIN_FILENO, &readfds);
		
		int retval = select(dsock+1, &readfds, NULL, NULL, NULL);
		
		if ( retval <= 0 ) continue;
		
		if (FD_ISSET(STDIN_FILENO, &readfds) ) { // we have read a new line to send
			//stdin ready
			//skontrolujeme frontu
			//ak volno posleme a pridame
			//fgets(inbuffer, MAX_DATA, stdin)>0
			sigprocmask(SIG_BLOCK, &sigmask, NULL);
			
			if ( window.size < winsize )
			{
				char chardata[81];
				if ( fgets(chardata, 80, stdin) != NULL )
				{
					//normal send

					chardata[80] = 0;
					struct rdt_segment ts = { SND, 0, lastsendseq, "" };
					strcpy(ts.data, chardata);
					lastsendseq++;
					ts.checksum = calc_checksum(&ts);
					qpush_back(&window, &ts);

					window.tail->sent = 1;
					window.tail->ack = 0;					
					window.tail->sent_time = (unsigned long) time (NULL);
					window.tail->delay = 5;

					udt_send(dsock, remote_addr, default_sport, &ts, sizeof(ts));	
					//printf("Pack seq: %d sent %ld: %s\n", lastsendseq-1, window.tail->sent_time, ts.data);				 
					
				} else
				{
					if ( requested_term != 1 )
					{
						struct rdt_segment tst = { TERM, 0, lastsendseq, "" };
						tst.checksum = calc_checksum(&tst);
						requested_term = 1;
						requested_term_time = (unsigned long) time (NULL);
						udt_send(dsock, remote_addr, default_sport, &tst, sizeof(tst));
						//printf("Pack TERM sent %d\n", lastsendseq);	
					}
				}
			} else
			{
				//printf("wait for ack");
			}
			
			sigprocmask(SIG_UNBLOCK, &sigmask, NULL);
		}
		
		if (FD_ISSET(dsock, &readfds)) {	// Prisla sprava - acknowledge alebo exit no
			sigprocmask(SIG_BLOCK, &sigmask, NULL);
			udt_recv(dsock, inbuffer, HEADER_SIZE + MAX_DATA + 1, NULL, NULL);
			struct rdt_segment rc = *((struct rdt_segment *) inbuffer);
			
			uint16_t chck = calc_checksum(&rc);
			if ( chck != rc.checksum ) {sigprocmask(SIG_UNBLOCK, &sigmask, NULL);continue;}
			
			switch( rc.opcode )
			{
				case ACK:
					;
					//normal ack
					//printf("Got ack for %d rc.seq_ack_num\n", rc.seq_ack_num);
					if ( rc.seq_ack_num >= lastackseq + winsize ) //pasuje do okna ?  
						break;
					
					//najdi a oznac za potvrdene
					struct qentry * tmp;
					for ( tmp = window.head; tmp != NULL; tmp = tmp->next )
					{
						if ( rc.seq_ack_num == tmp->seg.seq_ack_num )
							tmp->ack = 1;			
					}
					//posun okno
					for ( tmp = window.head; tmp != NULL; tmp = tmp->next )
					{
						if ( tmp->ack == 1 )
						{
							qpop_front( &window );
							lastackseq++;	
						} else
							break;		
					}	
									
					if ( (ack_term == 1) && (window.size == 0) )
					{
						//koniec
						exit(0);
					}
					
					break;
				case ACK_TERM:
					;
					ack_term = 1;
					//printf("TERM ackwged\n");
					if ( window.size == 0 )	//ako posledny
					{
						//koniec
						exit(0);
					}
					break;
				default:
					break;
			}
			
			sigprocmask(SIG_UNBLOCK, &sigmask, NULL);			
			
		}
				
		//reinstate arrazs ?
		
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
		case 's':
				default_lport = atol(optarg);
			break;
		case 'd':
				default_sport = atol(optarg);
			break;
		}
	}
	main_op_cycle();	
	
	return 0;
}
