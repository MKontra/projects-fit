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

/**
 *  LL(1) Parser
 *  Vytvoreny na zaklade specifikacie zaznamenanej  v dokumentacii.
 *  Pre kazdy non-terminal obsahuje modul funkciu ktora reprezentuje jednotlyve pravidla.
 *  Rozhranie s lexikalnym analyzatorom tvori funkcia int getNextToken(string *) ktora vracia dalsi token zo vstupu
 *  Modul dajel vyuziva tabuku symbolov ( funkcie bvs_insert a bvs_lookup )
 *  Instrukcie sa generuju do globalneho IListu funkciou emitInstr
 *  Vyrazy su spracovavane v module exprparser ktory je volany funkciou parseExpr()
**/       

//spolocne premenne uchovavajuce sucasnu polohu a atribut v parsovani
int token;        //current token
string attr;      //token attribute
Sym_Tab table;    //symbol table
ptr_IList ilist;  //instruction list

int count = 1;    //pocitadlo pouzite pri generovani novych pom. premennych

//******************************************************************************

// tato funkcia spusta ll parser pozaduje otvoreny subor a ukazatele na pracovne struktury
int
parse(FILE * source, Sym_Tab symtab, ptr_IList instlist)
{

  table = symtab; //nastavime si premenne
  ilist = instlist;
  
  if ( table == NULL || instlist == NULL )
    return printDesc( SYSTEM_ERR , "Neplatne ukazatele");
  
  loadSourceFile(source);     //nahrame ukazatel subora lexeru
  
  strInit(&attr);  //pripravenie retazca
  
  if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ ); //nahrame prvy token
  
  int code = prog();    //a pustime parsovanie
  
  strFree(&attr); //nakoniec uvolnime
  
  if ( (code == PARSE_OK) && (token != END_OF_FILE))
    return printDesc( SYNTAX_ERR, "Input continues after end of main" );
    
  return code;  //vratime kod
  
}

//******************************************************************************

//utility functions - impl
//user defined implicitne nula
//funcia vytvori novu pOMOCNU PREMENNU a nahra ju do tabulky symbolov + vrati ukazatel
Table_Entry 
newVariabile(int type)
{
  string * newname = uniqueID();                    //novy unikatny nazov      
  Table_Entry retval = bvs_insert(table, newname);  //vlozime
  strFree(newname);                                 //uvolnime docasny retazec
  free(newname);                                    
  retval->user_defined = 0;                         //nie uzivatelska
  retval->type = type;                              //typ
  return retval;                                    //vratenie ukazatela
  
}


//funkcia generuje z pocitadla unkatny nazov
//uVar#--num--
string * 
uniqueID()
{
  string * newstr = strNewFromConstChar( "uVar#");  //novy retazec
  
  for ( int app = count; app > 0; app /= 10 )       //pripojime cisla na koniec(obratene poradie)
      strAddChar(newstr, (app % 10) + '0' );
      
  count++;                                          //zvysenie globalneho ukazatela
  return newstr;                                    //vratime

}

//******************************************************************************
// FUNKCIE REPREZENTUJUCE NETERMINALY


// <prog> ::= <oth_decls>
// <prog> ::= <int_decl_or_main_def>
 
int 
prog()
{
  
  switch (token)
  
  {
  
    case DOUBLE : 
    case STRING :
      ;
      return oth_decls();
      
    case INTEGER :
      ;
      return int_decl_or_main_def();
      
    default :
      return printDesc( SYNTAX_ERR, "Unexpected token excepted type specifier" );
      
  }
  
}

//<oth_decls> ::= double id;
//<oth_decls> ::= string id;

int 
oth_decls()
{
  
  switch (token)
  
  {
  
    case DOUBLE : 
      ;
      //vypytame si dalsi token a skontrolujeme ci je typu ID
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
      if ( token != ID ) return printDesc( SYNTAX_ERR, "Unexpected token excepted identifier" );
      
      //vyhladame v tabulke ak najdeme => redefinicia semanticka chyba
      if ( bvs_lookup (table, &attr) != NULL ) return printDesc( SEMANTIC_ERR, "Multiple declarations of same variabile..." );
      
      //vkladame do tabulky a nastavyme atributy symbolu
      Table_Entry new_var_dbl = bvs_insert(table, &attr);
      new_var_dbl->type = DBL_TYPE;
      new_var_dbl->value.dbl = 0; //nula
      new_var_dbl->user_defined = 1;
      
      //skontrolujeme strednik...
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
      if ( token != SEMICOLON ) return printDesc( SYNTAX_ERR, "Unexpected token excepted semicolon");
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
      
      //pokracujeme v parsovani...
      return prog();   

    case STRING :
      ;
      //vypytame si dalsi token a skontrolujeme ci je typu ID
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
      if ( token != ID ) return printDesc( SYNTAX_ERR, "Unexpected token excepted identifier" );
      
      //vyhladame v tabulke ak najdeme => redefinicia semanticka chyba
      if ( bvs_lookup (table, &attr) != NULL ) return printDesc( SEMANTIC_ERR, "Multiple declarations of same variabile..." );
      
      //vkladame do tabulky a nastavyme atributy symbolu
      Table_Entry new_var_str = bvs_insert(table, &attr);
      new_var_str->type = STR_TYPE;
      new_var_str->value.str = newStr(); //prazdny retazec
      new_var_str->user_defined = 1;
      
      //skontrolujeme strednik...
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
      if ( token != SEMICOLON ) return printDesc( SYNTAX_ERR, "Unexpected token excepted semicolon");
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
      
      //parsujeme dalej...
      return prog();
           
    default :
      ;
      //sem by sme sa teoreticky nemali dostat
      return printDesc( SYNTAX_ERR, (char *) "Unexpected token" );
      
  }
  
}

//<int_decl_or_main_def> ::= int <id_or_main>

int
int_decl_or_main_def()
{
  
  switch (token)
  
  {
  
    case INTEGER : 
      ;
      
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );

      return id_or_main();
      
    default : return printDesc( SYNTAX_ERR, (char *) "Unexpected token" ); 
      
  }
  
}

//<id_or_main> ::= id;
//<id_or_main> ::= main() <comp_stmnt>

int
id_or_main()
{
 

int err_code = 0; 
  
  switch (token)
  
  {
  
    case ID : 
      ;
      //vyhladame v tabulke ak najdeme => redefinicia semanticka chyba
      if ( bvs_lookup (table, &attr) != NULL ) return printDesc( SEMANTIC_ERR, "Multiple declarations of same variabile..." );
      
      //redefinicia = semanticka chyba
      Table_Entry new_var_int = bvs_insert(table, &attr);
      new_var_int->type = INT_TYPE;
      new_var_int->value.i = 0; //nula
      new_var_int->user_defined = 1;
      
      //kontrola bodkociarky
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
      if ( token != SEMICOLON ) return printDesc( SYNTAX_ERR, NULL);
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
      
      //pokracujeme v parsovani
      return prog();
      
    case MAIN :
      ;
      //primeme lavu a pravu zatvorku
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
      if ( token != LEFT_BRACKET ) return printDesc( SYNTAX_ERR, NULL);
      
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
      if ( token != RIGHT_BRACKET ) return printDesc( SYNTAX_ERR, NULL);
      
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );

      //sparsujeme zlozeny prikaz {}
      if ( ( err_code = comp_stmnt() ) != PARSE_OK ) return err_code;
      
      //main je konec
      if ( token != END_OF_FILE ) return printDesc( SYNTAX_ERR, (char *) "Unexpected symbols after main()" );
      
      return PARSE_OK;
      
    default : return printDesc( SYNTAX_ERR, (char *) "Unexpected token after main" );
    
    
  }
   

}

//<comp_stmnt> ::= { <stmnt_list> }

int 
comp_stmnt()
{

  int err_code = 0;
  
  switch (token)
  
  {
  
    case LEFT_VINCULUM :
      ; 
      //lava zlozena     
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
      //stmnt list
      if ( ( err_code = stmnt_list() ) != PARSE_OK ) return err_code;
      //prava
      if ( token != RIGHT_VINCULUM ) return printDesc( SYNTAX_ERR, "Excepted right bracket" );
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
      return PARSE_OK;
    
    default : return printDesc( SYNTAX_ERR, (char *) "Unexpected token in compoud_stmnt" );
      
  }

}

//<stmnt_list> ::= <stmnt> <stmnt_list>
//<stmnt_list> ::= e

int
stmnt_list()
{

  int err_code = 0;

  switch (token)
  
  {
  
    case RIGHT_VINCULUM :   
      ;
      //prazdny zoznam ( ukoncenie compd_stmntu) ; epsilon pravidlo
      return PARSE_OK;
     
    case LEFT_VINCULUM:
    case CIN:
    case COUT:  
    case IF:
    case WHILE:
    case RETURN:
    case LENGTH:
    case SORT:
    case FIND:
    case CONCAT:
    case ID:
    case CONST_STR :
    case CONST_DBL :
    case CONST_INT :
    case B_INT_CON :
    case O_INT_CON : 
    case H_INT_CON :
    case LEFT_BRACKET:
      ;
      //spracuvavame prikaz
      if ( ( err_code = stmnt() ) != PARSE_OK ) return err_code;
      return stmnt_list();

    
    default :
      return printDesc( SYNTAX_ERR, (char *) "Unexpected token in stmnt " );
      
  }

}

//<stmnt> ::= jeden z terminalov prikazu....
//vid dokumentacia

int
stmnt()
{
  int err_code = 0;

  switch (token)
  
  {
  
    case LEFT_VINCULUM :  
      ;
      //zlozeny prikaz ako prikaz
      return comp_stmnt();
      
    // vyraz;  
    case LENGTH:
    case SORT:
    case FIND:
    case CONCAT:
    case ID:
    case CONST_STR :
    case CONST_DBL :
    case CONST_INT :
    case B_INT_CON :
    case O_INT_CON : 
    case H_INT_CON :
    case LEFT_BRACKET:
    case RIGHT_BRACKET:
      ;
      //expression volame parser vyrazov
      Table_Entry tmp_expr = NULL;  
      if ( ( err_code = parseExpr(&tmp_expr) ) != PARSE_OK ) return err_code;
      //kontrola strednika
      if ( token != SEMICOLON) return printDesc( SYNTAX_ERR, (char *) "Semicolon expected" );
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
      return PARSE_OK;
      break;

    case CIN :
      ;
      //nacitanie zo vstupu
      Table_Entry tmp_cin = NULL;
     
      //nacita sa prvy << a id
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );  
      if ( token != IN ) return printDesc( SYNTAX_ERR, "Excepted >>" );   
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );  
      if ( token != ID ) return printDesc( SYNTAX_ERR, "Excepted identifier" );   
      
      //skontroluje sa ci je premenna deklarovana
      if ( (tmp_cin = bvs_lookup (table, &attr)) == NULL ) return printDesc( SEMANTIC_ERR, "Nondeclared variabile." );
      
      //vlozi sa instrukcia
      emitInstr(ilist,I_READ,NO_OP, NULL, NO_OP, NULL, TABLEENTRY_OP_TYPE, &(tmp_cin) );
      
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
      //parsuje sa zvysna cast
      return in_id_list();
      
    case COUT :
      ;
      //vystup na stdout
      Table_Entry tmp_cout = NULL;
      //prvy >> a id
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );  
      if ( token != OUT ) return printDesc( SYNTAX_ERR, "Excepted <<" );
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ ); 
      
      //sparsuje sa vyraz
      if ( ( err_code = parseExpr(&tmp_cout) ) != PARSE_OK ) return err_code;       
      
      //a vlozi instrukcia   
      emitInstr(ilist,I_WRITE,NO_OP, NULL, NO_OP, NULL, TABLEENTRY_OP_TYPE, &(tmp_cout) );     
       
      //zvysok zoznamu
      return out_expr_list();
      
    case WHILE :
      ;
      //cyklus while
      Table_Entry tmp_while = NULL;
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
      
      //navestie pred vyrazom
      ptr_InstEntry lbl1 = emitInstr(ilist,I_LBL,NO_OP, NULL, NO_OP, NULL,  NO_OP , NULL);
      
      //zpracovanie vyrazu
      if ( ( err_code = parseExpr(&tmp_while) ) != PARSE_OK ) return err_code;  
      
      //semanticka kontrola retazec nemoze byt pouzity ako podmienka
      if (tmp_while->type == STR_TYPE) return printDesc( SEMANTIC_ERR, "String as conditional statement" );
      
      //podmienka a skok ukoncenia cyklu     
      ptr_InstEntry jmp1 = emitInstr(ilist,I_JZ,  TABLEENTRY_OP_TYPE, &(tmp_while), NO_OP, NULL, ADDRESS_OP_TYPE, NULL );
      
      //telo cyklu
      if ( ( err_code = comp_stmnt() ) != PARSE_OK ) return err_code;  
      
      //skok na zaciatok
      emitInstr(ilist,I_JMP,NO_OP, NULL, NO_OP, NULL, ADDRESS_OP_TYPE, &lbl1);
      
      //navestie na konci
      ptr_InstEntry lbl2 = emitInstr(ilist,I_LBL,NO_OP, NULL, NO_OP, NULL, NO_OP, NULL );
      jmp1->cont.op3.add = lbl2;
      
      //if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
      return PARSE_OK;
      
    case IF :
      ;
      //podmienka if-else
      Table_Entry tmp_if = NULL;  
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );//zaciatok vyrazu
      
      if ( ( err_code = parseExpr(&tmp_if) ) != PARSE_OK ) return err_code; //vyraz
      if (tmp_if->type == STR_TYPE) return printDesc( SEMANTIC_ERR, "String as conditional statement" );
      
      ptr_InstEntry jmp1_if = emitInstr(ilist,I_JZ,  TABLEENTRY_OP_TYPE, &(tmp_if), NO_OP, NULL, ADDRESS_OP_TYPE, NULL );//ak 0 skok na elsepart
      
      if ( ( err_code = comp_stmnt() ) != PARSE_OK ) return err_code; // zjedz prikazy v pravde ifu
        
      ptr_InstEntry jmp2_if = emitInstr(ilist,I_JMP,NO_OP, NULL, NO_OP, NULL, ADDRESS_OP_TYPE, NULL); //jump na koniec   
      
      ptr_InstEntry lbl1_if = emitInstr(ilist,I_LBL,NO_OP, NULL, NO_OP, NULL,  NO_OP , NULL);//label pred elsom
      
      //parsnutie volitelneho else
      if ( ( err_code = else_part() ) != PARSE_OK ) return err_code;     
      
      ptr_InstEntry lbl2_if = emitInstr(ilist,I_LBL,NO_OP, NULL, NO_OP, NULL, NO_OP, NULL );//label na konci koli obskoceniu elsu
      
      jmp1_if->cont.op3.add = lbl1_if;
      jmp2_if->cont.op3.add = lbl2_if;
         
      return PARSE_OK;
      
    case RETURN :
      ;
      //prikaz navratu
      Table_Entry tmp_ret = NULL;
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
      //zpracovanie vyrazu
      if ( ( err_code = parseExpr(&tmp_ret) ) != PARSE_OK ) return err_code;    
      //musi byt int
      if ( tmp_ret->type != INT_TYPE ) return printDesc( SEMANTIC_ERR, "Return value must be integer" );
      //instrukcia
      emitInstr(ilist,I_RET,TABLEENTRY_OP_TYPE, &tmp_ret, NO_OP, NULL,  NO_OP , NULL);
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
      return PARSE_OK;  
                
             
    default : return printDesc( SYNTAX_ERR, (char *) "Unexpected token" );

  }

}

//<in_id_list> ::= >>id <in_id_list> 
//<in_id_list> ::= ;

int
in_id_list()
{

  switch (token)

  {
  
    case IN:
      ;      
      //zoznam vstupnych identifikatorov vid CIN
      Table_Entry tmp_cin = NULL;    
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );  
      if ( token != ID ) return printDesc( SYNTAX_ERR, "Excepted identifier" );   
      
      if ( (tmp_cin = bvs_lookup (table, &attr)) == NULL ) return printDesc( SEMANTIC_ERR, "Nondeclared variabile." );
      
      
      emitInstr(ilist,I_READ,NO_OP, NULL, NO_OP, NULL, TABLEENTRY_OP_TYPE, &(tmp_cin) );
      
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
      return in_id_list();

    case SEMICOLON : 
      ;
      //koniec zoznamu
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
      return PARSE_OK;
      
    default : return printDesc( SYNTAX_ERR, (char *) "Unexpected token" );
    
  }
  
}

// <out_expr_list> ::= << <expr> <out_expr_list>
// <out_expr_list> ::= ;

int
out_expr_list()
{
  int err_code = 0;

  switch (token)

  {
  
    case OUT:
      ; 
      //zoznam vystupnych vyrazov vid COUT
      Table_Entry tmp_cout = NULL;    
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );  

      if ( ( err_code = parseExpr(&tmp_cout) ) != PARSE_OK ) return err_code;      
      
      emitInstr(ilist,I_WRITE,NO_OP, NULL, NO_OP, NULL, TABLEENTRY_OP_TYPE, &(tmp_cout) );
      
      return out_expr_list();

    case SEMICOLON : 
      ;
      //koniec listu
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );
      return PARSE_OK;
      
    default : return printDesc( SYNTAX_ERR, (char *) "Unexpected token" );
    
  }
  
}

int
else_part()
{
  int err_code = 0;

  switch (token)

  {
  
    case ELSE:
      ;     
      //ak pride else zviazeme ho s najblizsim neuzavretym ifom (pokial nie je inac ozatvorkovany)
      if ( ( token = getNextToken(&attr) ) == LEX_ERROR ) return printDesc( LEX_ERROR, (char *) __func__ );  
    
      //sparsujeme telo elsu
      if ( ( err_code = comp_stmnt() ) != PARSE_OK ) return err_code; // jjj         
      
      return PARSE_OK;
      
    case RIGHT_VINCULUM :     
    case LEFT_VINCULUM:
    case CIN:
    case COUT:  
    case IF:
    case WHILE:
    case RETURN:
    case LENGTH:
    case SORT:
    case FIND:
    case CONCAT:
    case ID:
    case CONST_STR :
    case CONST_DBL :
    case CONST_INT :
    case B_INT_CON :
    case O_INT_CON : 
    case H_INT_CON :
    case LEFT_BRACKET:      
      ;
      return PARSE_OK;      
      
    default :
      return printDesc( SYNTAX_ERR, (char *) "Unexpected token" );

  }
  
}

//******************************************************************************
// KONIEC
