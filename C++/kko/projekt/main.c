/*
 * Autor: Matus Kontra (xkontr00)
 * Datum: 22.4.2010
 * Soubor: ahed.h
 * Komentar: spustitelny program
 */ 
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include "ahed.h"


int main(int argc, char **argv)
{
  int op = 0;
  tAHED ahed;
  char *infile = NULL;
  char *outfile = NULL;
  char *logfile = NULL;
  int index;
  int c;
  
  opterr = 0;
 
  //spracuj command line 
  while ((c = getopt (argc, argv, "i:o:l:cxh")) != -1)
   switch (c)
     {
     case 'i':
       infile = optarg;
       break;
     case 'o':
       outfile = optarg;
       break;
     case 'l':
       logfile = optarg;
       break;
     case 'c':
       op = 1;
       break;
     case 'x':
       op = 2;
       break;
     case 'h':
       op = 3;
       break;
     case '?':
       if (optopt == 'i')
         fprintf (stderr, "Option -%c requires an argument.\n", optopt);
       else if (optopt == 'o')
         fprintf (stderr, "Option -%c requires an argument.\n", optopt);
       else if (optopt == 'l')
         fprintf (stderr, "Option -%c requires an argument.\n", optopt);
       else
         fprintf (stderr, "Unknown option `-%c'.\n", optopt);
       break;
     default:
       abort ();
     }
  
  //validacia kombinacie volieb   
  if ( op == 0 || infile == NULL || outfile == NULL )
    op = 0;

  //otvorenie suborov
  FILE * infileh = fopen(infile, "rb");
  if ( infileh == NULL && op != 0)
  {
    printf("Error opening input file\n");
    return -1;
  }
  FILE * outfileh = fopen(outfile, "wb");
  if ( outfileh == NULL && op != 0)
  {
    printf("Error opening output file\n");
    return -1;
  } 
  
  FILE * logfileh = NULL;
  if ( logfile != NULL )
  {
      logfileh = fopen(logfile, "wb");
      if ( logfileh == NULL && op != 0)
      {
        printf("Error opening log file\n");
        return -1;
      }
  }
  
  //koduj - volaj kniznicu 
  if ( op == 1 )
  {
    int retval = AHEDEncoding( &ahed, infileh, outfileh );
    fclose(infileh);
    fclose(outfileh);
    if ( retval == AHEDFail )
      printf("Kodovanie zlyhalo\n");
    if ( logfileh != NULL )
    {
      fprintf(logfileh, "login = xkontr00\n"); 
      fprintf(logfileh, "uncodedSize = %lld\n", ahed.uncodedSize);
      fprintf(logfileh, "codedSize = %lld\n", ahed.codedSize);
      fclose(logfileh);
    }
  }
  else if ( op == 2)  //dekoduj
  {
    int retval = AHEDDecoding( &ahed, infileh, outfileh );
    fclose(infileh);
    fclose(outfileh);
    if ( retval == AHEDFail )
      printf("Deodovanie zlyhalo\n");
    if ( logfileh != NULL )
    {
      fprintf(logfileh, "login = xkontr00\n"); 
      fprintf(logfileh, "uncodedSize = %lld\n", ahed.uncodedSize);
      fprintf(logfileh, "codedSize = %lld\n", ahed.codedSize);
      fclose(logfileh);
    }
  }        //help
  else
  {
    printf("Aplikacia vykonavajuca adaptivne huffmanove kodovanie:\n");  
    printf("Paramere:\n");
    printf("-i filename : vstupny subor\n");   
    printf("-o filename : vystupny subor\n");
    printf("-l filename : subor so spravou\n");
    printf("-c : koduj\n");   
    printf("-x : dekoduj\n");
    printf("-h : vypis tuto spravu\n"); 
  }

	return 0;
}


	
