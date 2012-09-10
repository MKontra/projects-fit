/*****************************
 *  Prekladac jazyka IF2008
 *  Matus Kontra (xkontr00) (veduci)
 *  Jozef Kobrtek (xkobrt00)
 *  Patrik Seben (xseben00)
 *  Matej Smetana (xsmeta02)
******************************/ 

//jednoducha knihovna pro praci s nekonecne dlouhymi retezci
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include "str.h"

#define STR_LEN_INC 8
// konstanta STR_LEN_INC udava, na kolik bytu provedeme pocatecni alokaci pameti
// pokud nacitame retezec znak po znaku, pamet se postupne bude alkokovat na
// nasobky tohoto cisla 

#define STR_ERROR   1
#define STR_SUCCESS 0

int strInit(string *s)
// funkce vytvori novy retezec
{
   if ((s->str = (char*) malloc(STR_LEN_INC)) == NULL)
      return STR_ERROR;
   s->str[0] = '\0';
   s->length = 0;
   s->allocSize = STR_LEN_INC;
   return STR_SUCCESS;
}

string *
newStr()
//vytvory sa novy ukazatel na retazec
{
  string * newptr;
  
  if ( (newptr = (string*) malloc(sizeof(string))) == NULL)
    return NULL;

   if ((newptr->str = (char*) malloc(STR_LEN_INC)) == NULL)
      return NULL;
      
   newptr->str[0] = '\0';
   newptr->length = 0;
   newptr->allocSize = STR_LEN_INC;    

  return newptr;
  
}

//uvolnenie retazca a vyresetovanie hodnot
void strFree(string *s)
// funkce uvolni retezec z pameti
{
  if ( s != NULL )
  {
  
    if ( s->str != NULL)
    {
      free(s->str);
      s->str = NULL;
    }
    s->length = 0;
    s->allocSize = 0;
    s->str = NULL;
  } else
    return;
}

void strClear(string *s)
// funkce vymaze obsah retezce
{
   s->str[0] = '\0';
   s->length = 0;
}


//vytvory ukazatel na retazec z char *
string *
strNewFromConstChar(  char * src)
{
  string * newptr;
  int len = strlen(src);
  if ( (newptr = (string*) malloc(sizeof(string))) == NULL)
    return NULL;
    
  if ((newptr->str = (char*) malloc(len+1)) == NULL)
    return NULL;
    
   strcpy(newptr->str, src );
   newptr->length = len;
   newptr->allocSize = len+1;
   return newptr;
}


//vlozy const * do naalokovaneho retazca
int 
strInsertConstChar(string * dest,  char * src)
{

  if ( strlen(src) >= dest->length )
  {
      if ((dest->str = (char*) realloc (dest->str, strlen(src) + 1)) == NULL)
      {
        printf("strInsertConstChar: realloc vratil NULL\n");
        return STR_ERROR;
      }
      dest->allocSize = strlen(src) + 1;  
  }
    

    
   strcpy(dest->str, src );
   dest->length = strlen(src);

  return STR_SUCCESS;
}

int strAddChar(string *s1, char c)
// prida na konec retezce jeden znak
{
   if (s1->length + 1 >= s1->allocSize)
   {
      if ((s1->str = (char*) realloc (s1->str, s1->allocSize + STR_LEN_INC)) == NULL)
      {
        printf("AddChar: realloc vratil NULL\n");
        return STR_ERROR;
      }
      s1->allocSize = s1->allocSize + STR_LEN_INC;
   }
   s1->str[s1->length] = c;
   s1->length++;
   s1->str[s1->length] = '\0';
   return STR_SUCCESS;
}

int strCopyString(string *s1, string *s2)
// prekopiruje retezec s2 do s1
{
   int newLength = s2->length;
   if (newLength +1 >= s1->allocSize)
   {
      // pamet nestaci, je potreba provest realokaci
      if ((s1->str = (char*) realloc(s1->str, newLength + 1)) == NULL)
         return STR_ERROR;
      s1->allocSize = newLength +1;
   }
   strcpy(s1->str, s2->str);
   s1->length = newLength;
   return STR_SUCCESS;
}

int strAppendString(string *dst, string *src)
//spoji retazce a ulozi do prveho...
{
  int newLength = dst->length + src->length; 
  if (newLength +1 >= dst->allocSize)
  { 
      // pamet nestaci, je potreba provest realokaci
     if ( (dst->str = (char*) realloc (dst->str, newLength + 1)) == NULL)
     {
        printf("realloc vratil NULL\n");
        return STR_ERROR;
      }
     dst->allocSize = newLength + 1;
  }
   
  strcat(dst->str, src->str);
  
  dst->length = newLength;
  return STR_SUCCESS;
}

int strCmpString(string *s1, string *s2)
// porovna oba retezce a vrati vysledek
{
   return strcmp(s1->str, s2->str);
}

int strCmpConstStr(string *s1,char* s2)
// porovna nas retezec s konstantnim retezcem
{
   return strcmp(s1->str, s2);
}

char *strGetStr(string *s)
// vrati textovou cast retezce
{
   return s->str;
}

int strGetLength(string *s)
// vrati delku daneho retezce
{
   return s->length;
}

//----------------------------------------------------------
//funkcie jazyka

//vrati dlzku retazca
int
length( string * s1)
{
  return strGetLength(s1);
}

//spoji dva retazce a vrati novy
string * 
concat(  string* s1,   string* s2)
{
  string * newstr = strNewFromConstChar(strGetStr(s1));
  strAppendString(newstr, s2);
  return newstr;
}

//zoradi retazec quicksortom
string * sort ( string * src)   //implementacia radenia quicksortom
{
  if ( src == NULL )
    return NULL;
    
  int len = strGetLength(src);

  srand( len );
  string * srtd = strNewFromConstChar( strGetStr(src)); 
  
  if ( len == 0)  //prazdny netriedime
    return srtd;
  
  quicksort_c(strGetStr(srtd) , 0, len-1);  //volame qs na pole
  return srtd;
  
}

//pomocne funkcie triedenia

//funkcia implementujuca quicksort
void 
quicksort_c ( char * arr, int left, int right)
{
  int pivotIndex,pivotVal, storeTo;
 
  if ( left >= right)   //ukoncenie rekurzie
    return;
  
  pivotIndex = left + ( rand() % (right-left+1) );  //vyber pivota
  pivotVal = arr[pivotIndex];   //hodnota
  storeTo = left;           //nastavenie pozicie kam sa zacnu ukladat hodnoty mensie ako pivot
  swap_chars(arr, pivotIndex, right); //pivot uplne doprava
  for ( int i = left; i < right; i++) //usporiadanie hodnot
  {
    if ( arr[i] <= pivotVal)          //ak je hodnota mensia ako pivot umiestnime ju na index
    {
      swap_chars(arr, i, storeTo);    //storeTo a ten zvysime o 1
      storeTo++;
    }
  }
  swap_chars(arr, storeTo, right);    //umiestnime pivota na jeho miest
  quicksort_c(arr, left, storeTo-1);  //rekurzivne volanie na lavy podlist
  quicksort_c(arr, storeTo+1, right); //pravy podlist
}


void
swap_chars( char * arr, int i1, int i2) //vymeni znaky v poli
{
  char tmp = arr[i1];
  arr[i1] = arr[i2];
  arr[i2] = tmp;
}


//podretazec na styl KMP
int
find ( string * in_this,  string * what)
{
  int m = 0; //namatchovany v zdrojovom
  int i = 0; //pozicia v hladanom
  int * pmtable = pm_table(what); //partial match table
  
  
  int len_in = strGetLength(in_this);
  int len_what = strGetLength(what);
  
  char * what_ptr = strGetStr(what);
  char * in_ptr = strGetStr(in_this);
  
  if ( pmtable == NULL )
  {
    return 0;
  }
  //specialne pripady
  if ( (len_in == 0) && (len_what == 0) )
  {
    return 0;
  }
  
  //algoritmus prehladavania
  while ( m + i < len_in )  //ukoncenie ak sa preslo celym retazcom a nenaslo sa
  {
    if ( what_ptr[i] == in_ptr[m+i] ) //zhoda
    {
      i++;            //posunieme sa vo vzorke
      if ( i == len_what )  //ak sme namatchovali celu vzorku vratime zhodu
        return m;
    } else
    {
      m = m + i - pmtable[i]; //nezhoda posuniem sa o i dopredu a vratim sa o potrebny pocet miest ktore zistim z tabulky
      if ( i > 0)
        i = pmtable[i];       //v zhode sa posuniem na pocet o ktory som sa musel vratit
    }
  }
  
  free(pmtable);
  return -1;
  
}

//pomocne funkcie findu...
//kalkulacia tabulky
int *
pm_table(string * s1)
{
  if ( s1 == NULL )
    return NULL;
    
  int len = strGetLength(s1);
  
  if ( len == 0)
    return NULL;
    
  int * tmp = malloc(len*sizeof(int));  //naalokuje miesto
  if (tmp == NULL)
    return NULL;
  
  int pos = 2;
  int cnd = 0;
  
  //staticke nastavenie prvych hodnot
  switch(len){
    case 1:
      tmp[0] = -1;
      break;
      
    case 2: 
      tmp[0] = -1;
      tmp[1] = 0;
      break;
      
    default:
      tmp[0] = -1;
      tmp[1] = 0;
      //generacia zvysnych miest
      //hladame "spravne" pripony retazca konciaceho na i-1 ktory je zaroven platnou "spravnou" predponou vzorky
      //
      while ( pos < len )
      {
        //
        // --ak pokracuje p.suffix ktory je zaroven prefixom prradime hodnotu o 1 vecsiu
        if ( strGetStr(s1)[pos-1] == strGetStr(s1)[cnd] )
        {
          tmp[pos]=cnd+1;
          pos++;
          cnd++;
        } else
        // podretazec nepokracuje a este sa mozeme vratit
        if (cnd > 0)
        {
          cnd = tmp[cnd];
        } else
        //ziadne dalsie mozne podretazce dame 0 a posuvame sa
        {
          tmp[pos] = 0;
          pos++;
        }
        
      }
  }
  
  return tmp;
}


//----------------------------------------------------------

//retazec na cislo
//retazec sa nekontroluje za to je zodpovedny lexikalny analyzer
double parseDbl( string * src )
{
  return strtod(strGetStr(src), NULL);
}


int parseNum
( string * src, int* retval, int base)
{
  if (base == 0)
    return -1; //chybny zaklad
  else 
  if ( base == 2)
  {
    return parseBin(src,retval);
  }
    
  *retval = strtol(strGetStr(src), NULL, base);
  return 0;
}


//parsing binary number
int
parseBin( string * src, int *retval)
{
  int len = strGetLength(src);
  int tmp = 0;
  for (int i = 0; i != len; i++)
  {
    char curr_char = strGetStr(src)[i];
    if ( (curr_char == 'b') && (i == len-1))
    {
      *retval = tmp;
      return 0;
    } else
    if ( curr_char > '1' || curr_char < '0')
      return -1;
    tmp = (tmp * 2) + ( curr_char - 48);
  }
  
  *retval = tmp;
  return 0;  

}
