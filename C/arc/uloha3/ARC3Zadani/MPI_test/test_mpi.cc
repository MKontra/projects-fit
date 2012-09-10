//+-------------------------------------------------------------+//
//|                                                             |//
//|            Jednoduchy testovaci prikladek v MPI             |//
//|                                                             |//
//|  Autor: Jiøí Jaro¹ 2007                                     |//
//|         jarosjir@fit.vutbr.cz                               |//
//|                                                             |//	
//| Kazdy proces nejprve vypise svoje ID a celkovy pocet        |//	
//| procesu, ktery se podili na paralelnim vypoctu              |//	
//|                                                             |//	
//| Kazdy proces nyni vygeneruje a zasle svemu sousedovi        |//	
//| jednoduchou zpravu, ten tuto zpravu prijme a zpracuje       |//
//|                                                             |//	
//| Uzly jsou zapojeny do kruhu - zasila se vzdy uzlu s ID      |//	
//| o 1 vyssim                                                  |//		
//|                                                             |//		
//+-------------------------------------------------------------+//

#include <iostream>
#include <mpi.h>
#include <stdio.h>

using namespace std;

const int MPI_TAG_Zprava_Sousedovi = 100; 
const int DelkaZpravy = 100;


void VypisKodJsi(int ProcID, int ProcCount);
void ZasliZpravu(const char* zprava, int delka_zpravy, int od, int komu);
void PrijmyZpravu(int kdo, int od, int delka_zpravy, char* zprava);

int GetSousedVlevo(int ProcID, int ProcCount);
int GetSousedVpravo(int ProcID, int ProcCount);



void VypisKodJsi(int ProcID, int ProcCount){
  //-- funkce vypise kdo jsem a kolik nas je --//
  cout << "Ahoj ja jsem " << ProcID <<" z celkového poètu " << ProcCount << " procesu" << endl;
} //-- VypisKodJsi --//


void ZasliZpravu(const char* zprava, int delka_zpravy, int od, int komu){
  //-- Zasle zpravu sousedovi s o 1 vyssim ID--//


    //-- odeslani zpravy -- //
  MPI_Send((void*)zprava,delka_zpravy,MPI_CHAR,komu,MPI_TAG_Zprava_Sousedovi,MPI_COMM_WORLD); // Poslu vpravo

    //-- vypis hlasky--//
  cout << "Proces " << od << " prave odeslal zpravu procesu "<< komu  << endl;
  
  
} //- ZasliZpravu --//


void PrijmyZpravu(int kdo, int od, int delka_zpravy, char** zprava){
  //-- Prijme zpravu od od uzlu s ID o 1 nizsim --//


  MPI_Status status;  

    //-- Prijmuti zpravy --//
  MPI_Recv(*zprava,delka_zpravy,MPI_CHAR,od,MPI_TAG_Zprava_Sousedovi,MPI_COMM_WORLD,&status); //od odesilatele, ktery me ma v pravo

    //-- Vypis hlasky --//  
  cout << "Proces " << kdo << " prave prijal zpravu od procesu "<< od <<  endl;
} //-- PrijmyZpravu --//


int GetSousedVlevo(int ProcID, int ProcCount){
  // -- vrati souseda vlevo --//

  return (ProcID ==0) ?  ProcCount -1 : ProcID -1;
} //-- GetSousedVlevo --//



int GetSousedVpravo(int ProcID, int ProcCount){
  //-- vrati souseda vpravo --//
  
  return (ProcID + 1) % ProcCount;  
} //-- GetSousedVpravo --//




int main(int argc, char** argv)
{
  int ProcCount; //-- Pocet procesoru se kterymi pracuji --//
  int ProcID;    //-- moje ID				--//

    //-- inicializace MPI --//
  MPI_Init(&argc,&argv);

  MPI_Comm_size(MPI_COMM_WORLD,&ProcCount);
  MPI_Comm_rank(MPI_COMM_WORLD,&ProcID);
   
    //-- Vypise kdo jsem --//
  VypisKodJsi(ProcID,ProcCount);
 
 
  char* Zprava = new char[DelkaZpravy];

  //-- zaslani zprav s vyloucenim dealocku //
  //-- sude uzly vysilaji a liche prijmaji --//
  if ((ProcID % 2) == 0) {     
     sprintf(Zprava,"%s %d %s %d","Ahoj ",GetSousedVpravo(ProcID,ProcCount), " ja jsem ",ProcID );
     cout << ProcID << " prave vygenerovala zpravu: " << Zprava << endl;
     ZasliZpravu(Zprava, DelkaZpravy, ProcID, GetSousedVpravo(ProcID,ProcCount));
  }
  if ((ProcID % 2) == 1) {
    PrijmyZpravu(ProcID,GetSousedVlevo(ProcID,ProcCount),DelkaZpravy,&Zprava);
    cout << ProcID << " prave zpracovala zpravu: " << Zprava << endl;
  }  

  
  //-- liche uzly vysilaji a sude prijmaji --//
  if ((ProcID % 2) == 1) {     
     sprintf(Zprava,"%s %d %s %d","Ahoj ",GetSousedVpravo(ProcID,ProcCount), " ja jsem ", ProcID);
     cout << ProcID << " prave vygenerovala zpravu: " << Zprava << endl;
     ZasliZpravu(Zprava, DelkaZpravy, ProcID, GetSousedVpravo(ProcID,ProcCount));
  }
  if ((ProcID % 2) == 0) {
    PrijmyZpravu(ProcID,GetSousedVlevo(ProcID,ProcCount),DelkaZpravy, &Zprava);
    cout << ProcID << " prave zpracovala zpravu: " << Zprava << endl;
  }  
  
  
  //-- sude uzly vysilaji a liche prijmaji --//  

  delete [] Zprava;
  
  
  cout << ProcID << " prave dokoncila algoritmus " << endl;
   //-- Finalizace MPI --//
  MPI_Finalize();
}

