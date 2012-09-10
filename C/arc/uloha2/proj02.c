///////////////////////////////////////////////////////////////////////////////
// Projekt 2 - ARC: Vzajemne silove pusobeni teles
// Sem doplnte svuj login
// Autor: xkontr00@stud.fit.vutbr.cz

#include <time.h>
#include <omp.h>

#include "globaldata.h"

void usage(char *prog)
{
  fprintf(stderr, "usage : %s filename time_step_size num_of_time_steps omp_threads\n", prog);
  exit(1);
}

/////////////////////////////////////////////////////////////////////////////
// Info: Po doplneni nize uvedenych funkci prelozte program prikazem make.
//       Nasledne jej spustte pripazem make run > output.txt.
//       (S generatorem dat gen-data.c nemusite samostatne vubec pracovat,
//       vygenerovani souboru je soucasti davky run v souboru Makefile.)
//       V souboru output.txt uvidite vygenerovana data, data vypoctena
//       sekvencni verzi a data vypoctena paralelnimi implementacemi vcetne
//       rozdilu vysledku kazde paralelni verze od vysledku sekvencni verze.
//       Po odladeni nastavte v Makefile konstantu N >= 1000 a provedte
//       vypocet na parlelnim stroji (napr. merlin) pro overeni efektivity
//       paralelizace vnejsi a vnitrni smycky.
//
//       !!!!!! Po vyreseni odevzdejte samotny soubor proj02.c !!!!!!
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////
// Sem doplnte sekvencni implementaci algoritmu.
void sequential(t_particle * in_particles, double time_step_size, double num_of_time_steps)
{
     //docasny vektor castic
     t_particle * tmppart = alocate_particle_vector();
     memcpy(tmppart, in_particles, N*sizeof(t_particle));
     t_particle * flopper = NULL;    
     //deklaracie riadiacich premennych cyklu
     double timei;
     int outi;
     int ini;
     
     //pocet iteracii -- preco double...
     for ( timei = 0.0; timei < num_of_time_steps; timei += 1.0 )
     {
       //prehod ukazatele na castice - udrziavanie povodneho vektora pozicii
       flopper = in_particles;
       in_particles = tmppart;
       tmppart = flopper;
       //vonkajsia slucka - vypocet novej pozicie vsetkych castic
       for ( outi = 0; outi < N; outi++ )
       {
           double forcex = 0.0;
           double forcey = 0.0;
           double forcez = 0.0;
           //vnutorna slucka - vypocet silovych posobeni 
           for ( ini = 0; ini < N; ini++ )
           {
               // premenne su pomennovane podla premmenych z pdf zadania - snad zrejme
               double Rx = tmppart[outi].position[0] - tmppart[ini].position[0];
               double Ry = tmppart[outi].position[1] - tmppart[ini].position[1];
               double Rz = tmppart[outi].position[2] - tmppart[ini].position[2];
               //printf("Rxses: %f %f %f\n", Rx, Ry, Rz );
               double r2 = Rx*Rx + Ry*Ry + Rz*Rz;
               if ( r2 == 0 ) continue;
               double r  = sqrt(r2);
               double Fg = ((G*tmppart[outi].weight*tmppart[ini].weight)/(r2));
               //printf("r r2 Fg: %f %f %f\n", r, r2, Fg);
               forcex   += Fg*Rx/r;
               forcey   += Fg*Ry/r;
               forcez   += Fg*Rz/r;
               //printf("partial forces: %f %f %f\n", forcex, forcey, forcez);
           }
           //printf("forces: %f %f %f\n", forcex, forcey, forcez);
           //uprava vektoru rychlosti castice
           in_particles[outi].velocity[0] = tmppart[outi].velocity[0] + ((time_step_size*forcex)/in_particles[outi].weight);
           in_particles[outi].velocity[1] = tmppart[outi].velocity[1] + ((time_step_size*forcey)/in_particles[outi].weight);
           in_particles[outi].velocity[2] = tmppart[outi].velocity[2] + ((time_step_size*forcez)/in_particles[outi].weight);                  

           //uprava pozicneho vektora
           in_particles[outi].position[0] = tmppart[outi].position[0] + (time_step_size*in_particles[outi].velocity[0]);
           in_particles[outi].position[1] = tmppart[outi].position[1] + (time_step_size*in_particles[outi].velocity[1]);
           in_particles[outi].position[2] = tmppart[outi].position[2] + (time_step_size*in_particles[outi].velocity[2]);
       
       }
     }
     //uvolnenie docasneho vektora
     free_particle_vector(tmppart); 
}

//////////////////////////////////////////////////////////////////
// Sem vlozte implementaci algoritmu s paralelizaci vnejsi smycky.
// Pripojte strucny komentar k Vasi implementaci.
void outer_parallel(t_particle * in_particles, double time_step_size, double num_of_time_steps)
{
  t_particle * tmppart = alocate_particle_vector();
  memcpy(tmppart, in_particles, N*sizeof(t_particle));
  t_particle * flopper = NULL;   
       
  double timei;
  int outi = 0;
  int ini = 0;
     
  for ( timei = 0.0; timei < num_of_time_steps; timei += 1.0 )
  {
    //prehod ukazatele na castice - udrziavanie povodneho vektora pozicii
    flopper = in_particles;
    in_particles = tmppart;
    tmppart = flopper;
    //funkcia premennych a cyklov je zhodna so sekvencnim riesenim vid komentare tam
    //deklaracie vytiahnute do vyssieho scopu koli pristupnosti v pragmach
    double forcex = 0.0;
    double forcey = 0.0;
    double forcez = 0.0;    
    
    double Rx;
    double Ry;
    double Rz;
    
    double r2;
    double r;
    double Fg;
    
    //paralerizacia vonkajsieho cyklu
    //vypocet novej pozicie je rozdeleny do vlakien po blokoch castic
    #pragma omp parallel for \
    private(outi, ini, forcex, forcey, forcez, Rx, Ry, Rz, r2, r, Fg) \
    shared(in_particles, tmppart)
    for ( outi = 0; outi < N; outi++ )
    {
      forcex = 0.0;
      forcey = 0.0;
      forcez = 0.0;  
      for ( ini = 0; ini < N; ini++ )
      {
        Rx = tmppart[outi].position[0] - tmppart[ini].position[0];
        Ry = tmppart[outi].position[1] - tmppart[ini].position[1];
        Rz = tmppart[outi].position[2] - tmppart[ini].position[2];
               //printf("Rxses: %f %f %f\n", Rx, Ry, Rz );
        r2 = Rx*Rx + Ry*Ry + Rz*Rz;
        if ( r2 == 0 ) continue;
        r  = sqrt(r2);
        Fg = ((G*tmppart[outi].weight*tmppart[ini].weight)/(r2));
               //printf("r r2 Fg: %f %f %f\n", r, r2, Fg);
        forcex   += Fg*Rx/r;
        forcey   += Fg*Ry/r;
        forcez   += Fg*Rz/r;
               //printf("partial forces: %f %f %f\n", forcex, forcey, forcez);
      }
           //printf("forces: %f %f %f\n", forcex, forcey, forcez);
      in_particles[outi].velocity[0] = tmppart[outi].velocity[0] + ((time_step_size*forcex)/in_particles[outi].weight);
      in_particles[outi].velocity[1] = tmppart[outi].velocity[1] + ((time_step_size*forcey)/in_particles[outi].weight);
      in_particles[outi].velocity[2] = tmppart[outi].velocity[2] + ((time_step_size*forcez)/in_particles[outi].weight);                  

           //uprava pozicneho vektora
      in_particles[outi].position[0] = tmppart[outi].position[0] + (time_step_size*in_particles[outi].velocity[0]);
      in_particles[outi].position[1] = tmppart[outi].position[1] + (time_step_size*in_particles[outi].velocity[1]);
      in_particles[outi].position[2] = tmppart[outi].position[2] + (time_step_size*in_particles[outi].velocity[2]);
       
    }
  }
  free_particle_vector(tmppart); 
}

///////////////////////////////////////////////////////////////////
// Sem vlozte implementaci algoritmu s paralelizaci vnitrni smycky.
// Pripojte strucny komentar k Vasi implementaci.
void inner_parallel(t_particle * in_particles, double time_step_size, double num_of_time_steps)
{
  t_particle * tmppart = alocate_particle_vector();
  memcpy(tmppart, in_particles, N*sizeof(t_particle));
  t_particle * flopper = NULL;   
    
  double timei;
  int outi = 0;
  int ini = 0;
  //funkcia premennych a cyklov je zhodna so sekvencnim riesenim vid komentare tam   
  for ( timei = 0.0; timei < num_of_time_steps; timei += 1.0 )
  {
    //prehod ukazatele na castice - udrziavanie povodneho vektora pozicii
    flopper = in_particles;
    in_particles = tmppart;
    tmppart = flopper;
    
    double forcex = 0.0;
    double forcey = 0.0;
    double forcez = 0.0;    
    
    double Rx;
    double Ry;
    double Rz;
    
    double r2;
    double r;
    double Fg;
     

    for ( outi = 0; outi < N; outi++ )
    {
      forcex = 0.0;
      forcey = 0.0;
      forcez = 0.0;  
      
      //paralerizacia vnutornej smycky
      //paralerizujeme vypocet siloveho posobenie castic na casticu
      //kumulujeme vypocet posobiacej sily paralerizaciou cez posobenia ostatnych castic
      //reduce - nascitava vysledky privatnych premennych do globalnej
      #pragma omp parallel for \
      private(ini, Rx, Ry, Rz, r2, r, Fg) \
      firstprivate(outi,tmppart,in_particles)\
      reduction(+:forcex, forcey, forcez)
      for ( ini = 0; ini < N; ini++ )
      {
        Rx = tmppart[outi].position[0] - tmppart[ini].position[0];
        Ry = tmppart[outi].position[1] - tmppart[ini].position[1];
        Rz = tmppart[outi].position[2] - tmppart[ini].position[2];
                  //printf("Rxses: %f %f %f\n", Rx, Ry, Rz );
        r2 = Rx*Rx + Ry*Ry + Rz*Rz;
        if ( r2 == 0 ) continue;
        r  = sqrt(r2);
        Fg = ((G*tmppart[outi].weight*tmppart[ini].weight)/(r2));
                  //printf("r r2 Fg: %f %f %f\n", r, r2, Fg);
        forcex   += Fg*Rx/r;
        forcey   += Fg*Ry/r;
        forcez   += Fg*Rz/r;
                  //printf("partial forces: %f %f %f\n", forcex, forcey, forcez);
      }
              //printf("forces: %f %f %f\n", forcex, forcey, forcez);
      in_particles[outi].velocity[0] = tmppart[outi].velocity[0] + ((time_step_size*forcex)/in_particles[outi].weight);
      in_particles[outi].velocity[1] = tmppart[outi].velocity[1] + ((time_step_size*forcey)/in_particles[outi].weight);
      in_particles[outi].velocity[2] = tmppart[outi].velocity[2] + ((time_step_size*forcez)/in_particles[outi].weight);                  

           //uprava pozicneho vektora
      in_particles[outi].position[0] = tmppart[outi].position[0] + (time_step_size*in_particles[outi].velocity[0]);
      in_particles[outi].position[1] = tmppart[outi].position[1] + (time_step_size*in_particles[outi].velocity[1]);
      in_particles[outi].position[2] = tmppart[outi].position[2] + (time_step_size*in_particles[outi].velocity[2]);
          
    }
  }
  free_particle_vector(tmppart); 
}

int main(int argc, char *argv[])
{
  FILE *fp;
  t_particle *seq_particles = NULL;
  t_particle *innp_particles = NULL;
  t_particle *outp_particles = NULL;
 
  clock_t start, end;

  // parametry aplicake...
  double time_step_size;
  double num_of_time_steps;
  int    omp_threads;

  if(argc != 5) usage(argv[0]);

  // ... inicializovany z argumentu prikazoveho radku
  time_step_size    = atof(argv[2]);
  num_of_time_steps = atof(argv[3]);
  omp_threads       = atoi(argv[4]);
  printf("time_step_size=%f, time_steps=%d, omp_threads=%d\n",
            time_step_size, (int) num_of_time_steps, omp_threads);

  seq_particles = alocate_particle_vector();
  innp_particles = alocate_particle_vector();
  outp_particles = alocate_particle_vector();

  if(!(fp=fopen(argv[1], "r"))){
    perror(argv[1]);
    exit(1);
  }

  // nacteni vygenerovanych dat ze souboru a zkopirovani do poli,
  // ktera budou zpracovavana paralelnimi implementacemi
  read_particle_vector(fp, seq_particles);
  memcpy(innp_particles, seq_particles, N*sizeof(t_particle));
  memcpy(outp_particles, seq_particles, N*sizeof(t_particle));

  fclose(fp);

  ////////////////////////////////////////////////////////////////////////////
  // Zde probiha volani funkci s implemntacemi reseni algoritmu a mereni casu.
  // Doplnte hlavicky funkci podle potreby, pro ladeni si muzete casti kodu
  // zakomentovat, ale pred odevzdanim uvedte do puvodniho stavu.
  //  !!!! Vysledky vypoctu musi byt pro vypisy v polich pod ukazateli    !!!!
  //  !!!! seq_particles, innp_particles a outp_particles (viz vyse).     !!!!
  time(&start);
  sequential(seq_particles, time_step_size, num_of_time_steps);
  time(&end);
  printf("SEQUENTIAL SOLUTION: %d SEC\n", (int) difftime(end, start));
  print_particle_vector(seq_particles);

  omp_set_num_threads(omp_threads);

  time(&start);
  inner_parallel(innp_particles, time_step_size, num_of_time_steps);
  time(&end);
  printf("INNER PARALLEL SOLUTION: %d SEC\n", (int) difftime(end, start));
  print_particle_vector(innp_particles);
  printf("---diff-par-seq--- idealne same 0.0000\n");
  print_particles_diff(innp_particles, seq_particles);

  time(&start);
  outer_parallel(outp_particles, time_step_size, num_of_time_steps);
  time(&end);
  printf("OUTER PARALLEL SOLUTION: %d SEC\n", (int) difftime(end, start));
  print_particle_vector(outp_particles);
  printf("---diff-par-seq--- idealne same 0.0000\n");
  print_particles_diff(outp_particles, seq_particles);
  ////////////////////////////////////////////////////////////////////////////
  
  free_particle_vector(seq_particles);
  free_particle_vector(innp_particles);
  free_particle_vector(outp_particles);

  return 0;
}
