/*****************************
 *  Prekladac jazyka IF2008
 *  Matus Kontra (xkontr00) (veduci)
 *  Jozef Kobrtek (xkobrt00)
 *  Patrik Seben (xseben00)
 *  Matej Smetana (xsmeta02)
******************************/ 

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "bvs.h"
#include "istacka.h"  //zasobnik pouzity pri ruseni stromu...

//---------------------------------------------------
//vytvorenie

bvs_ptr
createTree() 
//vytvory, naalokuje a inicializuje strom 
{
  bvs_ptr newptr = (bvs_ptr) malloc ( sizeof ( struct bvs ));
  if (newptr == NULL )
    return NULL;
  
  newptr -> root = NULL;
  return newptr;
}

int 
initTree( bvs_ptr tree)
//inicializuje strom 
{
  tree->root = NULL;
  return TREE_OK;
}  

//------------------------------------------------------------------------------
//Pomocne vytvorenie a zrusenie nody

node_ptr  //naalokuje a vrati nodu
create_node (string *name)
{
  node_ptr newnode = malloc (sizeof (struct node));
  
  if ( newnode == NULL )
    return NULL;
   
    newnode -> data.id_name = strNewFromConstChar( strGetStr(name) ); //skopiruje meno
    newnode -> data.value.str = NULL;                                 //nastavy sa str na NULL koli freeovaniu
  
  return newnode;
}

void
destroyNode(node_ptr ptr)
{
  
  strFree(ptr->data.id_name);         //uvolnime retazec meno
  free(ptr->data.id_name);            //uvolnime ukazatel nanho
  ptr->data.id_name = NULL;
  
  if ( ptr->data.type == STR_TYPE )   //ak je obshom retazec uvolnime a zrusime aj ten....
    if ( (ptr->data).value.str != NULL )
    {
      strFree((ptr->data).value.str);
      free((ptr->data).value.str);
      (ptr->data).value.str = NULL;
    }

}

//----------------------------------------------------
//nerekurzivny insert

Table_Entry
bvs_insert ( bvs_ptr tree, string *name )
{

  node_ptr back_ptr; //ukazatel na predchadzajuci prvok /otca
  node_ptr ptr;   //pomocny ukazatel
  bool iter_end;       //flag konca

  if (tree->root == NULL) //vytvorenie korena ak neexistuje
  {
    ptr = create_node( name );
    if (ptr == NULL)
      return NULL;
      
    ptr -> left = NULL;
    ptr -> right = NULL; 
    
    tree->root = ptr;
     
  } else
  {
    //pred vkladanim lokalizujeme
    iter_end = false;
    back_ptr = NULL;
    ptr = tree->root;
    do
    {
      back_ptr = ptr;
      if (  strCmpString(name, ptr->data.id_name) < 0 )
      {
        ptr=back_ptr->left;
      } else
      if (strCmpString(name, ptr->data.id_name ) > 0)
      {
        ptr=back_ptr->right;
      } else
      {
        iter_end = true;  //nasli sme
      }
    
    } while (iter_end!=true && (ptr!=NULL));
    
    if ( iter_end == false)  //ak sa nenaslo pridame
    {
      ptr = create_node( name );
      
      ptr -> left = NULL;
      ptr -> right = NULL; 
      
      if ( strCmpString(name, back_ptr->data.id_name) < 0)
        back_ptr->left = ptr;
      else
        back_ptr->right = ptr;  
      
    } else
      return NULL; //ak tam uz existuje vrat null;
  
  }

  return &(ptr->data);

}

//lookup
Table_Entry 
bvs_lookup ( bvs_ptr tree, string  * name )
{

  node_ptr ptr = tree->root; //pomocny ukazatel
  
  while ( ptr != NULL )
  {
    if (  strCmpString(name, ptr->data.id_name) < 0 )
      ptr=ptr->left;
    else
    if (  strCmpString(name, ptr->data.id_name) > 0 )
      ptr=ptr->right;
    else
      return &(ptr->data);
  }
  return NULL;
}

//------------------------------------------------------------
//clear or destroy
int 
clear( bvs_ptr tree )
{
  ptr_IStackA stack = newStack( sizeof(node_ptr) ); //zasobnik ukazatelov
  if ( stack == NULL )
    return TREE_ERROR;
    
    
  node_ptr curr_node = tree->root; // korenovy ukazatel
  node_ptr ptr;                    //pomocny ukazatel uzla
  
  while ( (curr_node != NULL) || (SEmpty(stack) != 1) )
  {
    if (curr_node == NULL){
      curr_node = *((node_ptr * )Stop(stack));
      Spop(stack);
      }
    else
    {
      if (curr_node->right != NULL)     //ulozenie praveeho podstromu
        Spush(stack, &(curr_node->right));
        
      ptr = curr_node;
      curr_node=curr_node->left;
      destroyNode(ptr);
      free(ptr);      
    }      
  }
  destroyStack(stack);
  free(stack);
  return TREE_OK;
}

int 
destroy ( bvs_ptr * to_tree ) //uvolni aj ukazatel naalokovany createTree...
{
  clear( (*to_tree) );
  free ( *to_tree );
  *to_tree = NULL;
  return TREE_OK;  
}

//------------------------------------------------------------
//testovaci inorder

void in_on_node( node_ptr root_node)
{
  if ( root_node == NULL)
    return;
  /**
  in_on_node(root_node->left);
  printf("Polozka Addresa %p <--> Kluc %s <--> %d Value: %d\n",root_node,strGetStr((root_node->data).id_name), root_node->data.type, root_node->data.value.i);
  if (root_node->data.type == STR_TYPE)
  {
    printf("-->%s<--\n",strGetStr(root_node->data.value.str));
  }
  in_on_node(root_node->right);
  **/
}

void
dumpSortedTree( bvs_ptr tree )
{
if ( tree->root == NULL)
  return;
  in_on_node( tree->root );  
}

