/*
proj03.c: project 3 for ARC
autor: NAPISTE SVE JMENO A LOGIN
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mpi.h>

/* definition of global data functions  - uses NUM_OF_PARTICLES */
#include "globaldata.h"


void usage(char *prog)
{
	fprintf(stderr, "usage : %s filename time_step_size num_of_time_steps\n", prog);
	exit(1);
}

/*
* Funkce abstrahujici implementaci kruhove topologie. Prevzato z MPI_test
*/
int GetSousedVlevo(int ProcID, int ProcCount){
  // -- vrati souseda vlevo --//

  return (ProcID ==0) ?  ProcCount -1 : ProcID -1;
} //-- GetSousedVlevo --//


/*
* Funkce abstrahujici implementaci kruhove topologie. Prevzato z MPI_test
*/
int GetSousedVpravo(int ProcID, int ProcCount){
  //-- vrati souseda vpravo --//
  
  return (ProcID + 1) % ProcCount;  
} //-- GetSousedVpravo --//

int GetChunkSizeFor(int ProcID, int ProcCount)
{
  return ProcID != ProcCount -1 ? NUM_OF_PARTICLES/ProcCount : NUM_OF_PARTICLES - (NUM_OF_PARTICLES/ProcCount) * ProcID;  
}

// Datovy typ, ktery slouzi pro posilani dat mezi procesy. Staci posilat pouze polohy.
typedef struct
{
	double position [3];
} t_pos;

typedef enum
{
	NO_OVERLAP,
	OVERLAP
} tOverlap;

/*
* Sekvencni reseni. Pouze vytazeno z funkce main. Netreba dalsiho komentare.
*/
void seqSolution(t_particle **part, int num_of_time_steps, double time_step_size)
{
	t_particle *particles = *part;
	int i = 0;
	int p = 0;
	t_particle *pp = NULL;


	pp = alocate_particle_vector();
	memcpy(pp,particles,sizeof(t_particle)*NUM_OF_PARTICLES);


	for (p = 0; p < num_of_time_steps; p++){
		t_particle *pom = NULL;
		pom = pp;
		pp = particles;
		particles = pom;

		for (i=0;i<NUM_OF_PARTICLES; i++) {
			double force0 = 0;
			double force1 = 0;
			double force2 = 0;

			double d0; 
			double d1;
			double d2;

			double f;
			int    j;
			double r;
			double r32;

			for (j=0;j<NUM_OF_PARTICLES;j++){
				d0 = pp[j].position[0]-pp[i].position[0];
				d1 = pp[j].position[1]-pp[i].position[1];	
				d2 = pp[j].position[2]-pp[i].position[2];

				r   = (d0 * d0) + (d1 * d1) + (d2 * d2);
				r32 = pow (r, 1.5);


				if (r != 0){
                                        f=(GRAVITY*pp[j].weight*pp[i].weight)/(r32);
					force0 += f*d0;
					force1 += f*d1;
					force2 += f*d2;
				} 	
			}

			particles[i].velocity[0] = pp[i].velocity[0] + (time_step_size * force0);
			particles[i].velocity[1] = pp[i].velocity[1] + (time_step_size * force1);
			particles[i].velocity[2] = pp[i].velocity[2] + (time_step_size * force2);

			particles[i].position[0] = pp[i].position[0] + (time_step_size * particles[i].velocity[0]);
			particles[i].position[1] = pp[i].position[1] + (time_step_size * particles[i].velocity[1]);
			particles[i].position[2] = pp[i].position[2] + (time_step_size * particles[i].velocity[2]);

		}
	}
	free_particle_vector(pp);
	*part = particles;
}

/*
* 1) Komunikace na konci iterace. Prvni bod zadani.
* Rozposlanie dat volanim BCast - broadcast
* kazdy proces pracuje nad castou pola i=myWorkIndex;i<workChunk
* komunikacia na konci iteracie MPI_Allgather - zozbiera data od procesou do pola a to rozposle vsetkym
*/

void parSolution(t_particle **part, int num_of_time_steps, double time_step_size, int ProcCount, int ProcID)
{
  t_particle *particles = *part;
  int i = 0;
  int p = 0;
  t_particle *pp = alocate_particle_vector();
  
  if (ProcID == 0)
  {
    memcpy(pp,particles,sizeof(t_particle)*NUM_OF_PARTICLES);
  }
  MPI_Bcast(&(pp[0]), NUM_OF_PARTICLES*sizeof(t_particle), MPI_BYTE, 0, MPI_COMM_WORLD); 

  if (ProcID != 0)
  {
    memcpy(particles,pp,sizeof(t_particle)*NUM_OF_PARTICLES);
  }  
  
  int workChunk = ProcID != ProcCount -1 ? NUM_OF_PARTICLES/ProcCount : NUM_OF_PARTICLES - (NUM_OF_PARTICLES/ProcCount) * ProcID;
  int myWorkIndex = ProcID*workChunk;

  for (p = 0; p < num_of_time_steps; p+= time_step_size)
  {
    t_particle *pom = NULL;
    pom = pp;
    pp = particles;
    particles = pom;
    printf("%d %d\n", myWorkIndex, workChunk+myWorkIndex); 
    for (i=myWorkIndex;i<workChunk+myWorkIndex; i++) {

      double force0 = 0;
      double force1 = 0;
      double force2 = 0;

      double d0; 
      double d1;
      double d2;

      double f;
      int    j;
      double r;
      double r32;

      for (j=0;j<NUM_OF_PARTICLES;j++){
        d0 = pp[j].position[0]-pp[i].position[0];
        d1 = pp[j].position[1]-pp[i].position[1];       
        d2 = pp[j].position[2]-pp[i].position[2];

        r   = (d0 * d0) + (d1 * d1) + (d2 * d2);
        r32 = pow (r, 1.5);


        if (r != 0){
          f=(GRAVITY*pp[j].weight*pp[i].weight)/(r32);
          force0 += f*d0;
          force1 += f*d1;
          force2 += f*d2;
        }       
      }

      //printf("force0: id %d %f\n",ProcID, force0);
      particles[i].velocity[0] = pp[i].velocity[0] + (time_step_size * force0);
      particles[i].velocity[1] = pp[i].velocity[1] + (time_step_size * force1);
      particles[i].velocity[2] = pp[i].velocity[2] + (time_step_size * force2);
      //printf("velocity: %f\n", particles[i].velocity[0] );
      particles[i].position[0] = pp[i].position[0] + (time_step_size * particles[i].velocity[0]);
      particles[i].position[1] = pp[i].position[1] + (time_step_size * particles[i].velocity[1]);
      particles[i].position[2] = pp[i].position[2] + (time_step_size * particles[i].velocity[2]);
      //printf("position: %f\n", particles[i].position[0] );
    }
    

    MPI_Allgather(particles + myWorkIndex,  workChunk*sizeof(t_particle), MPI_BYTE,
                  particles ,               workChunk*sizeof(t_particle), MPI_BYTE,
                  MPI_COMM_WORLD);
    
  }
  
  free_particle_vector(pp);
  *part = particles;
}



/*
* 2) Komunikace bìhem iterace - Implementace paralelního reseni na kruhovo tepologii.
* kazdy proces ma vlastny buffer obsahujuci jeho sadu castic ->myStaticData
* tieto ziska napociatku volanim Scatter -> casti buffera procesom
* cyklus for (c = 0; c < ProcCount; c++) udava pocet posunov po kruhovej zbernici
* v kazdom kroku sa pocita silova interakcia s casticami v procWorkBuff, ktory v 0 kroku obsahuje castice procesa samotneho
* pred zaciatkom kazdej dalsej iteracie si volanim IRecv, Send vymenia data - posun po kruznici
* na konci vypoctu sa data od vsetkych procesov skumuluju volanm Allgather do pola part
*/
void parSolutionCircle(t_particle **part, int num_of_time_steps, double time_step_size, int ProcCount, int ProcID, int mode)
{
  t_particle *particles = *part;
  int i = 0;
  int p = 0;
  int c = 0;
  
  int bufferSize = NUM_OF_PARTICLES / ProcCount;
  t_particle *myStaticData = (t_particle *)malloc( bufferSize*sizeof(t_particle) );
  t_particle *procWorkBuff = (t_particle *)malloc( bufferSize*sizeof(t_particle) );
  t_particle *sendBuff     = (t_particle *)malloc( bufferSize*sizeof(t_particle) );
  
  MPI_Scatter( particles,sizeof(t_particle)*bufferSize, MPI_BYTE,
               myStaticData, sizeof(t_particle)*bufferSize, MPI_BYTE,
               0, MPI_COMM_WORLD);
  
  
  MPI_Request requesthandle;
  MPI_Status recvwaitstat;
  
  for (p = 0; p < num_of_time_steps; p++){

    
    
    for (c = 0; c < ProcCount; c++){
      t_particle * procWork = c == 0 ? myStaticData : procWorkBuff;
      if ( c != 0 )
      {
        t_particle * senddata = c == 1 ? myStaticData : procWorkBuff;
        memcpy( sendBuff, senddata, bufferSize*sizeof(t_particle) );
        
        MPI_Irecv( procWorkBuff, bufferSize*sizeof(t_particle) , MPI_BYTE, GetSousedVlevo(ProcID, ProcCount),
                   0, MPI_COMM_WORLD, &requesthandle ); 
        
        MPI_Send ( sendBuff, bufferSize*sizeof(t_particle) , MPI_BYTE, GetSousedVpravo(ProcID, ProcCount),
                   0, MPI_COMM_WORLD);
         
        MPI_Wait ( &requesthandle, &recvwaitstat );
      }
        
        
        
      for (i=0;i<bufferSize; i++) {
        double force0 = 0, force1 = 0, force2 = 0;   
        double d0,d1,d2,f,r,r32;
        int    j;
  
        for (j=0;j<bufferSize;j++){
          d0 = procWork[j].position[0]-myStaticData[i].position[0];
          d1 = procWork[j].position[1]-myStaticData[i].position[1];       
          d2 = procWork[j].position[2]-myStaticData[i].position[2];
  
          r   = (d0 * d0) + (d1 * d1) + (d2 * d2);
          r32 = pow (r, 1.5);
  
  
          if (r != 0){
            f=(GRAVITY*myStaticData[i].weight*procWork[j].weight)/(r32);
            force0 += f*d0;
            force1 += f*d1;
            force2 += f*d2;
          }       
        }
        myStaticData[i].velocity[0] = myStaticData[i].velocity[0] + (time_step_size * force0);
        myStaticData[i].velocity[1] = myStaticData[i].velocity[1] + (time_step_size * force1);
        myStaticData[i].velocity[2] = myStaticData[i].velocity[2] + (time_step_size * force2);
      }

    }
    
    for (i=0;i<bufferSize; i++) {
      myStaticData[i].position[0] = myStaticData[i].position[0] + (time_step_size * myStaticData[i].velocity[0]);
      myStaticData[i].position[1] = myStaticData[i].position[1] + (time_step_size * myStaticData[i].velocity[1]);
      myStaticData[i].position[2] = myStaticData[i].position[2] + (time_step_size * myStaticData[i].velocity[2]); 
    }

    
  }
  
  MPI_Gather ( myStaticData,sizeof(t_particle)*bufferSize, MPI_BYTE,
               particles, sizeof(t_particle)*bufferSize, MPI_BYTE,
               0, MPI_COMM_WORLD);
  
  free (myStaticData) ;
  free (procWorkBuff) ;
  free (sendBuff) ;
}





int main(int argc, char *argv[]){

	FILE *fp;
	t_particle *particles = NULL;
        t_particle *particlesbck = NULL;
        

        
	double time_step_size;
	int    num_of_time_steps;

	int ProcCount;
	int ProcID;

	// Inicializace MPI
	MPI_Init(&argc,&argv);
	// Zjisteni poctu a Id procesu
	MPI_Comm_size(MPI_COMM_WORLD,&ProcCount);
	MPI_Comm_rank(MPI_COMM_WORLD,&ProcID);

	time_step_size    = 0.0;
	num_of_time_steps = 1;

	particles = alocate_particle_vector();
        particlesbck = alocate_particle_vector();

	if(argc != 4)
		usage(argv[0]);


	time_step_size    = atof(argv[2]);
	num_of_time_steps = atoi(argv[3]);

	// POuze proces nula cte vstup ze souboru.
	if (ProcID == 0)
	{
		if(!(fp=fopen(argv[1], "r")))
		{
			perror(argv[1]);
			exit(1);
		}
		read_particle_vector(fp,particles);
                memcpy(particlesbck,particles,sizeof(t_particle)*NUM_OF_PARTICLES); 
		//printf("input particles:\n");
		//print_particle_vector(particles);
	}

	// Odkomentovamim spustime pozadovanou funkci. Mela by bezet pouze jedna.
        
        //clock_t start, end;
        //time(&start);


        if (ProcID == 0)
        {
        //seqSolution(&particles, num_of_time_steps, time_step_size);
        }
        
        //POZNAMKA: V SEKVENCNOM RIESENI BOLA OPET CHYBA: OPRAVENE Z f=(GRAVITY*pp[j].weight)/(r32); NA f=(GRAVITY*pp[j].weight*pp[i].weight)/(r32);
        // --vybocet nebral do uvahy hmotnost druhej z castic
        
        parSolution(&particles, num_of_time_steps, time_step_size, ProcCount, ProcID);
        //parSolutionCircle(&particles, num_of_time_steps, time_step_size, ProcCount, ProcID, OVERLAP);
        
        /**
        MPI_Barrier( MPI_COMM_WORLD );
        time(&end);
        **/
        
        
        
        
	// Pouze proces nula tiskne vysledek
	if (ProcID == 0)
        {
          printf("\noutput particles:\n");

          print_particle_vector(particles);

          fclose(fp);  
          /**
          printf("PARALLEL: %d SEC\n", (int) difftime(end, start));
    

          t_particle * seq_particles = alocate_particle_vector();
          memcpy(seq_particles, particlesbck, NUM_OF_PARTICLES*sizeof(t_particle));
    
    
          time(&start);
          seqSolution(&seq_particles, num_of_time_steps, time_step_size );
          time(&end);
          printf("SEQUENTIAL: %d SEC\n", (int) difftime(end, start));
          printf("---diff-par-seq--- idealne same 0.0000\n");
          print_particles_diff(particles, seq_particles);

          //write_particle_vector(stdout, particles);
          fclose(fp);
          **/
	}


	free_particle_vector(particles);

	// Finalizace MPI
	MPI_Finalize();

	return 0;
}

