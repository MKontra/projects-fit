/*****************************
 *  Prekladac jazyka IF2008
 *  Matus Kontra (xkontr00) (veduci)
 *  Jozef Kobrtek (xkobrt00)
 *  Patrik Seben (xseben00)
 *  Matej Smetana (xsmeta02)
******************************/ 

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <malloc.h>
#include "parser.h"
#include "str.h"
#include "parser.h"
#include "bvs.h"
#include "lexer.h"
#include "ilist.h"
#include "codes.h"
#include "exprparser.h"
#include "interpret.h"

int
main(int argc, char *argv[])
{
  FILE * src = NULL;
  if ( argc >= 1 )
  {
      src = fopen(argv[1],"r");
      if (src == NULL)
        return FILE_ERROR;
      
  } else
  {
    return FILE_ERROR;
  }

  int err_flg = 0;
  Sym_Tab table = createTree();
  IList instlist;
  InitIList(&instlist);

  if ( (err_flg=parse(src, table, &instlist)) != PARSE_OK )
  {
    DestroyIList(&instlist);
    destroy(&table);  
    fclose(src);
    return err_flg;
  }
  //printList(&instlist);
  err_flg=runIList(&instlist,table);
  if (err_flg == RUNTIME_ERR)
  {
    DestroyIList(&instlist);
    destroy(&table);
    fclose(src);
    return RUNTIME_ERR;
  }


  fclose(src);
  DestroyIList(&instlist);
  destroy(&table);
  return OP_OK;

}
