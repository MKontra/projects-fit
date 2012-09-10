/*
 * Autor: Matus Kontra (xkontr00)
 * Datum: 22.4.2010
 * Soubor: ahed.h
 * Komentar: implementacny subor kniznice
 */ 

#include "ahed.h"

#include <stdint.h>
#include <limits.h>
#include <cstdlib>
#include <cstdio> 
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <list>
#include <queue>
#include <stack>

using namespace std;

//Definicia hodnoty vlastneho eof
#define CUSTEOF 257

//dopredne deklaracie
class AHTreeNode;
class AHTree;

//trieda vykonavajuca citanie po bitoch
class bitstreamreader
{
    public:
      FILE * isp;
      uint8_t data;
      int counter;
    
      bitstreamreader(FILE * is)
      {
         isp = is;
         data = 0;
         counter = 0;
      }
    
      //nacitaj byte zo streamu a postupne vracaj bity
      int getBit()
      {
        if ( counter == 0 )
        {
          fread (&data, 1, 1, isp);
          if ( feof(isp) || ferror (isp) ) { counter = 0; return -1;};
          counter = 8;
        }
        counter--;
        int retval = data & 0x01;
        data >>= 1;
        return retval;
        
      }
      
      int getByte()
      {
         uint8_t retval = 0x00;
         for ( int i=0;i<8;i++ )
         {
          int bit = getBit();
          if (bit == -1) return -1;
          retval = retval | (bit<<i);
         }
         return retval; 
      }
      
      //vrati 9 bitov
      int getNine()
      {
         uint16_t retval = 0x00;
         for ( int i=0;i<9;i++ )
         {
          int bit = getBit();
          if (bit == -1) return -1;
          retval = retval | (bit<<i);
         }
         return retval; 
      }
    
};

//bitovi zapis
class bitstreamwriter
{
    public:
      FILE * osp;
      uint8_t data;
      int count;
      int bitswritten;
      int64_t byteswritten;
    
      bitstreamwriter(FILE * os)
      {
         osp = os;
         data = 0;
         count = 0;
         bitswritten = 0;
         byteswritten = 0;
      }
      //zapisuj bity do premennej, po 8 zapis byte do suboru
      void putBit(int bitval)
      {
          //cout << "adding bit " << (bitval == 1) << endl;
          data |= (bitval == 1) << count;
          count++;
          bitswritten++;
          if ( count == 8 )
          {
            //cout << endl;
            fwrite( &data, 1, 1 , osp );
            byteswritten++;
            data = 0;
            count = 0;
          }
          //data <<= 1;
      }
      
      void putByte(uint8_t bitval)
      {
         for ( int i=0;i<8;i++ )
         {
           putBit( bitval & 0x01 );
           bitval = bitval >> 1;
         }
      }
      
      void putNine(uint16_t bitval)
      {
         for ( int i=0;i<9;i++ )
         {
           putBit( bitval & 0x01 );
           bitval = bitval >> 1;
         }
      }
      
      //zapise ostavajuce bity padnute nulami
      void flush()
      {
          if ( count != 0 )
            fwrite( &data, 1, 1 , osp );
          bitswritten+=count;
          byteswritten++;
          //cout << "Zapisane " << bitswritten << endl;
      }
      
      int64_t getWrittenCount() { return byteswritten;}
};

//trieda reprezentujuca uzol stromu
class AHTreeNode
{
    public:
      AHTree * ownerTree; //ukazatel na strom    
      uint32_t index;     //poradie v trome vzhladom na surodenecke usporiadanie
      uint32_t chdata;    //ak som list, data o znaku
      uint64_t weight;    //vaha

      AHTreeNode * parent; 
      
      AHTreeNode * lchild; 
      AHTreeNode * rchild;
      
      AHTreeNode(AHTree * ot, AHTreeNode * p, uint32_t i, uint32_t ch, uint64_t w,
                  AHTreeNode * lc, AHTreeNode * rc ):
                 ownerTree(ot), parent(p), index(i), chdata(ch), weight(w), lchild(lc), rchild(rc) 
      {}  //konstruktor - priradenie dat
      
};

//strom, pre adaptivne huffmanovo kodovanie
class AHTree
{
  public:                 
    AHTreeNode * root;
    AHTreeNode * zeronode;  //noda pre NYT kod
            
    map<uint32_t, AHTreeNode*> charmap; //asociativne pole priradujuce znakom nody ( vyhladanie)
    map<uint32_t, AHTreeNode*> indexmap;//priradenie podla usporiadania  
    
    AHTree()        //konstruktor = prazdny strom, koren je zaroven NYT noda
    {
      root = new AHTreeNode(this, NULL, 0, -1, 0, NULL, NULL);
      zeronode = root; 
      indexmap[0] = root;
    }

    /**
     * Metoda, ktora prepocita indexy v usporiadani pre zvolenu nodu a jej potomkov
     * a tieto prida do mapy indexov     
     **/         
    void _updateIndexMapFor( AHTreeNode* from )
    {
      stack<AHTreeNode*> stc;
      stc.push( from );
      while ( !stc.empty() )
      {
           AHTreeNode* tmp = stc.top();
           stc.pop();
           if ( tmp->parent == NULL ) tmp->index = 0;
           else if ( tmp == tmp->parent->lchild ) tmp->index = 2*tmp->parent->index +2;
           else tmp->index = 2*tmp->parent->index +1;
           indexmap[tmp->index] = tmp;
           if ( tmp->lchild != NULL ) stc.push(tmp->lchild);
           if ( tmp->rchild != NULL ) stc.push(tmp->rchild);           
      }      
    }
    
    /**
     * Metoda, ktora vymaze nodu a jej potomkov z mapy indexov  
     **/ 
    void _eraseIndexMapFor( AHTreeNode* from )
    {
      stack<AHTreeNode*> stc;
      stc.push( from );
      while ( !stc.empty() )
      {
           AHTreeNode* tmp = stc.top();
           stc.pop();
           indexmap.erase(tmp->index);
           if ( tmp->lchild != NULL ) stc.push(tmp->lchild);
           if ( tmp->rchild != NULL ) stc.push(tmp->rchild);           
      }    
    }

    /**
     *Vymeni podstromy zacinajuce na from a to a zaroven udrzi indexy v konzistencii
     **/             
    void _swapNodes( AHTreeNode* from, AHTreeNode* to )
    {
      //cout << "Swapping nodes..." << from->index << " and " << to->index <<endl;
      //odstran z mapy indexov podstromi
      _eraseIndexMapFor(from);
      _eraseIndexMapFor(to);
      
      //prehod ukazatele :/
       AHTreeNode* fromparent = from->parent;
       AHTreeNode* toparent = to->parent;

       if ( from == from->parent->lchild ) from->parent->lchild = to;
       else from->parent->rchild = to;

       if ( to == to->parent->rchild ) to->parent->rchild = from;
       else to->parent->lchild = from;
       
       from->parent = toparent;
       to->parent = fromparent;
       

       //pridaj do mapy indexov 
       _updateIndexMapFor(from);
       _updateIndexMapFor(to);
    }
    
    //je noda pc potomkom nody pp ?
    bool _isAncessor( AHTreeNode* pc, AHTreeNode* pp )
    {
      AHTreeNode* tmp = pc;
      while ( tmp != root )
      {
          if ( tmp == pp ) return true;
          tmp = tmp->parent;
      }
      return false;
    }

    /**
     * Klucova metoda programu
     * Zisti pre nodu from, ci je pre nu zachovana vlastnost surodenectva, ak nie
     * vykona vymenu
     **/                   
    void _enforceSibilingProperty( AHTreeNode* from )
    {
      //koren - nie je co riesit
      if ( from == root ) return;      
      
      //cout << "updating tree..." << from->index <<endl;
      //kontrolujeme susedne nody ci je splnena surodenecka vlastnost
      uint32_t origin = from->index; 
      while ( !hasIndex(--origin) );
      if ( indexmap[origin]->weight > (from->weight + 1) )
      {
        return;  // ak je skonci
      }
      
      //cout << "lookup done " << origin << endl; 
      
      //najdenie poslednej nody s rovnakym vyssim poradovim cislom
      uint32_t last = origin;
      while ( --origin && hasIndex(origin) && (indexmap[origin]->weight == indexmap[last]->weight) );
      origin++;
      //cout << "lookup update " << origin << endl;
      
      //podla indexu najdi ukazatel na nodu 
      AHTreeNode* target = indexmap[origin];
      
      //ak noda nie je predkom a nie je root - men
      if ( !_isAncessor(from, target) && origin != 0 ) _swapNodes ( from, target );
    
      //cout << "updating tree done\n";
    }
   
    //prepocita vahy od zvoleneho listu ku korenu from - list, newWeight nova vaha
    void updateWeightsInTree(AHTreeNode * from, uint64_t newWeight)
    {
      uint64_t diff = from->weight - newWeight; 
      AHTreeNode* tmp = from;
      while ( tmp != root )
      {
          tmp->weight -= diff; 
          tmp = tmp->parent;
      } 
    }
    
    
    /**
     *zmena rozsahu meritka - vykonane ako bolo popisane v dokumentacii
     **/         
    void rescaleTree()
    {
        //zmen vahy pre listy
       map<uint32_t, AHTreeNode*>::iterator it;
       for ( it = charmap.begin();  it != charmap.end(); it++ )
       {
            AHTreeNode* current = (*it).second;
             updateWeightsInTree( current,  current->weight/2 );
  
       }
       
       //skontroluj ci bola zachovana surodenecka vlastnost, ak nie uprav strom
       for ( it = charmap.begin();  it != charmap.end(); it++ )
       {
            AHTreeNode* current = (*it).second;
             _enforceSibilingProperty( current );
            while ( 1 )
            {
                if ( current == root ) break;
                //dumpTree();
                current = current->parent; 
               _enforceSibilingProperty( current );  
            } 
        }  
    }
    
    /**
     *Uprav strom pri vlozeni symbolu alebo zmene vahy
     **/         
    void updateTree( uint32_t chr )
    {
       AHTreeNode * current = NULL; //ukazatel na nodu kde doslo k zmene  
       int needrecalc = 0;
       if ( !hasChar(chr) ) //ak tento znak nie je strome rozbi NYT a pridaj ho do stromu
       {
         /**
         AHTree * ownerTree; //ukazatel na strom    
         uint32_t index;     //poradie v trome vzhladom na surodenecke usporiadanie
         uint32_t chdata;    //ak som list, data o znaku
         uint64_t weight;    //vaha

         AHTreeNode * parent; 
      
         AHTreeNode * lchild; 
         AHTreeNode * rchild;
      
         AHTreeNode(AHTree * ot, AHTreeNode * p, uint32_t i, uint32_t ch, uint64_t w,
                  AHTreeNode * lc, AHTreeNode * rc ):
         ownerTree(ot), parent(p), index(i), chdata(ch), weight(w), lchild(lc), rchild(rc) 
         {}  //konstruktor - priradenie dat
         **/
         
         AHTreeNode * newlchld = new AHTreeNode(this, zeronode, (2*zeronode->index + 2) , -1, 0, NULL, NULL);
         AHTreeNode * newrchld = new AHTreeNode(this, zeronode, (2*zeronode->index + 1) , chr,1, NULL, NULL);
         zeronode->lchild = newlchld;
         zeronode->rchild = newrchld;
         
         charmap[chr] = newrchld; //pridaj znak do mapy
         indexmap[2*zeronode->index + 2] = newlchld;   
         indexmap[2*zeronode->index + 1] = newrchld;
         zeronode = newlchld; 
         current = newlchld->parent; //po pridani zacni kontrolovat od rodica
      } else //znak je v strome
      {
         current = charmap[chr];
         _enforceSibilingProperty( current );
         if ( current->weight > ( ((uint32_t)-1)/2 ) ) //zmena mierky pri polovici uint64/2 - miesta je dost ale ak sme sa dostali k polovici 64bit cisla asi nie...
          needrecalc = 1;
      }
      
      //postupuj smerom ku korenu a kontroluj strom, ak nie je nieco v poriadku vyries to
      while ( 1 )
      {
          (current->weight)++;
          if ( current == root ) break;
          //dumpTree();
          current = current->parent; 
         _enforceSibilingProperty( current );  
      }
      if ( needrecalc == 1 )
        rescaleTree();

    }

    //je dany znak uz v strome ?
    bool hasChar( uint32_t chr )
    {
        map<uint32_t, AHTreeNode*>::iterator it;
        it = charmap.find(chr);
        return !( it == charmap.end() ); 
    }
    
    //existuje index ? (naco ?)
    bool hasIndex( uint32_t inx )
    {
        map<uint32_t, AHTreeNode*>::iterator it;
        it = indexmap.find(inx);
        return !( it == indexmap.end() );
    }

    //zapis cestu pre dany uzol do bitoveho streamu
    void _outputNode( AHTreeNode* prm, bitstreamwriter * bsw )
    {
      //ukladaj cestu na zasobnik
      stack<int> stck;
      AHTreeNode* aht = prm;
      while ( aht != root )
      {
        if ( aht == aht->parent->rchild )
          stck.push(1);
        else
          stck.push(0);
        
        aht = aht->parent;
      }
      //natlac ju do streamu
      while ( !stck.empty() )
      {
          bsw->putBit(stck.top() );
          stck.pop();
      } 
    }
    
    //najde list pre znak a zapise cestu do streamu
    void outputPathForChar( uint32_t chr, bitstreamwriter * bsw )
    {
      if (!hasChar(chr) ) return;
      AHTreeNode* aht = charmap[chr];
      _outputNode(aht, bsw);       
    }
    
    //zapis NYT   
    void outputZeronode( bitstreamwriter * bsw )
    {
      AHTreeNode* aht = zeronode;
      _outputNode(aht, bsw); 
    }
    
    //zapis do streamu vlastny EOF
    void outputEnd( bitstreamwriter * bsw )
    {
      uint32_t eofnode = (uint32_t) CUSTEOF;
      outputZeronode(bsw);
      bsw->putNine(eofnode); 
      updateTree(eofnode);
    }
    
    //najdi list podla dat zo streamu
    int getNodeForInput( bitstreamreader * bsr )
    {
        AHTreeNode* aht = root;
        while ( aht->lchild != NULL && aht->rchild != NULL )
        {
            int isr = bsr->getBit();
            //cout << "bitval " << isr << endl;
            if ( isr == -1 )
            {
              //cout << "wtf" << endl;
              return -1;
            }
            if ( isr == 1 )
              aht = aht->rchild;
            else
              aht = aht->lchild;
        } 
        return aht->chdata;
    }
    
    //vypis obsah mapy znakov
    void dumpCharMap()
    {
        map<uint32_t, AHTreeNode*>::iterator it;
          for ( it=charmap.begin() ; it != charmap.end(); it++ )
            cout << (*it).first << " => " << (*it).second << endl; 
    }
    
    //vypis obsah 
    void dumpIndexMap()
    {
        map<uint32_t, AHTreeNode*>::iterator it;
          for ( it=indexmap.begin() ; it != indexmap.end(); it++ )
            cout << (*it).first << " => " << (*it).second << endl;    
    }
    
    //BFS, vykresli prve 4 urovne stromu do konzole
    void dumpTree()
    {
        queue<AHTreeNode *> myqueue;
        myqueue.push(root);
        int count = 0;
        int nextmax = 1;
        while ( !myqueue.empty() )
        {
            AHTreeNode * tmp = myqueue.front();
            myqueue.pop();
            if ( tmp != NULL)
            { 
              cout << tmp->index <<"-";
              cout << (char)tmp->chdata; 
              cout <<"-"<< tmp->weight<< "   ";
            } else
            {
              cout << "null   ";
            }
            
            if ( tmp != NULL)
            { myqueue.push(tmp->lchild); }
            else
            { myqueue.push(NULL); } 
               
            if ( tmp!= NULL) 
            { myqueue.push(tmp->rchild); }
            else
            { myqueue.push(NULL); } 
            
            count++;
            if ( count >= nextmax )
            {  
                cout << endl;
                nextmax = nextmax*2;
                if (nextmax >= 32) break;
                count = 0;
            }       
        }    
    }
    
    //vycistenie
    void disposeTree()
    {
      disposeNodeAndDescendants( root );    
    }
    
    //postorder cistenie nod
    void disposeNodeAndDescendants( AHTreeNode * from )
    {
      if ( from->lchild != NULL ) disposeNodeAndDescendants( from->lchild );
      if ( from->rchild != NULL ) disposeNodeAndDescendants( from->rchild );  
      delete from;
    }
    
};

//zakoduj zvoleny znak chr v strome at do stremu bsw
int encode( AHTree * at, bitstreamwriter * bsw, uint32_t chr )
{
  //cout << "encoding " << chr << " " << (int) chr << " " <<(char)chr << endl;
  //at->dumpTree();
  
  if ( !at->hasChar(chr)   )
  {
      //cout << "not in\n";
      at->outputZeronode(bsw);
      //cout << "zeronode is: " << at->zeronode->index << endl;
      bsw->putNine(chr);
  } else
  {
      //cout << "is there\n";
      at->outputPathForChar(chr, bsw);
  }
  at->updateTree(chr);
    //system("pause");
}

//ziskaj nasledujuci znak v strome at zo streamu bsr
int decode( AHTree * at, bitstreamreader * bsr )
{
  int retval = at->getNodeForInput (bsr);
  if ( retval == -1 )
  {
    retval = bsr->getNine();
  }
  at->updateTree(retval);
  return retval;
}

/* Nazev:
 *   AHEDEncoding
 * Cinnost:
 *   Funkce koduje vstupni soubor do vystupniho souboru a porizuje zaznam o kodovani.
 * Parametry:
 *   ahed - zaznam o kodovani
 *   inputFile - vstupni soubor (nekodovany)
 *   outputFile - vystupní soubor (kodovany)
 * Navratova hodnota: 
 *    0 - kodovani probehlo v poradku
 *    -1 - poi kodovani nastala chyba
 */
int AHEDEncoding(tAHED *ahed, FILE *inputFile, FILE *outputFile)
{
  bitstreamwriter bsw( outputFile );
  uint8_t inpt = 0;
  AHTree at;
  
  //v cykle koduj znaky 
  while ( 1 )
  {
    fread(&inpt, 1, 1, inputFile);
    
    if ( feof(inputFile) || ferror(inputFile) )  break;
    encode(&at, &bsw, inpt );   
  }
  
  at.outputEnd(&bsw);
  bsw.flush(); 
  ahed->codedSize = bsw.getWrittenCount();
  ahed->uncodedSize = ftell(inputFile);
  at.disposeTree();
	return AHEDOK;
}

/* Nazev:
 *   AHEDDecoding
 * Cinnost:
 *   Funkce dekoduje vstupni soubor do vystupniho souboru a porizuje zaznam o dekodovani.
 * Parametry:
 *   ahed - zaznam o dekodovani
 *   inputFile - vstupni soubor (kodovany)
 *   outputFile - vystupní soubor (nekodovany)
 * Navratova hodnota: 
 *    0 - dekodovani probehlo v poradku
 *    -1 - poi dekodovani nastala chyba
 */
int AHEDDecoding(tAHED *ahed, FILE *inputFile, FILE *outputFile)
{
  bitstreamreader bsr( inputFile );
  uint8_t oupt = 0;
  AHTree at;
  ahed->uncodedSize = 0;
  while ( 1 )
  {
      int decret = decode(&at, &bsr);
      if ( decret == -1 ) {at.disposeTree();return AHEDFail;};
      if ( decret == CUSTEOF )
      {
        ahed->codedSize = ftell(inputFile);
        at.disposeTree();
        return AHEDOK; }; 
      (ahed->uncodedSize)++;    
      oupt = (uint8_t) decret;
      fwrite(&oupt, 1, 1, outputFile);
  } 
  at.disposeTree();
	return AHEDOK;
}
