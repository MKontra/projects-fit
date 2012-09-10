#include <iostream>
#include "simulator.h"

using namespace std;

/**
 * Jednotlive udalosti meniace stav transakcie Customer - zakaznika cerpaciej stanice
 * 
**/  
enum Events
{
  Customer_Arrival,
  Customer_StartFueling,
  Customer_FuelingDone,
  Customer_StartPayment,
  Customer_EndPayment
};

/**
 *Mame 4 stojany
 *a 2 pokladne
**/
  
Store stanice("Stojany", 4);
Facility f1("Pokladna 1");

/**
 *udalost prichodu na cerp. stanicu
**/ 
class Arrival: public Event
{
  public: Arrival(int prio): Event(prio){};
  public: void Act()
  {
    /** pokus sa zabrat jeden zo stojanov **/
    // Ak je momentalne plno zacne cakat a v momente uvolnenia bude avolana udalost Customer_StartFueling
    if (stanice.Enter(affects_state_of, Customer_StartFueling))
    {
      ExecuteTransEvent(Customer_StartFueling); //stojan sme dostali tankujeme
    }
  }
};

class StartFueling: public Event
{
  public: StartFueling(int prio): Event(prio){};
  public: void Act()
  {
    //tankujeme priblizne 20-40 sekund
    ActivateEventOfMyTransaction(Customer_FuelingDone, Time + Uniform(20,40));
  }
};

class FuelingDone: public Event
{
  public: FuelingDone(int prio): Event(prio){};
  public: void Act()
  {
    stanice.Leave(affects_state_of);  //uvolnime stojan
    //Ak je pokladna volna tankujeme - inac je spustena udalost Customer_StartPayment po prijduti na rad
    if ( f1.Seize(affects_state_of, Customer_StartPayment) )
    {
      ExecuteTransEvent(Customer_StartPayment); //Pokladna volna tankujeme
    }
  }
};

class StartPayment: public Event
{
  public: StartPayment(int prio): Event(prio){};
  public: void Act()
  {
    ActivateEventOfMyTransaction(Customer_EndPayment, Time + 5); //platime 5 sekund
  }
};

class EndPayment: public Event
{
  public: EndPayment(int prio): Event(prio){};
  public: void Act()
  {
    //uvolnime pokladnu v ktorej sme cakali - mohli sme ulozit ukazatel do transakcie nebolo ny treba kontrolovat
    if (f1.canRelease(affects_state_of)) f1.Release(affects_state_of);
    //koniec procesu mazeme - objekty ufdalosti zmaze detruktor transakcie
    delete affects_state_of;
  }
};

class Customer: public Transaction
{
  public:
    Customer(int prio = DEFAULT_PRIORITY): Transaction(prio)
    {
      registerEvent(Customer_Arrival,new Arrival(prio)); 
      registerEvent(Customer_StartFueling,new StartFueling(prio));
      registerEvent(Customer_FuelingDone,new FuelingDone(prio)); 
      registerEvent(Customer_StartPayment,new StartPayment(prio));
      registerEvent(Customer_EndPayment,new EndPayment(prio)); 
    }; 


};

//udalost generatora - prichod zkaznika v intervaloch danim exponencialnim rozlozenim v strede 20
class Generator: public Event
{
  public: void Act()
  {
    //vygeneruj zakaznika
    Customer * c = new Customer();

    //Aktivuj jeho prvu udalost
    ActivateEventOfOtherTransaction(c, Customer_Arrival, Time);
    //Dalsi prichod o..
    ActivateMe(Time + Exp(1/10.0));  
  }
};

int main()
{ 
  Init(0);
  Event * ne = new Generator();
  ActivateSingleEvent(ne, 0);
  RunFor(1000);
  
  cout << RunInfo();
  cout << stanice.getStatistics(); 
  cout << f1.getStatistics();  
}
