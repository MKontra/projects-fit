/*****************************
 *  Prekladac jazyka IF2008
 *  Matus Kontra (xkontr00) (veduci)
 *  Jozef Kobrtek (xkobrt00)
 *  Patrik Seben (xseben00)
 *  Matej Smetana (xsmeta02)
******************************/ 

#ifndef EXPRSTACK_H
#define EXPRSTACK_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "str.h"
#include "parser.h"
#include "bvs.h"
#include "lexer.h"
#include "ilist.h"
#include "codes.h"

//premenne z parsera
extern int token; 
extern string attr;
extern Sym_Tab table;
extern ptr_IList ilist;

//funkcia volana pri parsovani
int parseExpr(Table_Entry * retval);

//zasobnik pouzity pre precedencne parsovanie

/** definicie typu  z tab. symbolov
typedef enum  {
  INT_TYPE = 1, DBL_TYPE, STR_TYPE
} type_spec;
**/

//nazvy typov do parsera
typedef enum
{
  E_TERM = 11, E_ID = 111, E_INT = 1, E_DBL = 2, E_STR = 3
} stack_cont;

typedef struct sentry
{
  int type;
  string * attr;
  
  union
  {
    Table_Entry id;
    int i;
    double dbl;
    string * str;  
  } val;
  
  struct sentry * next;
} expr, *expr_ptr;

typedef struct exprstack 
{
  expr_ptr top;
} EStack, *ptr_EStack;

int EStackInit(ptr_EStack stack);
int EStackFree(ptr_EStack stack);

int EStackIsEmpty(ptr_EStack stack);
int EStackPush(ptr_EStack stack, expr_ptr cont);
int EStackPop(ptr_EStack stack);
expr_ptr EStackTop(ptr_EStack stack);
expr EStackTopPop(ptr_EStack stack);

expr_ptr EStackLookOn(ptr_EStack stack, int pos); //pozrie sa hlbsie do zasobnika...
void EStackPrintCont(ptr_EStack stack );

//shiftovacie funkcie
int shiftOther(ptr_EStack stack_ptr);
int shiftId(ptr_EStack stack_ptr);
int shiftConst(ptr_EStack stack_ptr);

//redukcne funkcie
int reduceId(ptr_EStack stack_ptr);
int reduceConst(ptr_EStack stack_ptr);

int reduceAdd(ptr_EStack stack_ptr);
int reduceSub(ptr_EStack stack_ptr);
int reduceMul(ptr_EStack stack_ptr);
int reduceDiv(ptr_EStack stack_ptr);

int reduceBrackets(ptr_EStack stack_ptr);
int reduceAssign(ptr_EStack stack_ptr);

int reduceEQ(ptr_EStack stack_ptr);
int reduceNEQ(ptr_EStack stack_ptr);
int reduceGT(ptr_EStack stack_ptr);
int reduceLT(ptr_EStack stack_ptr);
int reduceGE(ptr_EStack stack_ptr);
int reduceLE(ptr_EStack stack_ptr);

#endif //EXPRSTACK_H
