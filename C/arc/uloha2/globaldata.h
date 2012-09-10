///////////////////////////////////////////////////////////////////////////////
// Funkce pro data-gen.c, definice konstant a typu pro fyzikalni model
// NEMODIFUKUJTE !!!!

#if !defined _GLOBAL_DATA_H_
#define _GLOBAL_DATA_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>

// hodnota N je definovana v Makefile !!!
#if !defined N
    #define N 1000
#endif

// gravitacni konstanta
#define G 6.672e-11f

// struktura castice (hmotneho bodu)
struct particle{

	// soucasna pozice hmotneho bodu
	double position[3];

	// vektor rychlosti
	double velocity[3];
 
	// hmotnost
	double weight;

};

typedef struct particle t_particle;

t_particle* alocate_particle_vector(void);
void free_particle_vector(t_particle *p);
void generate_particle_vector(t_particle *p);
void print_particle_vector(t_particle *p);
void write_particle_vector(FILE *fp,t_particle *p);
void read_particle_vector(FILE *fp,t_particle *p);

// alokuj vektor pro N polozek t_particle
t_particle* alocate_particle_vector(void){
    static t_particle *p = NULL;
  if ((p = malloc(N*sizeof(t_particle))) == NULL){
    perror("malloc");
    exit(1);
  }

  return p;
}


// uvolni alokovany vektor t_particle
void free_particle_vector(t_particle *p){
  if (p != NULL)
    free(p);
}

// generuje nahodne pole pozic, rychlosti a hmotnosti teles
void generate_particle_vector(t_particle *p){
 int i; 
 for (i=0; i<N; i++){
           // set position
	p[i].position[0] = rand() % 100;
	p[i].position[1] = rand() % 100;
 	p[i].position[2] = rand() % 100;

           // create random velocity
	p[i].velocity[0] = (rand()%4000 -2000)/1000.0f;
        p[i].velocity[1] = (rand()%4000 -2000)/1000.0f;
 	p[i].velocity[2] = (rand()%4000 -2000)/1000.0f;

          // Set particle weight
    while ((p[i].weight = (double) (rand()/0xFFFFF) * (rand()%0xFFFFF)) == 0.0);
  }
}

// vytiskne pole pozic, rychlosti a hmotnosti teles na stdout 
void print_particle_vector(t_particle *p){

  int i;
  for(i=0; i < N; i++){
      printf("%10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f \n", 
         p[i].position[0], p[i].position[1], p[i].position[2],
         p[i].velocity[0], p[i].velocity[1], p[i].velocity[2],
         p[i].weight);
  }
}

// zapise pole pozic, rychlosti a hmotnosti teles do souboru fp 
void write_particle_vector(FILE *fp, t_particle *p){

  int i;
  for(i=0; i < N; i++){
      fprintf(fp,"%10.4f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f \n", 
         p[i].position[0], p[i].position[1], p[i].position[2],
         p[i].velocity[0], p[i].velocity[1], p[i].velocity[2],
         p[i].weight);
  }
}

// cte pole pozic, rychlosti a hmotnosti teles ze souboru fp 
void read_particle_vector(FILE *fp, t_particle *p){
 int i;
  for(i=0; i < N; i++){
      fscanf(fp,"%lf %lf %lf %lf %lf %lf %lf \n", 
          &p[i].position[0], &p[i].position[1], &p[i].position[2],
          &p[i].velocity[0], &p[i].velocity[1], &p[i].velocity[2],
          &p[i].weight);
  }
}

// vypise rozdily slozek polohy a rychlosti teles na stdout
void print_particles_diff(t_particle *p1, t_particle *p2){

  int i;
  for(i=0; i < N; i++){
      printf("%10.4f %10.4f %10.4f %10.4f %10.4f %10.4f\n", 
         fabs(p1[i].position[0]-p2[i].position[0]),
         fabs(p1[i].position[1]-p2[i].position[1]),
         fabs(p1[i].position[2]-p2[i].position[2]),
         fabs(p1[i].velocity[0]-p2[i].velocity[0]),
         fabs(p1[i].velocity[1]-p2[i].velocity[1]),
         fabs(p1[i].velocity[2]-p2[i].velocity[2]));
  }
}

#endif /* _GLOBAL_DATA_H_ */
