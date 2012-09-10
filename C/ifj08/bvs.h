/*****************************
 *  Prekladac jazyka IF2008
 *  Matus Kontra (xkontr00) (veduci)
 *  Jozef Kobrtek (xkobrt00)
 *  Patrik Seben (xseben00)
 *  Matej Smetana (xsmeta02)
******************************/ 

#ifndef BVS_H
#define BVS_H

#include <stdlib.h>
#include "str.h"

#define TREE_OK 0
#define TREE_ERROR -1

//vycet typov symbolu
typedef enum  
{
  INT_TYPE = 1, DBL_TYPE, STR_TYPE
} type_spec;

//typ unia obsahu symbolu
typedef union
{
    string * str;
    double dbl;
    int i;
} sym_cont;

//typ symbolu
typedef struct symbol 
{
  string * id_name;     //meno premennej
  int type;             //ciselko, float, string....

  int user_defined;     //uzivatelska alebo pomocna
  
  sym_cont value;       //obsah
  
} *ptr_symbol, symbol, *Table_Entry;

//noda stromu
typedef struct node 
{   
  symbol data;          //data symbolu
  
  struct node * left;   //lptr..
  struct node * right;  //rptr..
} *node_ptr;


//**Tabulka symbolov ako strom, obsahuje len korenovu nodu**
typedef struct bvs 
{
  
  node_ptr root;    //ukazaetel na koren
  
} *bvs_ptr, *Sym_Tab;
  
//inicializacia
bvs_ptr createTree();
int initTree( bvs_ptr tree); 

//vkladanie a vyhladanie
Table_Entry bvs_insert ( bvs_ptr tree, string * name );
Table_Entry bvs_lookup ( bvs_ptr tree, string * name );

//znycenie
int clear( bvs_ptr tree );
int destroy ( bvs_ptr * to_tree );

//testovaci rekurzivny inorder
void dumpSortedTree( bvs_ptr tree );

#endif //BVS_H
