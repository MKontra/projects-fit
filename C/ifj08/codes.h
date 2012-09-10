/*****************************
 *  Prekladac jazyka IF2008
 *  Matus Kontra (xkontr00) (veduci)
 *  Jozef Kobrtek (xkobrt00)
 *  Patrik Seben (xseben00)
 *  Matej Smetana (xsmeta02)
******************************/ 

#ifndef CODES_H
#define CODES_H

// chybove kody
#include <stdlib.h>
#include <stdio.h>

#define PARSE_OK     0
#define OP_OK     0

#define LEX_ERROR    1
#define SYNTAX_ERR   2
#define SEMANTIC_ERR 3
#define RUNTIME_ERR  4
#define FILE_ERROR   5
#define SYSTEM_ERR   6

//globalna premenna povolujuca vypis chybovych hlasok
int debug_off;
int printDesc( int code , char * add);


#endif //CODES_H
