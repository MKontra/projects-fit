/*****************************
 *  Prekladac jazyka IF2008
 *  Matus Kontra (xkontr00) (veduci)
 *  Jozef Kobrtek (xkobrt00)
 *  Patrik Seben (xseben00)
 *  Matej Smetana (xsmeta02)
******************************/ 

#include <stdlib.h>
#include "ilist.h"
#include "codes.h"

//implementacia zoznamu instrukcii

//inicializacia
void
InitIList(ptr_IList ilist_ptr)    //hlava koniec vIP...
{
  ilist_ptr->head = NULL;
  ilist_ptr->tail = NULL;
  ilist_ptr->virtual_ip = NULL;
}

void
DestroyIList( ptr_IList ilist_ptr)
//pri ruseni treba kontrolovat ci polozkou instrukcie nebola retazcova konstanta
{
  
  ptr_InstEntry next, temp = ilist_ptr->head;
  
  while ( temp != NULL )
  {
    next = temp->next;
  
    if ( temp->cont.op1type == STR_OP_TYPE )
    {
      if ( temp->cont.op1.str != NULL ){
        strFree(temp->cont.op1.str);
        free(temp->cont.op1.str);
        temp->cont.op1.str = NULL;
      }
    
    }
    
    if ( temp->cont.op2type == STR_OP_TYPE )
    {
      if ( temp->cont.op2.str != NULL ){
        strFree(temp->cont.op2.str);
        free(temp->cont.op2.str);
        temp->cont.op2.str = NULL;
      }
    
    }
    
    if ( temp->cont.op3type == STR_OP_TYPE )
    {
      if ( temp->cont.op3.str != NULL ){
        strFree(temp->cont.op3.str);
        free(temp->cont.op3.str);
        temp->cont.op3.str = NULL;
      }
    }
      
    free(temp) ;
  
    temp = next;
  }
  
  
  InitIList( ilist_ptr );
}

ptr_InstEntry
emitInstr(ptr_IList ilist_ptr, int opcode, int op1type, void * op1, int op2type, void * op2,int op3type, void * op3)
//Vlozenie instrukcie...
{
  ptr_InstEntry newptr = malloc ( sizeof(InstEntry) );  //pametove miesto...
  
  if ( newptr == NULL )
    return NULL;
  
  newptr->cont.opcode = opcode;
  
  //nastavenie operandu
  //na zaklade informacie o type ulozenej v instrukcii
  //prepis hodnotu z parametru do instrukcie
  //ak rertaec prekopiruj ho cely
  
  newptr->cont.op1type = op1type;   
  if ( op1 != NULL)
  {  
    if ( op1type == STR_OP_TYPE )
    {
      //vlozenie konstanty malo by byt ok...
      newptr->cont.op1type = STR_OP_TYPE;
      newptr->cont.op1.str = strNewFromConstChar(strGetStr( *((string **)op1) ));         
    } else
    {

        switch (op1type)
        {
          case ADDRESS_OP_TYPE :
            newptr->cont.op1.add = *((ptr_InstEntry *) op1);
            break;
            
          case TABLEENTRY_OP_TYPE :
            newptr->cont.op1.id = *((Table_Entry *) op1);
            break;
                        
          case INT_OP_TYPE :
            newptr->cont.op1.i = *((int *) op1);
            break;
            
          case DBL_OP_TYPE :
            newptr->cont.op1.dbl = *((double *) op1);
            break;            
                  
 
        }
      }
  } else 
    newptr->cont.op1.i = 0;
    
  newptr->cont.op2type = op2type;     
  if ( op2 != NULL)
  {    
    if ( op2type == STR_OP_TYPE )
    {
      newptr->cont.op2type = STR_OP_TYPE;
      newptr->cont.op2.str = strNewFromConstChar(strGetStr( *((string **)op2) ));           
    } else
    {

        switch (op2type)
        {
          case ADDRESS_OP_TYPE :          
            newptr->cont.op2.add = *((ptr_InstEntry *) op2);
            break;
            
          case TABLEENTRY_OP_TYPE :         
            newptr->cont.op2.id = *((Table_Entry *) op2);
            break;
                        
          case INT_OP_TYPE :         
            newptr->cont.op2.i = *((int *) op2);
            break;
            
          case DBL_OP_TYPE :          
            newptr->cont.op2.dbl = *((double *) op2);
            break;            
                  
 
        }
      }
  } else 
    newptr->cont.op2.i = 0;
 
 
  newptr->cont.op3type = op3type;

  if ( op3 != NULL)
  {    
    if ( op3type == STR_OP_TYPE )
    {
      newptr->cont.op3type = STR_OP_TYPE;
      newptr->cont.op3.str = strNewFromConstChar(strGetStr( *((string **)op3) ));          
    } else
    {

        switch (op3type)
        {
          case ADDRESS_OP_TYPE :
            newptr->cont.op3.add = *((ptr_InstEntry *) op3);
            break;
            
          case TABLEENTRY_OP_TYPE :
            newptr->cont.op3.id = *((Table_Entry *) op3);
            break;
                        
          case INT_OP_TYPE :
            newptr->cont.op3.i = *((int *) op3);
            break;
            
          case DBL_OP_TYPE :
            newptr->cont.op3.dbl = *((double *) op3);
            break;            
                  
 
        }
      }
  } else 
    newptr->cont.op3.i = 0;
    
  if ( ilist_ptr == NULL)
  {
      return NULL;
  }
    
  newptr->next = NULL;
    
  if (ilist_ptr->tail == NULL)    //prazdny zoznam prva = hlava aj koniec
  {
    ilist_ptr->head = newptr;
    ilist_ptr->tail = newptr;
  }
  else
  {
    ilist_ptr->tail->next = newptr;//iba koniec
    ilist_ptr->tail = newptr;
  }

  return newptr;
}

//funkcie pre manipulaciu s VirtualIP
ptr_InstEntry
AdvanceIP(ptr_IList ilist_ptr)
{
  if ( ilist_ptr->virtual_ip == NULL)
    return NULL;
    
  return (ilist_ptr->virtual_ip = ilist_ptr->virtual_ip->next);
}

ptr_InstEntry
ResetIP(ptr_IList ilist_ptr)
{
    return (ilist_ptr->virtual_ip = ilist_ptr->head);
}

ptr_InstEntry
getVirtualIP(ptr_IList ilist_ptr)
{
  return (ilist_ptr->virtual_ip);
}

void
setVirtualIP(ptr_IList ilist_ptr, ptr_InstEntry newIP)
{
  ilist_ptr->virtual_ip = newIP;
}


//------------------------------------------------------------------------------
//pomocna funkcia vypise obsah zoznamu
void
printList(ptr_IList ilist_ptr)
{

  ptr_InstEntry newptr = ilist_ptr->head;

  while( newptr != NULL)
  { 
    printf("---------------------------\n");
    printf("Opcode  %d\n",newptr->cont.opcode);
    printf("Op1type %d\n",newptr->cont.op1type);
        switch (newptr->cont.op1type)
        {
          case ADDRESS_OP_TYPE :          
            printf("Pointer: %p\n",(void *)newptr->cont.op1.add );
            break;
            
          case TABLEENTRY_OP_TYPE :         
            printf("Premenna meno: %s\n",strGetStr(newptr->cont.op1.id->id_name));
            break;
                        
          case INT_OP_TYPE :         
            printf("Cislo cele %d\n",newptr->cont.op1.i);
            break;
            
          case DBL_OP_TYPE :          
            printf("Cislo desatinne %lf\n",newptr->cont.op1.dbl);
            break;            
                  
          case STR_OP_TYPE :
            if (newptr->cont.op1.str == NULL) break;          
            printf("Retazec: %s\n",strGetStr(newptr->cont.op1.str));
            break;                  
 
        }
        
        
    printf("Op2type %d\n",newptr->cont.op2type);
    
        switch (newptr->cont.op2type)
        {
          case ADDRESS_OP_TYPE :          
            printf("Pointer: %p\n",(void *)newptr->cont.op2.add );
            break;
            
          case TABLEENTRY_OP_TYPE :         
            printf("Premenna meno: %s\n",strGetStr(newptr->cont.op2.id->id_name));
            break;
                        
          case INT_OP_TYPE :         
            printf("Cislo cele %d\n",newptr->cont.op2.i);
            break;
            
          case DBL_OP_TYPE :          
            printf("Cislo desatinne %lf\n",newptr->cont.op2.dbl);
            break;            
                  
          case STR_OP_TYPE :
            if (newptr->cont.op2.str == NULL) break;          
            printf("Retazec: %s\n",strGetStr(newptr->cont.op2.str));
            break;                  
 
        }    
    
    
    printf("Op3type %d\n",newptr->cont.op3type);
    
        switch (newptr->cont.op3type)
        {
          case ADDRESS_OP_TYPE :          
            printf("Pointer: %p\n",(void *)newptr->cont.op3.add );
            break;
            
          case TABLEENTRY_OP_TYPE :         
            printf("Premenna meno: %s\n",strGetStr(newptr->cont.op3.id->id_name));
            break;
                        
          case INT_OP_TYPE :         
            printf("Cislo cele %d\n",newptr->cont.op3.i);
            break;
            
          case DBL_OP_TYPE :          
            printf("Cislo desatinne %lf\n",newptr->cont.op3.dbl);
            break;            
                  
          case STR_OP_TYPE : 
            if (newptr->cont.op3.str == NULL) break;          
            printf("Retazec: %s\n",strGetStr(newptr->cont.op3.str));
            break;                  
 
        }    
    
    printf("---------------------------\n");
    newptr=newptr->next;
  }

}

