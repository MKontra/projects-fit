/*****************************
 *  Prekladac jazyka IF2008
 *  Matus Kontra (xkontr00) (veduci)
 *  Jozef Kobrtek (xkobrt00)
 *  Patrik Seben (xseben00)
 *  Matej Smetana (xsmeta02)
******************************/ 

#include "codes.h"
#include "lexer.h"

int
printDesc( int code , char * add)
{

  extern int debug_off;


  if (debug_off != 0) return code;
  
  char * desc = NULL;
  
  switch( code )
  {
  
    case 0 : desc = "All OK.";
    
    case 1 : desc = "Error during lexical analysis."; break;
    
    case 2 : desc = "Error during syntactic analysis."; break;
    
    case 3 : desc = "Error during semantical analysis."; break;
    
    case 4 : desc = "Runtime error."; break;
    
    case 5 : desc = "File does not exists or cannot be accessed."; break;
    
    case 6 : desc = "Critical system error. Memory allocation failed"; break;
    
    default : desc = "Unknown error code.";
  
  }

    #ifdef DEBUG
    extern int line; 
    fprintf(stderr,"Error occured on line: %d code: %d description: %s\nAdditional info: %s\n", line, code, desc, ( ( add == NULL ) ? "No info." : add ) );
    #endif
//  #else
//    printf("Error occured code: %d description: %s\nAdditional info: %s\n", code, desc, ( ( add == NULL ) ? "No info." : add ) );
//  #endif
  
  return code;
  
}
