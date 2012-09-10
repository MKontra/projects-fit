///////////////////////////////////////////////////////////////////////////////
// Generator nahodnych pocatecnich dat hmotnych bodu - NEMODIFIKOVAT !!!!

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

  particles = alocate_particle_vector();
  
  srand(time(NULL));

  generate_particle_vector(particles);

  print_particle_vector(particles);
  
  write_particle_vector(fp, particles);

  free_particle_vector(particles);

  fclose(fp);
  
  return 0;
};
