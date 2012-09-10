/*
  proj02.c: project 2 for ARC
  autor: xseben00@stud.fit.vutbr.cz

*/

#define DEBUG

#define VERSION SERIAL
//#define VERSION PARALLEL_OUTERLOOP
//#define VERSION PARALLEL_INNERLOOP  

/*----------------------------------------------------------------------------*/
/*---------------------------- Vase vysledky ---------------------------------*/
/*	
  
   - vyberte si pocet castic tak, aby program bezel ve vsech pripadech alespon 30s
     (zmente Makefile nebo zkompilujte runce s pouzitim -DNUM_OF_PARTICLES=xxx)
   - zakomentujte DEBUG 
   - zmerte "real" cas s pouzitim /usr/bin/time na vhodnem PC (napr. Merlin)
   - vyplnte nasledujici:


   NUM_OF_PARTICLES=1800

   vzhladom na nadmerne zatazenie merlina bola vonkajsia varianta testovana na
   eve, avsak vnutorna varianta bola testovana na merlinovi pretoze na eve
   boli vysledne casi vsetky z neznamej priciny dlhsie ako u seriovej varianty

	          VERSION PARALLEL_OUTERLOOP
      +---------+----------------+-----------+------------+
      | Vlaken  |    Cas behu    | Zrychleni | Efektivita |
      +---------+----------------+-----------+------------+
      |   1     |   158.18       |  0.907    |  0.907     |
      |   2     |    79.57       |  1.802    |  0.901     |
      |   3     |    53.53       |  2.679    |  0.893     |
      |   4     |    40.47       |  3.543    |  0.886     |
      |   5     |    33.58       |  4.270    |  0.854     |
      |   6     |    31.15       |  4.604    |  0.767     |
      |   7     |    29.47       |  4.866    |  0.695     |
      |   8     |    41.78       |  3.433    |  0.429     |
      +---------+----------------+-----------+------------+

    		  VERSION PARALLEL_INNERLOOP  	 
      +---------+----------------+-----------+------------+
      | Vlaken  |    Cas behu    | Zrychleni | Efektivita |
      +---------+----------------+-----------+------------+
      |   1     |    337.11      |  0.425    |  0.425     |
      |   2     |    116.32      |  1.233    |  0.616     |
      |   3     |     78.61      |  1.824    |  0.608     |
      |   4     |     62.22      |  2.305    |  0.576     |
      |   5     |     68.32      |  2.099    |  0.419     |
      |   6     |    155.28      |  0.923    |  0.154     |
      |   7     |    204.68      |  0.701    |  0.100     |
      |   8     |    267.53      |  0.536    |  0.067     |
      +---------+----------------+-----------+------------+

*/
/*----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <math.h>

/* definice globalnich dat - pouziva NUM_OF_PARTICLES */
#include "globaldata.h"

void usage(char *prog)
{
  fprintf(stderr, "usage : %s filename time_step_size num_of_time_steps omp_threads\n", prog);
  exit(1);
}


/*----------------------------------------------------------------------------
                    SERIOVA REFERENCNI IMPLEMENTACE
  ----------------------------------------------------------------------------*/

void UpdateParticlesSerialVersion(t_particle *particles,t_particle *particlesTmp,double time_step_size)
{
    	// definice pomocnych promennych
	double forceIX,forceIY,forceIZ;
        static const double GRAVITY_CONST = G; //9.81; melo by byt 6.67e-11 pro atomy, ale to je prilis malo pro detekci chyby vypoctu po rozumnem poctu iteraci
	double FGpart = 0.0;	  // castecny vypocet sily FG
	unsigned int dim = 0 ;	// dimenze (x,y,z)
	int i = 0;              // promenna cyklu
	int j = 0;              // promenna cyklu
	double rna2 = 0;        // vzalenost na druhou
	double r[3];            // vzdalenost

	  for( i = 0 ; i < NUM_OF_PARTICLES ; i++ ) {

			forceIX=forceIY=forceIZ=0.0; //vynulovani souctove sily
		        //vypocet sily pusobici na castici  (kumulovana sila od vsech ostatnich castic, Newtonuv gravitacni zakon)
			for( j = 0 ; j < NUM_OF_PARTICLES ; j++ ) {
				for ( dim = 0 ; dim < 3 ; dim++ ) {
						r[dim] = particlesTmp[i].position[dim] - particlesTmp[j].position[dim];
				}					
					
				rna2 = r[0]*r[0] + r[1]*r[1] + r[2]*r[2];
				
				// test, pokud jsou castice ve stejnem bode, tak se nic nepricita (pusobi nulova sila)	
				if ( rna2 != 0 ) {
                                  FGpart = (GRAVITY_CONST * particles[i].weight * particles[j].weight) / ( rna2 * sqrt(rna2)); 
					forceIX += FGpart * r[0];
					forceIY += FGpart * r[1];
					forceIZ += FGpart * r[2];
				} // if( rna2 != 0 )
 																	
			} // for ( j = 0 ; j < NUM_OF_PARTICLES ; j++ )
          
         		//vypocet nove rychlosti		
                        particles[i].velocity[0] += (time_step_size * forceIX)/particles[i].weight;
                        particles[i].velocity[1] += (time_step_size * forceIY)/particles[i].weight;
                        particles[i].velocity[2] += (time_step_size * forceIZ)/particles[i].weight;

        		// vypocet nove pozice
      			particles[i].position[0] += time_step_size * particles[i].velocity[0];
      			particles[i].position[1] += time_step_size * particles[i].velocity[1];
			particles[i].position[2] += time_step_size * particles[i].velocity[2];
      
	} // for( i = 0 ; i < NUM_OF_PARTICLES ; i++ )

}


/*----------------------------------------------------------------------------
                       PARALELNI VERZE - VNEJSI SMYCKA

  TODO: VAS KOMENTAR KODU
  ----------------------------------------------------------------------------*/

void UpdateParticlesParallelVerstionOuterLoop(t_particle *particles,t_particle *particlesTmp,double time_step_size)
{



	double forceIX,forceIY,forceIZ;
	static const double GRAVITY_CONST = 9.81; // melo by byt 6.67e-11 pro atomy, ale to je prilis malo pro detekci chyby vypoctu po rozumnem poctu iteraci
	double FGpart = 0.0;	  // castecny vypocet sily FG
	unsigned int dim = 0 ;	// dimenze (x,y,z)
	int i = 0;              // promenna cyklu
	int j = 0;              // promenna cyklu
	double rna2 = 0;        // vzalenost na druhou
	double r[3];            // vzdalenost

	#pragma omp parallel for private(i,j,dim,rna2,forceIZ,forceIX,forceIY,r,FGpart) shared(particles)
	  for( i = 0 ; i < NUM_OF_PARTICLES ; i++ ) {

			forceIX=forceIY=forceIZ=0.0; //vynulovani souctove sily
		        //vypocet sily pusobici na castici  (kumulovana sila od vsech ostatnich castic, Newtonuv gravitacni zakon)
			for( j = 0 ; j < NUM_OF_PARTICLES ; j++ ) {
				for ( dim = 0 ; dim < 3 ; dim++ ) {
						r[dim] = particlesTmp[i].position[dim] - particlesTmp[j].position[dim];
				}					
					
				rna2 = r[0]*r[0] + r[1]*r[1] + r[2]*r[2];
				
				// test, pokud jsou castice ve stejnem bode, tak se nic nepricita (pusobi nulova sila)	
				if ( rna2 != 0 ) {
					FGpart = GRAVITY_CONST * particles[i].weight * particles[j].weight / ( rna2 * sqrt(rna2)); 
					forceIX += FGpart * r[0];
					forceIY += FGpart * r[1];
					forceIZ += FGpart * r[2];
				} // if( rna2 != 0 )
 																	
			} // for ( j = 0 ; j < NUM_OF_PARTICLES ; j++ )
          
         		//vypocet nove rychlosti		
                        particles[i].velocity[0] += (time_step_size * forceIX)/particles[i].weight;
                        particles[i].velocity[1] += (time_step_size * forceIY)/particles[i].weight;
                        particles[i].velocity[2] += (time_step_size * forceIZ)/particles[i].weight;

        		// vypocet nove pozice
      			particles[i].position[0] += time_step_size * particles[i].velocity[0];
      			particles[i].position[1] += time_step_size * particles[i].velocity[1];
			particles[i].position[2] += time_step_size * particles[i].velocity[2];
      
	}
	
}




/*----------------------------------------------------------------------------
                       PARALELNI VERZE - VNITRNI SMYCKA

     TODO: VAS KOMENTAR KODU
  ----------------------------------------------------------------------------*/
void UpdateParticlesParallelVerstionInnerLoop(t_particle *particles,t_particle *particlesTmp,double time_step_size)
{

   	double forceIX,forceIY,forceIZ;
	static const double GRAVITY_CONST = 9.81; // melo by byt 6.67e-11 pro atomy, ale to je prilis malo pro detekci chyby vypoctu po rozumnem poctu iteraci
	double FGpart = 0.0;	  // castecny vypocet sily FG
	unsigned int dim = 0 ;	// dimenze (x,y,z)
	int i = 0;              // promenna cyklu
	int j = 0;              // promenna cyklu
	double rna2 = 0;        // vzalenost na druhou
	double r[3];            // vzdalenost

	
	  for( i = 0 ; i < NUM_OF_PARTICLES ; i++ ) {

			forceIX=forceIY=forceIZ=0.0; //vynulovani souctove sily
		    
			//vypocet sily pusobici na castici  (kumulovana sila od vsech ostatnich castic, Newtonuv gravitacni zakon)
			#pragma omp parallel for default(none) private(j,r,dim,rna2,FGpart) shared(particles,i,particlesTmp) schedule(static) reduction(+:forceIX,forceIY,forceIZ)
			for( j = 0 ; j < NUM_OF_PARTICLES ; j++ ) {
				for ( dim = 0 ; dim < 3 ; dim++ ) {
						r[dim] = particlesTmp[i].position[dim] - particlesTmp[j].position[dim];
				}					
					
				rna2 = r[0]*r[0] + r[1]*r[1] + r[2]*r[2];
				
				// test, pokud jsou castice ve stejnem bode, tak se nic nepricita (pusobi nulova sila)	
				if ( rna2 != 0 ) {
					FGpart = GRAVITY_CONST * particles[i].weight * particles[j].weight / ( rna2 * sqrt(rna2)); 
					forceIX += FGpart * r[0];
					forceIY += FGpart * r[1];
					forceIZ += FGpart * r[2];
				} // if( rna2 != 0 )
 																	
			} // for ( j = 0 ; j < NUM_OF_PARTICLES ; j++ )
          
         		//vypocet nove rychlosti		
                        particles[i].velocity[0] += (time_step_size * forceIX)/particles[i].weight;
                        particles[i].velocity[1] += (time_step_size * forceIY)/particles[i].weight;
                        particles[i].velocity[2] += (time_step_size * forceIZ)/particles[i].weight;

        		// vypocet nove pozice
      			particles[i].position[0] += time_step_size * particles[i].velocity[0];
      			particles[i].position[1] += time_step_size * particles[i].velocity[1];
			particles[i].position[2] += time_step_size * particles[i].velocity[2];
      
	}
}



int main(int argc, char *argv[]){

  FILE *fp;
  t_particle *particles = NULL;
  t_particle *particlesTmp; 

  double time_step_size    = 0.0;
  double num_of_time_steps = 0.0;
  int    omp_threads       = 1;
  int time = 0;           // promenna cyklu -- casove posuny

  particles = alocate_particle_vector();

  if(argc != 5)
  	usage(argv[0]);
    
  if(!(fp=fopen(argv[1], "r"))){
    perror(argv[1]);
  	exit(1);
  }

  time_step_size    = 0.001;//atof(argv[2]);
  num_of_time_steps = 1000; //atof(argv[3]);
  omp_threads       = 4;//atoi(argv[4]);
 
  omp_set_num_threads(omp_threads);
  
  read_particle_vector(fp,particles);

 #ifdef DEBUG
  printf("castice na vstupu:\n");
  print_particle_vector(particles);
 #endif

  // pomocne pole pro vypocet 
  particlesTmp = (t_particle *) malloc( sizeof(*particles)*NUM_OF_PARTICLES );
  memcpy ( particlesTmp, particles, sizeof(t_particle)*NUM_OF_PARTICLES );

	// casove kroky
	for ( time = 0 ; time < num_of_time_steps ; time++ ) {

	  #if (VERSION == SERIAL)
	  UpdateParticlesSerialVersion(particles,particlesTmp,time_step_size);
	  #endif 

	  #if (VERSION == PARALLEL_OUTERLOOP)
 	  UpdateParticlesParallelVerstionOuterLoop(particles,particlesTmp,time_step_size);
          #endif

          #if (VERSION == PARALLEL_INNERLOOP)
          UpdateParticlesParallelVerstionInnerLoop(particles,particlesTmp,time_step_size);
          #endif

	  // slo by i prohazovanim pointeru
	  memcpy ( particlesTmp, particles, sizeof(t_particle)*NUM_OF_PARTICLES );
	} // for ( time = 0 ; time < num_of_time_steps ; time++ )
  
  free(particlesTmp); // uvolneni pomocneho pole

 
 #ifdef DEBUG
  printf("\ncastice na vystupu:\n");
  print_particle_vector(particles);
 #endif

  fclose(fp);

  free_particle_vector(particles);

  return 0;
}
