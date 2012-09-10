/*****************************
 *  Prekladac jazyka IF2008
 *  Matus Kontra (xkontr00) (veduci)
 *  Jozef Kobrtek (xkobrt00)
 *  Patrik Seben (xseben00)
 *  Matej Smetana (xsmeta02)
******************************/ 

// lexikalni analyzator
#include <stdio.h>
#include <ctype.h>
#include "str.h"
#include "lexer.h"

#define EXT_LINE
int line = 1;
FILE * source;

int
loadSourceFile(FILE * src)  
{
  if (src == NULL)
  {
    return FILE_ERROR;
  }

/**
  if ( feof(src) )
  {
    return FILE_ERROR;
  }
 **/
  
  source = src;
  return 0;
}

int 
s_ungetc( char c, FILE * stream)
{
  if ( c =='\n')
  {
    line-=1;
  }
  return ungetc(c, stream);  
}


int
getNextToken(string * attr)
{
  int c;  //citany znak
  int state = INITIAL; // pociatocny stav
  strClear(attr);
  
  //hlavny cyklus analyzz reprezentujuci KA
  while ( 1 )
  {
    c = getc( source );
    if ( c == '\n') line++;
    //printf("entering state: %d read character %c on line:%d\n", state,c,line);
    switch ( state )
    {
      case INITIAL:         //pociatocny stav
        if ( isspace(c) )   //biele znaky preskakujeme
        {
          state = INITIAL;
        } else
        if ( isalpha(c) )   //IDENTIFIKATOR
        {
          state = IN_ID;    //stav nacitania identifikatora
          strAddChar( attr,c );
        } else
        if ( c == '/' )     //obycajne lomitko
        {
          state = DIV_OR_COM; //pozreme sa na dalsi znak aby sme rozhodli
        } else
        if ( c == '=' )     //rovna sa
        {
          state = ASSIGN_OR_EQ; //moze byt priradenie alebo rovnost
        } else
        if ( c == '<')    //mensi nez
        {
          state = LT_OR_LE_OR_OUT; //mensi or mensi rovny or vystup
        } else
        if ( c == '>')    //vetsi nez
        {
          state = GT_OR_GE_OR_IN; //vetsi or vetsi rovny or vstup
        } else
        if ( c == '!' )   //vykricnik
        {
          state = IN_NEQ; //cakame na rovnasa
        } else 
        if ( c == '+')    //plus
        {
          return ADD;
        } else
        if ( c == '-')    //minus
        {
          return SUBSTRACT;
        } else
        if ( c == '*')    //asterisk
        {
          return MULTIPLY;
        } else
        if ( c == '{')    //lava zlozena
        {
          return LEFT_VINCULUM;
        } else
        if ( c == '}')    //prava zlozena
        {
          return RIGHT_VINCULUM;
        } else
        if ( c == '(')    //left zatvorka
        {
          return LEFT_BRACKET;
        } else
        if ( c == ')')    //right bracket
        {
          return RIGHT_BRACKET;
        } else
        if ( c == ',')    //carka
        {
          return COLON;
        } else
        if ( c == ';')    //semicolon
        {
          return SEMICOLON;
        } else
        if ( c == '\"')   //uvodzovky = retazcova konstanta
        {
          state = STR_CON;
        } else
        if ( isdigit(c) && c == '1')   //cislo = budeme predpokladat binarne ale nie oct
        {
          strAddChar( attr,c );
          state = READ_BIN_NOT_OCT;
        } else
        if ( isdigit(c) && c == '0')   //prisla 0 - musime rozhodnut co sa bude diat
        {
          strAddChar( attr,c );
          state = FIRST_ZERO; 
        } else
        if ( isdigit(c) && c >= '2')   //prisla cifra 2 a vyssia nacitame 10 cislo
        {
          strAddChar( attr,c );
          state = READ_DEC;
        } else
        if ( c == EOF)    //end-of-file
        {
          return END_OF_FILE;
        } else
          return LEX_ERROR; //nic prijatelne
        break;
          
      case IN_ID:     //citame identifikator
        if ( isalnum(c) )
        {
          state = IN_ID;    //stav nacitania identifikatora
          strAddChar( attr,c );          
        } else
        {
          s_ungetc( c, source );
          
          //potreba rozlisit ID a keyword
          if (strCmpConstStr(attr, "int") == 0) return INTEGER;
          else
          if (strCmpConstStr(attr, "double") == 0) return DOUBLE;
          else
          if (strCmpConstStr(attr, "string") == 0) return STRING;
          else
          if (strCmpConstStr(attr, "if") == 0) return IF;
          else
          if (strCmpConstStr(attr, "else") == 0) return ELSE;
          else
          if (strCmpConstStr(attr, "while") == 0) return WHILE;
          else
          if (strCmpConstStr(attr, "cin") == 0) return CIN;
          else
          if (strCmpConstStr(attr, "cout") == 0) return COUT;
          else
          if (strCmpConstStr(attr, "main") == 0) return MAIN;
          else
          if (strCmpConstStr(attr, "length") == 0) return LENGTH;
          else
          if (strCmpConstStr(attr, "find") == 0) return FIND;
          else
          if (strCmpConstStr(attr, "sort") == 0) return SORT;
          else
          if (strCmpConstStr(attr, "concat") == 0) return CONCAT;
          else
          if (strCmpConstStr(attr, "return") == 0) return RETURN;
          else
            return ID;
        }
        break;
      
      case DIV_OR_COM:  //nacitali sme '/' delene alebo comentar ?
        if (c == '/')
        {
          state = COM_SINLINE;
        } else 
        if (c == '*')
        {
          state = COM_MULT_IN;
        } else
        {
          s_ungetc( c,source );
          return DIVIDE;
        }
        break;
        
      case COM_SINLINE: //jednoriadkovy kom. nacitavame po endlin
        if ( c == '\n' )
        {
          state = INITIAL;
        } else 
        if ( c == EOF)
        {
          return LEX_ERROR;
        }
        break;
        
      case COM_MULT_IN: //multiline komentar
        if ( c == '*' )
        {
          state = COM_MULT_AST;
        } else 
        if ( c == EOF)
        {
          return LEX_ERROR;
        }
        break;
        
      case COM_MULT_AST: //vo viacriadkovom hviezdicka
        if ( c == '*')
        {
          state = COM_MULT_AST; //ak hviezdicka ostavame tu
        } else
        if ( c == '/')
        {
          state = INITIAL;
        } else
        if ( c == EOF )
        {
          return LEX_ERROR;
        } else
        {
          state = COM_MULT_IN;
        }
        break;
        
      case ASSIGN_OR_EQ: //rovnitko alebo porovnanie
        if ( c == '=' )
        {
          return EQ;
        } else
        {
          s_ungetc ( c,source);
          return ASSIGN;
        }
        break;
        
      case IN_NEQ:    //nasli sme ! co je dalej
        if ( c == '=' )
        {
          return NEQ;
        } else
          return LEX_ERROR;
        break;
        
      case LT_OR_LE_OR_OUT: // "<"
        if ( c == '=' )
        {
          return LE;
        } else
        if ( c == '<')
        {
          return OUT;
        } else
        {
          s_ungetc(c,source);
          return LT;
        }
        break;
        
      case GT_OR_GE_OR_IN:  // ">"
        if ( c == '=' )
        {
          return GE;
        } else
        if ( c == '>')
        {
          return IN;
        } else
        {
          s_ungetc(c,source);
          return GT;
        }
        break;
        
      case STR_CON: //retazcova konstanta
        if ( c == '\\')
        {
          state = STR_SLASH; // nasli sme lomitko
        } else 
        if ( c == '\"')
        {
          return CONST_STR;
        } else
        if ( c == EOF || c <= 31)
        {
          return LEX_ERROR;
        } else
          strAddChar( attr,c );
        break;
        
      case STR_SLASH: //lomitko
        if ( c == 't' )
        {
          strAddChar( attr,'\t' );
          state = STR_CON;
        } else
        if ( c == 'n' )
        {
          strAddChar( attr,'\n' );
          state = STR_CON;
        } else
        if ( c == '\\' )
        {
          strAddChar( attr,'\\' );
          state = STR_CON;
        } else
        if ( c == '\"' )
        {
          strAddChar( attr,'\"' );
          state = STR_CON;
        } else
            return LEX_ERROR;
        break;
       
      // Automaty pre cisla  
      
      case FIRST_ZERO:
        if ( c == 'x')
        {
          strAddChar( attr,'x');
          state = READ_HEX; 
        } else
        if ( isdigit(c) && c <='1' )
        {
          strAddChar( attr,c );
          state = READ_BIN;
        } else
        if ( isdigit(c) && c <='7' )
        {
          strAddChar( attr,c );
          state = READ_OCT;
        } else
        if ( isdigit(c) && c <='9' )
        {
          strAddChar( attr,c );
          state = READ_DEC;
        } else
        if ( c == '.')    
        {
          strAddChar( attr,'.' );
          state = DBL_DECIMAL_ONE;
        } else
        if ( c == 'e' || c == 'E')
        {
          strAddChar( attr,'e');
          state = DBL_EXP_ONE;
        } else
        {
          s_ungetc (c, source);
          return CONST_INT;
        }
        break;
      
      case READ_BIN:   //2kove cislo 0 na zaciatku a b nakonci
        if ( c == '.')    //i ked sa tvarime ze nacitavame binarne pri spec znaku skocime 
        {
          strAddChar( attr,'.' );
          state = DBL_DECIMAL_ONE;
        } else
        if ( c == 'e' || c == 'E')
        {
          strAddChar( attr,'e');
          state = DBL_EXP_ONE;
        } else 
        if ( isdigit (c) && c <='1')
        {
          strAddChar( attr,c);
        } else
        if ( isdigit (c) && c <='7')  //ak sme prisli na cislo mensie ako 7 a vesie ako 1 nacitame ako oktal (prva bola nula)
        {
          strAddChar( attr,c);
          state = READ_OCT;
        } else
        if ( isdigit (c) && c <='9')  //ak sme prisli na cislo mensie ako 9 a vesie ako 7 nacitame ako desiatkove
        {
          strAddChar( attr,c);
          state = READ_DEC;
        } else
        if ( c =='b' )  //prislo b nakonci 2 kova konstanta
        {
          strAddChar( attr,c);
          return B_INT_CON;
        } else
        {
          s_ungetc (c, source);
          return O_INT_CON;
        }
        break;
        
      case READ_BIN_NOT_OCT:   //2kove cislo 1 na zaciatku (nebude oktalove pi 2-7 ) a b nakonci
        if ( c == '.')    //i ked sa tvarime ze nacitavame binarne pri spec znaku skocime 
        {
          strAddChar( attr,'.' );
          state = DBL_DECIMAL_ONE;
        } else
        if ( c == 'e' || c == 'E')
        {
          strAddChar( attr,'e');
          state = DBL_EXP_ONE;
        } else 
        if ( isdigit (c) && c <='1')
        {
          strAddChar( attr,c);
        } else
        if ( isdigit (c) && c <='9')  //ak sme prisli na cislo mensie ako 9 a vesie ako 7 nacitame ako desiatkove
        {
          strAddChar( attr,c);
          state = READ_DEC;
        } else
        if ( c =='b' )  //prislo b nakonci 2 kova konstanta
        {
          strAddChar( attr,c);
          return B_INT_CON;
        } else
        {
          s_ungetc (c, source);
          return CONST_INT;
        }
        break;
        
      case READ_OCT: //nula na zaciatku cisla 0-7
        if ( c == '.')
        {
          strAddChar( attr,'.' );
          state = DBL_DECIMAL_ONE;
        } else
        if ( c == 'e' || c == 'E')
        {
          strAddChar( attr,'e');
          state = DBL_EXP_ONE;
        } else
        if ( isdigit(c) && c < '8')
        {
          strAddChar( attr,c);
          state = READ_OCT;
        } else
        if ( isdigit(c) && c >= '8')
        {
          strAddChar( attr,c);
          state = READ_DEC;
        } else
        {
          s_ungetc (c, source);
          return O_INT_CON;
        }
        break;
        
      case READ_DEC:   
        if ( c == '.')    
        {
          strAddChar( attr,'.' );
          state = DBL_DECIMAL_ONE;
        } else
        if ( c == 'e' || c == 'E')
        {
          strAddChar( attr,'e');
          state = DBL_EXP_ONE;
        } else 
        if ( isdigit (c))
        {
          strAddChar( attr,c);
        } else
        {
          s_ungetc (c, source);
          return CONST_INT;
        }
        break;
        
      case READ_HEX:    // nasli sme 0x prjdeme na NUM_cON alebo chybu
        if ( isxdigit(c) )
        {
          strAddChar( attr,c);
        } else
        {
          s_ungetc (c, source);
          return H_INT_CON;
        }
        break;
        
        
      case DBL_DECIMAL_ONE:   //aspon jedna cislica desatinnej casti
        if ( isdigit (c) )
        {
          strAddChar( attr,c );
          state = DBL_DECIMAL_RPT;          
        } else
          return LEX_ERROR;
        break;
          
      case DBL_DECIMAL_RPT: //opakovanie v des casti
        if ( isdigit (c) )
        {
          strAddChar( attr,c );        
        } else   
        if ( c == 'e' || c == 'E')
        {
          strAddChar( attr,c );
          state = DBL_EXP_ONE;
        } else
        {
          s_ungetc (c,source);
          return CONST_DBL;
        }
        break;
        
      case DBL_EXP_ONE:    //exponentova cast
        if ( isdigit (c) )
        {
          strAddChar( attr,c );
          state = DBL_EXP_RPT;  
        } else
        if ( c == '+' || c == '-' )
        {
          strAddChar( attr,c );
          state = DBL_EXP_PM; 
        } else
          return LEX_ERROR;
        break;
        
      case DBL_EXP_PM:  //nacitali sme + al - musime nacitat cifru
        if ( isdigit(c) )
        {
          strAddChar( attr,c );
          state = DBL_EXP_RPT;          
        } else
          return LEX_ERROR;
        break;
          
      case DBL_EXP_RPT:
        if ( isdigit(c) )
        {
          strAddChar( attr,c );
          state = DBL_EXP_RPT;          
        } else
        {
          s_ungetc(c,source);
          return CONST_DBL;
        }
        break;
  
    }
  }
  
}
