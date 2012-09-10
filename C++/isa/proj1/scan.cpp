#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <cctype>
#include <cstring>
#include <cstdlib>
#include <getopt.h>
#include <stdint.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pcap.h>
#include <pthread.h>
#include <signal.h> 

#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
//#include <netinet/ip_icmp.h>
//#include <netinet/ip.h>
//#include <netinet/udp.h>
//#include <netinet/tcp.h>

using namespace std;

//konstanty reprezentujuce stavy portov v map
#define PORT_NOTPROCD	  -1
#define PORT_OPEN	  0
#define PORT_CLOSED	  1
#define PORT_FILTERED     2

#define VERB 1

//maximum palarernych poziadavkou pri danych scanoch

//udp mensi pocet - na testovacom stroji chodili nespravne icmp odpovede pri zahlteni 100+...
#define MAXTHREADSUDP 4
//tcp takyto problem nemalo
#define MAXTHREADSTCP 100


/** POPISY STRUKTUR HEADROV ZKOPIROVANE Z HLAVICKOVYCH SUBOROV JADRA BSD **/

/** pri includoch vznikali rozne konfliky takze som zvolil radsej takyto sposob
    pravdepodobne sposobene vyvojom na linuxe... **/

/** kopirovane z netinet/ip.h resp netinet/tcp.h **/
struct ip {
         u_char  ip_hl:4,ip_v:4;	 /*version and headder length*/
         u_char  ip_tos;                 /* type of service */
         u_short ip_len;                 /* total length */
         u_short ip_id;                  /* identification */
         u_short ip_off;                 /* fragment offset field */
         #define IP_RF 0x8000            /* reserved fragment flag */
         #define IP_DF 0x4000            /* dont fragment flag */
         #define IP_MF 0x2000            /* more fragments flag */
         #define IP_OFFMASK 0x1fff       /* mask for fragmenting bits */
         u_char  ip_ttl;                 /* time to live */
         u_char  ip_p;                   /* protocol */
         u_short ip_sum;                 /* checksum */
         struct  in_addr ip_src,ip_dst;  /* source and dest address */
};

typedef u_int32_t tcp_seq;

struct tcphdr {
           u_short th_sport;
           u_short th_dport;
           tcp_seq th_seq;
           tcp_seq th_ack;

           u_int   th_x2:4,
                   th_off:4;

		   
/**
           u_int   th_off:4,
                   th_x2:4;
**/
           u_char  th_flags;
   #define TH_FIN  0x01
   #define TH_SYN  0x02
   #define TH_RST  0x04
   #define TH_PUSH 0x08
   #define TH_ACK  0x10
   #define TH_URG  0x20
   #define TH_ECE  0x40
   #define TH_CWR  0x80
   #define TH_FLAGS        (TH_FIN|TH_SYN|TH_RST|TH_PUSH|TH_ACK|TH_URG|TH_ECE|TH_CWR)
   #define PRINT_TH_FLAGS  "\2\1FIN\2SYN\3RST\4PUSH\5ACK\6URG\7ECE\10CWR"
 
           u_short th_win;
           u_short th_sum;
           u_short th_urp;
 };

 /** custom udp header **/
struct udphdr {
	u_short uh_sport;               /* source port */
	u_short uh_dport;               /* destination port */
	u_short uh_ulen;                /* udp length */
	u_short uh_sum;                 /* udp checksum */
};

/** pseudo hlavicka pouzite pri vypocte checksumov **/
struct pseudo_hdr {
        u_int32_t src;
        u_int32_t dst;
        u_char mbz;
        u_char proto;
        u_int16_t len;
};

/** klasicky internetovy checksum - ako posisany v rfc - pozity v ip checksume**/
uint16_t
calc_checksum( char* addr, int count )
{
	long sum = 0;
	
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

/** chcecksum pouzity do tcp headru - zmeneny koli byte orderingu **/
uint16_t checksum_comp (uint16_t *addr, int len) {   
	register long sum = 0;
	int count = len;
	uint16_t temp;

	while (count > 1)  {
		temp = htons(*addr++);
		sum += temp;
		count -= 2;
	}

	/*  Add left-over byte, if any */
	if(count > 0)
		sum += *(unsigned char *)addr;

	/*  Fold 32-bit sum to 16 bits */
	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	uint16_t checksum = ~sum;
	return checksum;
}

/**
	trieda obalujuca socket
	uklada si informacie o zdroji a destinacii a
	umoznuje posielat ip datagramy so spravne nastavenymi poliami a cheksumom
**/

class RawSender
{
	int sock_id;
	int protoid;
	struct in_addr source;
	struct in_addr dest;

	
	
	public:
	RawSender(int protocolid, const char * sraddr,const char * destaddr)
	{

		protoid = protocolid;

		hostent * lolwar;

		lolwar = gethostbyname(destaddr);

		if ( lolwar == NULL )
		{	
			cout <<"Cant lookup target host\n";
			throw "Cant lookup target host";
		}
		dest = *((in_addr *)lolwar->h_addr_list[0]);
		//cout << "dest:" << (int) dest.s_addr << endl;

		lolwar = gethostbyname(sraddr);

		if ( lolwar == NULL )
		{	
			cout <<"Cant lookup our hostname\n";
			throw "Cant lookup our hostname";
		}
		
		source = *((in_addr *)lolwar->h_addr_list[0]);
		
		if ( 0 > (sock_id = socket (AF_INET, SOCK_RAW, IPPROTO_RAW)))
		{
			//free(hostname);
			//free(me);
			throw "Cant create socket";
		}	

		int jeden = 1;
		int * pjeden = &jeden;
		if ( 0 > ( setsockopt(sock_id, IPPROTO_IP, IP_HDRINCL, pjeden, sizeof(jeden))) )
		{
			close(sock_id);
			throw "Cant force custom ip headers";
		}
		
	}
	~RawSender()
	{
		close(sock_id);
	}
	
	
	in_addr getDestAddr()
	{
		return dest;
	}
	
	in_addr getSourceAddr()
	{
		return source;
	}

	void
	set_protocolid(int id)
	{
		protoid = id;
	}
	
	int sendIPdatagram(char * payload, int length)
	{
		char * packet = new char[sizeof(ip) + length];
		if ( packet == NULL )
			return -1;
		memset(packet, 0, sizeof(ip) + length); 
		ip * iphp = (ip *)packet;
		iphp->ip_hl = 5;
		iphp->ip_v = 4;
		iphp->ip_tos = 0;
		iphp->ip_len = sizeof(ip) + length;
		iphp->ip_id = htonl(54321);
		iphp->ip_off = 0;
		iphp->ip_ttl = 255;
		iphp->ip_p = protoid;
		iphp->ip_src.s_addr = source.s_addr;
		iphp->ip_dst.s_addr = dest.s_addr;
		iphp->ip_sum = calc_checksum( packet, sizeof(ip));

		memcpy(packet+sizeof(ip), payload, length);

		sockaddr_in dest;
		memset (&dest, 0, sizeof(dest));
		dest.sin_family = AF_INET;
		dest.sin_addr.s_addr = iphp->ip_dst.s_addr;

		if ( sendto(sock_id, packet, iphp->ip_len, 0, (struct sockaddr*) &dest, sizeof(dest)) < 0 )
		{
			perror("Sendto failed");
			return -1;
		}
		delete packet;
		return 0;
		
	}

};

//zamok nad odosielacim socketom
pthread_mutex_t rawsenderlock = PTHREAD_MUTEX_INITIALIZER;
RawSender * rs;

//zamok pre pristup k zoznamom portov
pthread_mutex_t mapaccesslock = PTHREAD_MUTEX_INITIALIZER;
map<int, int> * udpports;
map<int, int> * tcpports;

//cielova adresa
string addr;
//adresa zariadenia na ktorom cakat odpovede
string localinterface;

int threadsin = 0;
int thcount = 0;
pthread_mutex_t maxthread_mutex;
pthread_cond_t maxthread_count;

/** funkcia spustana v kazdom separatnom vlakne, co port to vlakno
	ziska stav portu - ak bol prijaty paket v sniffery - je bud open alebo closed 
	vlakno sa ukonci

	po troch pokusoch nastavi port na filtovany a ukonci sa
**/

void *
synscanfunc(void * ptr)
{
	// cucni si port z parametru - prevod  64 bitoveho pointra na 32 bitovy integer
	long ptval = reinterpret_cast<long> (ptr);
	int ourport = static_cast<int>(ptval);
	int tries = 3;
	while (tries--)
	{
		//ziskanie excluzivneho pristupu k mape
		pthread_mutex_lock(&mapaccesslock);
		if ( tcpports->find(ourport)->second != PORT_NOTPROCD )	//port uz bol spracovany
		{
				pthread_mutex_unlock(&mapaccesslock);	
				pthread_mutex_lock(&maxthread_mutex);
				//cout << "Decreasing thread count...\n";
				thcount--;
				pthread_cond_signal(&maxthread_count);
				pthread_mutex_unlock(&maxthread_mutex);	
				return NULL;
		}
		pthread_mutex_unlock(&mapaccesslock);		
		
		//priprav tcp paket pre destinaciu
		char * psdandtcp = new char[sizeof(tcphdr) + sizeof (pseudo_hdr)];
		if ( psdandtcp == NULL )
		{
				cout << "Alloc failed...\n";
				pthread_mutex_lock(&maxthread_mutex);
				//cout << "Decreasing thread count...\n";
				thcount--;
				pthread_cond_signal(&maxthread_count);
				pthread_mutex_unlock(&maxthread_mutex);
				return NULL;	
		}
		struct pseudo_hdr * pseudo = (pseudo_hdr *) psdandtcp;
		
		pseudo->src = (rs->getSourceAddr()).s_addr;
		pseudo->dst = (rs->getDestAddr()).s_addr;
		pseudo->mbz = 0;
		pseudo->proto = IPPROTO_TCP;
		pseudo->len = ntohs(0x14);		
		
		struct tcphdr * tosendhdr = (tcphdr *)(psdandtcp + sizeof (pseudo_hdr));
		tosendhdr->th_sport = htons(12345);
		tosendhdr->th_dport = htons(ourport);
		tosendhdr->th_seq = random();
		tosendhdr->th_ack = 0;
		tosendhdr->th_off = 5;
		tosendhdr->th_x2 = 0;
		tosendhdr->th_flags = TH_SYN;
		tosendhdr->th_win = (65535);
		tosendhdr->th_sum = 0;
		tosendhdr->th_urp = 0;
		
		tosendhdr->th_sum = htons(checksum_comp((uint16_t*)psdandtcp,sizeof(tcphdr) + sizeof (pseudo_hdr) ));

		pthread_mutex_lock(&rawsenderlock);
		//odosli paket
		if(0>rs->sendIPdatagram((char *)tosendhdr, sizeof(tcphdr)))
		{
			cout << "SEND FAILED\n";
			pthread_mutex_unlock(&rawsenderlock);
			return 0;
		}	
		pthread_mutex_unlock(&rawsenderlock);			
		if ( tries != 0 )
			sleep(3);
	}

	// po troch neuspesnich pokusoch - oznac port ako filtrovany	
	pthread_mutex_lock(&mapaccesslock);
	if ( tcpports->find(ourport)->second == PORT_NOTPROCD )
		(*tcpports)[ourport] = PORT_FILTERED;
	pthread_mutex_unlock(&mapaccesslock);
	
	//zniz pocet vlakien a notifikuj monitor
	pthread_mutex_lock(&maxthread_mutex);
	thcount--;
	pthread_cond_signal(&maxthread_count);
	pthread_mutex_unlock(&maxthread_mutex);	
	
	return 0;
	
}

/**
	funkcia ktora spawnuje vlakna skenujuce tcp porty
**/

int
tcp_syn_scan()
{
  	map<int,int>::iterator it;	//iterator
	pthread_t pt;
	pthread_attr_t atts;
	pthread_attr_init(&atts);
	pthread_attr_setdetachstate(&atts, PTHREAD_CREATE_DETACHED);
	threadsin = MAXTHREADSTCP;
	thcount = 0;
	int retval = 0;
	for ( it=tcpports->begin() ; it != tcpports->end(); it++ )
	{
		//cout << "Spawning thread for TCP port "<<(*it).first<<endl;
		pthread_mutex_lock(&maxthread_mutex);
		if ( 0 != (retval = pthread_create( &pt, &atts, synscanfunc, (void *) ((*it).first))))
		{
			//cout << " FIRST ATTEMPT FAILED...\n";
			while (1)
			{
				
				retval = pthread_create( &pt, &atts, synscanfunc, (void *) ((*it).first));
				if (retval == 0) break;
				if (retval == 11) {cout<<"Resource unaviable\n"; sleep(1); continue;}
				perror("");
				cout << "Creation failed..." << retval << endl;;
				exit(-1);
			}
		}
		thcount++;
		// Ak bol dosiahnuty maximalny pocet paralernych vlakien pre ktore este nedosli odpovede - cakaj
		if ( thcount == threadsin )
		{
			//cout << "Maximum threads spawned waiting for some to finish...\n";
			//cakaj na dokoncenie niejakych vlakien....
			pthread_cond_wait(&maxthread_count, &maxthread_mutex);
		}
		pthread_mutex_unlock(&maxthread_mutex);
    	//	cout << (*it).first << " => " << (*it).second << endl;	
	}
	pthread_attr_destroy(&atts);
	
	//cakanie na koniec ???
	//cout << "ALL threadds created waiting to finish..." << i << endl;;
	
	//sleduj kondicnu premennu monitora az kym nedosiahne nulu - vsetky vlakna ukoncili cinnost
	while (1)
	{
		pthread_mutex_lock(&maxthread_mutex);	
		if ( thcount == 0 )
		{
			//cout << "TCP DONE\n";
			pthread_mutex_unlock(&maxthread_mutex);
			break;
		}
		pthread_cond_wait(&maxthread_count, &maxthread_mutex);
		pthread_mutex_unlock(&maxthread_mutex);
	}
	
	return 0;
}

/**
	funkcia je obrazom jej tcp verzie - sleduje sa zaznam v map ci nahodou nedoslo k zmene
	ak po troch pokusoch port nebol znaceny ako uzavrety- nastav ho na open
**/


void *
udpscanfunc(void * ptr)
{
	long ptval = reinterpret_cast<long> (ptr);
	int ourport = static_cast<int>(ptval);
	int tries = 3;
	while (tries--)
	{
		pthread_mutex_lock(&mapaccesslock);
		if ( udpports->find(ourport)->second != PORT_NOTPROCD )
		{
				pthread_mutex_unlock(&mapaccesslock);	
				pthread_mutex_lock(&maxthread_mutex);
				//cout << "Decreasing thread count...\n";
				thcount--;
				pthread_cond_signal(&maxthread_count);
				pthread_mutex_unlock(&maxthread_mutex);	
				return NULL;
		}
		pthread_mutex_unlock(&mapaccesslock);		
		
		char * psdandudp = new char[sizeof(udphdr) + sizeof (pseudo_hdr)];
		if ( psdandudp == NULL )
		{
				cout << "Alloc failed...\n";
				pthread_mutex_lock(&maxthread_mutex);
				cout << "Decreasing thread count...\n";
				thcount--;
				pthread_cond_signal(&maxthread_count);
				pthread_mutex_unlock(&maxthread_mutex);
				return NULL;	
		}
		struct pseudo_hdr * pseudo = (pseudo_hdr *) psdandudp;
		
		pseudo->src = (rs->getSourceAddr()).s_addr;
		pseudo->dst = (rs->getDestAddr()).s_addr;
		pseudo->mbz = 0;
		pseudo->proto = IPPROTO_UDP;
		pseudo->len = htons(8);//ntohs(sizeof(udphdr) + sizeof (pseudo_hdr));		
		
		
		struct udphdr * tosendhdr = (udphdr *)(psdandudp + sizeof (pseudo_hdr));
		tosendhdr->uh_sport = htons(12345);
		tosendhdr->uh_dport = htons(ourport);
		tosendhdr->uh_ulen = htons(8);
		tosendhdr->uh_sum = 0;

		
		tosendhdr->uh_sum = htons(checksum_comp((uint16_t*)psdandudp,sizeof(udphdr) + sizeof (pseudo_hdr) ));

		pthread_mutex_lock(&rawsenderlock);
		if(0>rs->sendIPdatagram((char *)tosendhdr, sizeof(udphdr)))
		{
			cout << "SEND FAILED\n";
			pthread_mutex_unlock(&rawsenderlock);
			pthread_mutex_lock(&maxthread_mutex);
			//cout << "Decreasing thread count...\n";
			thcount--;
			pthread_cond_signal(&maxthread_count);
			pthread_mutex_unlock(&maxthread_mutex);
			return NULL;
		}	
		pthread_mutex_unlock(&rawsenderlock);	
				
		if ( tries != 0 )
			sleep(3);
	}

		
	pthread_mutex_lock(&mapaccesslock);
	if ( udpports->find(ourport)->second == PORT_NOTPROCD )
		(*udpports)[ourport] = PORT_OPEN;
	pthread_mutex_unlock(&mapaccesslock);
	
	
	pthread_mutex_lock(&maxthread_mutex);
	thcount--;
	pthread_cond_signal(&maxthread_count);
	pthread_mutex_unlock(&maxthread_mutex);	
	
	return 0;
	
}
/**
	vytvorenie thredov a ich manazment podobne ako u tcp
**/
int
udp_scan()
{
  	map<int,int>::iterator it;	//iterator
	pthread_t pt;
	pthread_attr_t atts;
	pthread_attr_init(&atts);
	pthread_attr_setdetachstate(&atts, PTHREAD_CREATE_DETACHED);
	threadsin = MAXTHREADSUDP;
	thcount = 0;
	int retval = 0;
	for ( it=udpports->begin() ; it != udpports->end(); it++ )
	{
		//cout << "Spawning thread for TCP port "<<(*it).first<<endl;
		pthread_mutex_lock(&maxthread_mutex);
		if ( 0 != (retval = pthread_create( &pt, &atts, udpscanfunc, (void *) ((*it).first))))
		{
			//cout << " FIRST ATTEMPT FAILED...\n";
			while (1)
			{
				
				retval = pthread_create( &pt, &atts, udpscanfunc, (void *) ((*it).first));
				if (retval == 0) break;
				if (retval == 11) {cout<<"Resource unaviable\n"; sleep(1); continue;}
				perror("");
				cout << "Creation failed..." << retval << endl;;
				exit(-1);
			}
		}
		thcount++;
		if ( thcount == threadsin )
		{
			//cout << "Maximum threads spawned waiting for some to finish...\n";
			pthread_cond_wait(&maxthread_count, &maxthread_mutex);
		}
		pthread_mutex_unlock(&maxthread_mutex);
    	//	cout << (*it).first << " => " << (*it).second << endl;	
	}
	pthread_attr_destroy(&atts);
	
	//cakanie na koniec ???
	//cout << "ALL threadds created waiting to finish..." << i << endl;;
	
	while (1)
	{
		pthread_mutex_lock(&maxthread_mutex);	
		if ( thcount == 0 )
		{
			//cout << "TCP DONE\n";
			pthread_mutex_unlock(&maxthread_mutex);
			break;
		}
		pthread_cond_wait(&maxthread_count, &maxthread_mutex);
		pthread_mutex_unlock(&maxthread_mutex);
	}
	
	return 0;
}

/**
	funkcia co spracovava prijate pakety
**/
pcap_t *session;
void got_pack_callback(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
	// osetrenie datalink layera - nizsej vrstvy
	// problem napr pri loopbacku - nedostaneme ethernetovy header ale daco ine kratsie
	int ippos = 0;
	const int dlt = pcap_datalink(session);
	switch (dlt)
	{
		case DLT_NULL: ippos = 4;break;
		case DLT_EN10MB: ippos = ETHER_HDR_LEN;break;
		case DLT_LOOP: ippos = 4;break;
		case DLT_LINUX_SLL: ippos = 16;break;
		default: ippos = 0;
	}
	
	//lokalizuj ip header
	ip * ipheader;
	ipheader = (ip *) (packet + ippos);
	
	in_addr insrc = ipheader->ip_src;
	//cout << "From "<<inet_ntoa(insrc) <<" prot: ->"<< (int)ipheader->ip_p <<"<-"<<endl;
	
	//pripravene premenne
	tcphdr * tcp = NULL;
	int ourport = 0;
	int icmp_type = 0;
	int icmp_code = 0;
	char * icmp_start = NULL;
	map<int,int>::iterator it;
	
	switch ( (int)ipheader->ip_p )
	{
		//prisiel tcp paket
		case IPPROTO_TCP:
				;
				tcp = (tcphdr *)(packet + ippos + ipheader->ip_hl*4);
				ourport = ntohs(tcp->th_sport);
				//cout << "tcp paket from port "<< ourport <<endl;;
				// ACK - znamena ze port je open
				if (((tcp->th_flags & 0x02) == TH_SYN) && (tcp->th_flags & 0x10) == TH_ACK) {
					//cout << "synack reply\n";
					pthread_mutex_lock(&mapaccesslock);
					it = (*tcpports).find(ourport);
					if ( !(it == (*tcpports).end()) )
					{
						(*tcpports)[ourport] = PORT_OPEN;
					}
					pthread_mutex_unlock(&mapaccesslock);
				}
				// RESET - Closed
         			else if ((tcp->th_flags & 0x04 ) == TH_RST) {
					//cout << "got reset\n";
					pthread_mutex_lock(&mapaccesslock);
					it = (*tcpports).find(ourport);
					if ( !(it == (*tcpports).end()) )
					{
						(*tcpports)[ourport] = PORT_CLOSED;
					}
					pthread_mutex_unlock(&mapaccesslock);
				}
				break;
		//icmp - ak je to port unreachable zisti ci patri niektoremu nasemu poziadavku			
		case IPPROTO_ICMP:
				;
				icmp_start = (char *) (packet + ippos + ipheader->ip_hl*4);
				icmp_type = (int) (*icmp_start);
				icmp_code = (int) (*(icmp_start+1));
				//cout << "ICMP type: " << icmp_type << " code: " << icmp_code << endl;
				if ( (icmp_type == 3) && (icmp_code == 3 ) )
				{
					//icmp paket obsahuje kopiu ip paketu co sme poslali - minimalne hlavicku
					ip * icrep = (ip *) (packet + ippos + ipheader->ip_hl*4 + 8);
					udphdr * udrep = (udphdr*) (packet + ippos + ipheader->ip_hl*4 + 8 + icrep->ip_hl*4);
					//cout << "Unreachable port " << ntohs(udrep->uh_dport) << endl;
					pthread_mutex_lock(&mapaccesslock);
					it = (*udpports).find(ntohs(udrep->uh_dport));
					if ( !(it == (*udpports).end()) )
					{
						(*udpports)[ntohs(udrep->uh_dport)] = PORT_CLOSED;
					}
					pthread_mutex_unlock(&mapaccesslock);
					
				}
				break;
					
		default: break;
	}
	
}

/**
	konfiguracia sniffera
**/

void* cap_packs(void* param)
{
	struct bpf_program filter;
	char filtexp[128] = "src host ";
	char ipbuff[32];
	inet_ntop(AF_INET, &rs->getDestAddr().s_addr,ipbuff, 32);
	strcpy(filtexp+9, ipbuff);
	//cout << "filtering by: ->>"<<filtexp<<"<--"<<endl;
	
   	 char errbuf[PCAP_ERRBUF_SIZE];
	 
         if ((session = pcap_open_live ( (char*)param, 1024, 0, 10, errbuf)) == NULL) {
                 fprintf(stderr, "Could not open any device to listen on error: %s \n ", errbuf);
                 exit(EXIT_FAILURE);
         }
	 
         if (pcap_compile(session, &filter, filtexp, 0, 0) == -1) {
                 fprintf(stderr, "Couldn't parse filter %s: %s \n ", filtexp, pcap_geterr(session));
                 exit(EXIT_FAILURE);
         }

         if (pcap_setfilter(session, &filter) == -1) {
                 fprintf(stderr, "Couldn't install filter %s: %s\n", filtexp, pcap_geterr(session));
                 exit(EXIT_FAILURE);
         }
	
	//pcap_set_datalink(session, DLT_RAW);
	int errcode = pcap_loop(session, -1, got_pack_callback, NULL);
	cout << "should not happen " <<errcode<<"\n";
	return NULL;
}

/**
	funkcia na sparsovanie jednoducheho zoznamu
**/

map<int, int> *
parse_list(char * plist)
{
	map<int,int> * nops = new map<int, int>();

	stringstream ss (stringstream::in | stringstream::out);
	ss << plist;
	
	if ( !isdigit(ss.peek() ) )
	{
		cout << "Error parsing param list\n";
		return NULL;
	}

	while ( ss.good() )
	{
		int tval = ss.peek();
		if ( isdigit(tval) )
		{
			int fval;
			int sval;
			try {
				ss >> fval;
				if ( ss.peek() == '-' )
				{
					ss.get();
					ss >> sval;
					for ( int i = fval; i <= sval; i++ )
						(*nops)[i]=PORT_NOTPROCD;
				} else 
					(*nops)[fval]=PORT_NOTPROCD;
			} catch (ios_base::failure& interr)
			{
				cout<< "Eroor retrieving numbers\n";
				return NULL;
			}
			
		} else
		if ( tval == ',')
		{
			//if ( !isdigit(ss.peek() ))
			//	cout << "Redundant colon at end of port list\n";
			ss.get();
		}
		else
		{
		cout << "Error parsing param list\n";
		return NULL;
		}

	}
	return nops;
}

// zparsovanie command-line getoptom

int
parse_args_and_fill
(
	int cnt,
	char * pa[]
)
{
	if ( cnt < 2 ) return -1;
	
	int pc;
	map<int,int> * tmp;

	
        static struct option long_options[] =
        {
               /* These options set a flag. */
               {"pt", required_argument, 0, 't'},
               {"pu", required_argument, 0, 'u'},
               {0, 0, 0, 0}
        };
	int option_index = 0;

	
	while ( (pc = getopt_long_only (cnt, pa, "i:t:u:", long_options, &option_index)) != -1 )
	{
		switch (pc)
		{
			case 'i':
				;
				localinterface = string(optarg);
				break;
			case 't':
				;
				tmp = parse_list(optarg);
				if ( tmp == NULL )
				{
					cout << "List parsing error" <<endl;
					return -1;
				} else {
				tcpports->insert(tmp->begin(), tmp->end());
				delete tmp;}
				break;
			case 'u':
				;
				tmp = parse_list(optarg);
				if ( tmp == NULL )
				{
					cout << "List parsing error" <<endl;
					return -1;
				} else {
				udpports->insert(tmp->begin(), tmp->end());
				delete tmp;}
				break;
			default:
				;
				cout << "Unknown option" << endl;
				return -1;
		}
		option_index = 0;
	}

	if ( optind == cnt )
	{
		cout << "Missing ip address\n";
		return -1;
	}
	addr = string(pa[optind]);

	return 0;
}
	

int
main
(
	int argc,
	char *argv[]
)
{
	udpports = new map<int, int>;
	tcpports = new map<int, int>;

	//spracuj prikazovy riadok
	if ( 0 > parse_args_and_fill(argc, argv) )
	{
		delete udpports;
		delete tcpports;
		cout << "Bad command line" << endl;
		return -1;
	}
	
	//pravdepodobne nemoze nastat ale osetrenie nikdy nezaskodi
	if ( (udpports->size()==0) && (tcpports->size()==0) )
	{
		delete udpports;
		delete tcpports;
		cout << "Bad command line - no ports selected" << endl;
		return -1;
	}
	
	
	//vyhladanie zariadenia ktore bude odposluchavat
   	char errbuf[PCAP_ERRBUF_SIZE];

	pcap_if_t *alldev;
	if ((pcap_findalldevs(&alldev, errbuf)) == -1) {
		fprintf (stderr, "%s\n", errbuf);
		return -1;
	}
	
	if ( alldev == NULL ) {
		fprintf (stderr, "Run with higher privileges...\n");
		return -1;
	}
		
	
	char * dev = NULL;
	sockaddr_in * locip = NULL;
	sockaddr_in lback;
	inet_pton(AF_INET,"127.0.0.1",&(lback.sin_addr));
			
	//ak bol specifikovany parameter - pokus sa najst apouzit tento interface
	if ( !localinterface.empty() )
	{	
		//cout << "trying to lookup specified interface in list provided by pcap...";
		pcap_if_t * itr = NULL;
		for (itr = alldev; itr != NULL; itr = itr->next)
		{
			if (strcmp(itr->name, localinterface.c_str()) == 0 )
			{
			    if ((strcmp(itr->name,"lo")==0) || (strcmp(itr->name, "lo0")==0) )
                {
                    locip = &lback;
                }
                else
                { 
				    locip = (struct sockaddr_in *)itr->addresses->next->addr;
				}
				dev = itr->name;
				//cout << "OK\n";	
			}
		}
		
	}
	
	//ak parameter nebol pouzity - vyhladaj prve verejne - vacsinou eth0, em0 - prve fyzycke rozhranie
	if ( dev == NULL )
	{
		//cout << "FAILED\n";
		//cout << "no or incorrect interface parameter detected - trying first pulic one...\n";
		locip = (struct sockaddr_in *)alldev->addresses->next->addr;
		dev = alldev->name;		
	}
	
	cout << "Using device: "<< dev << " on address: " <<inet_ntoa((locip)->sin_addr)<<endl;
	
	//vytvor raw socket a uloz zdrojovu a cielovu adresu
	try {
		rs = new RawSender( IPPROTO_TCP ,inet_ntoa((locip)->sin_addr), addr.c_str() );
	} catch (char const * errcode)
	{
		return -1;
	}
	//vlakno v ktorom bezi odposluchavanie...
	pthread_t pt;

	pthread_create( &pt, NULL, cap_packs,(void*) dev);

	//cout << "Sniffing...\n";
	
	if ( !tcpports->empty() )
	{	
		//vykonaj SYN scan tcp portov
		tcp_syn_scan();
		map<int,int>::iterator it;	//iterator
		//tisk vysledkou
		cout << "TCP:\n";
		for ( it=tcpports->begin() ; it != tcpports->end(); it++ )
		{	
			cout << (*it).first << " => ";
			switch ( (*it).second )	
			{
				case PORT_OPEN: cout << "OPEN\n"; break;
				case PORT_CLOSED: cout << "CLOSED\n"; break;
				case PORT_FILTERED: cout << "FILTERED\n"; break;
				default: break;
			}
		}	
	}
	if ( !udpports->empty() )
	{	
		//prepni raw socket na udp pakety
		rs->set_protocolid(IPPROTO_UDP);
		//vykonaj UDP scan
		udp_scan();
		map<int,int>::iterator it;	//iterator
		cout << "UDP:\n";
		for ( it=udpports->begin() ; it != udpports->end(); it++ )
		{
			cout << (*it).first << " => ";
			switch ( (*it).second )	
			{
				case PORT_OPEN: cout << "OPEN\n"; break;
				case PORT_CLOSED: cout << "CLOSED\n"; break;
				case PORT_FILTERED: cout << "FILTERED\n"; break;
				default: break;
			}	
		}	
	}
	//scany dokoncene - zastav vlakno odpocuvania
	pthread_cancel(pt);
	//pthread_join(pt, NULL);
	//uvolni dynamicke zdroje
	pcap_freealldevs(alldev);
	delete udpports;
	delete tcpports;
	delete rs; // zatvor soket
	return 0;
}

