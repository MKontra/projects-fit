/*****************************
 *  Prekladac jazyka IF2008
 *  Matus Kontra (xkontr00) (veduci)
 *  Jozef Kobrtek (xkobrt00)
 *  Patrik Seben (xseben00)
 *  Matej Smetana (xsmeta02)
******************************/ 

#ifndef STR_H
#define STR_H

//hlavickovy soubor pro praci s nekonecne dlouhymi retezci

typedef struct
{
  char* str;		// misto pro dany retezec ukonceny znakem '\0'
  int length;		// skutecna delka retezce
  int allocSize;	// velikost alokovane pameti
} string;


//praca s ADT string
int strInit(string *s);
string * newStr();
void strFree(string *s);

void strClear(string *s);
int strAddChar(string *s1, char c);
int strCopyString(string *s1, string *s2);
int strCmpString(string *s1, string *s2);
int strCmpConstStr(string *s1, char *s2);
int strAppendString(string *dst, string *src);
string * strNewFromConstChar(  char * src);
int strInsertConstChar(string * dest,  char * src);

char *strGetStr(string *s);
int strGetLength(string *s);

//funkcie zabudovane do jazyka...

int length( string * s1);
int find ( string * in_this,  string * what);
string * sort(string * src);
string *concat( string* s1,  string* s2);

//pomocne budovanych
void quicksort_c ( char * arr, int left, int right);
void swap_chars( char * arr, int i1, int i2);
int * pm_table(string * s1);

// parsovanie cisel
double parseDbl( string * src );
int parseNum( string * src, int* retval, int base);
int parseBin( string * src, int *retval);

#endif //STR_H
