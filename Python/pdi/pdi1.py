import threading, Queue, sys, random

class OpCodes:
    DIE=1
    SUSPEND=2
    ACTIVATE=3
    
    CHECKREQUESTS=4
    REQUESTLEADERRESPONSE=5
    LEADERALIVE=6
    ISHOSTALIVE=7
    HOSTALIVE=8
    SETLEADER=9

class HostState:
    CREATED=1
    RUNNING=2
    SUSPENDED=3
    DEAD=4
    
class SimHost(threading.Thread):
  def __init__(self, id, hostmap):
    self.leader = -1
    self.pendingleadercheck = False
    self.pendingrequests = None
    self.id = id
    self.hostmap = hostmap
    self.commandqueue = Queue.Queue(150)
    self.state = HostState.CREATED
    threading.Thread.__init__(self)
    
  def __str__(self):
    return "Host id: " + str(self.id) + " ThreadRunning: " +\
           str(self.state==HostState.RUNNING) + " Leader id: " + str(self.leader) + "\n"

  def __repr__(self):
    return self.__str__()
  
  def post_message(self, message):
    self.commandqueue.put(message)    
   
  def post_die_message(self):
    self.post_message((-1,OpCodes.DIE, None ))
    
  def post_suspend_message(self):         
    self.post_message((-1,OpCodes.SUSPEND, None ))     

  def post_activate_message(self):         
    self.post_message((-1,OpCodes.ACTIVATE, None ))
  
  def start_leader_check(self):
    if ( self.leader == -1 ): return
    if self.pendingleadercheck == False:
      self.pendingleadercheck = True
      self.hostmap[self.leader].post_message( (self.id, OpCodes.REQUESTLEADERRESPONSE, None) )
    else:
      self.pendingleadercheck = False
      self.init_leader_election()  
      
  def init_leader_election(self):
    if self.state != HostState.RUNNING: return
    if (self.pendingrequests is not None):
      return

    for x in [x.id for x in hostdict.values() if x.id > self.id]:
      hostdict[x].post_message( (self.id, OpCodes.ISHOSTALIVE, None) ) 
       
    self.pendingrequests = threading.Timer(3.0, lambda: self.post_message( (self.id, OpCodes.CHECKREQUESTS, None) ) )
    self.pendingrequests.start()

  def cancel_leader_election(self):
    if self.pendingrequests is not None:
      self.pendingrequests.cancel()
      self.pendingrequests = None
    
  def finish_leader_election(self):
    print "Thread " + str(self.id) + " assumes leadership"
    self.leader = self.id
    for x in [x.id for x in hostdict.values() if x.id != self.id]:
      hostdict[x].post_message( (self.id, OpCodes.SETLEADER, self.id) )
    self.pendingrequests = None
  
  def start(self):
    self.state = HostState.RUNNING
    threading.Thread.start(self)
       
  def run(self):  
    self.init_leader_election()
    while(True):
      msg = None
      try:
        msg = self.commandqueue.get(True, 5+random.randint(-2,2) )
      except Queue.Empty:
        if random.uniform(0,1)<0.1:
          self.init_leader_election()
        continue
      #(sender, code, contents)
      #print self.__str__() + " | Got message " + str(msg)
      if msg[1] == OpCodes.DIE:
        break
      if msg[1] == OpCodes.ACTIVATE: 
        self.state = HostState.RUNNING 
        self.init_leader_election() 
        continue
      
      if self.state == HostState.SUSPENDED:
        continue
      
      #if (self.id == 9): print "inhere"
      
      if msg[1] == OpCodes.SUSPEND:
        self.state = HostState.SUSPENDED
        self.cancel_leader_election()
      elif msg[1] == OpCodes.LEADERALIVE:
        self.pendingleadercheck = False
      elif msg[1] == OpCodes.CHECKREQUESTS:
        self.finish_leader_election()
      elif msg[1] == OpCodes.ISHOSTALIVE:
        hostdict[msg[0]].post_message( (self.id, OpCodes.HOSTALIVE, None) )
        self.init_leader_election() 
      elif msg[1] == OpCodes.HOSTALIVE:
        self.cancel_leader_election()
      elif msg[1] == OpCodes.SETLEADER:
        self.leader = msg[2]
      else:
        pass
    self.state = HostState.DEAD    
    
    
hostdict = {}

def prepare_hosts(count):
  hostdict.update( (k, SimHost(k, hostdict)) for k in range(count) )

def mark_host_inactive():
  reply  = raw_input('Enter host id>')
  id = int(reply) 
  hostdict[id].post_suspend_message() 
  
def mark_host_active():
  reply  = raw_input('Enter host id>')
  id = int(reply) 
  hostdict[id].post_activate_message() 

if __name__ == "__main__":
  random.seed()
  reply  = raw_input('Enter number of hosts>')
  hostcount = int(reply)
  
  #prepare stuff
  prepare_hosts(hostcount)
  print hostdict
  
  actions = {
    1 : (lambda : [ x.start() for x in hostdict.values() ] ),
    2 : (lambda : mark_host_inactive()),
    3 : (lambda : mark_host_active()),
    4 : (lambda : sys.stdout.write(str(hostdict))) ,
    6 : (lambda : [ x.init_leader_election() for x in hostdict.values() ] ),
  }
  
  while (True):
    print "Select action:"
    print "1 - Start hosts"
    print "2 - Mark host inactive"
    print "3 - Reactivate host"
    print "4 - Show hosts state"
    print "5 - Terminate all hosts and quit"
    print "6 - Force election"    
    selection  = raw_input('>')
    try:
      isel = int(selection)
    except ValueError:
      print "Bad input"
      continue
    if isel == 5: break
    actions[isel]()
    
  #terminate stuff
  [x.post_die_message() for x in hostdict.values()]