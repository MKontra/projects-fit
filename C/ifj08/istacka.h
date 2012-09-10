/*****************************
 *  Prekladac jazyka IF2008
 *  Matus Kontra (xkontr00) (veduci)
 *  Jozef Kobrtek (xkobrt00)
 *  Patrik Seben (xseben00)
 *  Matej Smetana (xsmeta02)
******************************/  

#ifndef STACK_H
#define STACK_H

//Jednoduchy zasobnik nad dynamickym polom
//Pridana maoznost ziskania ukazatela na zaciatok
//aby bol umozneny prechod do hlbky

#define ALLOC_BLOCK_MULT 8 //narast po 8 nasobkoch zakladnej velkosti
#define STACK_OK 0
#define STACK_ERROR -1

typedef struct stackA {
  int elem_size;
  int top_index;
  int alloc_size;
  
  void * data;
  } *ptr_IStackA, IStackA;
  
//vytvorenie a znicenie  
ptr_IStackA newStack( int unit_size );
int StackInit ( ptr_IStackA stck, int unit_size);
int destroyStack( ptr_IStackA stck );

//push, pop, top, empty - zakladne operacie
int Spush( ptr_IStackA stck, void * data ); //ulozi na zasobnik data o velkosti elem_size z pamete danej ukazattelom
void Spop ( ptr_IStackA stck );
void * Stop ( ptr_IStackA stck );
int SEmpty ( ptr_IStackA stck );

//"dalsie nestandardne operacie"
int Sdepth ( ptr_IStackA stck );
void * SgetTopPtr ( ptr_IStackA stck ); //ukazatel na vrch
void * StopPopCpy ( ptr_IStackA stck ); //okopiruje polozku vrati ukazatel a odstrani zo zasobnika

#endif //STACK_H
