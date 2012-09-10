#include <iostream>
#include <map>
#include <queue>
#include <sstream>
#include <iomanip>
#include <limits>
#include <stdlib.h>
#include <time.h>
#include "simulator.h"

using namespace std;

Transaction::Transaction(int prio): p_defeventprio(prio)
{
  affectingEvents = new map<int, Event *>();
  tid = id_counter;
  id_counter++;
}
Transaction::~Transaction()
{
  affectingEvents->clear();
  delete affectingEvents;
}

void Transaction::registerEvent(int eventID, Event * eventptr)
{
  if ( !eventptr )
  {
    cerr<<"NULL event pointer...\n";
    abort();
  }
  
  if ( eventptr->affects_state_of != 0 )
  {
    cerr<<"this instance of event is already part of transaction...\n";
    abort();  
  }
  
  eventptr->affects_state_of = this;
  
  (*affectingEvents)[eventID] = eventptr;
}

void Transaction::Activate(int event_id, float time)
{
  ActivateTransaction(const_cast<Transaction *>(this), event_id, time ) ;
}

Event * Transaction::dispatcher(int eventID)
{
  //implementuje vybratie metody z mapy a zavolanie spravania
  map<int, Event*>::iterator it;
  it = affectingEvents->find(eventID);
  if ( it == affectingEvents->end() )
  {
    cerr<<"Unknown event scheduled...\n";
    abort();
  }
  return ((*it).second);
}

Event::Event(int prio, Transaction * affects_state_of) :
 p_prority(prio), affects_state_of(affects_state_of)
{



}

Event::~Event()
{
}

void Event::ActivateMe(float time)
{
  ActivateSingleEvent(this, time );
}

void Event::ExecuteTransEvent( int event_id )
{
  if (!affects_state_of)
  {
    cerr<<"Event is not part of spec tranaction...\n";
    abort();  
  }
  
  affects_state_of->dispatcher(event_id)->Act();
  
}

void Event::ActivateSingle(Event * e, float time ) 
{
  ActivateSingleEvent(e, time );
}

void Event::ActivateEventOfMyTransaction(int event_id, float time)
{
  if (!affects_state_of)
  {
    cerr<<"Event is not part of spec tranaction...\n";
    abort();  
  }
  
  affects_state_of->Activate(event_id, time);
  
}

void Event::ActivateEventOfOtherTransaction(Transaction * t, int event_id, float time)
{
  if (!t)
  {
    cerr<<"Event is not part of spec tranaction...\n";
    abort();  
  }
  
  t->Activate(event_id, time);
  
}

void Event::Act()
{
}

int Transaction::id_counter = 0;  //inicializuj staticke pocitadlo

typedef pair<Transaction *, int> qsched;
typedef pair<int, float> compi;
typedef pair<compi, qsched> qentry;

bool operator<(const qentry &a, const qentry &b)
{
  if ( a.first.first == b.first.first )  //rovnake priority
  {
    return a.first.second > b.first.second;
  } else
  if ( a.first.first > b.first.first )
  {
    return true;
  } else 
    return false;
};

SimQueue::SimQueue(string n): name(n)
{
  maxlen = pushnum = popnum = 0;
  wstat = new map<int, qstat>();
  intq = new custom_pqueue<qentry>();   
}

SimQueue::~SimQueue()
{
  delete(wstat);
  delete(intq);  
}

void SimQueue::pushback( Transaction * t, int event_id, int prio )
{
  if (!t)
  {
    cerr<<"NULL transaction enqueued...\n";
    abort();
  }

  qstat ns = { Time, -1, intq->size(), -1};
  (*wstat)[t->get_tid()] = ns;
  
  qsched nsc(t, event_id);
  compi nqi(prio, Time);
  qentry nw(nqi, nsc);
  intq->push(nw);
  
  pushnum++;
  if ( intq->size() > maxlen )
    maxlen = intq->size();
}

qentry * SimQueue::top()
{
  if ( intq->size() > 0 )
    return const_cast<qentry *> (&intq->top());
  return NULL;  
}

int SimQueue::size()
{
  return intq->size();  
}

void SimQueue::popfront()
{
  if ( intq->size() <= 0 )
  {
    cerr<<"Empty queue...\n";
    abort();
  }

  popnum++;

  (*wstat)[intq->top().second.first->get_tid()].deptime = Time;
  (*wstat)[intq->top().second.first->get_tid()].deplen = intq->size();

  intq->pop();  
  
}

string SimQueue::getStatistics()
{
  stringstream sstr;
  
  map<int, qstat>::iterator it; 
  
  float minwait = 0;
  float maxwait = numeric_limits<float>::max();
  float avgwaitcnt = 0;
  
  float avglencnt = 0;
  
  it=wstat->begin();
  
  if ( it == wstat->end() )
    return string("queue unused...\n");
        
  avgwaitcnt = maxwait = minwait = ( (*it).second.deptime - (*it).second.arrtime );
  avglencnt = (*it).second.arrlen;
  

  

  it++;
  for ( ; it != wstat->end(); it++ )
  {
  
    if ( !( (*it).second.deptime == -1) )
    {
  
    float deltat = ( (*it).second.deptime - (*it).second.arrtime );
    float deltal = ( (*it).second.deplen - (*it).second.arrlen );
    
    if ( deltat < minwait )
      minwait = deltat;
    if ( deltat > maxwait )
      maxwait = deltat;
      
      avgwaitcnt +=deltat; 
        
      avglencnt += (*it).second.arrlen;
      avglencnt += (*it).second.deplen;
            
    } else
    {
      avglencnt += (*it).second.arrlen;
     } 
  }
  sstr << "--------------------------------\nName: ";
  sstr << name;
  sstr << "\nIncoming requests: ";
  sstr << pushnum ;
  sstr <<  "\nOutcoming requests: ";
  sstr << popnum ;
  sstr <<  "\nMaximum queued: ";
  sstr << maxlen ;
  sstr << "\nAverage queue length: " << avglencnt/((pushnum+popnum)+0.0) << endl;
  sstr << "\n"; 
  sstr << "Minimal waiting time: " << minwait << endl;
  sstr << "Maximum waiting time: " << maxwait << endl;  
  sstr << "Average waiting time: " << avgwaitcnt/popnum << endl;
  sstr << "--------------------------------\n";    
  /**
  for ( it=wstat->begin() ; it != wstat->end(); it++ )
  {
    sstr << "Trans ID: " << (*it).first; 
    sstr<< "     ArrivalTime: " << (*it).second.arrtime; 
    sstr<< "     DepartureTime: " << (*it).second.deptime; 
    sstr<< "     ArrivalLength: " << (*it).second.arrlen; 
    sstr<< "     DepartureLength: " << (*it).second.deplen; 
    sstr<< endl;
  } 
  sstr << "--------------------------------\n"; **/ 
  return sstr.str();
  
}

Facility::Facility(string n): name(n)
{
  string qname;
  qname += name;
  qname += ".";
  qname += "Incoming_queue"; 
  iq = new SimQueue(qname);
  state = IDLE;
  bserviced = 0;
  
  totrequests = 0;
  totalserviced = 0;
  
  lastservicestarttime = 0;
  busytime = 0;
}

Facility::~Facility()
{
  delete iq;
}

bool Facility::Seize(Transaction * t, int event_id, int prio)
{
  totrequests++;
    if ( state == IDLE )
    {
      bserviced = t;
      lastservicestarttime = Time;
      state = BUSY;
      return true;
    } else
    {
      iq->pushback(t, event_id, prio);
      return false;
    }
    return false;
}

/**
typedef pair<Transaction *, int> qsched;
typedef pair<int, float> compi;
typedef pair<compi, qsched> qentry;
**/

int Facility::waitingRequests()
{
  return iq->size();
}

bool Facility::empty()
{
  return (state == IDLE); 
}

bool Facility::canRelease(Transaction * t)
{
  return (t == bserviced);
}

void Facility::Release( Transaction * currentprocess )
{
    if ( state == IDLE )
    {
      cerr<<"IDLE facility...\n";
      abort();
    } else
    {
      if ( currentprocess != bserviced )
      {
        cerr<<"You "<< currentprocess <<" cant release facility...\n";
        abort();      
      }
      
      busytime += ( Time-lastservicestarttime);
      totalserviced++;
      state = IDLE; 
      bserviced = NULL;
      if ( iq->size() > 0 )
      {
        state = BUSY;
        lastservicestarttime = Time;
        bserviced = iq->top()->second.first;
        ActivateTransaction( iq->top()->second.first, iq->top()->second.second, Time );
        iq->popfront();
      }
    
  
    }
}

string Facility::getStatistics()
{
  stringstream sstr;
  sstr << setprecision(10);
  sstr << "--------------------------------\n";
  sstr << "Name: " << name << endl;
  sstr << "State(0-empty, 1-busy): " << state << endl;  
  sstr << "Total requests: " << totrequests << endl;
  sstr << "Total processed requests: " << totalserviced << endl; 
  sstr << "Utilization: " << busytime/(EndTime - InitTime) << endl;   
  sstr << "--------------------------------\n";  
  sstr << iq->getStatistics();
  return sstr.str();  
}

Store::Store(string n, int sizep): name(n), size(sizep)
{
  string qname;
  qname += name;
  qname += ".";
  qname += "Incoming queue"; 
  iq = new SimQueue(qname);
  tin = new list<Transaction *> ();
  bserviced = 0;
  
  totalserviced = totrequests = 0;
  maxutil = 0;
  
  lastservicestarttime = 0;
  busytime = 0;  
}

Store::~Store()
{
  delete iq;
}

bool Store::Enter(Transaction * t, int event_id, int prio)
{
  totrequests++;
  if ( tin->size() < size )
  {
    if ( tin->size() == 0 )
      lastservicestarttime = Time;
    tin->push_back(t);
    if ( tin->size() > maxutil )
      maxutil = tin->size(); 
    return true;
  }
  else
  {
    iq->pushback(t, event_id, prio);
    return false;
  }
  return false;  
}

bool hasTID(list<Transaction *> * tlst, int tid)
{
   for (list<Transaction *>::iterator it = tlst->begin(); it != tlst->end(); it++)
   {
      if ( (*it)->get_tid() == tid ) 
        return true;
   }
   return false;
}

void Store::Leave( Transaction * currentprocess )
{
  if ( tin->size() == 0 )
  {
    cerr<<"Empty store...\n";
    abort();
  } else
  {
    if ( !hasTID(tin, currentprocess->get_tid()) )
    {
      cerr<<"You "<< currentprocess <<" cant release facility...\n";
      abort();      
    }
    totalserviced++;
    //cout << Time <<" im leaving and adding " << Time-lastservicestarttime << "time\n";
    busytime +=  ( Time-lastservicestarttime );
    lastservicestarttime = Time;
    tin->remove(currentprocess);
 
    if ( iq->size() > 0 )
    {
      tin->push_back(iq->top()->second.first);
      if ( tin->size() > maxutil )
        maxutil = tin->size(); 

      ActivateTransaction( iq->top()->second.first, iq->top()->second.second, Time );
      iq->popfront();
    }    
  }
}

bool Store::empty()
{
  return ( tin->size() == 0 );
}

int Store::waitingRequests()
{
  return iq->size();
}

bool Store::canRelease(Transaction * t)
{
  return hasTID(tin, t->get_tid());
}

string Store::getStatistics()
{
  stringstream sstr;
  sstr << setprecision(10);
  sstr << "--------------------------------\n";
  sstr << "Meno: " << name << endl;
  sstr << "Maximalne vyuzitie zdrojov: " << maxutil << endl;  
  sstr << "Totalne poziadavkov: " << totrequests << endl;
  sstr << "Totalne spracovanych poziadavkov: " << totalserviced << endl; 
  sstr << "Vyuzitie: " << busytime/(EndTime - InitTime) << endl;   
  sstr << "--------------------------------\n";
   
  sstr << iq->getStatistics();
  return sstr.str();  
}

float InitTime;
float EndTime;
float Time;

Simulator * Instance;

float Random()
{
  return (rand()+0.0)/RAND_MAX;
}

float Uniform(float x, float y)
{

  return x+(Random()*(y-x));

}

float Exp(float x)
{
  return (-1*log(Random()))/x;
}

void Init(float stime)
{
  srand ( time( 0 ) );
  InitTime=stime;
  Time = InitTime;
  Instance = new Simulator();
}

void Run()
{
  EndTime = numeric_limits<int>::max();
  Instance->Run();
}

void RunFor(float tlen)
{
  EndTime = InitTime + tlen;
  Instance->Run();
}

void Finish()
{
  delete Instance; 
}

string RunInfo()
{
  return Instance->Stats();
}

void ActivateSingleEvent ( Event * e, float when)
{

  Instance->scheduleEntry(when, e->get_priority(), e );
}

void ActivateTransaction ( Transaction * t, int event_id, float when)
{
    Instance->scheduleEntry
      (when,
       t->dispatcher(event_id)->get_priority(),
       t->dispatcher(event_id) );
}

void Cancel( Event * e )
{
  Instance->cancel(e);
}

void CancelTrans( Transaction * t, int event_id )
{
  Cancel(t->dispatcher(event_id));
}

Simulator::Simulator()
{
  event_list = new custom_pqueue<ActivationEntry>();
  processed_events = 0;
}


Simulator::~Simulator()
{
  delete event_list;
}

void Simulator::scheduleEntry( float when, int priority, Event * what )
{
  //cout << "Udalost na cas " << when <<endl;
  event_list->push(ActivationEntry(when, priority, what));
}

void Simulator::cancel( Event * what )
{
  vector<ActivationEntry>::iterator it;
  for ( it= event_list->get_list_ptr()->begin(); it != event_list->get_list_ptr()->end() ; it++)
  {
    if ( what == (*it).what )
    {
      event_list->remove(it);
      break;
    } 
  } 
  
  if ( it == event_list->get_list_ptr()->end() )
  {
    cerr<<"Canceling nonexistent event\n";
    abort();
  }
  
}

bool operator<(const ActivationEntry &a, const ActivationEntry &b)
{
  if ( fabs(a.when-b.when) < PRECISION )
  {
    if (a.priority < b.priority)
    {
      return false;
    } else
      return true;
  } else
  if ( a.when < b.when )
  {
    return false;
  } else
    return true;
};

void Simulator::Run()
{

  while ( event_list->size()>0 )
  {
    ActivationEntry current = event_list->top();
    Time = current.when;
    if ( Time > EndTime )
      break;
    event_list->pop(); 
    current.what->Act();
    processed_events++;
  }
  
  EndTime = Time;

}

string Simulator::Stats()
{
  stringstream sstr;
  sstr << setprecision(10);
  sstr << "--------------------------------\n";
  sstr << "Running time: " << EndTime - InitTime << endl;
  sstr << "Processed events: " << processed_events << endl;
  sstr << "Events per time unit: " << (processed_events+0.0)/(EndTime - InitTime + 0.0) << endl;
  sstr << "--------------------------------\n";
  return sstr.str();   

}

//newline
