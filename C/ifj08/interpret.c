/*****************************
 *  Prekladac jazyka IF2008
 *  Matus Kontra (xkontr00) (veduci)
 *  Jozef Kobrtek (xkobrt00)
 *  Patrik Seben (xseben00)
 *  Matej Smetana (xsmeta02)
******************************/  

#include "interpret.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int
runIList(ptr_IList ilist_ptr, Sym_Tab table)
{
  
  //nastavime sa na pociatok
  ResetIP(ilist_ptr);
  //lokalna premenna sucasnej instrukcie
  Instr current;
  
  //cyklus...
  while (1)
  {
    
    //ak sme na konci z cyklu vyskocime
    if ( getVirtualIP(ilist_ptr) == NULL ) break;
    
    switch ( (current = getVirtualIP(ilist_ptr)->cont).opcode )       //na zaklade opcodu vybereme operaciu
    {
      
      //op1 je zdrojova premenna alebo konstanta, op3 je cielova premenna
      case I_SET : 
        ;
        //instrukcia priradenia
        switch (current.op1type)
        { 
          //na zaklade typu sa do premennej priradi hodnota 1. operandu  
          case TABLEENTRY_OP_TYPE :
            if (current.op3.id->type == STR_TYPE)
            {
              strFree(current.op3.id->value.str);
              free(current.op3.id->value.str);
              current.op3.id->value.str = strNewFromConstChar( strGetStr(current.op1.id->value.str));            
            } else
            if (current.op3.id->type == DBL_TYPE)
            {
              if ( current.op1.id->type == INT_TYPE )
              {
                current.op3.id->value.dbl = (double) current.op1.id->value.i;
              } else
              if ( current.op1.id->type == DBL_TYPE )
              {
                current.op3.id->value.dbl = current.op1.id->value.dbl;
              } else
                return printDesc(RUNTIME_ERR, "Internal error"); 
            } else
            if (current.op3.id->type == INT_TYPE)
            {
              current.op3.id->value.i = current.op1.id->value.i;
            } else
              return printDesc(RUNTIME_ERR, "Internal error");
            
            break;   
          
          case INT_OP_TYPE :
            current.op3.id->value.i = current.op1.i;
            break;
            
          case DBL_OP_TYPE :
            current.op3.id->value.dbl = current.op1.dbl;
            break;
            
          case STR_OP_TYPE :
            strFree(current.op3.id->value.str);
            free(current.op3.id->value.str);
            current.op3.id->value.str = strNewFromConstChar( strGetStr(current.op1.str));//toto...
            break;                             
            
        }
        break;
        
      case I_ADD :
      case I_SUB :
      case I_MUL :
      case I_DIV :    
        ;
        //aritmeticke operacie
        // 3 pripady - obidva operandy ID 1 ID a druhy CONST a opacne
        
        //osetrenie delenia 0
        
        if ( current.opcode == I_DIV)
        {
          if ( current.op1type == TABLEENTRY_OP_TYPE )
            if ( ((double) (current.op1.id->type == INT_TYPE ? current.op1.id->value.i : current.op1.id->value.dbl)) == 0 )
              return printDesc(RUNTIME_ERR, "Division by zero");  
          
          if ( current.op1type != TABLEENTRY_OP_TYPE )
            if ( (double) (current.op1type == INT_TYPE ? current.op1.i : current.op1.dbl) == 0 )
              return printDesc(RUNTIME_ERR, "Division by zero");

        }
        
        if ( current.op1type == TABLEENTRY_OP_TYPE && current.op2type == TABLEENTRY_OP_TYPE )
        {
          
          //obidva su IDENTIFIKATORY
          if ( (current.op1.id->type == INT_TYPE) && (current.op2.id->type == INT_TYPE))  
          {
            //ak su obidva INTY vysledny typ je int
            current.op3.id->value.i = fiop(current.op1.id->value.i,current.op2.id->value.i,current.opcode);
          } else
          {
            //inac double
            //operandy vyberame ternarnym operatorom
            current.op3.id->value.dbl = fdop( (double) current.op1.id->type == INT_TYPE ? current.op1.id->value.i : current.op1.id->value.dbl,
                                              (double) current.op2.id->type == INT_TYPE ? current.op2.id->value.i : current.op2.id->value.dbl,
                                             current.opcode);
          }
          
        } else
        if ( current.op1type != TABLEENTRY_OP_TYPE && current.op2type == TABLEENTRY_OP_TYPE )
        {
          //iba jedna je ID druha je konstanta...
          //spracuje sa tym istym sposobom iba sa pristupuje do inej zlozky instrukcie
          if ( (current.op1type == INT_OP_TYPE) && (current.op2.id->type == INT_TYPE))  
          {
            current.op3.id->value.i = fiop(current.op1.i,current.op2.id->value.i,current.opcode);
          } else
          {
            current.op3.id->value.dbl = fdop( (double) current.op1type == INT_OP_TYPE ? current.op1.i : current.op1.dbl,
                                              (double) current.op2.id->type == INT_TYPE ? current.op2.id->value.i : current.op2.id->value.dbl,
                                             current.opcode);
          }
        
        } else
        if ( current.op1type == TABLEENTRY_OP_TYPE && current.op2type != TABLEENTRY_OP_TYPE )
        {
          
          //iba jedna je ID druha je konstanta...
          //spracuje sa tym istym sposobom iba sa pristupuje do inej zlozky instrukcie
          if ( (current.op2type == INT_OP_TYPE) && (current.op1.id->type == INT_TYPE))  
          {
            current.op3.id->value.i = fiop(current.op1.id->value.i,current.op2.i,current.opcode);
          } else
          {
            current.op3.id->value.dbl = fdop( (double) current.op1.id->type == INT_TYPE ? current.op1.id->value.i : current.op1.id->value.dbl,
                                              (double) current.op2type == INT_OP_TYPE ? current.op2.i : current.op2.dbl,
                                             current.opcode);
          }
        
        }
        break;
      
      case I_CMPEQ :
      case I_CMPNEQ :
      case I_CMPGT :
      case I_CMPGE :
      case I_CMPLT :
      case I_CMPLE : 
        ;
        //instrukcie porovnania
        //znova treba rozlysovat medzi konstantou a ID v SymTable
        if ( current.op1type == TABLEENTRY_OP_TYPE && current.op2type == TABLEENTRY_OP_TYPE )
        {
          //obidva zaznamy v tabulke symbolov
          if ( (current.op1.id->type == current.op2.id->type ) )  
          {
            switch (current.op1.id->type)
            {
              case INT_OP_TYPE : 
                ;
                current.op3.id->value.i = ficom(current.op1.id->value.i,current.op2.id->value.i,current.opcode);
                break;
                
              case DBL_OP_TYPE : 
                ;
                current.op3.id->value.i = fdcom(current.op1.id->value.dbl,current.op2.id->value.dbl,current.opcode);
                break;

              case STR_OP_TYPE : 
                ;
                current.op3.id->value.i = fscom(current.op1.id->value.str,current.op2.id->value.str,current.opcode);
                break;
            }
          } 
          
        } else
        if ( current.op1type != TABLEENTRY_OP_TYPE && current.op2type == TABLEENTRY_OP_TYPE )
        {
          //iba jeden z nich podobne aj ta moznost podtym
          if ( (current.op1type == current.op2.id->type ) )  
          {
            switch (current.op1type)
            {
              case INT_OP_TYPE : 
                ;
                current.op3.id->value.i = ficom(current.op1.i,current.op2.id->value.i,current.opcode);
                break;
                
              case DBL_OP_TYPE : 
                ;
                current.op3.id->value.i = fdcom(current.op1.dbl,current.op2.id->value.dbl,current.opcode);
                break;

              case STR_OP_TYPE : 
                ;
                current.op3.id->value.i = fscom(current.op1.str,current.op2.id->value.str,current.opcode);
                break;
            }
          }
          
        
        } else
        if ( current.op1type == TABLEENTRY_OP_TYPE && current.op2type != TABLEENTRY_OP_TYPE )
        {

          if ( (current.op1.id->type == current.op2type ) )  
          {
            switch (current.op1.id->type)
            {
              case INT_OP_TYPE : 
                ;
                current.op3.id->value.i = ficom(current.op1.id->value.i,current.op2.i,current.opcode);
                break;
                
              case DBL_OP_TYPE : 
                ;
                current.op3.id->value.i = fdcom(current.op1.id->value.dbl,current.op2.dbl,current.opcode);
                break;

              case STR_OP_TYPE : 
                ;
                current.op3.id->value.i = fscom(current.op1.id->value.str,current.op2.str,current.opcode);
                break;
            }
          }
        
        }
        break;
       
      //instrukcie volania funkcii 
      case I_SORT :
        ;
        strFree(current.op3.id->value.str);
        free(current.op3.id->value.str);
        current.op3.id->value.str = sort( current.op1type == TABLEENTRY_OP_TYPE ? current.op1.id->value.str : current.op1.str);
        break;
        
      case I_LEN :
        ;
        current.op3.id->value.i = length( current.op1type == TABLEENTRY_OP_TYPE ? current.op1.id->value.str : current.op1.str);
        break;
        
      case I_FIND :
        ;
        current.op3.id->value.i = find( current.op2type == TABLEENTRY_OP_TYPE ? current.op2.id->value.str : current.op2.str,
                                        current.op1type == TABLEENTRY_OP_TYPE ? current.op1.id->value.str : current.op1.str);
        break;
        
      case I_CONCAT :
        ;
        strFree(current.op3.id->value.str);
        free(current.op3.id->value.str);
        current.op3.id->value.str = concat( current.op2type == TABLEENTRY_OP_TYPE ? current.op2.id->value.str : current.op2.str,
                                            current.op1type == TABLEENTRY_OP_TYPE ? current.op1.id->value.str : current.op1.str);
        break;
      
      case I_READ :
        ;
        //instrukcia nacitania pouziva sa scanf atd....
        switch (current.op3.id->type)
        {
        
          case INT_TYPE : 
            ;
            string * inttmp = newStr();
            int c1;
            while (  ( (c1 = getchar()) != EOF ) )
            {
              if ( c1 == '\n' )
                break;
              strAddChar(inttmp, (char) c1);
            }
            //strAddChar(inttmp, '\n');
            
            char * read1 = strGetStr(inttmp);
            char * readNum1 = NULL;
            current.op3.id->value.i = (int) strtol (read1, &readNum1, 10);
            if (readNum1 - read1 != strlen(read1) )
              return printDesc(RUNTIME_ERR, "Bad input"); 
            /**
            if (  sscanf(strGetStr(inttmp),"%d\n",&(current.op3.id->value.i)) != 1 )
              return printDesc(RUNTIME_ERR, "Bad input"); 
            **/
            strFree(inttmp);
            free(inttmp);
            inttmp = NULL;
            break;
                
          case DBL_TYPE : 
            ;
            string * dbltmp = newStr();
            int c2;
            while (  ( (c2 = getchar()) != EOF ) )
            {
              if ( c2 == '\n' )
                break;
              strAddChar(dbltmp, (char) c2);
            }
            //strAddChar(inttmp, '\n');
            
            char * read2 = strGetStr(dbltmp);
            char * readNum2 = NULL;
            current.op3.id->value.dbl = (double) strtod (read2, &readNum2);
            if (readNum2 - read2 != strlen(read2) )
              return printDesc(RUNTIME_ERR, "Bad input"); 
            /**
            if (  sscanf(strGetStr(inttmp),"%d\n",&(current.op3.id->value.i)) != 1 )
              return printDesc(RUNTIME_ERR, "Bad input"); 
            **/
            strFree(dbltmp);
            free(dbltmp);
            inttmp = NULL;
            break;

          case STR_TYPE : //doriesit
            ;
            int c3 = 0;
            strFree(current.op3.id->value.str);
            free(current.op3.id->value.str);
            current.op3.id->value.str = newStr();
            //printf("reading...to inicilized %p  ",current.op3.id->value.str );
            while (  ( (c3 = getchar()) != EOF ) )
            {
              if ( c3 == '\n' )
                break;
              //printf("pridavam znak\n");
              strAddChar(current.op3.id->value.str, (char) c3);
            }
            
            //printf("done   %p  ",current.op3.id->value.str);
            break;
                    
        }
        break;
      
      
      case I_WRITE :
        ;
        //instrukcia zapisu
        switch (current.op3.id->type)
        {
        
          case INT_TYPE : 
            ;
            printf("%d",current.op3.id->value.i);
            break;
                
          case DBL_TYPE : 
            ;
            printf("%lf",current.op3.id->value.dbl);
            break;

          case STR_TYPE : 
            ;
            printf("%s",strGetStr(current.op3.id->value.str) );
            break;
                    
        }
        break;
        
      case I_LBL :
        ;
        //navestie
        break;
        
      case I_JMP :
        ;
        //unconditional jump
        setVirtualIP(ilist_ptr, current.op3.add) ;
        break; 
         
      case I_JNZ :
        ;
        //jump if not zero
        switch (current.op1.id->type)
        {
          case INT_TYPE: if ( current.op1.id->value.i != 0) setVirtualIP(ilist_ptr, current.op3.add) ;
          case DBL_TYPE: if ( current.op1.id->value.dbl != 0) setVirtualIP(ilist_ptr, current.op3.add) ;
        }
        break;
        
      case I_JZ :
        ;
        //jump if zero
        switch (current.op1.id->type)
        {
          case INT_TYPE: if ( current.op1.id->value.i == 0) setVirtualIP(ilist_ptr, current.op3.add) ;
                        break;
          case DBL_TYPE: if ( current.op1.id->value.dbl == 0) setVirtualIP(ilist_ptr, current.op3.add) ;
                        break;
        }
        break;
        
      case I_RET :
        ;
        //instrukcia navratu
        switch (current.op1.id->type)
        {
          case INT_TYPE: return ( current.op1.id->value.i) ;
        }
        break;
        
      default : 
        ;
        return printDesc(RUNTIME_ERR, "Unknown OPcode");
    }
  
  
    AdvanceIP(ilist_ptr);
  }


  return OP_OK;
}

//prevedie typ operandu na polozku obsahu symbolu
sym_cont
instr_to_sym(operand in, int type)
{
  sym_cont newtmp;
  
  switch (type)
  {
          case INT_OP_TYPE :
            newtmp.i = in.i;
            break;
            
          case DBL_OP_TYPE :
            newtmp.dbl = in.dbl;
            break;
            
          case STR_OP_TYPE :
            newtmp.str = in.str;
            break;  
  
  }
  
  return newtmp;
}


//aritmeticka operacia nad integermi a vracia sa int
int fiop(int i1, int i2, int OP)
{
  if (OP==I_ADD)
    return i1+i2;
  else 
  if (OP==I_SUB)
    return i2-i1;
  else 
  if (OP==I_MUL)
    return i1*i2;
  else 
  if (OP==I_DIV)
    return i2/i1;
    
  return 0;
}

//nad doublami a vracia sa double
double fdop(double i1, double i2, int OP)
{
  if (OP==I_ADD)
    return i1+i2;
  else 
  if (OP==I_SUB)
    return i2-i1;
  else 
  if (OP==I_MUL)
    return i1*i2;
  else 
  if (OP==I_DIV)
    return i2/i1;
    
  return 0;}

//porovnania integerov
int ficom(int i2, int i1, int OP)
{
  if (OP==I_CMPEQ)
    return i1==i2;
  else 
  if (OP==I_CMPNEQ)
    return i1!=i2;
  else 
  if (OP==I_CMPGT)
    return i1>i2;
  else 
  if (OP==I_CMPGE)
    return i1>=i2;
  else 
  if (OP==I_CMPLT)
    return i1<i2;
  else 
  if (OP==I_CMPLE)
    return i1<=i2;
    
  return 0;
}

//porovnanie doublov
int fdcom(double i2, double i1, int OP)
{
  if (OP==I_CMPEQ)
    return i1==i2;
  else 
  if (OP==I_CMPNEQ)
    return i1!=i2;
  else 
  if (OP==I_CMPGT)
    return i1>i2;
  else 
  if (OP==I_CMPGE)
    return i1>=i2;
  else 
  if (OP==I_CMPLT)
    return i1<i2;
  else 
  if (OP==I_CMPLE)
    return i1<=i2;
    
  return 0;
}

//porovnanie stringov
int fscom(string* i2, string* i1, int OP)
{
  if (OP==I_CMPEQ)
    return !strCmpString(i1,i2);
  else 
  if (OP==I_CMPNEQ)
    return strCmpString(i1,i2);
  else 
  if (OP==I_CMPGT)
    return strCmpString(i1,i2)>0;
  else 
  if (OP==I_CMPGE)
    return strCmpString(i1,i2)>=0;
  else 
  if (OP==I_CMPLT)
    return strCmpString(i1,i2)<0;
  else 
  if (OP==I_CMPLE)
    return strCmpString(i1,i2)<=0;
    
  return 0;
}


