/*****************************
 *  Prekladac jazyka IF2008
 *  Matus Kontra (xkontr00) (veduci)
 *  Jozef Kobrtek (xkobrt00)
 *  Patrik Seben (xseben00)
 *  Matej Smetana (xsmeta02)
******************************/     

#ifndef LEXER_H
#define LEXER_H
#include <stdio.h>
#include "str.h"
#include "codes.h"

#define ID         10

typedef enum  {
  INTEGER = 11, DOUBLE, STRING, IF, ELSE, WHILE, CIN,
  COUT, MAIN, LENGTH, FIND, SORT, CONCAT, RETURN
} keywords;

typedef enum  {
  CONST_STR = 30, CONST_DBL, CONST_INT, B_INT_CON, O_INT_CON, 
  H_INT_CON
} constants;

typedef enum  {
  OUT = 50, IN
} inouts;

typedef enum  {
  ASSIGN = 60, EQ, NEQ, LT, LE, GT,
  GE, ADD, SUBSTRACT, MULTIPLY, DIVIDE
} operators;

typedef enum  {
  LEFT_VINCULUM = 80, RIGHT_VINCULUM, SEMICOLON, LEFT_BRACKET,
  RIGHT_BRACKET, COLON
} symbols;

#define END_OF_FILE    99

typedef enum  {
  INITIAL = 100,
  
  IN_ID, 
  DIV_OR_COM, 
  ASSIGN_OR_EQ,
  LT_OR_LE_OR_OUT, 
  GT_OR_GE_OR_IN, 
  IN_NEQ, 
  
  COM_SINLINE,
  COM_MULT_IN, 
  COM_MULT_AST, 
  
  STR_CON, 
  STR_SLASH,
   
  FIRST_ZERO,
  READ_BIN, 
  READ_BIN_NOT_OCT, 
  READ_OCT, 
  READ_DEC, 
  READ_HEX,
  
  DBL_DECIMAL_ONE, 
  DBL_DECIMAL_RPT, 
  DBL_EXP_ONE, 
  DBL_EXP_RPT,
  DBL_EXP_PM
  
} states;

int loadSourceFile(FILE* src);
int getNextToken(string *attr);

#endif //LEXER_H
