#if !defined _GLOBAL_DATA_H_
#define _GLOBAL_DATA_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#if !defined NUM_OF_PARTICLES
#define NUM_OF_PARTICLES 1000
#endif


//define GRAVITY           9.81f
#define GRAVITY 6.672e-11f

#define DEBUG 0


// struktura uchovavajici castici
struct particle{

	// soucasna pozice castice
	double position[3];

	// vektor rychlosti castice
	double velocity[3];
 
	// hmotnost castice
	double weight;

};

typedef struct particle t_particle;

t_particle* alocate_particle_vector(void);
void free_particle_vector(t_particle *p);
void generate_particle_vector(t_particle *p);
void print_particle_vector(t_particle *p);
void write_particle_vector(FILE *fp,t_particle *p);
void read_particle_vector(FILE *fp,t_particle *p);

// alokuj pole castic 
t_particle* alocate_particle_vector(void){
    static t_particle *p = NULL;
  if ((p = (t_particle*) malloc(NUM_OF_PARTICLES*sizeof(t_particle))) == NULL){
    perror("malloc");
    exit(1);
  }

  return p;
}


// uvolni pole castic 
void free_particle_vector(t_particle *p){
  if (p != NULL)
    free(p);
}

// generuje pole castic 
void generate_particle_vector(t_particle *p){
 int i; 
 for (i=0; i<NUM_OF_PARTICLES; i++){
           // set position
	p[i].position[0] = 5.0f;
	p[i].position[1] = 5.0f;
 	p[i].position[2] = 5.0f;
           // create random velocity
	p[i].velocity[0] = (rand()%4000 -2000)/1000.0f;
        p[i].velocity[1] = (rand()%4000 -2000)/1000.0f;
 	p[i].velocity[2] = (rand()%4000 -2000)/1000.0f;

          // Set particle weight
        p[i].weight = (rand()%100)/10.0f+0.0001;
  }
}

// vytiskne pole castic na stdout 
void print_particle_vector(t_particle *p){
  int i;
//	printf("%d\n",NUM_OF_PARTICLES);
  for(i=0; i < NUM_OF_PARTICLES; i++){
//			printf("NUMBER %d\n",i);

	/// the current particle position
      printf("%d %8.4f %8.4f %8.4f %8.4f %8.4f %8.4f %8.4f \n", i,
         p[i].position[0], p[i].position[1], p[i].position[2],
         p[i].velocity[0], p[i].velocity[1], p[i].velocity[2],
         p[i].weight);
  }

//	printf("end result\n");
}


// zapise pole castic do souboru fp 
void write_particle_vector(FILE *fp, t_particle *p){

  int i;
  for(i=0; i < NUM_OF_PARTICLES; i++){
      fprintf(fp,"%8.4f %8.4f %8.4f %8.4f %8.4f %8.4f %8.4f \n", 
         p[i].position[0], p[i].position[1], p[i].position[2],
         p[i].velocity[0], p[i].velocity[1], p[i].velocity[2],
         p[i].weight);
  }
}



// cte pole castic ze souboru fp 
void read_particle_vector(FILE *fp, t_particle *p){
 int i;
  for(i=0; i < NUM_OF_PARTICLES; i++){
      fscanf(fp,"%lf %lf %lf %lf %lf %lf %lf \n", 
          &p[i].position[0], &p[i].position[1], &p[i].position[2],
          &p[i].velocity[0], &p[i].velocity[1], &p[i].velocity[2],
          &p[i].weight);
  }
}

#endif /* _GLOBAL_DATA_H_ */
