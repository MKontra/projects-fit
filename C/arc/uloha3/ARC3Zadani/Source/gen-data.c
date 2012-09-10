/*
  global-data.c: generate initial positions, velocity vectors and wights of particles.
  autor: jarosjir@fit.vutbr.cz
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* definition of global data functions  - uses NUM_OF_PARTICLES */
#include "globaldata.h"

void usage(char *prog)
{
  fprintf(stderr, "usage : %s filename \n", prog);
  exit(1);
}

int main(int argc, char *argv[]){

  FILE *fp;
  t_particle *particles=NULL;
    
  if(argc != 2)
  	usage(argv[0]);
    
  if(!(fp=fopen(argv[1], "w+"))){
    perror(argv[1]);
  	exit(1);
  }

  /* alloc new dyn. matrix */
  particles = alocate_particle_vector();
  
  srand(time(NULL));

  generate_particle_vector(particles);

  print_particle_vector(particles);
  
  write_particle_vector(fp, particles);

  free_particle_vector(particles);

  fclose(fp);
  
  return 0;
};
