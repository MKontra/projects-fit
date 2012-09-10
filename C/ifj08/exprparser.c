/*****************************
 *  Prekladac jazyka IF2008
 *  Matus Kontra (xkontr00) (veduci)
 *  Jozef Kobrtek (xkobrt00)
 *  Patrik Seben (xseben00)
 *  Matej Smetana (xsmeta02)
******************************/ 


#include "exprparser.h"

//------------------------------------------------------------------------------
//pomocna funkcia vytvori z polozky zasobnika polozku tabulky symbolov naleziteho typu
int
exprToTableEntry(expr_ptr in, Table_Entry * dst)
{
  switch ( in->type )
  {
    case E_ID :
      *dst = in->val.id;
      break;
    
    //pre konstanty novy zaznam ....   
    case E_INT :
      *dst = newVariabile(INT_TYPE);
      (*dst)->value.i = in->val.i;
      break;
  
    case E_DBL :
      *dst = newVariabile(DBL_TYPE);    
      (*dst)->value.dbl = in->val.dbl;
      break;
      
    case E_STR :
      *dst = newVariabile(STR_TYPE); 
      (*dst)->value.str = strNewFromConstChar(strGetStr(in->val.str));
      break;  
  }
  
  return OP_OK;
  
}

//------------------------------------------------------------------------------
//hlavna funkcia parsovania vyrazu
//vracia chybovy kod a ukazatel do TS

int 
parseExpr(Table_Entry * retval)
{
  //Zasobnik a jeho inicializacia
  EStack stack;
  ptr_EStack stack_ptr = &stack;
  EStackInit(stack_ptr);

  int errval;
  
  while(1)
  {
    //najdeme terminal na vrchu zasobnika... 
    int pos = 0;
    expr_ptr lastterm = EStackTop(stack_ptr);
    //iterujeme kym nenajdeme vrch alebo terminal (typ_terminal)
    while (lastterm != NULL)
    {
      if (lastterm->type == E_TERM) break;
      lastterm = EStackLookOn(stack_ptr, ++pos);
    }
    
    /**
    printf("Processing token %d\n",token);
    if (lastterm != NULL)
    {
      printf("Top nonterm %d\n", lastterm->val.i);
      }
    EStackPrintCont(stack_ptr);
    **/
    
    //vstupny token ...
    switch (token)
    
    /**
     *  V tomo switchy reprezentujucom riadky precedencnej
     *  tabulky sa rozhodujeme ktory TERMINAL JE NA VSTUPE
     *  v jednotlyvych blokoch sa dalej porovnava vrh zasobnika
     *  
     *
     *  treba povedat ze sa jedna o trochu nestastne riesenie
     *  pravdepodobne by som si usetril "zopar" riadkov kodu,
     *  keby som to implementoval cez pole a mapovacie funkcie                         
    **/
                       
    {
    
      /**
       *  Priradenie: 
       *  nizka priorita, vecsina sa redukuje
       *  ...       
       **/
                           
      case ASSIGN :
        ;
        /**
         *  zakladny pattern :
         *  prvy if - porovnanie lastterm oproti NULL
         *  => prazdny zasobnik aka $ v tabulke
        **/
                                           
        if ( lastterm == NULL  )
        {
              shiftOther(stack_ptr);
              if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ ); 
          
        } else
          switch (lastterm->val.i)
          /**
           *  neprazdny zasobnik
           *  obsahuje skupiny case
           *  1. shiftovanie
           *  2. redukovanie
           *    --redukovania su roztriedene nakolko vieme uz na zaklade vrch zasobnika pri re
           *    dukovani povedat ktore pravidlo pouzit
          **/                                                                            
          {
          
            //na vrchu bol terminal funkcie
            case LENGTH :
            case FIND :
            case SORT :
            case CONCAT :
                return printDesc( SYNTAX_ERR, "Rovna sa po funkcii" );                        
          
            // id redukcia...
            case ID :
              if ( ( errval = reduceId(stack_ptr) ) != OP_OK ) return errval; 
              break;
            
            //redukcia konstanty  
            case CONST_STR :
            case CONST_DBL :
            case CONST_INT :
            case B_INT_CON :
            case O_INT_CON : 
            case H_INT_CON :
              if ( ( errval = reduceConst(stack_ptr) ) != OP_OK ) return errval; 
              break;           
          
            //tu vyberame pozadovanu redukciu operacie
            //nazvy funkcii su myslim dostatocne zretelne....
            case ASSIGN :
              shiftOther(stack_ptr);
              if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
               break;          
               
            case RIGHT_BRACKET :
              if ( ( errval = reduceBrackets(stack_ptr)  ) != OP_OK ) return errval;
              break;
            
            case COLON : 
              shiftOther(stack_ptr);
              if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );              
              
            case LEFT_BRACKET :
              shiftOther(stack_ptr);
              if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
              break;                
                  
            case ADD :
              if ( ( errval = reduceAdd(stack_ptr) ) != OP_OK ) return errval;
              break;
                
            case SUBSTRACT :
              if ( ( errval = reduceSub(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case MULTIPLY :
              if ( ( errval = reduceMul(stack_ptr) ) != OP_OK ) return errval;
              break;

            case DIVIDE :
              if ( ( errval = reduceDiv(stack_ptr) ) != OP_OK ) return errval;
              break;
          
            case EQ :
              if ( ( errval = reduceEQ(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case NEQ :
              if ( ( errval = reduceNEQ(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case LT :
              if ( ( errval = reduceLT(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case LE :
              if ( ( errval = reduceLE(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case GT :
              if ( ( errval = reduceGT(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case GE :
              if ( ( errval = reduceGE(stack_ptr) ) != OP_OK ) return errval;
              break;        
          
        }
        break;    

      /**
       *  Lava zatvorka: 
       *  oznacuje zaciatok vyrazu alebo zoznamu parametrov tj.
       *  skoro vsetko sa shiftuje       
       **/

    
      case LEFT_BRACKET ://(
        ;
        if ( lastterm == NULL  )
        {
          shiftOther(stack_ptr);
          if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
        }
        else  
         switch (lastterm->val.i)
         {
            case ID :
            case CONST_STR :
            case CONST_DBL :
            case CONST_INT :
            case B_INT_CON :
            case O_INT_CON : 
            case H_INT_CON :
            case RIGHT_BRACKET :         
              return printDesc( SYNTAX_ERR, (char *) "( after ID or const or )" );  
              break;         
         
         
         
            //skoro vzdy shiftujeme alebo vzdy ?
            case COLON :
            case LEFT_BRACKET :        
            case ASSIGN :  
            case ADD :  
            case SUBSTRACT : 
            case MULTIPLY :
            case DIVIDE :
            case EQ :
            case NEQ :
            case LT :
            case LE :
            case GT : 
            case GE :
            case LENGTH :
            case FIND :
            case SORT :
            case CONCAT :
              shiftOther(stack_ptr);
              if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ ); //skontrolujeme
              break;         
         
         
         }  
        break;

      /**
       *  Prava zatvorka: 
       *  ) koniec volania alebo vyrazu skoro vsetko redukujeme
       **/

        
      case RIGHT_BRACKET :  // )
        ;
        if ( lastterm == NULL  )
        {
          return printDesc( SYNTAX_ERR, (char *) "Expression starting with )" ); 
        }
        else  
         switch (lastterm->val.i)
         {
         
            case LENGTH :
            case FIND :
            case SORT :
            case CONCAT :
              return printDesc( SYNTAX_ERR, (char *) "Expression starting with )" );           
            
            case ID :
              if ( ( errval = reduceId(stack_ptr) ) != OP_OK ) return errval; 
              break;
            
            //redukcia konstanty  
            case CONST_STR :
            case CONST_DBL :
            case CONST_INT :
            case B_INT_CON :
            case O_INT_CON : 
            case H_INT_CON :
              if ( ( errval = reduceConst(stack_ptr) ) != OP_OK ) return errval; 
              break;

            case RIGHT_BRACKET :
              if ( ( errval = reduceBrackets(stack_ptr)  ) != OP_OK ) return errval;
              break;
              
            case LEFT_BRACKET :
              shiftOther(stack_ptr);
              if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ ); 
              break; 
              
            case COLON : 
              shiftOther(stack_ptr);
              if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
              break;  
              
            case ASSIGN :
              if ( ( errval = reduceAssign(stack_ptr) ) != OP_OK ) return errval;
              break;               
              
            case ADD :
              if ( ( errval = reduceAdd(stack_ptr) ) != OP_OK ) return errval;
              break;
              
              
            case SUBSTRACT :
              if ( ( errval = reduceSub(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case MULTIPLY :
              if ( ( errval = reduceMul(stack_ptr) ) != OP_OK ) return errval;
              break;

            case DIVIDE :
              if ( ( errval = reduceDiv(stack_ptr) ) != OP_OK ) return errval;
              break;                       
 
            case EQ :
              if ( ( errval = reduceEQ(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case NEQ :
              if ( ( errval = reduceNEQ(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case LT :
              if ( ( errval = reduceLT(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case LE :
              if ( ( errval = reduceLE(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case GT :
              if ( ( errval = reduceGT(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case GE :
              if ( ( errval = reduceGE(stack_ptr) ) != OP_OK ) return errval;
              break;        
         
         }  
        break;

      /** Pricitanie:
       *  aritmeticka operacia s nizsou prioritou
      **/             

      case ADD :  // +
        ;
        if ( lastterm == NULL  )
        {
          shiftOther(stack_ptr);
          if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
        }
        else
          switch (lastterm->val.i)
          {
          
            case LENGTH :
            case FIND :
            case SORT :
            case CONCAT :
              return printDesc( SYNTAX_ERR, (char *) "Expression starting with )" );
              
              
            //SHIFT
            case COLON :
            case ASSIGN :
            case EQ :
            case NEQ :
            case LT :
            case LE :
            case GT :
            case GE :
            case LEFT_BRACKET :
              ;
              shiftOther(stack_ptr);
              if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ ); //skontrolujeme
              break; 
             
            // id redukcia...
            case ID :
              if ( ( errval = reduceId(stack_ptr) ) != OP_OK ) return errval; 
              break;
            
            //redukcia konstanty  
            case CONST_STR :
            case CONST_DBL :
            case CONST_INT :
            case B_INT_CON :
            case O_INT_CON : 
            case H_INT_CON :
              if ( ( errval = reduceConst(stack_ptr) ) != OP_OK ) return errval; 
              break;                          

            case RIGHT_BRACKET :
              if ( ( errval = reduceBrackets(stack_ptr)  ) != OP_OK ) return errval;
              break;
              
                                            
            case ADD :
              if ( ( errval = reduceAdd(stack_ptr) ) != OP_OK ) return errval;
              break;
              
  
            case SUBSTRACT :
              if ( ( errval = reduceSub(stack_ptr) ) != OP_OK ) return errval;
              break;              
              
              
            case MULTIPLY :
              if ( ( errval = reduceMul(stack_ptr) ) != OP_OK ) return errval;
              break;              
              

            case DIVIDE :
              if ( ( errval = reduceDiv(stack_ptr) ) != OP_OK ) return errval;
              break;
          
          }
          break;
          
      //minusko...
      case SUBSTRACT :  // -
        ;
        if ( lastterm == NULL  )
        {
          shiftOther(stack_ptr);
          if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
        }
        else
          switch (lastterm->val.i)
          {
          
            case LENGTH :
            case FIND :
            case SORT :
            case CONCAT :
              return printDesc( SYNTAX_ERR, (char *) "Expression starting with )" );
              
              
            //SHIFT
            case COLON :
            case ASSIGN :
            case EQ :
            case NEQ :
            case LT :
            case LE :
            case GT :
            case GE :
            case LEFT_BRACKET :
              ;
              shiftOther(stack_ptr);
              if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ ); //skontrolujeme
              break; 
             
            // id redukcia...
            case ID :
              if ( ( errval = reduceId(stack_ptr) ) != OP_OK ) return errval; 
              break;
            
            //redukcia konstanty  
            case CONST_STR :
            case CONST_DBL :
            case CONST_INT :
            case B_INT_CON :
            case O_INT_CON : 
            case H_INT_CON :
              if ( ( errval = reduceConst(stack_ptr) ) != OP_OK ) return errval; 
              break;                          

            case RIGHT_BRACKET :
              if ( ( errval = reduceBrackets(stack_ptr)  ) != OP_OK ) return errval;
              break;
              
                                            
            case ADD :
              if ( ( errval = reduceAdd(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case SUBSTRACT :
              if ( ( errval = reduceSub(stack_ptr) ) != OP_OK ) return errval;
              break;              
              
              
            case MULTIPLY :
              if ( ( errval = reduceMul(stack_ptr) ) != OP_OK ) return errval;
              break;              
              

            case DIVIDE :
              if ( ( errval = reduceDiv(stack_ptr) ) != OP_OK ) return errval;
              break;
          
          }
          break;
 
      //krat 
      case MULTIPLY :  // *
        ;
        if ( lastterm == NULL  )
        {
          shiftOther(stack_ptr);
          if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
        }
        else
          switch (lastterm->val.i)
          {
          
            case LENGTH :
            case FIND :
            case SORT :
            case CONCAT :
              return printDesc( SYNTAX_ERR, (char *) "Excepted left bracket" );
              
              
            //SHIFT
            case COLON :
            case ASSIGN :
            case EQ :
            case NEQ :
            case LT :
            case LE :
            case GT :
            case GE :
            case LEFT_BRACKET :
              ;
              shiftOther(stack_ptr);
              if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
              break; 
             
            // id redukcia...
            case ID :
              if ( ( errval = reduceId(stack_ptr) ) != OP_OK ) return errval; 
              break;
            
            //redukcia konstanty  
            case CONST_STR :
            case CONST_DBL :
            case CONST_INT :
            case B_INT_CON :
            case O_INT_CON : 
            case H_INT_CON :
              if ( ( errval = reduceConst(stack_ptr) ) != OP_OK ) return errval; 
              break;                          

            case RIGHT_BRACKET :
              if ( ( errval = reduceBrackets(stack_ptr)  ) != OP_OK ) return errval;
              break;
              
                                            
         
            case ADD :
              shiftOther(stack_ptr);
              if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
              break;    
            
            case SUBSTRACT :
              shiftOther(stack_ptr);
              if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );              
              break;
              
            case MULTIPLY :
              if ( ( errval = reduceMul(stack_ptr) ) != OP_OK ) return errval;
              break;              
              

            case DIVIDE :
              if ( ( errval = reduceDiv(stack_ptr) ) != OP_OK ) return errval;
              break;
          
          }
          break; 
      
      //delenie...
      case DIVIDE :  // /
        ;
        if ( lastterm == NULL  )
        {
          shiftOther(stack_ptr);
          if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ ); 
        }
        else
          switch (lastterm->val.i)
          {
          
            case LENGTH :
            case FIND :
            case SORT :
            case CONCAT :
              return printDesc( SYNTAX_ERR, (char *) "Expression starting with )" );
              
              
            //SHIFT
            case COLON :
            case ASSIGN :
            case EQ :
            case NEQ :
            case LT :
            case LE :
            case GT :
            case GE :
            case LEFT_BRACKET :
              ;
              shiftOther(stack_ptr);
              if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ ); //skontrolujeme
              break; 
             
            // id redukcia...
            case ID :
              if ( ( errval = reduceId(stack_ptr) ) != OP_OK ) return errval; 
              break;
            
            //redukcia konstanty  
            case CONST_STR :
            case CONST_DBL :
            case CONST_INT :
            case B_INT_CON :
            case O_INT_CON : 
            case H_INT_CON :
              if ( ( errval = reduceConst(stack_ptr) ) != OP_OK ) return errval; 
              break;                          

            case RIGHT_BRACKET :
              if ( ( errval = reduceBrackets(stack_ptr)  ) != OP_OK ) return errval;
              break;
              
 
            case ADD :
              shiftOther(stack_ptr);
              if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
              break; 
                    
            case SUBSTRACT :
              shiftOther(stack_ptr);
              if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );              
              break;
              
            case MULTIPLY :
              if ( ( errval = reduceMul(stack_ptr) ) != OP_OK ) return errval;
              break;              
              

            case DIVIDE :
              if ( ( errval = reduceDiv(stack_ptr) ) != OP_OK ) return errval;
              break;
          
          }
          break;

      
      //Greater Than...
      case GT :  // +
        ;
        if ( lastterm == NULL  )
        {
          shiftOther(stack_ptr);
          if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ ); 
        }
        else
          switch (lastterm->val.i)
          {
            //SHIFT

            case LENGTH :
            case FIND :
            case SORT :
            case CONCAT :
              return printDesc( SYNTAX_ERR, (char *) "Expression starting with )" );

            case COLON :
            case EQ :
            case NEQ :            
            case ASSIGN :
            case LEFT_BRACKET :
              ;
              shiftOther(stack_ptr);
              if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ ); 
              break; 
             
            // id redukcia...
            case ID :
              if ( ( errval = reduceId(stack_ptr) ) != OP_OK ) return errval; 
              break;
            
            //redukcia konstanty  
            case CONST_STR :
            case CONST_DBL :
            case CONST_INT :
            case B_INT_CON :
            case O_INT_CON : 
            case H_INT_CON :
              if ( ( errval = reduceConst(stack_ptr) ) != OP_OK ) return errval; 
              break;                            

            case RIGHT_BRACKET :
              if ( ( errval = reduceBrackets(stack_ptr)  ) != OP_OK ) return errval;
              break;

                                             
            case ADD :
              if ( ( errval = reduceAdd(stack_ptr) ) != OP_OK ) return errval;
              break;
              

            case SUBSTRACT :
              if ( ( errval = reduceSub(stack_ptr) ) != OP_OK ) return errval;
              break;              
              
              
            case MULTIPLY :
              if ( ( errval = reduceMul(stack_ptr) ) != OP_OK ) return errval;
              break;              
              

            case DIVIDE :
              if ( ( errval = reduceDiv(stack_ptr) ) != OP_OK ) return errval;
              break;

              
            case LT :
              if ( ( errval = reduceLT(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case LE :
              if ( ( errval = reduceLE(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case GT :
              if ( ( errval = reduceGT(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case GE :
              if ( ( errval = reduceGE(stack_ptr) ) != OP_OK ) return errval;
              break; 
         
          }
          break;    

      //Greater Equal...
      case GE :  // >=
        ;
        if ( lastterm == NULL  )
        {
          shiftOther(stack_ptr);
          if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ ); 
        }
        else
          switch (lastterm->val.i)
          {
            //SHIFT

            case LENGTH :
            case FIND :
            case SORT :
            case CONCAT :
              return printDesc( SYNTAX_ERR, (char *) "Expression starting with )" );

            case COLON :
            case EQ :
            case NEQ :            
            case ASSIGN :
            case LEFT_BRACKET :
              ;
              shiftOther(stack_ptr);
              if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ ); 
              break; 
             
            // id redukcia...
            case ID :
              if ( ( errval = reduceId(stack_ptr) ) != OP_OK ) return errval; 
              break;
            
            //redukcia konstanty  
            case CONST_STR :
            case CONST_DBL :
            case CONST_INT :
            case B_INT_CON :
            case O_INT_CON : 
            case H_INT_CON :
              if ( ( errval = reduceConst(stack_ptr) ) != OP_OK ) return errval; 
              break;                            

            case RIGHT_BRACKET :
              if ( ( errval = reduceBrackets(stack_ptr)  ) != OP_OK ) return errval;
              break;

                                            
            case ADD :
              if ( ( errval = reduceAdd(stack_ptr) ) != OP_OK ) return errval;
              break;
                
            case SUBSTRACT :
              if ( ( errval = reduceSub(stack_ptr) ) != OP_OK ) return errval;
              break;              
              
              
            case MULTIPLY :
              if ( ( errval = reduceMul(stack_ptr) ) != OP_OK ) return errval;
              break;              
              

            case DIVIDE :
              if ( ( errval = reduceDiv(stack_ptr) ) != OP_OK ) return errval;
              break;

              
            case LT :
              if ( ( errval = reduceLT(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case LE :
              if ( ( errval = reduceLE(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case GT :
              if ( ( errval = reduceGT(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case GE :
              if ( ( errval = reduceGE(stack_ptr) ) != OP_OK ) return errval;
              break; 
         
          }
          break;

      //Lesser Than...
      case LT :  // <
        ;
        if ( lastterm == NULL  )
        {
          shiftOther(stack_ptr);
          if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ ); 
        }
        else
          switch (lastterm->val.i)
          {
            //SHIFT

            case LENGTH :
            case FIND :
            case SORT :
            case CONCAT :
              return printDesc( SYNTAX_ERR, (char *) "Expression starting with )" );

            case COLON :
            case EQ :
            case NEQ :            
            case ASSIGN :
            case LEFT_BRACKET :
              ;
              shiftOther(stack_ptr);
              if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ ); //skontrolujeme
              break; 
             
            // id redukcia...
            case ID :
              if ( ( errval = reduceId(stack_ptr) ) != OP_OK ) return errval; 
              break;
            
            //redukcia konstanty  
            case CONST_STR :
            case CONST_DBL :
            case CONST_INT :
            case B_INT_CON :
            case O_INT_CON : 
            case H_INT_CON :
              if ( ( errval = reduceConst(stack_ptr) ) != OP_OK ) return errval; 
              break;                            

            case RIGHT_BRACKET :
              if ( ( errval = reduceBrackets(stack_ptr)  ) != OP_OK ) return errval;
              break;

                                            
            // + na + redukcia  
            case ADD :
              if ( ( errval = reduceAdd(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            // + na + redukcia  
            case SUBSTRACT :
              if ( ( errval = reduceSub(stack_ptr) ) != OP_OK ) return errval;
              break;              
              
              
            case MULTIPLY :
              if ( ( errval = reduceMul(stack_ptr) ) != OP_OK ) return errval;
              break;              
              

            case DIVIDE :
              if ( ( errval = reduceDiv(stack_ptr) ) != OP_OK ) return errval;
              break;

              
            case LT :
              if ( ( errval = reduceLT(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case LE :
              if ( ( errval = reduceLE(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case GT :
              if ( ( errval = reduceGT(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case GE :
              if ( ( errval = reduceGE(stack_ptr) ) != OP_OK ) return errval;
              break; 
         
          }
          break;

      //Less or Equal...
      case LE :  // +
        ;
        if ( lastterm == NULL  )
        {
          shiftOther(stack_ptr);
          if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ ); //skontrolujeme
        }
        else
          switch (lastterm->val.i)
          {
            //SHIFT

            case LENGTH :
            case FIND :
            case SORT :
            case CONCAT :
              return printDesc( SYNTAX_ERR, (char *) "Expression starting with )" );

            case COLON :
            case EQ :
            case NEQ :            
            case ASSIGN :
            case LEFT_BRACKET :
              ;
              shiftOther(stack_ptr);
              if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ ); //skontrolujeme
              break; 
             
            // id redukcia...
            case ID :
              if ( ( errval = reduceId(stack_ptr) ) != OP_OK ) return errval; 
              break;
            
            //redukcia konstanty  
            case CONST_STR :
            case CONST_DBL :
            case CONST_INT :
            case B_INT_CON :
            case O_INT_CON : 
            case H_INT_CON :
              if ( ( errval = reduceConst(stack_ptr) ) != OP_OK ) return errval; 
              break;                            

            case RIGHT_BRACKET :
              if ( ( errval = reduceBrackets(stack_ptr)  ) != OP_OK ) return errval;
              break;

                                             
            case ADD :
              if ( ( errval = reduceAdd(stack_ptr) ) != OP_OK ) return errval;
              break;
               
            case SUBSTRACT :
              if ( ( errval = reduceSub(stack_ptr) ) != OP_OK ) return errval;
              break;              
              
              
            case MULTIPLY :
              if ( ( errval = reduceMul(stack_ptr) ) != OP_OK ) return errval;
              break;              
              

            case DIVIDE :
              if ( ( errval = reduceDiv(stack_ptr) ) != OP_OK ) return errval;
              break;

              
            case LT :
              if ( ( errval = reduceLT(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case LE :
              if ( ( errval = reduceLE(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case GT :
              if ( ( errval = reduceGT(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case GE :
              if ( ( errval = reduceGE(stack_ptr) ) != OP_OK ) return errval;
              break; 
         
          }
          break;


      // Equal...
      case EQ :  // ==
        ;
        if ( lastterm == NULL  )
        {
          shiftOther(stack_ptr);
          if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ ); //skontrolujeme
        }
        else
          switch (lastterm->val.i)
          {
            //SHIFT

            case LENGTH :
            case FIND :
            case SORT :
            case CONCAT :
              return printDesc( SYNTAX_ERR, (char *) "Expression starting with )" );

            case COLON :            
            case ASSIGN :
            case LEFT_BRACKET :
              ;
              shiftOther(stack_ptr);
              if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ ); //skontrolujeme
              break; 
             
            // id redukcia...
            case ID :
              if ( ( errval = reduceId(stack_ptr) ) != OP_OK ) return errval; 
              break;
            
            //redukcia konstanty  
            case CONST_STR :
            case CONST_DBL :
            case CONST_INT :
            case B_INT_CON :
            case O_INT_CON : 
            case H_INT_CON :
              if ( ( errval = reduceConst(stack_ptr) ) != OP_OK ) return errval; 
              break;                            

            case RIGHT_BRACKET :
              if ( ( errval = reduceBrackets(stack_ptr)  ) != OP_OK ) return errval;
              break;

                                            
            // + na + redukcia  
            case ADD :
              if ( ( errval = reduceAdd(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            // + na + redukcia  
            case SUBSTRACT :
              if ( ( errval = reduceSub(stack_ptr) ) != OP_OK ) return errval;
              break;              
              
              
            case MULTIPLY :
              if ( ( errval = reduceMul(stack_ptr) ) != OP_OK ) return errval;
              break;              
              

            case DIVIDE :
              if ( ( errval = reduceDiv(stack_ptr) ) != OP_OK ) return errval;
              break;

            case EQ :
              if ( ( errval = reduceEQ(stack_ptr) ) != OP_OK ) return errval;
              break;
                            
            case NEQ :
              if ( ( errval = reduceNEQ(stack_ptr) ) != OP_OK ) return errval;
              break;
                            
            case LT :
              if ( ( errval = reduceLT(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case LE :
              if ( ( errval = reduceLE(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case GT :
              if ( ( errval = reduceGT(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case GE :
              if ( ( errval = reduceGE(stack_ptr) ) != OP_OK ) return errval;
              break; 
         
          }
          break;

       //NotEqual...
       case NEQ :  // !=
        ;
        if ( lastterm == NULL  )
        {
          shiftOther(stack_ptr);
          if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ ); //skontrolujeme
        }
        else
          switch (lastterm->val.i)
          {
            //SHIFT

            case LENGTH :
            case FIND :
            case SORT :
            case CONCAT :
              return printDesc( SYNTAX_ERR, (char *) "Expression starting with )" );

            case COLON :            
            case ASSIGN :
            case LEFT_BRACKET :
              ;
              shiftOther(stack_ptr);
              if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ ); //skontrolujeme
              break; 
             
            // id redukcia...
            case ID :
              if ( ( errval = reduceId(stack_ptr) ) != OP_OK ) return errval; 
              break;
            
            //redukcia konstanty  
            case CONST_STR :
            case CONST_DBL :
            case CONST_INT :
            case B_INT_CON :
            case O_INT_CON : 
            case H_INT_CON :
              if ( ( errval = reduceConst(stack_ptr) ) != OP_OK ) return errval; 
              break;                            

            case RIGHT_BRACKET :
              if ( ( errval = reduceBrackets(stack_ptr)  ) != OP_OK ) return errval;
              break;

                                            
            // + na + redukcia  
            case ADD :
              if ( ( errval = reduceAdd(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            // + na + redukcia  
            case SUBSTRACT :
              if ( ( errval = reduceSub(stack_ptr) ) != OP_OK ) return errval;
              break;              
              
              
            case MULTIPLY :
              if ( ( errval = reduceMul(stack_ptr) ) != OP_OK ) return errval;
              break;              
              

            case DIVIDE :
              if ( ( errval = reduceDiv(stack_ptr) ) != OP_OK ) return errval;
              break;

            case EQ :
              if ( ( errval = reduceEQ(stack_ptr) ) != OP_OK ) return errval;
              break;
                            
            case NEQ :
              if ( ( errval = reduceNEQ(stack_ptr) ) != OP_OK ) return errval;
              break;
                            
            case LT :
              if ( ( errval = reduceLT(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case LE :
              if ( ( errval = reduceLE(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case GT :
              if ( ( errval = reduceGT(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case GE :
              if ( ( errval = reduceGE(stack_ptr) ) != OP_OK ) return errval;
              break; 
         
          }
          break;


                 
      //IDENTIFIKATOR
      case ID : 
        ;
        if ( lastterm == NULL  )
        {
        
          shiftId(stack_ptr);
          if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ ); //skontrolujeme
        
        }
        else
          switch (lastterm->val.i)
          {
            //ERRORS
            case CONST_STR :
            case CONST_DBL :
            case CONST_INT :
            case B_INT_CON :
            case O_INT_CON : 
            case H_INT_CON :
            case ID :
            case LENGTH :
            case FIND :
            case SORT :
            case CONCAT :
            case RIGHT_BRACKET :
            ;
              return printDesc( SYNTAX_ERR, "Unexpected token after identifier..." );   
              
            case LEFT_BRACKET :
              shiftId(stack_ptr);
              if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
              break;             
              
            default :
            ;
            shiftId(stack_ptr);
            if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ ); //skontrolujeme
            break;
          
          }
          break;

      //FUNKCIE
      case LENGTH:
      case SORT:
      case FIND:
      case CONCAT: 
        ;
        if ( lastterm == NULL  )
        {
          shiftOther(stack_ptr);
          if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ ); //skontrolujeme
        
        }
        else
          switch (lastterm->val.i)
          {
            //ERRORS
            case CONST_STR :
            case CONST_DBL :
            case CONST_INT :
            case B_INT_CON :
            case O_INT_CON : 
            case H_INT_CON :
            case ID :
            case LENGTH :
            case FIND :
            case SORT :
            case CONCAT :
            case RIGHT_BRACKET :
            ;
              return printDesc( SYNTAX_ERR, "Unexpected token after identifier..." );               
              
            default :
            ;
            shiftOther(stack_ptr);
            if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ ); //skontrolujeme
            break;
          
          }
          break;       

      case COLON:
        ;
        if ( lastterm == NULL  )
        {
        
          return printDesc( SYNTAX_ERR, "Colon on empty stack..." );  
        
        }
        else
          switch (lastterm->val.i)
          {
            case LENGTH:
            case SORT:
            case FIND:
            case CONCAT: 
              return printDesc( SYNTAX_ERR, "Colon after fncname..." ); 
              
                                       
            // id redukcia...
            case ID :
              if ( ( errval = reduceId(stack_ptr) ) != OP_OK ) return errval; 
              break;
            
            //redukcia konstanty  
            case CONST_STR :
            case CONST_DBL :
            case CONST_INT :
            case B_INT_CON :
            case O_INT_CON : 
            case H_INT_CON :
              if ( ( errval = reduceConst(stack_ptr) ) != OP_OK ) return errval; 
              break;
   
              
            case RIGHT_BRACKET :
              if ( ( errval = reduceBrackets(stack_ptr)  ) != OP_OK ) return errval;
              break;
            
            case COLON :  
            case LEFT_BRACKET :
              shiftOther(stack_ptr);
              if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ ); //skontrolujeme
              break;
              
            case ASSIGN :
              if ( ( errval = reduceAssign(stack_ptr)  ) != OP_OK ) return errval;
              break;
            
            case ADD :
              if ( ( errval = reduceAdd(stack_ptr) ) != OP_OK ) return errval;
              break;
              
       
            case SUBSTRACT :
              if ( ( errval = reduceSub(stack_ptr) ) != OP_OK ) return errval;
              break;              
              
              
            case MULTIPLY :
              if ( ( errval = reduceMul(stack_ptr) ) != OP_OK ) return errval;
              break;              
              

            case DIVIDE :
              if ( ( errval = reduceDiv(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case EQ :
              if ( ( errval = reduceEQ(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case NEQ :
              if ( ( errval = reduceNEQ(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case LT :
              if ( ( errval = reduceLT(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case LE :
              if ( ( errval = reduceLE(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case GT :
              if ( ( errval = reduceGT(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case GE :
              if ( ( errval = reduceGE(stack_ptr) ) != OP_OK ) return errval;
              break;
            
                           
          
          }
          break;

       
      // Prisla konstanta  
      case CONST_STR :
      case CONST_DBL :
      case CONST_INT :
      case B_INT_CON :
      case O_INT_CON : 
      case H_INT_CON :
        ;
        if ( lastterm == NULL  )
        {
        
          shiftConst(stack_ptr);
          if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ ); //skontrolujeme
        
        }
        else
          switch (lastterm->val.i)
          {
            //ERRORS
            case CONST_STR :
            case CONST_DBL :
            case CONST_INT :
            case B_INT_CON :
            case O_INT_CON : 
            case H_INT_CON :
            case ID :
            case LENGTH :
            case FIND :
            case SORT :
            case CONCAT :
            case RIGHT_BRACKET :
            ;
              return printDesc( SYNTAX_ERR, "Unexpected token after identifier..." );   
                           
             
             
              
            default :
            ;
            shiftConst(stack_ptr);
            if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ ); //skontrolujeme
            break;
          
          }
          break;

      
      case OUT:  
      case LEFT_VINCULUM:
      case SEMICOLON:
        if (lastterm == NULL)
        {
          if ( EStackTop(stack_ptr) == NULL )
            return printDesc( SYNTAX_ERR, "Unexpected token at end of expression" );
            
          //na zasobniku je 1 neterminal <expr> inac nic a na vstupe je ; alebo { --jedine platne tokeny konca vyrazu
          //printf("posledny prevod a popnutia\n");
          exprToTableEntry( EStackTop(stack_ptr) , retval);    //naladuje prislusne hodnoty do navratoveho symbolu   
          EStackFree(stack_ptr);    
          //printf("vynorenie...\n");
          return PARSE_OK;       
        } else
          switch (lastterm->val.i)
          {
            case ASSIGN :
              if ( ( errval = reduceAssign(stack_ptr) ) != OP_OK ) return errval;
              break;
            
            case RIGHT_BRACKET :
              if ( ( errval = reduceBrackets(stack_ptr)  ) != OP_OK ) return errval;
              break;
            
            case COLON :
            case LENGTH :
            case FIND :
            case SORT :
            case CONCAT :              
            case LEFT_BRACKET :
              return printDesc( SYNTAX_ERR, "Unexpected end of expression" ); 
              break;
                        
            case MULTIPLY :
              if ( ( errval = reduceMul(stack_ptr) ) != OP_OK ) return errval;
              break;           
          
            case ADD :
              if ( ( errval = reduceAdd(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case SUBSTRACT :
              if ( ( errval = reduceSub(stack_ptr) ) != OP_OK ) return errval;
              break;           
          
            case DIVIDE :
              if ( ( errval = reduceDiv(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case EQ :
              if ( ( errval = reduceEQ(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case NEQ :
              if ( ( errval = reduceNEQ(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case LT :
              if ( ( errval = reduceLT(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case LE :
              if ( ( errval = reduceLE(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case GT :
              if ( ( errval = reduceGT(stack_ptr) ) != OP_OK ) return errval;
              break;
              
            case GE :
              if ( ( errval = reduceGE(stack_ptr) ) != OP_OK ) return errval;
              break;
                          
            case ID :
              if ( ( errval = reduceId(stack_ptr) ) != OP_OK ) return errval; 
              break;
            
            //redukcia konstanty  
            case CONST_STR :
            case CONST_DBL :
            case CONST_INT :
            case B_INT_CON :
            case O_INT_CON : 
            case H_INT_CON :
              if ( ( errval = reduceConst(stack_ptr) ) != OP_OK ) return errval; 
              break;
            
            default : 
              return printDesc( SYNTAX_ERR, "Unexpected token at end of expression" ); 
          }
          break;
               
            
      default :
        return printDesc( SYNTAX_ERR, "Unexpected token on input" );
    }
    
    
  }    
  
  
  
  EStackFree(stack_ptr);
  return PARSE_OK;
}

// <-----------------------------------------XXXX SHIFTOVACIE FUNKCIE XXXXX-------------------------------->

int
shiftOther(ptr_EStack stack_ptr)
{
  //printf("Shifting term...\n");
  expr newexid;
  newexid.type = E_TERM;
  newexid.val.i = token;
  newexid.attr = NULL;
  EStackPush(stack_ptr, &newexid);
  return OP_OK;

}

//vlozi na zasobnik identifikator - kopiruje sa attribut
int
shiftId(ptr_EStack stack_ptr)
{
  //printf("Shifting id...\n");
  expr newexid;
  newexid.type = E_TERM;
  newexid.val.i = token;
  newexid.attr = strNewFromConstChar(strGetStr(&attr));
  EStackPush(stack_ptr, &newexid);
  return OP_OK;
}

//vlozi konstantu
int
shiftConst(ptr_EStack stack_ptr)
{
  //printf("Shifting const...\n");
  expr newexcon;
  newexcon.type = E_TERM;
  newexcon.val.i = token;
  newexcon.attr = strNewFromConstChar(strGetStr(&attr));  
  EStackPush(stack_ptr, &newexcon);
  return OP_OK;
}

// <-----------------------------------------XXXX REDUKCNE FUNKCIE XXXXX-------------------------------->

//zmeni identifikator na <expr>
int
reduceId(ptr_EStack stack_ptr)
{
  
  expr_ptr idterm = EStackTop(stack_ptr);
  if ( idterm == NULL || idterm->type != E_TERM || idterm->val.i != ID)
    return printDesc( SEMANTIC_ERR, "Wrong rule" );
      
  expr newexid;
  newexid.type = E_ID;        
  if ( (newexid.val.id = bvs_lookup (table, idterm->attr)) == NULL ) return printDesc( SEMANTIC_ERR, "Undeclared variabile" );
  EStackPop(stack_ptr);
  EStackPush(stack_ptr, &newexid);
  return OP_OK;
  
}

int
reduceConst(ptr_EStack stack_ptr)
{
  //zredukuje konstantu a na zasobnik vlozi <expr> reprezentujuci const

  expr_ptr idterm = EStackTop(stack_ptr);
  if ( idterm == NULL || idterm->type != E_TERM )
    return printDesc( SEMANTIC_ERR, "Wrong rule" );
    
  expr newexcon;

  if ( idterm->val.i == CONST_STR )
  {
    newexcon.type = E_STR;
    newexcon.val.str = strNewFromConstChar(strGetStr(idterm->attr));
  } else
  if ( idterm->val.i == CONST_DBL )
  {
    newexcon.type = E_DBL;
    newexcon.val.dbl = parseDbl(idterm->attr);
  } else
  if ( idterm->val.i == CONST_INT )
  {
  newexcon.type = E_INT;
  int tmp;
  parseNum( idterm->attr, &tmp, 10);
  newexcon.val.i = tmp;     
  } else
  switch (idterm->val.i)
  {
    case B_INT_CON :
    ;
    newexcon.type = E_INT;
    int tmpb;
    parseNum( idterm->attr, &tmpb, 2);
    newexcon.val.i = tmpb;     
    break;  
                
    case O_INT_CON :
    ;
    newexcon.type = E_INT;
    int tmpo;
    parseNum( idterm->attr, &tmpo, 8);
    newexcon.val.i = tmpo;   
    break; 
                
    case H_INT_CON :
    ;
    newexcon.type = E_INT;
    int tmph;
    parseNum( idterm->attr, &tmph, 16);
    newexcon.val.i = tmph;     
    break; 
                      
  }
  //printf("Reducing const DONE...\n");  
  EStackPop(stack_ptr);
  EStackPush(stack_ptr, &newexcon);
  return OP_OK;
}

int
reduceBrackets(ptr_EStack stack_ptr)
{
  //printf("Reducing brackets or call...\n");  
  
  expr_ptr e1 = EStackLookOn(stack_ptr, 1); // E
  expr_ptr e2 = EStackLookOn(stack_ptr, 2); // ( alebo ,
  expr_ptr e3 = EStackLookOn(stack_ptr, 3); // fnccall , E
  expr_ptr e4 = EStackLookOn(stack_ptr, 4); // ( alebo ,
  expr_ptr e5 = EStackLookOn(stack_ptr, 5); // fnccall , E
   
  if ( e1 == NULL || e2 == NULL )
    return printDesc( SYNTAX_ERR, (char *) "Operator excepted" ); 

  //podmienka zatvorky ako zatvorky nie ako volanie
  int trigger1 = (e1->type != E_TERM) && (e2->type==E_TERM) && (e2->val.i ==LEFT_BRACKET);
  trigger1 = (trigger1) && ((e3==NULL) || (e3->type != E_TERM) || ( e3->type== E_TERM && !( e3->val.i == LENGTH || e3->val.i == SORT)));

  if (trigger1)
  {
      expr newexpr;
      newexpr.type = e1->type;
      if (newexpr.type == E_STR)
      {
        newexpr.val.str = strNewFromConstChar(strGetStr(e1->val.str));
      }
      else
       newexpr.val = e1->val;       
        
      EStackPop(stack_ptr); //odmazeme (E)
      EStackPop(stack_ptr);
      EStackPop(stack_ptr);
      EStackPush(stack_ptr, &newexpr); //pushneme E    
  
  }
  else
  if ( (e1->type != E_TERM) && (e2->type==E_TERM) ) //volania
  {
    //teoreticky volania
    if ( (e2->val.i == LEFT_BRACKET) && (e3 != NULL) && ( e3->type == E_TERM ) && ( e3->val.i == SORT || e3->val.i == LENGTH) )
    {
      //sort alebo length E musi byt typu str
      if ( e1->type == E_STR || ( (e1->type== E_ID) && (e1->val.id->type==STR_TYPE ) ) )
      {
        if ( e3->val.i == SORT )  //volame sort
        {
          expr newexpr;


          if ( e1->type == E_STR )
          {
            newexpr.type = E_STR;
            newexpr.val.str = sort(e1->val.str);          
            //emitInstr(ilist,I_SORT, I_STR, e1->val.str, 0, null, newexpr.val.id);
          }
          else
          {
            newexpr.type = E_ID;
            newexpr.val.id = newVariabile(STR_TYPE);
            newexpr.val.id->value.str=newStr();                                  //inicializovat alebo nie ?
            emitInstr(ilist,I_SORT, TABLEENTRY_OP_TYPE, &(e1->val.id), NO_OP, NULL, TABLEENTRY_OP_TYPE, &(newexpr.val.id) );
          }
          //printf("Zatvorky ako volanie SORT\n");
          EStackPop(stack_ptr); //odmazeme sort(E)
          EStackPop(stack_ptr);
          EStackPop(stack_ptr);
          EStackPop(stack_ptr);
          EStackPush(stack_ptr, &newexpr); //pushneme E           
        } else
        {
          expr newexpr;


          if ( e1->type == E_STR )
          {
            newexpr.type = E_INT;
            newexpr.val.i = length(e1->val.str);
          }
          else
          {
            newexpr.type = E_ID;
            newexpr.val.id = newVariabile(STR_TYPE);
            emitInstr(ilist,I_LEN, TABLEENTRY_OP_TYPE, &(e1->val.id), NO_OP, NULL, TABLEENTRY_OP_TYPE, &(newexpr.val.id) );
          }
          //printf("Zatvorky ako volanie LENGTH\n");        
          EStackPop(stack_ptr); //odmazeme length(E)
          EStackPop(stack_ptr);
          EStackPop(stack_ptr);
          EStackPop(stack_ptr);
          EStackPush(stack_ptr, &newexpr); //pushneme E  
        }        
      } else
          return printDesc( SEMANTIC_ERR, (char *) "Wrong type for function call" );
    
    }
    else     //find a concat
    if ( (e2->val.i == COLON) && ( e3->type != E_TERM ) && ( e4 != NULL ) && (e4->type == E_TERM) && (e4->val.i == LEFT_BRACKET)) //(E,E)
    {
      if ( (e5 != NULL) && (e5->type == E_TERM) && ( e5->val.i == FIND || e5->val.i == CONCAT ) )
      //find(E,E);concat(E,E)
      {
         if ( e1->type == E_STR || ( (e1->type== E_ID) && (e1->val.id->type==STR_TYPE ) ) )
         {
          if ( e3->type == E_STR || ( (e3->type== E_ID) && (e3->val.id->type==STR_TYPE ) ) )
          {
            
            
            
            if ( e5->val.i == FIND )  //volame sort
            {
              expr newexpr;
    
              if ( e1->type == E_STR && e3->type == E_STR )
              {
                //printf("volam find nad dvoma konstantami hodnoty:\n%s\n%s\n", strGetStr(e3->val.str),strGetStr(e1->val.str));
                newexpr.type = E_INT;
                newexpr.val.i = find(e3->val.str, e1->val.str);
                //printf("hotovo vysldok %d\n",newexpr.val.i );
                //emitInstr(I_FIND, I_STR, e1->val.str, 0, null, newexpr.val.id);
              }
              else
              if ( e1->type == E_ID && e3->type == E_STR )
              {
                newexpr.type = E_ID;
                newexpr.val.id = newVariabile(INT_TYPE);
                emitInstr(ilist,I_FIND, TABLEENTRY_OP_TYPE, &(e1->val.id), STR_OP_TYPE, &(e3->val.str), TABLEENTRY_OP_TYPE, &(newexpr.val.id) );
              }
              else
              if ( e1->type == E_STR && e3->type == E_ID )
              {
                newexpr.type = E_ID;
                newexpr.val.id = newVariabile(INT_TYPE);
                emitInstr(ilist,I_FIND, STR_OP_TYPE, &(e1->val.str),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newexpr.val.id) );
              }
              else
              if ( e1->type == E_ID && e3->type == E_ID )
              {
                newexpr.type = E_ID;
                newexpr.val.id = newVariabile(INT_TYPE);
                emitInstr(ilist,I_FIND, TABLEENTRY_OP_TYPE, &(e1->val.id), TABLEENTRY_OP_TYPE, &(e3->val.id), TABLEENTRY_OP_TYPE, &(newexpr.val.id) );
              }

              //printf("Zatvorky ako volanie FIND\n");
                                        
              EStackPop(stack_ptr); //odmazeme find(E,E)
              EStackPop(stack_ptr);
              EStackPop(stack_ptr);
              EStackPop(stack_ptr);
              EStackPop(stack_ptr);
              EStackPop(stack_ptr);
              //printf("Popnute idem pushovat\n");
              EStackPush(stack_ptr, &newexpr); //pushneme E  
              //printf("Pushnute...\n");         
            } else
            {
              expr newexpr;
    
              if ( e1->type == E_STR && e3->type == E_STR )
              {
                newexpr.type = E_STR;
                newexpr.val.str = concat(e3->val.str, e1->val.str);
                //emitInstr(I_CONCAT, I_STR, e1->val.str, 0, null, newexpr.val.id);
              }
              else
              if ( e1->type == E_ID && e3->type == E_STR )
              {
                newexpr.type = E_ID;
                newexpr.val.id = newVariabile(STR_TYPE);
                emitInstr(ilist,I_CONCAT, TABLEENTRY_OP_TYPE, &(e1->val.id), STR_OP_TYPE, &(e3->val.str), TABLEENTRY_OP_TYPE, &(newexpr.val.id) );
              }
              else
              if ( e1->type == E_STR && e3->type == E_ID )
              {
                newexpr.type = E_ID;
                newexpr.val.id = newVariabile(STR_TYPE);              
                emitInstr(ilist,I_CONCAT, STR_OP_TYPE, &(e1->val.str),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newexpr.val.id) );
              }
              else
              if ( e1->type == E_ID && e3->type == E_ID )
              {
                newexpr.type = E_ID;
                newexpr.val.id = newVariabile(STR_TYPE);
                emitInstr(ilist,I_CONCAT, TABLEENTRY_OP_TYPE, &(e1->val.id), TABLEENTRY_OP_TYPE, &(e3->val.id), TABLEENTRY_OP_TYPE, &(newexpr.val.id) );
              }

              //printf("Zatvorky ako volanie CONCAT\n");
                                        
              EStackPop(stack_ptr); //odmazeme concat(E,E)
              EStackPop(stack_ptr);
              EStackPop(stack_ptr);
              EStackPop(stack_ptr);
              EStackPop(stack_ptr);
              EStackPop(stack_ptr);
              EStackPush(stack_ptr, &newexpr); //pushneme E             
            }


          
          
          
          } else
            return printDesc( SEMANTIC_ERR, (char *) "Wrong type for function call" );
         } else
          return printDesc( SEMANTIC_ERR, (char *) "Wrong type for function call" ); 
      } else
        return printDesc( SYNTAX_ERR, (char *) "Unknown function name" );  
     
     } else
        return printDesc( SYNTAX_ERR, (char *) "Bad function call" ); 
         
  }  else
      return printDesc( SYNTAX_ERR, (char *) "Wrong syntax before right column." );      

  

 
    
  //printf("reducing brackets DONE...\n"); 
  return OP_OK;
}

/**
 *  Vsetky redukcne funkcie aritmetickych a relacnych operatorov
 *  maju podobnu strukturu:
 *  1. vyhodnoti sa obsah zasobnika
 *  2. vyhodnotia sa typy operandov
 *  3. ak je treba zapisu sa instrukcie do zoznamu
 *  4. upravy sa obsah zasobnika
**/      

int
reduceEQ(ptr_EStack stack_ptr)  //2
{
  //printf("Reducing eq...\n");
  
  expr_ptr e1 = EStackTop(stack_ptr);  // E
  expr_ptr e2 = EStackLookOn(stack_ptr, 1); // ==  
  expr_ptr e3 = EStackLookOn(stack_ptr, 2); // E
  
  if ( e1==NULL || e2==NULL || e3==NULL )
    return printDesc( SYNTAX_ERR, "Missing operands..." );
    
  if ( e1->type == E_TERM || e2->type !=E_TERM || e3->type == E_TERM)
    return printDesc( SYNTAX_ERR, "Invalid operands..." );
    
  expr newexpr;     //novy obsah zasobnika

  //obidva typy konstanty...

  if ( (e1->type == E_INT) && (e3->type == E_INT) )
  {
    newexpr.type = E_INT;
    newexpr.val.i = (e1->val.i == e3->val.i);
  } else
  if ( (e1->type == E_DBL) && (e3->type == E_DBL) )
  {
    newexpr.type = E_INT;
    newexpr.val.i = (e1->val.dbl == e3->val.dbl);
  } else 
  if ( (e1->type == E_STR) && (e3->type == E_STR) )
  {
    newexpr.type = E_INT;
    newexpr.val.i = !strCmpString(e1->val.str, e3->val.str);
  } else  
  // Identifikatory aj instrukcie
  if ( (e1->type == E_ID) && (e3->type == E_ID) ) 
  {
    if ( (e1->val.id->type == INT_TYPE) && (e3->val.id->type == INT_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_ID,e1->val.id, I_ID,e3->val.id, newent);
      emitInstr(ilist,I_CMPEQ, TABLEENTRY_OP_TYPE, &(e1->val.id),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->val.id->type == STR_TYPE) && (e3->val.id->type == STR_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      emitInstr(ilist,I_CMPEQ, TABLEENTRY_OP_TYPE, &(e1->val.id),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      //emitInst(CMPEQ, I_ID,e1->val.id, I_ID,e3->val.id, newent);
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->val.id->type == DBL_TYPE) && (e3->val.id->type == DBL_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      emitInstr(ilist,I_CMPEQ, TABLEENTRY_OP_TYPE, &(e1->val.id),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      //emitInst(CMPEQ, I_ID,e1->val.id, I_ID,e3->val.id, newent);
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
        return printDesc( SYNTAX_ERR, "incomparable items...." );
  } 
  else

  //iba prvy identifikator...
  if ( (e1->type == E_ID) && (e3->type != E_ID) )    
  {
     if ( (e1->val.id->type == INT_TYPE) && (e3->type == INT_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_ID,e1->val.id, I_INT,e3->val.i, newent);
      emitInstr(ilist,I_CMPEQ, TABLEENTRY_OP_TYPE, &(e1->val.id),INT_OP_TYPE, &(e3->val.i),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->val.id->type == DBL_TYPE) && (e3->type == DBL_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_ID,e1->val.id, I_DBL,e3->val.dbl, newent);
      emitInstr(ilist,I_CMPEQ, TABLEENTRY_OP_TYPE, &(e1->val.id),DBL_OP_TYPE, &(e3->val.dbl),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->val.id->type == STR_TYPE) && (e3->type == STR_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_ID,e1->val.id, I_STR,e3->val.str, newent);
      emitInstr(ilist,I_CMPEQ, TABLEENTRY_OP_TYPE, &(e1->val.id),STR_OP_TYPE, &(e3->val.str),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
        return printDesc( SEMANTIC_ERR, "incomparable types..." );
                           
  } else
  
  //iba druhy identifikator...
  if ( (e1->type != E_ID) && (e3->type == E_ID) )   
  {
    if ( (e1->type == INT_TYPE) && (e3->val.id->type == INT_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_INT,e1->val.i, I_ID,e3->val.id, newent);
      emitInstr(ilist,I_CMPEQ, INT_OP_TYPE, &(e1->val.i),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->type == DBL_TYPE) && (e3->val.id->type == DBL_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_DBL,e1->val.dbl, I_ID,e3->val.id, newent);
      emitInstr(ilist,I_CMPEQ, DBL_OP_TYPE, &(e1->val.dbl),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->type == STR_TYPE) && (e3->val.id->type == STR_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_STR,e1->val.str, I_ID,e3->val.id, newent);
      emitInstr(ilist,I_CMPEQ, STR_OP_TYPE, &(e1->val.str),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
        return printDesc( SEMANTIC_ERR, "incomparable types..." );    
              
  } else
    return printDesc( SEMANTIC_ERR, "incomparable types..." );
  
  EStackPop(stack_ptr); //odmazeme E == E
  EStackPop(stack_ptr);
  EStackPop(stack_ptr);
  EStackPush(stack_ptr, &newexpr); //pushneme E   

  return OP_OK;
}

int
reduceNEQ(ptr_EStack stack_ptr)  //2
{
  //printf("Reducing neq...\n");
  
  expr_ptr e1 = EStackTop(stack_ptr);  // E
  expr_ptr e2 = EStackLookOn(stack_ptr, 1); // ==  
  expr_ptr e3 = EStackLookOn(stack_ptr, 2); // E
  
  if ( e1==NULL || e2==NULL || e3==NULL )
    return printDesc( SYNTAX_ERR, "Missing operands..." );
    
  if ( e1->type == E_TERM || e2->type !=E_TERM || e3->type == E_TERM)
    return printDesc( SYNTAX_ERR, "Invalid operands..." );
    
  expr newexpr;

  if ( (e1->type == E_INT) && (e3->type == E_INT) )
  {
    newexpr.type = E_INT;
    newexpr.val.i = (e1->val.i != e3->val.i);
  } else
  if ( (e1->type == E_DBL) && (e3->type == E_DBL) )
  {
    newexpr.type = E_INT;
    newexpr.val.i = (e1->val.dbl != e3->val.dbl);
  } else 
  if ( (e1->type == E_STR) && (e3->type == E_STR) )
  {
    newexpr.type = E_INT;
    newexpr.val.i = strCmpString(e1->val.str, e3->val.str);
  } else  
  // Identifikatory aj instrukcie
  if ( (e1->type == E_ID) && (e3->type == E_ID) ) 
  {
    if ( (e1->val.id->type == INT_TYPE) && (e3->val.id->type == INT_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_ID,e1->val.id, I_ID,e3->val.id, newent);
      emitInstr(ilist,I_CMPNEQ, TABLEENTRY_OP_TYPE, &(e1->val.id),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->val.id->type == STR_TYPE) && (e3->val.id->type == STR_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      emitInstr(ilist,I_CMPNEQ, TABLEENTRY_OP_TYPE, &(e1->val.id),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      //emitInst(CMPEQ, I_ID,e1->val.id, I_ID,e3->val.id, newent);
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->val.id->type == DBL_TYPE) && (e3->val.id->type == DBL_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      emitInstr(ilist,I_CMPNEQ, TABLEENTRY_OP_TYPE, &(e1->val.id),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      //emitInst(CMPEQ, I_ID,e1->val.id, I_ID,e3->val.id, newent);
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
        return printDesc( SYNTAX_ERR, "incomparable items...." );
  } 
  else

  //iba prvy identifikator...
  if ( (e1->type == E_ID) && (e3->type != E_ID) )    
  {
     if ( (e1->val.id->type == INT_TYPE) && (e3->type == INT_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_ID,e1->val.id, I_INT,e3->val.i, newent);
      emitInstr(ilist,I_CMPNEQ, TABLEENTRY_OP_TYPE, &(e1->val.id),INT_OP_TYPE, &(e3->val.i),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->val.id->type == DBL_TYPE) && (e3->type == DBL_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_ID,e1->val.id, I_DBL,e3->val.dbl, newent);
      emitInstr(ilist,I_CMPNEQ, TABLEENTRY_OP_TYPE, &(e1->val.id),DBL_OP_TYPE, &(e3->val.dbl),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->val.id->type == STR_TYPE) && (e3->type == STR_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_ID,e1->val.id, I_STR,e3->val.str, newent);
      emitInstr(ilist,I_CMPNEQ, TABLEENTRY_OP_TYPE, &(e1->val.id),STR_OP_TYPE, &(e3->val.str),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
        return printDesc( SEMANTIC_ERR, "incomparable types..." );
                           
  } else
  
  //iba druhy identifikator...
  if ( (e1->type != E_ID) && (e3->type == E_ID) )   
  {
    if ( (e1->type == INT_TYPE) && (e3->val.id->type == INT_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_INT,e1->val.i, I_ID,e3->val.id, newent);
      emitInstr(ilist,I_CMPNEQ, INT_OP_TYPE, &(e1->val.i),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->type == DBL_TYPE) && (e3->val.id->type == DBL_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_DBL,e1->val.dbl, I_ID,e3->val.id, newent);
      emitInstr(ilist,I_CMPNEQ, DBL_OP_TYPE, &(e1->val.dbl),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->type == STR_TYPE) && (e3->val.id->type == STR_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_STR,e1->val.str, I_ID,e3->val.id, newent);
      emitInstr(ilist,I_CMPNEQ, STR_OP_TYPE, &(e1->val.str),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
        return printDesc( SEMANTIC_ERR, "incomparable types..." );    
              
  } else
    return printDesc( SEMANTIC_ERR, "incomparable types..." );
  
  EStackPop(stack_ptr); //odmazeme E != E
  EStackPop(stack_ptr);
  EStackPop(stack_ptr);
  EStackPush(stack_ptr, &newexpr); //pushneme E   

  return OP_OK;
}


int
reduceGT(ptr_EStack stack_ptr)  //2
{
  //printf("Reducing GT...\n");
  
  expr_ptr e1 = EStackTop(stack_ptr);  // E
  expr_ptr e2 = EStackLookOn(stack_ptr, 1); // >  
  expr_ptr e3 = EStackLookOn(stack_ptr, 2); // E
  
  if ( e1==NULL || e2==NULL || e3==NULL )
    return printDesc( SYNTAX_ERR, "Missing operands..." );
    
  if ( e1->type == E_TERM || e2->type !=E_TERM || e3->type == E_TERM)
    return printDesc( SYNTAX_ERR, "Invalid operands..." );
    
  expr newexpr;

  if ( (e1->type == E_INT) && (e3->type == E_INT) )
  {
    newexpr.type = E_INT;
    newexpr.val.i = (e1->val.i < e3->val.i);
  } else
  if ( (e1->type == E_DBL) && (e3->type == E_DBL) )
  {
    newexpr.type = E_INT;
    newexpr.val.i = (e1->val.dbl < e3->val.dbl);
  } else 
  if ( (e1->type == E_STR) && (e3->type == E_STR) )
  {
    newexpr.type = E_INT;
    newexpr.val.i = strCmpString( e3->val.str,e1->val.str)>0;
  } else  
  // Identifikatory aj instrukcie
  if ( (e1->type == E_ID) && (e3->type == E_ID) ) 
  {
    if ( (e1->val.id->type == INT_TYPE) && (e3->val.id->type == INT_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_ID,e1->val.id, I_ID,e3->val.id, newent);
      emitInstr(ilist,I_CMPGT, TABLEENTRY_OP_TYPE, &(e1->val.id),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->val.id->type == STR_TYPE) && (e3->val.id->type == STR_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      emitInstr(ilist,I_CMPGT, TABLEENTRY_OP_TYPE, &(e1->val.id),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      //emitInst(CMPEQ, I_ID,e1->val.id, I_ID,e3->val.id, newent);
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->val.id->type == DBL_TYPE) && (e3->val.id->type == DBL_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      emitInstr(ilist,I_CMPGT, TABLEENTRY_OP_TYPE, &(e1->val.id),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      //emitInst(CMPEQ, I_ID,e1->val.id, I_ID,e3->val.id, newent);
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
        return printDesc( SYNTAX_ERR, "incomparable items...." );
  } 
  else

  //iba prvy identifikator...
  if ( (e1->type == E_ID) && (e3->type != E_ID) )    
  {
     if ( (e1->val.id->type == INT_TYPE) && (e3->type == INT_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_ID,e1->val.id, I_INT,e3->val.i, newent);
      emitInstr(ilist,I_CMPGT, TABLEENTRY_OP_TYPE, &(e1->val.id),INT_OP_TYPE, &(e3->val.i),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->val.id->type == DBL_TYPE) && (e3->type == DBL_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_ID,e1->val.id, I_DBL,e3->val.dbl, newent);
      emitInstr(ilist,I_CMPGT, TABLEENTRY_OP_TYPE, &(e1->val.id),DBL_OP_TYPE, &(e3->val.dbl),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->val.id->type == STR_TYPE) && (e3->type == STR_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_ID,e1->val.id, I_STR,e3->val.str, newent);
      emitInstr(ilist,I_CMPGT, TABLEENTRY_OP_TYPE, &(e1->val.id),STR_OP_TYPE, &(e3->val.str),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
        return printDesc( SEMANTIC_ERR, "incomparable types..." );
                           
  } else
  
  //iba druhy identifikator...
  if ( (e1->type != E_ID) && (e3->type == E_ID) )   
  {
    if ( (e1->type == INT_TYPE) && (e3->val.id->type == INT_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_INT,e1->val.i, I_ID,e3->val.id, newent);
      emitInstr(ilist,I_CMPGT, INT_OP_TYPE, &(e1->val.i),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->type == DBL_TYPE) && (e3->val.id->type == DBL_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_DBL,e1->val.dbl, I_ID,e3->val.id, newent);
      emitInstr(ilist,I_CMPGT, DBL_OP_TYPE, &(e1->val.dbl),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->type == STR_TYPE) && (e3->val.id->type == STR_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_STR,e1->val.str, I_ID,e3->val.id, newent);
      emitInstr(ilist,I_CMPGT, STR_OP_TYPE, &(e1->val.str),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
        return printDesc( SEMANTIC_ERR, "incomparable types..." );    
              
  } else
    return printDesc( SEMANTIC_ERR, "incomparable types..." );
  
  EStackPop(stack_ptr); //odmazeme E > E
  EStackPop(stack_ptr);
  EStackPop(stack_ptr);
  EStackPush(stack_ptr, &newexpr); //pushneme E   

  return OP_OK;
}


int
reduceGE(ptr_EStack stack_ptr)  //2
{
  //printf("Reducing GE...\n");
  
  expr_ptr e1 = EStackTop(stack_ptr);  // E
  expr_ptr e2 = EStackLookOn(stack_ptr, 1); // >  
  expr_ptr e3 = EStackLookOn(stack_ptr, 2); // E
  
  if ( e1==NULL || e2==NULL || e3==NULL )
    return printDesc( SYNTAX_ERR, "Missing operands..." );
    
  if ( e1->type == E_TERM || e2->type !=E_TERM || e3->type == E_TERM)
    return printDesc( SYNTAX_ERR, "Invalid operands..." );
    
  expr newexpr;

  if ( (e1->type == E_INT) && (e3->type == E_INT) )
  {
    newexpr.type = E_INT;
    newexpr.val.i = (e1->val.i <= e3->val.i);
  } else
  if ( (e1->type == E_DBL) && (e3->type == E_DBL) )
  {
    newexpr.type = E_INT;
    newexpr.val.i = (e1->val.dbl <= e3->val.dbl);
  } else 
  if ( (e1->type == E_STR) && (e3->type == E_STR) )
  {
    newexpr.type = E_INT;
    newexpr.val.i = strCmpString( e3->val.str,e1->val.str)>=0;
  } else  
  // Identifikatory aj instrukcie
  if ( (e1->type == E_ID) && (e3->type == E_ID) ) 
  {
    if ( (e1->val.id->type == INT_TYPE) && (e3->val.id->type == INT_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_ID,e1->val.id, I_ID,e3->val.id, newent);
      emitInstr(ilist,I_CMPGE, TABLEENTRY_OP_TYPE, &(e1->val.id),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->val.id->type == STR_TYPE) && (e3->val.id->type == STR_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      emitInstr(ilist,I_CMPGE, TABLEENTRY_OP_TYPE, &(e1->val.id),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      //emitInst(CMPEQ, I_ID,e1->val.id, I_ID,e3->val.id, newent);
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->val.id->type == DBL_TYPE) && (e3->val.id->type == DBL_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      emitInstr(ilist,I_CMPGE, TABLEENTRY_OP_TYPE, &(e1->val.id),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      //emitInst(CMPEQ, I_ID,e1->val.id, I_ID,e3->val.id, newent);
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
        return printDesc( SYNTAX_ERR, "incomparable items...." );
  } 
  else

  //iba prvy identifikator...
  if ( (e1->type == E_ID) && (e3->type != E_ID) )    
  {
     if ( (e1->val.id->type == INT_TYPE) && (e3->type == INT_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_ID,e1->val.id, I_INT,e3->val.i, newent);
      emitInstr(ilist,I_CMPGE, TABLEENTRY_OP_TYPE, &(e1->val.id),INT_OP_TYPE, &(e3->val.i),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->val.id->type == DBL_TYPE) && (e3->type == DBL_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_ID,e1->val.id, I_DBL,e3->val.dbl, newent);
      emitInstr(ilist,I_CMPGE, TABLEENTRY_OP_TYPE, &(e1->val.id),DBL_OP_TYPE, &(e3->val.dbl),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->val.id->type == STR_TYPE) && (e3->type == STR_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_ID,e1->val.id, I_STR,e3->val.str, newent);
      emitInstr(ilist,I_CMPGE, TABLEENTRY_OP_TYPE, &(e1->val.id),STR_OP_TYPE, &(e3->val.str),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
        return printDesc( SEMANTIC_ERR, "incomparable types..." );
                           
  } else
  
  //iba druhy identifikator...
  if ( (e1->type != E_ID) && (e3->type == E_ID) )   
  {
    if ( (e1->type == INT_TYPE) && (e3->val.id->type == INT_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_INT,e1->val.i, I_ID,e3->val.id, newent);
      emitInstr(ilist,I_CMPGE, INT_OP_TYPE, &(e1->val.i),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->type == DBL_TYPE) && (e3->val.id->type == DBL_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_DBL,e1->val.dbl, I_ID,e3->val.id, newent);
      emitInstr(ilist,I_CMPGE, DBL_OP_TYPE, &(e1->val.dbl),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->type == STR_TYPE) && (e3->val.id->type == STR_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_STR,e1->val.str, I_ID,e3->val.id, newent);
      emitInstr(ilist,I_CMPGE, STR_OP_TYPE, &(e1->val.str),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
        return printDesc( SEMANTIC_ERR, "incomparable types..." );    
              
  } else
    return printDesc( SEMANTIC_ERR, "incomparable types..." );
  
  EStackPop(stack_ptr); //odmazeme E >= E
  EStackPop(stack_ptr);
  EStackPop(stack_ptr);
  EStackPush(stack_ptr, &newexpr); //pushneme E   

  return OP_OK;
}



int
reduceLT(ptr_EStack stack_ptr)  //2
{
  //printf("Reducing LT...\n");
  
  expr_ptr e1 = EStackTop(stack_ptr);  // E
  expr_ptr e2 = EStackLookOn(stack_ptr, 1); // >  
  expr_ptr e3 = EStackLookOn(stack_ptr, 2); // E
  
  if ( e1==NULL || e2==NULL || e3==NULL )
    return printDesc( SYNTAX_ERR, "Missing operands..." );
    
  if ( e1->type == E_TERM || e2->type !=E_TERM || e3->type == E_TERM)
    return printDesc( SYNTAX_ERR, "Invalid operands..." );
    
  expr newexpr;

  if ( (e1->type == E_INT) && (e3->type == E_INT) )
  {
    newexpr.type = E_INT;
    newexpr.val.i = (e1->val.i > e3->val.i);
  } else
  if ( (e1->type == E_DBL) && (e3->type == E_DBL) )
  {
    newexpr.type = E_INT;
    newexpr.val.i = (e1->val.dbl > e3->val.dbl);
  } else 
  if ( (e1->type == E_STR) && (e3->type == E_STR) )
  {
    newexpr.type = E_INT;
    newexpr.val.i = strCmpString( e3->val.str,e1->val.str)<0;
  } else  
  // Identifikatory aj instrukcie
  if ( (e1->type == E_ID) && (e3->type == E_ID) ) 
  {
    if ( (e1->val.id->type == INT_TYPE) && (e3->val.id->type == INT_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_ID,e1->val.id, I_ID,e3->val.id, newent);
      emitInstr(ilist,I_CMPLT, TABLEENTRY_OP_TYPE, &(e1->val.id),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->val.id->type == STR_TYPE) && (e3->val.id->type == STR_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      emitInstr(ilist,I_CMPLT, TABLEENTRY_OP_TYPE, &(e1->val.id),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      //emitInst(CMPEQ, I_ID,e1->val.id, I_ID,e3->val.id, newent);
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->val.id->type == DBL_TYPE) && (e3->val.id->type == DBL_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      emitInstr(ilist,I_CMPLT, TABLEENTRY_OP_TYPE, &(e1->val.id),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      //emitInst(CMPEQ, I_ID,e1->val.id, I_ID,e3->val.id, newent);
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
        return printDesc( SYNTAX_ERR, "incomparable items...." );
  } 
  else

  //iba prvy identifikator...
  if ( (e1->type == E_ID) && (e3->type != E_ID) )    
  {
     if ( (e1->val.id->type == INT_TYPE) && (e3->type == INT_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_ID,e1->val.id, I_INT,e3->val.i, newent);
      emitInstr(ilist,I_CMPLT, TABLEENTRY_OP_TYPE, &(e1->val.id),INT_OP_TYPE, &(e3->val.i),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->val.id->type == DBL_TYPE) && (e3->type == DBL_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_ID,e1->val.id, I_DBL,e3->val.dbl, newent);
      emitInstr(ilist,I_CMPLT, TABLEENTRY_OP_TYPE, &(e1->val.id),DBL_OP_TYPE, &(e3->val.dbl),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->val.id->type == STR_TYPE) && (e3->type == STR_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_ID,e1->val.id, I_STR,e3->val.str, newent);
      emitInstr(ilist,I_CMPLT, TABLEENTRY_OP_TYPE, &(e1->val.id),STR_OP_TYPE, &(e3->val.str),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
        return printDesc( SEMANTIC_ERR, "incomparable types..." );
                           
  } else
  
  //iba druhy identifikator...
  if ( (e1->type != E_ID) && (e3->type == E_ID) )   
  {
    if ( (e1->type == INT_TYPE) && (e3->val.id->type == INT_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_INT,e1->val.i, I_ID,e3->val.id, newent);
      emitInstr(ilist,I_CMPLT, INT_OP_TYPE, &(e1->val.i),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->type == DBL_TYPE) && (e3->val.id->type == DBL_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_DBL,e1->val.dbl, I_ID,e3->val.id, newent);
      emitInstr(ilist,I_CMPLT, DBL_OP_TYPE, &(e1->val.dbl),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->type == STR_TYPE) && (e3->val.id->type == STR_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_STR,e1->val.str, I_ID,e3->val.id, newent);
      emitInstr(ilist,I_CMPLT, STR_OP_TYPE, &(e1->val.str),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
        return printDesc( SEMANTIC_ERR, "incomparable types..." );    
              
  } else
    return printDesc( SEMANTIC_ERR, "incomparable types..." );
  
  EStackPop(stack_ptr); //odmazeme E < E
  EStackPop(stack_ptr);
  EStackPop(stack_ptr);
  EStackPush(stack_ptr, &newexpr); //pushneme E   

  return OP_OK;
}

int
reduceLE(ptr_EStack stack_ptr)  //2
{
  //printf("Reducing LE...\n");
  
  expr_ptr e1 = EStackTop(stack_ptr);  // E
  expr_ptr e2 = EStackLookOn(stack_ptr, 1); // >  
  expr_ptr e3 = EStackLookOn(stack_ptr, 2); // E
  
  if ( e1==NULL || e2==NULL || e3==NULL )
    return printDesc( SYNTAX_ERR, "Missing operands..." );
    
  if ( e1->type == E_TERM || e2->type !=E_TERM || e3->type == E_TERM)
    return printDesc( SYNTAX_ERR, "Invalid operands..." );
    
  expr newexpr;

  if ( (e1->type == E_INT) && (e3->type == E_INT) )
  {
    newexpr.type = E_INT;
    newexpr.val.i = (e1->val.i >= e3->val.i);
  } else
  if ( (e1->type == E_DBL) && (e3->type == E_DBL) )
  {
    newexpr.type = E_INT;
    newexpr.val.i = (e1->val.dbl >= e3->val.dbl);
  } else 
  if ( (e1->type == E_STR) && (e3->type == E_STR) )
  {
    newexpr.type = E_INT;
    newexpr.val.i = strCmpString( e3->val.str,e1->val.str)<=0;
  } else  
  // Identifikatory aj instrukcie
  if ( (e1->type == E_ID) && (e3->type == E_ID) ) 
  {
    if ( (e1->val.id->type == INT_TYPE) && (e3->val.id->type == INT_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_ID,e1->val.id, I_ID,e3->val.id, newent);
      emitInstr(ilist,I_CMPLE, TABLEENTRY_OP_TYPE, &(e1->val.id),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->val.id->type == STR_TYPE) && (e3->val.id->type == STR_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      emitInstr(ilist,I_CMPLE, TABLEENTRY_OP_TYPE, &(e1->val.id),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      //emitInst(CMPEQ, I_ID,e1->val.id, I_ID,e3->val.id, newent);
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->val.id->type == DBL_TYPE) && (e3->val.id->type == DBL_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      emitInstr(ilist,I_CMPLE, TABLEENTRY_OP_TYPE, &(e1->val.id),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      //emitInst(CMPEQ, I_ID,e1->val.id, I_ID,e3->val.id, newent);
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
        return printDesc( SYNTAX_ERR, "incomparable items...." );
  } 
  else

  //iba prvy identifikator...
  if ( (e1->type == E_ID) && (e3->type != E_ID) )    
  {
     if ( (e1->val.id->type == INT_TYPE) && (e3->type == INT_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_ID,e1->val.id, I_INT,e3->val.i, newent);
      emitInstr(ilist,I_CMPLE, TABLEENTRY_OP_TYPE, &(e1->val.id),INT_OP_TYPE, &(e3->val.i),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->val.id->type == DBL_TYPE) && (e3->type == DBL_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_ID,e1->val.id, I_DBL,e3->val.dbl, newent);
      emitInstr(ilist,I_CMPLE, TABLEENTRY_OP_TYPE, &(e1->val.id),DBL_OP_TYPE, &(e3->val.dbl),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->val.id->type == STR_TYPE) && (e3->type == STR_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_ID,e1->val.id, I_STR,e3->val.str, newent);
      emitInstr(ilist,I_CMPLE, TABLEENTRY_OP_TYPE, &(e1->val.id),STR_OP_TYPE, &(e3->val.str),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
        return printDesc( SEMANTIC_ERR, "incomparable types..." );
                           
  } else
  
  //iba druhy identifikator...
  if ( (e1->type != E_ID) && (e3->type == E_ID) )   
  {
    if ( (e1->type == INT_TYPE) && (e3->val.id->type == INT_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_INT,e1->val.i, I_ID,e3->val.id, newent);
      emitInstr(ilist,I_CMPLE, INT_OP_TYPE, &(e1->val.i),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->type == DBL_TYPE) && (e3->val.id->type == DBL_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_DBL,e1->val.dbl, I_ID,e3->val.id, newent);
      emitInstr(ilist,I_CMPLE, DBL_OP_TYPE, &(e1->val.dbl),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
    if ( (e1->type == STR_TYPE) && (e3->val.id->type == STR_TYPE) )
    {
      Table_Entry newent = newVariabile(INT_TYPE);
      //emitInst(CMPEQ, I_STR,e1->val.str, I_ID,e3->val.id, newent);
      emitInstr(ilist,I_CMPLE, STR_OP_TYPE, &(e1->val.str),TABLEENTRY_OP_TYPE, &(e3->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
      newexpr.type = E_ID;
      newexpr.val.id = newent;
    } else
        return printDesc( SEMANTIC_ERR, "incomparable types..." );    
              
  } else
    return printDesc( SEMANTIC_ERR, "incomparable types..." );
  
  EStackPop(stack_ptr); //odmazeme E <= E
  EStackPop(stack_ptr);
  EStackPop(stack_ptr);
  EStackPush(stack_ptr, &newexpr); //pushneme E   

  return OP_OK;
}


int 
reduceAssign(ptr_EStack stack_ptr)
{

  //printf("Reducing assign...\n");

  
  expr_ptr e1 = EStackTop(stack_ptr);  // E
  expr_ptr e2 = EStackLookOn(stack_ptr, 2); //must be lval
  
  if ( e1 == NULL || e2 == NULL)
    return printDesc( SYNTAX_ERR, "Missing operands..." );   
  
  if ( e2 -> type != E_ID )
    return printDesc( SEMANTIC_ERR, "Invalid Lvalue" );
  
  if ( e2->val.id->user_defined == 0)
    return printDesc( SEMANTIC_ERR, "Invalid Lvalue" );
    
  if ( e1->type == E_TERM )
    return printDesc( SYNTAX_ERR, "Asigning terminal");  

/**  
  printf ("e2 id type %d\n",e2->val.id->type );
  printf("assigning to var: %s\n", strGetStr(e2->val.id->id_name));
  printf("e1->type right type %d\n",e1->type);
  if( e1->type == E_ID)
  printf("e1 id type %d\n",e1->val.id->type);
**/
  
  expr newexass;    
  newexass.type = E_ID;
  if ( (e1->type == E_ID) && ( e1->val.id->type == INT_TYPE) && ( e2->val.id->type == INT_TYPE) )
  {
    newexass.val.id = newVariabile(INT_TYPE);
    emitInstr(ilist,I_SET, TABLEENTRY_OP_TYPE, &(e1->val.id), NO_OP,NULL, TABLEENTRY_OP_TYPE, &(newexass.val.id) );
    emitInstr(ilist,I_SET, TABLEENTRY_OP_TYPE, &(e1->val.id), NO_OP,NULL, TABLEENTRY_OP_TYPE, &(e2->val.id) );
  } else
  if ( (e1->type == E_ID) && ( e1->val.id->type == DBL_TYPE) && ( e2->val.id->type == DBL_TYPE) )//tu
  {
    newexass.val.id = newVariabile(DBL_TYPE);
    emitInstr(ilist,I_SET, TABLEENTRY_OP_TYPE, &(e1->val.id), NO_OP,NULL, TABLEENTRY_OP_TYPE, &(newexass.val.id) );
    emitInstr(ilist,I_SET, TABLEENTRY_OP_TYPE, &(e1->val.id), NO_OP,NULL, TABLEENTRY_OP_TYPE, &(e2->val.id) ); 
  }
  else
  if ( (e1->type == E_ID) && ( e1->val.id->type == STR_TYPE) && ( e2->val.id->type == STR_TYPE) )
  {
    newexass.val.id = newVariabile(STR_TYPE);
    emitInstr(ilist,I_SET, TABLEENTRY_OP_TYPE, &(e1->val.id), NO_OP,NULL, TABLEENTRY_OP_TYPE, &(newexass.val.id) );
    emitInstr(ilist,I_SET, TABLEENTRY_OP_TYPE, &(e1->val.id), NO_OP,NULL, TABLEENTRY_OP_TYPE, &(e2->val.id) ); 
  }
  else  
  if ( (e1->type == E_ID) && ( e1->val.id->type == INT_TYPE) && ( e2->val.id->type == DBL_TYPE) )
  {
    newexass.val.id = newVariabile(DBL_TYPE);
    emitInstr(ilist,I_SET, TABLEENTRY_OP_TYPE, &(e1->val.id), NO_OP,NULL, TABLEENTRY_OP_TYPE, &(newexass.val.id) );
    emitInstr(ilist,I_SET, TABLEENTRY_OP_TYPE, &(e1->val.id), NO_OP,NULL, TABLEENTRY_OP_TYPE, &(e2->val.id) );    
  } else
  
    // priradzujeme konstantu mozeme propagovat....
  
  if ( (e1->type == E_INT) && ( e2->val.id->type == INT_TYPE) )
  {
    newexass.type = E_INT;
    newexass.val.i = e1->val.i;
    // ??? emitInstr(SET, e1, null, newexass.val.id); 
    emitInstr(ilist,I_SET,  INT_OP_TYPE, &(e1->val.i), NO_OP,NULL,TABLEENTRY_OP_TYPE, &(e2->val.id));
  } else
  if ( (e1->type == E_DBL) && ( e2->val.id->type == DBL_TYPE) )
  {
    newexass.type = E_DBL;
    newexass.val.dbl = e1->val.dbl;

    //emitInstr(SET, e1, null, newexass.val.id);
    emitInstr(ilist,I_SET, DBL_OP_TYPE, &(e1->val.dbl), NO_OP,NULL, TABLEENTRY_OP_TYPE, &(e2->val.id) ); 
  }
  else
  if ( (e1->type == E_STR) && ( e2->val.id->type == STR_TYPE) )
  {
    newexass.type = E_STR;
    newexass.val.str = strNewFromConstChar(strGetStr(e1->val.str));
    //emitInstr(SET, e1, null, newexass.val.id);
    //printf("priradenie kond %s\n",strGetStr(e1->val.str));
    emitInstr(ilist,I_SET, STR_OP_TYPE, &(e1->val.str), NO_OP,NULL, TABLEENTRY_OP_TYPE, &(e2->val.id) );
  }
  else  
  if ( (e1->type == E_INT) && ( e2->val.id->type == DBL_TYPE) )
  {
    newexass.type = E_DBL;
    newexass.val.dbl = (double) e1->val.i;
    //emitInstr(SET, e1, null, newexass.val.id);
    emitInstr(ilist,I_SET, DBL_OP_TYPE, &(newexass.val.dbl), NO_OP,NULL, TABLEENTRY_OP_TYPE, &(e2->val.id) );    
  } else
    return printDesc( SEMANTIC_ERR, "Invalid operands..." );

  EStackPop(stack_ptr); //odmazeme LVal(E) = E
  EStackPop(stack_ptr);
  EStackPop(stack_ptr);
  EStackPush(stack_ptr, &newexass); //pushneme E
  return OP_OK;
}

int
reduceAdd(ptr_EStack stack_ptr)
{
      //printf("Reducing add...\n");
              expr_ptr e1 = EStackTop(stack_ptr);  // E1
              expr_ptr e2 = EStackLookOn(stack_ptr, 2); //E2
              expr_ptr e3 = EStackLookOn(stack_ptr, 1); //E3
              
              if ( e1==NULL || e2==NULL || e3==NULL )
                return printDesc( SYNTAX_ERR, "Missing operands..." );
                
              if ( e1->type == E_TERM || e3->type !=E_TERM || e2->type == E_TERM)
                return printDesc( SYNTAX_ERR, "Invalid operands..." );
                
              //printf("operand check\n");
              expr newexadd;
              
              //semanticke kontroly a nastavenie typu pri konstantach...
              
              if ( (e1->type != E_ID) && (e2->type!= E_ID) ) 
              {
                if ( (e1->type == E_INT) && (e2->type == E_INT) ) //2x int
                {
                  newexadd.type = E_INT;
                  newexadd.val.i = e1->val.i + e2->val.i;
                } else
                if ( (e1->type == E_DBL) && (e2->type == E_DBL) )   //2x doble
                {
                  newexadd.type = E_DBL;
                  newexadd.val.dbl = e1->val.dbl + e2->val.dbl;              
                  } else
                if ( (e1->type == E_DBL) && (e2->type == E_INT) ) //E1 double -> implicitne pretypovanie
                {
                  newexadd.type = E_DBL;
                  newexadd.val.dbl = e1->val.dbl + e2->val.i;              
                } else
                if ( (e1->type == E_INT) && (e2->type == E_DBL) ) //E2 double -> implicitne pretypovanie
                {
                  newexadd.type = E_DBL;
                  newexadd.val.dbl = e1->val.i + e2->val.dbl;              
                } else
                if ( (e1->type == E_STR) || (e2->type == E_STR) ) 
                {
                  return printDesc( SEMANTIC_ERR, "Adding strings..." );  
                } 
              } else
              // Identifikatory uz sa budu aj robit instrukcie...
              if ( (e1->type == E_ID) && (e2->type == E_ID) ) 
              {
                if ( (e1->val.id->type == INT_TYPE) && (e2->val.id->type == INT_TYPE) )
                {
                  Table_Entry newent = newVariabile(INT_TYPE);
                  emitInstr(ilist,I_ADD, TABLEENTRY_OP_TYPE, &(e1->val.id), TABLEENTRY_OP_TYPE, &(e2->val.id), TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->val.id->type == DBL_TYPE) && (e2->val.id->type == INT_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  emitInstr(ilist,I_ADD, TABLEENTRY_OP_TYPE, &(e1->val.id), TABLEENTRY_OP_TYPE, &(e2->val.id), TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->val.id->type == INT_TYPE) && (e2->val.id->type == DBL_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  emitInstr(ilist,I_ADD, TABLEENTRY_OP_TYPE, &(e1->val.id), TABLEENTRY_OP_TYPE, &(e2->val.id), TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->val.id->type == DBL_TYPE) && (e2->val.id->type == DBL_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  emitInstr(ilist,I_ADD, TABLEENTRY_OP_TYPE, &(e1->val.id), TABLEENTRY_OP_TYPE, &(e2->val.id), TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                {
                   return printDesc( SEMANTIC_ERR, "Adding strings..." );
                }
                                                
              } else
              //iba prvy identifikator...
              if ( (e1->type == E_ID) && (e2->type != E_ID) )    
              {
               if ( (e1->val.id->type == INT_TYPE) && (e2->type == INT_TYPE) )
                {
                  Table_Entry newent = newVariabile(INT_TYPE);
                  //emitInst(ADD, e1->val.id, e2->val.i, newent);
                  emitInstr(ilist,I_ADD, TABLEENTRY_OP_TYPE, &(e1->val.id), INT_OP_TYPE, &(e2->val.i), TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->val.id->type == DBL_TYPE) && (e2->type == INT_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  //emitInst(ADD, e1->val.id, e2->val.dbl, newent);
                  emitInstr(ilist,I_ADD, TABLEENTRY_OP_TYPE, &(e1->val.id), INT_OP_TYPE, &(e2->val.i), TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->val.id->type == INT_TYPE) && (e2->type == DBL_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  //emitInst(ADD, e1->val.id, e2->val.dbl, newent);
                  emitInstr(ilist,I_ADD, TABLEENTRY_OP_TYPE, &(e1->val.id), DBL_OP_TYPE, &(e2->val.dbl), TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->val.id->type == DBL_TYPE) && (e2->type == DBL_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  //emitInst(ADD, e1->val.id, e2->val.dbl, newent);
                  emitInstr(ilist,I_ADD, TABLEENTRY_OP_TYPE, &(e1->val.id), DBL_OP_TYPE, &(e2->val.dbl), TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                {
                   return printDesc( SEMANTIC_ERR, "Adding strings..." );
                }             
              } else
              //iba druhy identifikator...
              if ( (e1->type != E_ID) && (e2->type == E_ID) )   
              {
               if ( (e1->type == INT_TYPE) && (e2->val.id->type == INT_TYPE) )
                {
                  Table_Entry newent = newVariabile(INT_TYPE);
                  //emitInst(ADD, e1->val.id, e2->val.id, newent);
                  emitInstr(ilist,I_ADD, INT_OP_TYPE, &(e1->val.i), TABLEENTRY_OP_TYPE, &(e2->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->type == DBL_TYPE) && (e2->val.id->type == INT_TYPE) )
                {
                                  
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  //emitInst(ADD, e1->val.dbl, e2->val.id, newent);
                  emitInstr(ilist,I_ADD, DBL_OP_TYPE, &(e1->val.dbl), TABLEENTRY_OP_TYPE, &(e2->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->type == INT_TYPE) && (e2->val.id->type == DBL_TYPE) )
                {

                  Table_Entry newent = newVariabile(DBL_TYPE);
                  emitInstr(ilist,I_ADD, INT_OP_TYPE, &(e1->val.i), TABLEENTRY_OP_TYPE, &(e2->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->type == DBL_TYPE) && (e2->val.id->type == DBL_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  emitInstr(ilist,I_ADD, DBL_OP_TYPE, &(e1->val.dbl), TABLEENTRY_OP_TYPE, &(e2->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                {
                   return printDesc( SEMANTIC_ERR, "Adding strings..." );
                }             
              } else
              {
                return printDesc( SEMANTIC_ERR, "Adding strings..." );
              }
              
                        
              EStackPop(stack_ptr); //odmazeme E + E
              EStackPop(stack_ptr);
              EStackPop(stack_ptr);
              EStackPush(stack_ptr, &newexadd);     //vlozime E 

              return OP_OK;

}

int
reduceMul(ptr_EStack stack_ptr)
{
    //printf("Reducing mul...\n");
              expr_ptr e1 = EStackTop(stack_ptr);  // E1
              expr_ptr e2 = EStackLookOn(stack_ptr, 2); //E2
              expr_ptr e3 = EStackLookOn(stack_ptr, 1); //E3
              
              if ( e1==NULL || e2==NULL || e3==NULL )
                return printDesc( SYNTAX_ERR, "Missing operands..." );
                
              if ( e1->type == E_TERM || e3->type !=E_TERM || e2->type == E_TERM)
                return printDesc( SYNTAX_ERR, "Invalid operands..." );
              
              expr newexadd;
              
              //semanticke kontroly a nastavenie typu pri konstantach...
              
              if ( (e1->type != E_ID) && (e2->type!= E_ID) ) 
              {
              if ( (e1->type == E_INT) && (e2->type == E_INT) ) //2x int
              {
                newexadd.type = E_INT;
                newexadd.val.i = e1->val.i * e2->val.i;
              } else
              if ( (e1->type == E_DBL) && (e2->type == E_DBL) )   //2x doble
              {
                newexadd.type = E_DBL;
                newexadd.val.dbl = e1->val.dbl * e2->val.dbl;              
              } else
              if ( (e1->type == E_DBL) && (e2->type == E_INT) ) //E1 double -> implicitne pretypovanie
              {
                newexadd.type = E_DBL;
                newexadd.val.dbl = e1->val.dbl * e2->val.i;              
              } else
              if ( (e1->type == E_INT) && (e2->type == E_DBL) ) //E2 double -> implicitne pretypovanie
              {
                newexadd.type = E_DBL;
                newexadd.val.dbl = e1->val.i * e2->val.dbl;              
              } else
              if ( (e1->type == E_STR) || (e2->type == E_STR) ) 
              {
                return printDesc( SEMANTIC_ERR, "Multiplying strings..." );  
              } 
              } else
              // Identifikatory uz sa budu aj robit instrukcie...
              if ( (e1->type == E_ID) && (e2->type == E_ID) ) 
              {
                if ( (e1->val.id->type == INT_TYPE) && (e2->val.id->type == INT_TYPE) )
                {
                  Table_Entry newent = newVariabile(INT_TYPE);
                  emitInstr(ilist,I_MUL, TABLEENTRY_OP_TYPE, &(e1->val.id), TABLEENTRY_OP_TYPE, &(e2->val.id), TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->val.id->type == DBL_TYPE) && (e2->val.id->type == INT_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  emitInstr(ilist,I_MUL, TABLEENTRY_OP_TYPE, &(e1->val.id), TABLEENTRY_OP_TYPE, &(e2->val.id), TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->val.id->type == INT_TYPE) && (e2->val.id->type == DBL_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  emitInstr(ilist,I_MUL, TABLEENTRY_OP_TYPE, &(e1->val.id), TABLEENTRY_OP_TYPE, &(e2->val.id), TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->val.id->type == DBL_TYPE) && (e2->val.id->type == DBL_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  emitInstr(ilist,I_MUL, TABLEENTRY_OP_TYPE, &(e1->val.id), TABLEENTRY_OP_TYPE, &(e2->val.id), TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                {
                   return printDesc( SEMANTIC_ERR, "Multiplying strings..." );
                }
                                                
              } else
              //iba prvy identifikator...
              if ( (e1->type == E_ID) && (e2->type != E_ID) )    
              {
               if ( (e1->val.id->type == INT_TYPE) && (e2->type == INT_TYPE) )
                {
                  Table_Entry newent = newVariabile(INT_TYPE);
                  emitInstr(ilist,I_MUL, TABLEENTRY_OP_TYPE, &(e1->val.id), INT_OP_TYPE, &(e2->val.i), TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->val.id->type == DBL_TYPE) && (e2->type == INT_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  //emitInst(MUL, e1->val.id, e2->val.dbl, newent);
                  emitInstr(ilist,I_MUL, TABLEENTRY_OP_TYPE, &(e1->val.id), INT_OP_TYPE, &(e2->val.i), TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->val.id->type == INT_TYPE) && (e2->type == DBL_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  //emitInst(MUL, e1->val.id, e2->val.dbl, newent);
                  emitInstr(ilist,I_MUL, TABLEENTRY_OP_TYPE, &(e1->val.id), DBL_OP_TYPE, &(e2->val.dbl), TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->val.id->type == DBL_TYPE) && (e2->type == DBL_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  //emitInst(MUL, e1->val.id, e2->val.dbl, newent);
                  emitInstr(ilist,I_MUL, TABLEENTRY_OP_TYPE, &(e1->val.id), DBL_OP_TYPE, &(e2->val.dbl), TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                {
                   return printDesc( SEMANTIC_ERR, "Multiplying strings..." );
                }             
              } else
              //iba druhy identifikator...
              if ( (e1->type != E_ID) && (e2->type == E_ID) )   
              {
               if ( (e1->type == INT_TYPE) && (e2->val.id->type == INT_TYPE) )
                {
                  Table_Entry newent = newVariabile(INT_TYPE);
                  //emitInst(MUL, e1->val.id, e2->val.id, newent);
                  emitInstr(ilist,I_MUL,INT_OP_TYPE, &(e1->val.i), TABLEENTRY_OP_TYPE, &(e2->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->type == DBL_TYPE) && (e2->val.id->type == INT_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  //emitInst(MUL, e1->val.dbl, e2->val.id, newent);
                  emitInstr(ilist,I_MUL,DBL_OP_TYPE, &(e1->val.dbl), TABLEENTRY_OP_TYPE, &(e2->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->type == INT_TYPE) && (e2->val.id->type == DBL_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  //emitInst(MUL, e1->val.dbl, e2->val.id, newent);
                  emitInstr(ilist,I_MUL,INT_OP_TYPE, &(e1->val.i), TABLEENTRY_OP_TYPE, &(e2->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->type == DBL_TYPE) && (e2->val.id->type == DBL_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  //emitInst(MUL, e1->val.dbl, e2->val.id, newent);
                  emitInstr(ilist,I_MUL,DBL_OP_TYPE, &(e1->val.dbl), TABLEENTRY_OP_TYPE, &(e2->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                {
                   return printDesc( SEMANTIC_ERR, "Multiplying strings..." );
                }             
              } else
              {
                return printDesc( SEMANTIC_ERR, "Multiplying strings..." );
              }
              
                        
              EStackPop(stack_ptr); //odmazeme E * E
              EStackPop(stack_ptr);
              EStackPop(stack_ptr);
              EStackPush(stack_ptr, &newexadd);     //vlozime E 

              return OP_OK;

}

int
reduceSub(ptr_EStack stack_ptr)
{
    //printf("Reducing substraction...\n");
              expr_ptr e1 = EStackTop(stack_ptr);  // E1
              expr_ptr e2 = EStackLookOn(stack_ptr, 2); //E2
              expr_ptr e3 = EStackLookOn(stack_ptr, 1); //E3
              
              if ( e1==NULL || e2==NULL || e3==NULL )
                return printDesc( SYNTAX_ERR, "Missing operands..." );
                
              if ( e1->type == E_TERM || e3->type !=E_TERM || e2->type == E_TERM)
                return printDesc( SYNTAX_ERR, "Invalid operands..." );
              
              expr newexadd;
              
              //semanticke kontroly a nastavenie typu pri konstantach...
              
              if ( (e1->type != E_ID) && (e2->type!= E_ID) ) 
              {              
              if ( (e1->type == E_INT) && (e2->type == E_INT) ) //2x int
              {
                newexadd.type = E_INT;
                newexadd.val.i = -(e1->val.i - e2->val.i);
              } else
              if ( (e1->type == E_DBL) && (e2->type == E_DBL) )   //2x doble
              {
                newexadd.type = E_DBL;
                newexadd.val.dbl = e2->val.dbl - e1->val.dbl;              
              } else
              if ( (e1->type == E_DBL) && (e2->type == E_INT) ) //E1 double -> implicitne pretypovanie
              {
                newexadd.type = E_DBL;
                newexadd.val.dbl = e2->val.i - e1->val.dbl;              
              } else
              if ( (e1->type == E_INT) && (e2->type == E_DBL) ) //E2 double -> implicitne pretypovanie
              {
                newexadd.type = E_DBL;
                newexadd.val.dbl = e2->val.dbl - e1->val.i;              
              } 
              } else
              if ( (e1->type == E_STR) || (e2->type == E_STR) ) 
              {
                return printDesc( SEMANTIC_ERR, "Substracting strings..." );  
              } else
              // Identifikatory uz sa budu aj robit instrukcie...
              if ( (e1->type == E_ID) && (e2->type == E_ID) ) 
              {
                if ( (e1->val.id->type == INT_TYPE) && (e2->val.id->type == INT_TYPE) )
                {
                  Table_Entry newent = newVariabile(INT_TYPE);
                  //emitInst(SUB, e1->val.id, e2->val.id, newent);
                  emitInstr(ilist,I_SUB, TABLEENTRY_OP_TYPE, &(e1->val.id), TABLEENTRY_OP_TYPE, &(e2->val.id), TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->val.id->type == DBL_TYPE) && (e2->val.id->type == INT_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  //emitInst(SUB, e1->val.id, e2->val.id, newent);
                  emitInstr(ilist,I_SUB, TABLEENTRY_OP_TYPE, &(e1->val.id), TABLEENTRY_OP_TYPE, &(e2->val.id), TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->val.id->type == INT_TYPE) && (e2->val.id->type == DBL_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  //emitInst(SUB, e1->val.id, e2->val.id, newent);
                  emitInstr(ilist,I_SUB, TABLEENTRY_OP_TYPE, &(e1->val.id), TABLEENTRY_OP_TYPE, &(e2->val.id), TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->val.id->type == DBL_TYPE) && (e2->val.id->type == DBL_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  emitInstr(ilist,I_SUB, TABLEENTRY_OP_TYPE, &(e1->val.id), TABLEENTRY_OP_TYPE, &(e2->val.id), TABLEENTRY_OP_TYPE, &(newent) );
                  //emitInst(SUB, e1->val.id, e2->val.id, newent);
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                {
                   return printDesc( SEMANTIC_ERR, "Substracting strings..." );
                }
                                                
              } else
              //iba prvy identifikator...
              if ( (e1->type == E_ID) && (e2->type != E_ID) )    
              {
               if ( (e1->val.id->type == INT_TYPE) && (e2->type == INT_TYPE) )
                {
                  Table_Entry newent = newVariabile(INT_TYPE);
                  //emitInst(SUB, e1->val.id, e2->val.i, newent);
                  emitInstr(ilist,I_SUB, TABLEENTRY_OP_TYPE, &(e1->val.id), INT_OP_TYPE, &(e2->val.i), TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->val.id->type == DBL_TYPE) && (e2->type == INT_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  //emitInst(SUB, e1->val.id, e2->val.dbl, newent);
                  emitInstr(ilist,I_SUB, TABLEENTRY_OP_TYPE, &(e1->val.id), INT_OP_TYPE, &(e2->val.i), TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->val.id->type == INT_TYPE) && (e2->type == DBL_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  emitInstr(ilist,I_SUB, TABLEENTRY_OP_TYPE, &(e1->val.id), DBL_OP_TYPE, &(e2->val.dbl), TABLEENTRY_OP_TYPE, &(newent) );
                  //emitInst(SUB, e1->val.id, e2->val.dbl, newent);
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->val.id->type == DBL_TYPE) && (e2->type == DBL_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  emitInstr(ilist,I_SUB, TABLEENTRY_OP_TYPE, &(e1->val.id), DBL_OP_TYPE, &(e2->val.dbl), TABLEENTRY_OP_TYPE, &(newent) );
                  //emitInst(SUB, e1->val.id, e2->val.dbl, newent);
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                {
                   return printDesc( SEMANTIC_ERR, "Substracting strings..." );
                }             
              } else
              //iba druhy identifikator...
              if ( (e1->type != E_ID) && (e2->type == E_ID) )   
              {
               if ( (e1->type == INT_TYPE) && (e2->val.id->type == INT_TYPE) )
                {
                  Table_Entry newent = newVariabile(INT_TYPE);
                  //emitInst(SUB, e1->val.id, e2->val.id, newent);
                  emitInstr(ilist,I_SUB, INT_OP_TYPE, &(e1->val.dbl),TABLEENTRY_OP_TYPE, &(e2->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->type == DBL_TYPE) && (e2->val.id->type == INT_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  emitInstr(ilist,I_SUB, DBL_OP_TYPE, &(e1->val.dbl),TABLEENTRY_OP_TYPE, &(e2->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
                  //emitInst(SUB, e1->val.dbl, e2->val.id, newent);
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->type == INT_TYPE) && (e2->val.id->type == DBL_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  emitInstr(ilist,I_SUB, INT_OP_TYPE, &(e1->val.i),TABLEENTRY_OP_TYPE, &(e2->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
                  //emitInst(SUB, e1->val.dbl, e2->val.id, newent);
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->type == DBL_TYPE) && (e2->val.id->type == DBL_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  emitInstr(ilist,I_SUB, DBL_OP_TYPE, &(e1->val.dbl),TABLEENTRY_OP_TYPE, &(e2->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
                  //emitInst(SUB, e1->val.dbl, e2->val.id, newent);
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                {
                   return printDesc( SEMANTIC_ERR, "Substracting strings..." );
                }             
              } else
              {
                return printDesc( SEMANTIC_ERR, "Substracting strings..." );
              }
              
                        
              EStackPop(stack_ptr); //odmazeme E - E
              EStackPop(stack_ptr);
              EStackPop(stack_ptr);
              EStackPush(stack_ptr, &newexadd);     //vlozime E 

              return OP_OK;

}

int
reduceDiv(ptr_EStack stack_ptr)
{
    //printf("Reducing division...\n");
              expr_ptr e1 = EStackTop(stack_ptr);  // E1
              expr_ptr e2 = EStackLookOn(stack_ptr, 2); //E2
              expr_ptr e3 = EStackLookOn(stack_ptr, 1); //E3
              
              if ( e1==NULL || e2==NULL || e3==NULL )
                return printDesc( SYNTAX_ERR, "Missing operands..." );
                
              if ( e1->type == E_TERM || e3->type !=E_TERM || e2->type == E_TERM)
                return printDesc( SYNTAX_ERR, "Invalid operands..." );
              
              expr newexadd;
              
              //CO S NULOU
              //semanticke kontroly a nastavenie typu pri konstantach...
              if ( (e1->type != E_ID) && (e2->type!= E_ID) ) 
              {             
              if ( (e1->type == E_INT) && (e2->type == E_INT) ) //2x int
              {
                newexadd.type = E_INT;
                //delenie konstantnou nulou...
                if( e1->val.i == 0)
                {
                  Table_Entry newent = newVariabile(INT_TYPE);
                  newexadd.val.i = 0;
                  emitInstr(ilist,I_DIV, INT_OP_TYPE, &(newexadd.val.i), INT_OP_TYPE, &(newexadd.val.i), TABLEENTRY_OP_TYPE, &(newent) );
                } else {
                  newexadd.val.i = e2->val.i/e1->val.i ;
                }
              } else
              if ( (e1->type == E_DBL) && (e2->type == E_DBL) )   //2x doble
              {
                newexadd.type = E_DBL;
                
                if( e1->val.dbl == 0.0)
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  newexadd.val.dbl = 0.0;
                  emitInstr(ilist,I_DIV, DBL_OP_TYPE, &(newexadd.val.dbl), DBL_OP_TYPE, &(newexadd.val.dbl), TABLEENTRY_OP_TYPE, &(newent) );
                } else {
                  newexadd.val.dbl = e2->val.dbl/e1->val.dbl ;
                }                
                
                              
              } else
              if ( (e1->type == E_DBL) && (e2->type == E_INT) ) //E1 double -> implicitne pretypovanie
              {
                newexadd.type = E_DBL;
                
                if( e1->val.dbl == 0.0)
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  newexadd.val.dbl = 0.0;
                  emitInstr(ilist,I_DIV, DBL_OP_TYPE, &(newexadd.val.dbl), DBL_OP_TYPE, &(newexadd.val.dbl), TABLEENTRY_OP_TYPE, &(newent) );
                } else {
                  newexadd.val.dbl = e2->val.i/e1->val.dbl ;
                }                
                                
                
                              
              } else
              if ( (e1->type == E_INT) && (e2->type == E_DBL) ) //E2 double -> implicitne pretypovanie
              {
                newexadd.type = E_DBL;
                
                if( e1->val.i == 0)
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  newexadd.val.dbl = 0.0;
                  emitInstr(ilist,I_DIV, DBL_OP_TYPE, &(newexadd.val.dbl), DBL_OP_TYPE, &(newexadd.val.dbl), TABLEENTRY_OP_TYPE, &(newent) );
                } else {
                  newexadd.val.dbl =e2->val.dbl/ e1->val.i ;
                }                   
                
                
                              
              } else
              if ( (e1->type == E_STR) || (e2->type == E_STR) ) 
              {
                return printDesc( SEMANTIC_ERR, "Dividing strings..." );  
              } 
              } else
              // Identifikatory uz sa budu aj robit instrukcie...
              if ( (e1->type == E_ID) && (e2->type == E_ID) ) 
              {
                if ( (e1->val.id->type == INT_TYPE) && (e2->val.id->type == INT_TYPE) )
                {
                  Table_Entry newent = newVariabile(INT_TYPE);
                  //emitInst(DIV, e1->val.id, e2->val.id, newent);
                  emitInstr(ilist,I_DIV, TABLEENTRY_OP_TYPE, &(e1->val.id), TABLEENTRY_OP_TYPE, &(e2->val.id), TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->val.id->type == DBL_TYPE) && (e2->val.id->type == INT_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  //emitInst(DIV, e1->val.id, e2->val.id, newent);
                  emitInstr(ilist,I_DIV, TABLEENTRY_OP_TYPE, &(e1->val.id), TABLEENTRY_OP_TYPE, &(e2->val.id), TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->val.id->type == INT_TYPE) && (e2->val.id->type == DBL_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  //emitInst(DIV, e1->val.id, e2->val.id, newent);
                  emitInstr(ilist,I_DIV, TABLEENTRY_OP_TYPE, &(e1->val.id), TABLEENTRY_OP_TYPE, &(e2->val.id), TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->val.id->type == DBL_TYPE) && (e2->val.id->type == DBL_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  //emitInst(DIV, e1->val.id, e2->val.id, newent);
                  emitInstr(ilist,I_DIV, TABLEENTRY_OP_TYPE, &(e1->val.id), TABLEENTRY_OP_TYPE, &(e2->val.id), TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                {
                   return printDesc( SEMANTIC_ERR, "Dividing strings..." );
                }
                                                
              } else
              //iba prvy identifikator...
              if ( (e1->type == E_ID) && (e2->type != E_ID) )    
              {
               if ( (e1->val.id->type == INT_TYPE) && (e2->type == INT_TYPE) )
                {
                  Table_Entry newent = newVariabile(INT_TYPE);
                  emitInstr(ilist,I_DIV, TABLEENTRY_OP_TYPE, &(e1->val.id), INT_OP_TYPE, &(e2->val.i), TABLEENTRY_OP_TYPE, &(newent) );
                  //emitInst(DIV, e1->val.id, e2->val.i, newent);
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->val.id->type == DBL_TYPE) && (e2->type == INT_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  //emitInst(DIV, e1->val.id, e2->val.dbl, newent);
                  emitInstr(ilist,I_DIV, TABLEENTRY_OP_TYPE, &(e1->val.id), INT_OP_TYPE, &(e2->val.i), TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->val.id->type == INT_TYPE) && (e2->type == DBL_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  //emitInst(DIV, e1->val.id, e2->val.dbl, newent);
                  emitInstr(ilist,I_DIV, TABLEENTRY_OP_TYPE, &(e1->val.id), DBL_OP_TYPE, &(e2->val.dbl), TABLEENTRY_OP_TYPE, &(newent) );
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->val.id->type == DBL_TYPE) && (e2->type == DBL_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  emitInstr(ilist,I_DIV, TABLEENTRY_OP_TYPE, &(e1->val.id), DBL_OP_TYPE, &(e2->val.dbl), TABLEENTRY_OP_TYPE, &(newent) );
                  //emitInst(DIV, e1->val.id, e2->val.dbl, newent);
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                {
                   return printDesc( SEMANTIC_ERR, "Dividing strings..." );
                }             
              } else
              //iba druhy identifikator...
              if ( (e1->type != E_ID) && (e2->type == E_ID) )   
              {
               if ( (e1->type == INT_TYPE) && (e2->val.id->type == INT_TYPE) )
                {
                  Table_Entry newent = newVariabile(INT_TYPE);
                  emitInstr(ilist,I_DIV, INT_OP_TYPE, &(e1->val.i),TABLEENTRY_OP_TYPE, &(e2->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
                  //emitInst(DIV, e1->val.id, e2->val.id, newent);
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->type == DBL_TYPE) && (e2->val.id->type == INT_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  emitInstr(ilist,I_DIV, DBL_OP_TYPE, &(e1->val.dbl),TABLEENTRY_OP_TYPE, &(e2->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
                  //emitInst(DIV, e1->val.dbl, e2->val.id, newent);
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->type == INT_TYPE) && (e2->val.id->type == DBL_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  emitInstr(ilist,I_DIV, INT_OP_TYPE, &(e1->val.i),TABLEENTRY_OP_TYPE, &(e2->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
                  //emitInst(DIV, e1->val.dbl, e2->val.id, newent);
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                if ( (e1->type == DBL_TYPE) && (e2->val.id->type == DBL_TYPE) )
                {
                  Table_Entry newent = newVariabile(DBL_TYPE);
                  emitInstr(ilist,I_DIV, DBL_OP_TYPE, &(e1->val.dbl),TABLEENTRY_OP_TYPE, &(e2->val.id),  TABLEENTRY_OP_TYPE, &(newent) );
                  //emitInst(DIV, e1->val.dbl, e2->val.id, newent);
                  newexadd.type = E_ID;
                  newexadd.val.id = newent;
                } else
                {
                   return printDesc( SEMANTIC_ERR, "Dividing strings..." );
                }             
              } else
              {
                return printDesc( SEMANTIC_ERR, "Dividing strings..." );
              }
              
                        
              EStackPop(stack_ptr); //odmazeme E / E
              EStackPop(stack_ptr);
              EStackPop(stack_ptr);
              EStackPush(stack_ptr, &newexadd);     //vlozime E 

              return OP_OK;

}

//Operacie nad zasobnikom

//inicializacia
int
EStackInit(ptr_EStack stack)
{
  stack->top = NULL;
  return OP_OK;
}

//uvolnenie
int
EStackFree(ptr_EStack stack)
{

  while ( !EStackIsEmpty(stack) )
  {
    EStackPop(stack);
  }
  return OP_OK;
}

//prazdnota
int
EStackIsEmpty(ptr_EStack stack)
{
  return (stack->top == NULL) ? 1 : 0;
}

//push
int
EStackPush(ptr_EStack stack, expr_ptr cont)
{

  expr_ptr newexpr = malloc( sizeof (expr) );
  if (newexpr == NULL)
    return SYSTEM_ERR;    

  newexpr->type = cont->type;  
  newexpr->attr = cont->attr;
  newexpr->val = cont->val;
  
  newexpr->next = stack->top; 
  stack->top = newexpr;

  return OP_OK;

}

//pop
int
EStackPop(ptr_EStack stack)
{

  if ( stack-> top == NULL )
    return SYSTEM_ERR;
    
  
  expr_ptr oldtop = stack->top;
  stack->top = stack->top->next;

  //ak bol na zasobniku retazec maze aj ten

  if ( oldtop->type == E_TERM )
    if ( oldtop->attr != NULL)
    {
      strFree(oldtop->attr);
      free(oldtop->attr);

    }
  
  if ( oldtop->type == E_STR )
    if ( oldtop->val.str != NULL)
    {
      strFree(oldtop->val.str);
      free(oldtop->val.str);
    }
  
  free(oldtop);
  return OP_OK;

}

expr_ptr
EStackTop(ptr_EStack stack)
{
  return stack->top;
}

expr
EStackTopPop(ptr_EStack stack)
{
  expr retval;
  
  if ( stack-> top == NULL )
  {
    retval.type = -1;
    return (retval);
  }
    
  retval.type = stack->top->type;
  retval.val = stack->top->val;
  
  EStackPop(stack);
  
  return retval;
  
}

//funkcia ktora sa pozrie pod vrch zasobnika
expr_ptr
EStackLookOn(ptr_EStack stack, int pos)
{
  expr_ptr retptr = stack->top;
  
  for ( int i = 0; (retptr != NULL) && i < pos; i++, retptr=retptr->next) ;
  
  return retptr;
}

//pomocna funkcia na vypis obsahu
void
EStackPrintCont(ptr_EStack stack )
{
  printf("-----------------Start of stack--------------------------\n");
  int i = 0;
  expr_ptr tmpptr = stack->top;
  while ( tmpptr != NULL )
  {
    printf("Position %d Contents type %d Value: %d\n",i, tmpptr->type,tmpptr->val.i);
   // printf("Attributes %s\n",strGetStr(tmpptr->attr));
    printf("NextPtr %p\n", (void *)tmpptr->next);
    tmpptr = tmpptr->next;
    i++;
  }
  printf("-----------------End of stack--------------------------\n");
}
