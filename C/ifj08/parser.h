/*****************************
 *  Prekladac jazyka IF2008
 *  Matus Kontra (xkontr00) (veduci)
 *  Jozef Kobrtek (xkobrt00)
 *  Patrik Seben (xseben00)
 *  Matej Smetana (xsmeta02)
******************************/ 

#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include "str.h"
#include "parser.h"
#include "bvs.h"
#include "lexer.h"
#include "ilist.h"
#include "codes.h"
#include "exprparser.h"

//hlavna funkcia vrati chybovy stav
//ak vsetko prebehlo v poriadku obsahuje tabulka symbolov platne zaznamy 
//a zoznam instrukcii je pripraveny na vykonanie

int parse(FILE * source, Sym_Tab symtab, ptr_IList instlist);

//utility functions
string * uniqueID();
Table_Entry newVariabile(int type);

//LL(1) grammar / Recursive descent parsing fncs.
//for non-terminals

int prog(); //pociatocny nont.

int oth_decls();

int int_decl_or_main_def();
int id_or_main();

int comp_stmnt();
int stmnt_list();

int stmnt();

int in_id_list();
int out_expr_list();

int else_part();

#endif //PARSER_H
