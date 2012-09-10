/*****************************
 *  Prekladac jazyka IF2008
 *  Matus Kontra (xkontr00) (veduci)
 *  Jozef Kobrtek (xkobrt00)
 *  Patrik Seben (xseben00)
 *  Matej Smetana (xsmeta02)
******************************/  

#ifndef INTERPRET_H
#define INTERPRET_H

#include "bvs.h"
#include "str.h"
#include "codes.h"
#include "ilist.h"

//primarna funkcia spusti vykonavanie IListu a pouzije tabulku symbolov z parametra
int runIList(ptr_IList ilist_ptr, Sym_Tab table);

//pomocne funkcie
sym_cont instr_to_sym(operand in, int type);
int fiop(int i1, int i2, int OP);
double fdop(double i1, double i2, int OP);
int ficom(int i1, int i2, int OP);
int fdcom(double i1, double i2, int OP);
int fscom(string* i1, string* i2, int OP);

#endif //INTERPRET_H
