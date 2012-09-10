#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <iostream>
#include <map>
#include <list>
#include <queue>
#include <algorithm>
#include <iterator>
#include <vector>
#include <math.h>

using namespace std;

//standardna priorita 0
#define DEFAULT_PRIORITY 0

//presnost porovnania na casovu zhodu udalosti
#define PRECISION 0.0001

class Event;
class Transaction;
class Simulator;

//premenne simulatora s informaciami o podstatnych casoch
extern float InitTime;
extern float EndTime;
extern float Time;
extern Simulator * Instance;

//generatory nahodnych cisel
float Exp(float x);
float Uniform(float x, float y);
float Random();

//Metody na pracu so simulatorom
void Init(float stime);
void Run();
void RunFor(float tlen);
string RunInfo();
void Finish();

//Zaradzovanie a rusenie udalosti
void Cancel( Event * e );
void ActivateSingleEvent ( Event * e, float when);
void ActivateTransaction ( Transaction * t, int event_id, float when);

//Implementacia prioritnej fronty - pouzivame container vector
// a algoritmy pre pracu s haldou
// rozsirena o moznost odobrania prvku - toto fronta standardne nema
template<typename T>
class custom_pqueue
{
  public:
  
    typedef typename vector<T>::iterator  vec_iter;
  
    custom_pqueue (){};
    ~custom_pqueue (){};
    const T& top() { return cont.front(); };
    void push(T a){cont.push_back(a); push_heap (cont.begin(),cont.end());  };
    void pop() {pop_heap (cont.begin(),cont.end()); cont.pop_back();};
    int size() { return cont.size();}
    
    void remove( vec_iter it )
    {
      cont.erase(it);
      heapify();
    };
    
    vector<T> * get_list_ptr() { return &cont; };
    void heapify() { make_heap (cont.begin(),cont.end());};

  private:
    vector<T> cont;

};

/*
  Trieda reprezentujuca vseobecnu udalost - zmenu stavu systemu
  
  Udalosti logicky paria k istim transakciam - avsak nie vzdy.
  
  Spravanie udalosti urcuje metoda Act();
*/

class Event
{        
  public:
    Event(int prio = DEFAULT_PRIORITY, Transaction * affects_state_of = 0 );
    virtual ~Event();
    
    virtual void Act();
    
    void ActivateMe(float time);  //Aktivuj tuto udalost v case time
    void ActivateSingle(Event * e, float time ) ; //aktivuj lubovolnu udalost
    void ActivateEventOfMyTransaction(int event_id, float time); //ak patrime k dakej transakcii -> mozeme aktivovat udalost na zaklade const.
    void ActivateEventOfOtherTransaction(Transaction * t, int event_id, float time);
    
    void ExecuteTransEvent( int event_id ); //vykonaj niektoru inu z udalosti ak parti k transakcii
    
    Transaction * affects_state_of;   //transakcia ktorej stav ovplivnujeme
    int get_priority(){ return p_prority; }
  protected:
    int p_prority;
}; 

/* trieda reprezentujuca proces
  - pozostava zo sady udalosti ktore reprezentuju zmeny stavu
  - dost potrebna na zobrazenie logiky obsadenia zariadeni
*/

class Transaction
{
  public:   
    Transaction(int prio = DEFAULT_PRIORITY);
    virtual ~Transaction();
    void registerEvent(int eventID, Event * eventptr);
    Event * dispatcher(int eventID);
    int get_tid() { return tid; };

    void Activate(int event_id, float time);
    
  private:
    static int id_counter;
    int tid;
    map<int, Event*> * affectingEvents;
      
  protected:
    int p_defeventprio;
};

/*
  Fronta pouzita na ulozenie cakajucich poziadavkou o obsluhu
*/

typedef pair<Transaction *, int> qsched;
typedef pair<int, float> compi;
typedef pair<compi, qsched> qentry;

class SimQueue
{
  public:
    SimQueue(string n);
    ~SimQueue();
    
    void pushback( Transaction * t, int event_id, int prio = DEFAULT_PRIORITY );
    void popfront();
    qentry * top();
    int size();
    
    string getStatistics();
  
  private:
    custom_pqueue<qentry> * intq;
    string name;
    int pushnum;
    int popnum;
    int maxlen;
    struct qstat
    {
      float arrtime;
      float deptime;
      int arrlen;
      int deplen;
    };
    map<int, qstat> * wstat;  
};

// facility stavy
enum FacStates
{
  IDLE,
  BUSY  
};


// trieda reprezentujuca linku
class Facility 
{
  public:
    Facility(string n);
    ~Facility();
    
    bool Seize(Transaction * t, int event_id, int prio = DEFAULT_PRIORITY);
    void Release(Transaction * t);
    
    bool empty();
    int waitingRequests();
    bool canRelease(Transaction * t);

    string getStatistics();
  
  private:
    string name;
    SimQueue * iq;
    int state;
    Transaction * bserviced;
    float currreltime;
    
    int totrequests;
    int totalserviced;

    
    float lastservicestarttime;
    float busytime;
           

};

//Store
class Store
{
  public:
    Store(string n, int sizep);
    ~Store();
    
    bool Enter(Transaction * t, int event_id, int prio = DEFAULT_PRIORITY);
    void Leave(Transaction * t);

    bool empty();
    int waitingRequests();
    bool canRelease(Transaction * t);
  
    string getStatistics();
  
  private:
    string name;
    int size;
    SimQueue * iq;
    list<Transaction *> * tin;
    Transaction * bserviced;
    
    int totrequests;
    int totalserviced;
    int maxutil;
            
    float lastservicestarttime;
    float busytime;
    
};

// Struktura aktivacneho zaznamu
struct ActivationEntry
{
  float when;
  int priority;
  Event * what;
  ActivationEntry(float when, int priority, Event * what):
    when(when), priority(priority), what(what) {};    
       
};

//operator ktory vyuziva fronta pri stavani haldy
extern bool operator<(const ActivationEntry &a, const ActivationEntry &b);

//Simulator - riadenie kalendarom udalost
class Simulator
{
  public:
    Simulator();
    ~Simulator();
    
    void scheduleEntry( float when, int priority, Event * what );
    void cancel( Event * what );
    
    void Run(); //spustenie simulatora - slucka kalendara
    string Stats();
    
  private:
    custom_pqueue<ActivationEntry> * event_list;
    int processed_events;

};

#endif
