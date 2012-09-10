/*****************************
 *  Prekladac jazyka IF2008
 *  Matus Kontra (xkontr00) (veduci)
 *  Jozef Kobrtek (xkobrt00)
 *  Patrik Seben (xseben00)
 *  Matej Smetana (xsmeta02)
******************************/ 

#ifndef ILIST_H
#define ILIST_H
#include "bvs.h"
#include "str.h"
#include "codes.h"


//vycet OPcodov
typedef enum 
{
  I_SET,I_ADD,I_SUB,I_MUL,I_DIV,I_CMPEQ,I_CMPNEQ,I_CMPGT,I_CMPGE,I_CMPLT,I_CMPLE,
  I_SORT, I_LEN,I_FIND,I_CONCAT,
  I_READ, I_WRITE,
  I_LBL, I_JMP, I_JNZ, I_JZ,
  I_RET
} opcodes;

//typy v instrukcii
typedef enum 
{
  NO_OP = 0,
  TABLEENTRY_OP_TYPE = 11, ADDRESS_OP_TYPE = 111 , INT_OP_TYPE = 1, DBL_OP_TYPE = 2, STR_OP_TYPE = 3
} op_type_spec;


//unia toho co moze byt operand
typedef union 
{

    Table_Entry id;
    struct tin * add;
    int i;
    double dbl;
    string * str;

} operand;

//typ instrukcie
typedef struct tinst {
  int opcode;
  
  int op1type;
  int op2type;
  int op3type;
  
  operand op1, op2, op3;
  
} Instr, *ptr_Instr;

//typ polozky zoznamu
typedef struct tin {
  Instr cont;
  struct tin * next;
} InstEntry, *ptr_InstEntry;

//typ zoznamu
typedef struct til {
  ptr_InstEntry head;
  ptr_InstEntry tail;
  ptr_InstEntry virtual_ip;
} IList, *ptr_IList;

//Procedury...
void InitIList(ptr_IList ilist_ptr);
void DestroyIList( ptr_IList ilist_ptr);

ptr_InstEntry emitInstr(ptr_IList ilist_ptr, int opcode, int op1type, void * op1 , int op2type, void * op2 ,int op3type, void * op3);
ptr_InstEntry AdvanceIP(ptr_IList ilist_ptr);
ptr_InstEntry ResetIP(ptr_IList ilist_ptr);
void setVirtualIP(ptr_IList ilist_ptr, ptr_InstEntry newIP);
ptr_InstEntry getVirtualIP(ptr_IList ilist_ptr);

void printList(ptr_IList ilist_ptr);
 
#endif //ILIST_H
