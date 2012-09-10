package client;

import java.util.*;
import java.util.concurrent.*;
import org.jdom.*;
import org.jdom.input.*;
import org.jdom.output.*;
import java.io.*;
import COMMON.*;
/**
 * Represents local cache  of client. Inludes metodes for working with local cache.
 * @author Matus Kontra
 */
public class LocalCache implements Runnable{

     private static LocalCache default_LocalCache;
     private Thread synchronizer = null;
     /**
      * Represents map users logins to users objects
      */
     HashMap<String, User> users;
     /**
      * Represents map of groups names and group object.
      */
     HashMap<String, Group> groups;
     
      /**
      * Represents  File object of directory where user files are stored.
      */
     protected File userdict;
       /**
      * Represents  File object of directory where groups files are stored.
      */
     protected File gropudict;
    /**
      * Create LocalCache object with specified paths to users and group folder with specified synchronization.
      * @param users string representation of direcrtory with users files.
      * @param groups string representation of direcrtory with groups files.
      * @param autosync boolean representation of synchronization mode.
      */
     public LocalCache(String users, String groups, boolean autosync) {
	  this.users = new HashMap<String, User>();
	  this.groups = new HashMap<String, Group>();
	  
	  try {
	       userdict = new File(users);
	       gropudict = new File(groups);

	       if (userdict.isFile()) {
		    throw new Exception("Supposed to be directory...");
	       }

	       if (gropudict.isFile()) {
		    throw new Exception("Supposed to be directory...");
	       }

	  } catch (Exception e) {
	       e.printStackTrace();
	       System.out.println("Users and groups directory does not exists");
	       System.exit(-1);
	  }



	  SAXBuilder builder = new SAXBuilder();

	  System.out.println(System.getProperty("user.dir"));

	  for (String fname : userdict.list()) {
	       try {
		    if (fname.charAt(0) == '.') {
			 continue;
		    }

		    FileInputStream fis = new FileInputStream(users + "/" + fname);
		    User u = User.fromXML(builder.build(fis).getRootElement());
		    this.users.put(u.get_login(), u);
		    fis.close();
	       } catch (Exception e) {
		    e.printStackTrace();
		    System.out.println("Users directory contains non xml files");
		    System.exit(-1);
	       }
	  }

	  for (String fname : gropudict.list()) {
	       try {

		    if (fname.charAt(0) == '.') {
			 continue;
		    }

		    FileInputStream fis = new FileInputStream(groups + "/" + fname);
		    Group g = Group.fromXML(builder.build(fis).getRootElement());
		    this.groups.put(g.get_group_name(), g);
		    fis.close();
	       } catch (Exception e) {
		    e.printStackTrace();
		    System.out.println("Groups directory contains non xml files");
		    System.exit(-1);
	       }
	  }
	  if (autosync)
	   (synchronizer = new Thread(this)).start();	  
	  
     }
     /**
      * Synchronization's run metod. Calls syncToDisc in periodical intervals.
      */
     public void run(){
     
      //periodicka synchronizacia na disk
      Thread thisThread = Thread.currentThread();
		  while(synchronizer == thisThread)
      {
      try{
        Thread.sleep(5000);
        syncToDisc();
        } catch (Exception e)
        {
          e.printStackTrace();
        }

      }
     
     }
     /**
      * Synchronizes server cache to disk.   
      * @throws java.lang.Exception
      */
     public synchronized void syncToDisc() throws Exception {
     
     //pridaj parameter FIle priecinka
     
  	  for (User u : users.values()) {
  	       User.put_User(u, userdict);
  	  }
  	  for (Group g : groups.values()) {
  	       Group.put_group(g, gropudict);
  	  }
  	  
     }
      /**
      * Returns User object from maps of users of local's cache accordning to user's login specified by parameter.
      * @param login string represention of user's login.
      * @return User object.
      */
     public synchronized User getUser(String login) {
	  return users.get(login);
     }
      /**
      * Retruns Group from maps of groups in local's cache according to group name specified by parameter.
      * @param groupname string represention of group's name.
      * @return Group object.
      */
     public synchronized Group getGroup(String groupname) {
	  return groups.get(groupname);
     }
      /**
      *  Finds out whether user specified by parameter is member of group specified by parameter.
      * @param u User object represention of user.
      * @param g Group object represention of group.
      * @return true if user is member of group, false otherwise.
      */
     public synchronized boolean isMember(User u, Group g) {
	  for (Group_membership gg : u.get_groups()) {
	       if (gg.get_group_name().compareTo(g.get_group_name()) == 0) {
		    return true;
	       }
	  }
	  return false;
     }
     /**
      * Find outs whether user specified by parameter is leader of group specified by parameter or no.
      * @param u User object represention of user.
      * @param g Group object represention of group.
      * @return true if user is leader, false otherwise.
      */
     public synchronized boolean isLeader(User u, Group g) {
	  return g.get_boss().compareTo(u.get_name()) == 0;
     }
     /**
      * Returns list of users who are members of group specified by parameter
      * @param g Group object representation of group.
      * @return list of User objects.
      */
     public synchronized List<User> listMembers(Group g) {
	  List<User> ulist = new ArrayList<User>();
	  for (User u : users.values()) {
	       for (Group_membership m : u.get_groups()) {
		    if (m.get_group_name().equals(g.get_group_name())) {
			 ulist.add(u);
		    }
	       }
	  }
	  return ulist;
     }
      /**
      * Authentificate user speficified by parameter and use password specified by parameter.
      * @param username string representation of user's login.
      * @param password string representation of password's hash.
      * @return true if user was authetificated, false otherwise.
      */
     public synchronized boolean authenticateUser(String username, String password) {
	  //vyhladanie uzivatela v pamati
	  for (String u : this.users.keySet()) {
	   //System.out.println("mam " + password + " "+ users.get(u).get_password());
	       if ( (u.equals(username)) && (users.get(u).get_password().equals(password)) ) {
		    return true;
	       }
	  }
	  return false;
     }
     /**
      * Adds user specified by parameter to local cache.
      * @param u User object representation of user.
      */
     public synchronized void addUser(User u) {

        //if (users.keySet().contains(u.get_login()))
        //   users.remove(u.get_login());
            
        users.put(u.get_login(), u);

     }
	 /**
	  * Adds list of groups to local cache.
	  * @param grlist List of Group objects.
	  */
     public synchronized void addGroups( List<Group> grlist )
     {
        HashMap<String, Group> tmp = new HashMap<String, Group>();
        for ( Group g: grlist )
        {
          tmp.put(g.get_group_name(), g);
        }
        groups.putAll(tmp);
     } 
     /**
      * Adds group to local cache.
      * @param g Group object represents group.
      */
     public synchronized void addGroup(Group g)
     {          
        groups.put(g.get_group_name(), g);     
     }
     
      /**
      * Adds user to map of users in local cache.
      * @param login  string representation of user's login.
      * @param password  string representation of user's password hash.
      * @param name  string representation of user's name.
      * @param surname  string representation of user's surname.
      * @throws java.lang.Exception
      */
     public synchronized void addUser(String username, String password, String name, String surname) {
	  User created_usr = new User(name, surname, username, password);
	  this.users.put(username, created_usr);
     }
       /**
      * Creates a new group and adds it to local cache.
      * @param groupname  string representation of group's name.
      * @param leader  string representation of user's login.
      * @return true if group was created, false otherwise.
      */
     public synchronized boolean createGroup(String groupname, String leader) {
	  try {
	       this.groups.put(groupname, new Group(groupname, leader));
	       getUser(leader).add_group(new Group_membership(groupname, true));
	       return true;
	  } catch (Exception e) {
	       return false;
	  }
     }
      /**
      * Deletes group from local cache.
      * @param groupname  string representation of group's name.
      * @param user_name  string representation of user's login.
      * @return true if group was deleted, false otherwise.
      */
     public synchronized boolean removeGroup(String groupname) {
	  try {
	       this.groups.remove(groupname);
	       return true;
	  } catch (Exception e) {
	       return false;
	  }
     }

      /**
      * Adds event specified by parameter to user specified by parameter. 
      * @param user  user representation of user.
      * @param e Callendar_event representation of event.
      * @return true if event was added, false otherwise.
      */
     public synchronized boolean addEvent(User user, Callendar_event e) {
	  try {
	       user.get_callendar().add_event(e);
	       return true;
	  } catch (Exception hmm) {
	       hmm.printStackTrace();
	       System.out.println("Local:  Event was not added to user.");
	       return false;
	  }
     }
      /**
      * Removes event specified by parameter from user specified by parameter.
      * @param user string representation of user's login.
      * @param event_name string representation of event's name.
      * @return true if event was removed, false otherwise.
      */
     public synchronized boolean removeEvent(User user, String event_name) {
	  try {
	       for (Callendar_event e : new LinkedList<Callendar_event>(user.get_callendar().get_events())) {
		    if (e.get_name().compareTo(event_name) == 0) {
			     user.get_callendar().remove_event(e);
			 return true;
		    }
	       }
	       return false;
	  } catch (Exception e) {
	       System.out.println("Local: Event was not removed from user.");
	       return false;
	  }
     }
      /**
      * Adds group event to local cache.
      * @param user User object representation of user.
      * @param g Group object representation of group.
      * @param e Callendar_event object  representation of evet..
      * @return true if event was added, false otherwise.
      */
     public synchronized boolean addGroupEvent(Group g, Callendar_event e) {
     try{
	  g.get_callendar().add_event(e);
	  } catch (Exception ee) {
	       System.out.println("Local: addgroupevent failed.");
	       return false;
	  }
	  return true;
     }
     
     /**
      * Returns list events which user specified by parameter owns.
      * @param u User object representation of user
      * @return list of Callendar_event objects.
      */
    public List<Callendar_event> listUserEventsAll( User u )
    {
      
      return u.get_callendar().get_events();
      
    } 
     /**
      * Returns list of user's events, which have day specified by parameter. User is also specified by parameter.
      * @param u User object representation of user.
      * @param d Date object representation of date.
      * @return List of Callendar_event objects.
      */
    public List<Callendar_event> listUserEventsOnDay(User u, Date d)
    {
      List<Callendar_event> retval = new LinkedList<Callendar_event>();
      
      Calendar c = new GregorianCalendar();
      c.setTime(d);
      c.add(Calendar.DATE , 1);
      Date plushour = c.getTime();
      
      for ( Callendar_event ce: u.get_callendar().get_events() )
      {
        if (ce.get_repeating())
        {
               if ( (ce.get_Event_date_and_time().before(plushour)) && (ce.get_Event_date_and_time().after(d)) )
              {
              if ( !retval.contains(ce) )
              retval.add(ce);    
              } else {
                  Calendar tmp = new GregorianCalendar();
                  tmp.setTime(d);
                  for ( int i = 0; i < 24 ; i++)
                  {
                    List<Callendar_event> sht = listUserEventsOnHour(u, tmp.getTime());
                    if ( sht.contains(ce) )
                    {
                    
                                  if ( !retval.contains(ce) )
                                  retval.add(ce);    
                                                        break;
                        }
                                      tmp.add(Calendar.HOUR_OF_DAY, 1);
                    }

                  }
        
          
        } else {
          if ( ce.get_Event_date_and_time().before(plushour) && ce.get_Event_date_and_time().after(d) )
          {
              if ( !retval.contains(ce) )
              retval.add(ce);    
              }
            }
      }
      
      return retval;
      
    
    }
     /**
      * Returns list of user's events, which have hour specified by parameter. User is also specified by parameter.
      * @param u User object representation of user.
      * @param d Date object representation of date.
      * @return List of Callendar_event objects.
      */
    public List<Callendar_event> listUserEventsOnHour(User u, Date d)
    {
      List<Callendar_event> retval = new LinkedList<Callendar_event>();
      
      Calendar c = new GregorianCalendar();
      c.setTime(d);
      c.add(Calendar.HOUR , 1);
      Date plushour = c.getTime();
      
      for ( Callendar_event ce: u.get_callendar().get_events() )
      {
        if (ce.get_repeating())
        {
              if ( (ce.get_Event_date_and_time().before(plushour)) && (ce.get_Event_date_and_time().after(d)) )
              {
              if ( !retval.contains(ce) )
              retval.add(ce);    
              } else {
          int hoursdelta = ce.get_how_often();
          if (hoursdelta == 0) continue; 
          Date evinitsc = ce.get_Event_date_and_time();
          
          long h1 = evinitsc.getTime()/(1000 * 60 * 60);
          long h2 = d.getTime()/(1000 * 60 * 60);
          
          long delta = h2 - h1;

          if ( delta % hoursdelta == 0 )
          {
            if ( !retval.contains(ce) )
              retval.add(ce);          
          }
          }
        } else {
          if ( (ce.get_Event_date_and_time().before(plushour)) && (ce.get_Event_date_and_time().after(d)) )
              {
              if ( !retval.contains(ce) )
              retval.add(ce);    
              }
            }
            }

      
      return retval;  
    }

     /**
      * Returns list of group's events, specified by parameter,
      * @param g Group object representation of group.
      * @return List of Callendar_event objects.
      */
    public List<Callendar_event> listGroupEventsAll( Group g )
    {
      return g.get_callendar().get_events();    
    }
     /**
      * Returns list of group's events, which have day specified by parameter. Group is also specified by parameter.
      * @param u Group object representation of group.
      * @param d Date object representation of date.
      * @return List of Callendar_event objects.
      */
    public List<Callendar_event> listGroupEventsOnDay(Group g, Date d)
    {
      List<Callendar_event> retval = new LinkedList<Callendar_event>();
      
      Calendar c = new GregorianCalendar();
      c.setTime(d);
      c.add(Calendar.DATE , 1);
      Date plushour = c.getTime();
      
      for ( Callendar_event ce: g.get_callendar().get_events() )
      {
        if (ce.get_repeating())
        {
               if ( (ce.get_Event_date_and_time().before(plushour)) && (ce.get_Event_date_and_time().after(d)) )
              {
              if ( !retval.contains(ce) )
              retval.add(ce);    
              } else {
                  Calendar tmp = new GregorianCalendar();
                  tmp.setTime(d);
                  for ( int i = 0; i < 24 ; i++)
                  {
                    List<Callendar_event> sht = listGroupEventsOnHour(g, tmp.getTime());
                    if ( sht.contains(ce) )
                    {
                    
                                  if ( !retval.contains(ce) )
                                  retval.add(ce);    
                                                        break;
                        }
                                      tmp.add(Calendar.HOUR_OF_DAY, 1);
                    }

                  }
        
          
        } else {
          if ( ce.get_Event_date_and_time().before(plushour) && ce.get_Event_date_and_time().after(d) )
          {
              if ( !retval.contains(ce) )
              retval.add(ce);    
              }
            }
      }
      
      return retval;
      
    
    }
     /**
      * Returns list of group's events, which have hour specified by parameter. Group is also specified by parameter.
      * @param g Group object representation of group.
      * @param d Date object representation of date.
      * @return List of Callendar_event objects.
      */
    public List<Callendar_event> listGroupEventsOnHour(Group g, Date d)
    {
      List<Callendar_event> retval = new LinkedList<Callendar_event>();
      
      Calendar c = new GregorianCalendar();
      c.setTime(d);
      c.add(Calendar.HOUR , 1);
      Date plushour = c.getTime();
      
      for ( Callendar_event ce: g.get_callendar().get_events() )
      {
        if (ce.get_repeating())
        {
              if ( (ce.get_Event_date_and_time().before(plushour)) && (ce.get_Event_date_and_time().after(d)) )
              {
              if ( !retval.contains(ce) )
              retval.add(ce);    
              } else {
          int hoursdelta = ce.get_how_often();
          Date evinitsc = ce.get_Event_date_and_time();
          if (hoursdelta == 0) continue;         
          long h1 = evinitsc.getTime()/(1000 * 60 * 60);
          long h2 = d.getTime()/(1000 * 60 * 60);
          
          long delta = h2 - h1;

          if ( delta % hoursdelta == 0 )
          {
            if ( !retval.contains(ce) )
              retval.add(ce);          
          }
          }
        } else {
          if ( (ce.get_Event_date_and_time().before(plushour)) && (ce.get_Event_date_and_time().after(d)) )
              {
              if ( !retval.contains(ce) )
              retval.add(ce);    
              }
            }
            }

      
      return retval;  
    }
    
    
    /**
      * Adds group event to local cache.
      * @param user User object representation of user.
      * @param g Group object representation of group.
      * @param e Callendar_event object  representation of evet..
      * @return true if event was added, false otherwise.
      */
     public synchronized boolean addGroupEvent(User user, Group g, Callendar_event e) {
	  //musim overit ci je uzivatel clenom skupiny a ci je vedci
	  boolean is_b = false;
	  for (Group_membership m : user.get_groups()) {
	       if (m.get_group_name().equals(g.get_group_name()) && m.is_boss()) {
		    is_b = true;
	       }
	  }
	  if (!is_b) {
	       return false;
	  }
	  if (groups.containsValue(g)) {
	  try{
	       g.get_callendar().add_event(e);
	       return true;
	      } catch (Exception ee) {
	       System.out.println("addGroupEvent fld.");
	       return false;
	  }
	  } else {
	       System.out.println("Event was not added to group.");
	       return false;
	  }
     }
     /**
      * Removes group event from local cache.
      * @param user User object representation of user.
      * @param g Group object representation of group.
      * @param e Callendar_event object  representation of evet..
      * @return true if event was deleted, false otherwise.
      */
     public synchronized boolean removeGroupEvent(User user, Group g, String event_name) {
	  try {
	       ///musim overit ci je uzivatel clenom skupiny a ci je vedci
	       boolean is_b = false;
	       for (Group_membership m : user.get_groups()) {
		    if (m.get_group_name().equals(g.get_group_name()) && m.is_boss()) {
			 is_b = true;
		    }
	       }
	       if (!is_b) {
		    return false;
	       }
	       for (Callendar_event e : g.get_callendar().get_events()) {
		    if (e.get_name().compareTo(event_name) == 0) {
			 g.get_callendar().remove_event(e);
			 return true;
		    }
	       }
	       return false;
	  } catch (Exception e) {
	       System.out.println("Event was not added to user.");
	       return false;
	  }
     }
     /**
      * Removes group event from local cache.
      * @param g Group object representation of group.
      * @param event_name string representation of event name.
      * @return true if event was deleted, false otherwise.
      */
     public synchronized boolean removeGroupEvent(Group g, String event_name) {
	  try {

	       for (Callendar_event e : g.get_callendar().get_events()) {
		    if (e.get_name().compareTo(event_name) == 0) {
			 g.get_callendar().remove_event(e);
			 return true;
		    }
	       }
	       return false;
	  } catch (Exception e) {
	       System.out.println("Event was not added to user.");
	       return false;
	  }
     }
     /**
      * Adds group membership to user, both specified by parameter.
      * @param g string representation of group name.
      * @param to_user string representation of user's login
      * @return true if membership was added, false otherwise.
      */
     public synchronized boolean addMembership(String g, String to_user) {
	  Group recv = getGroup(g);
	  User u = getUser(to_user);
	  try{
	  u.add_group(new Group_membership(recv.get_group_name(), false));
	  } catch (Exception e)
	  {
	   e.printStackTrace();
      return false;
    }
	  return true;

     }
      /**
      * Adds notice specified by parameter to event of user, both specified by parameter. 
      * @param user_name string representation of user's login.
      * @param event_name string representation of event's name.
      * @param note Event_notice representation of notice.
      * @return true if notice was added, false otherwise.
      */
     public synchronized boolean addEventNotice(String user_name, String event_name, Event_notice note) {
	  try {
	       for (Callendar_event e : getUser(user_name).get_callendar().get_events()) {
		    if (e.get_name().equals(event_name)) {
			 e.add_notice(note);
			 return true;
		    }
	       }
	       System.out.println("Notice was not added.");
	       return false;
	  } catch (Exception b) {
	       System.out.println("Notice was not added.");
	       return false;
	  }
     }
       
      /**
      * Removes notice specified by parameter from event of user, both specified by parameter. 
      * @param user_name string representation of user's login.
      * @param event_name string representation of event's name.
      * @param note Event_notice representation of notice.
      * @return true if notice was removed, false otherwise.
      */
     public synchronized boolean removeEventNotice(String user_name, String event_name, Event_notice note) {
	  try {
	       for (Callendar_event e : getUser(user_name).get_callendar().get_events()) {
		    if (e.get_name().equals(event_name)) {
			 e.remove_notice(note);
			 return true;
		    }
	       }
	       System.out.println("Notice was not deleted.");
	       return false;
	  } catch (Exception b) {
	       System.out.println("Notice was not deleted.");
	       return false;
	  }
     }
       /**
      * Edits notice specified by parameter of event of user, both specified by parameter. 
      * @param user_name string representation of user's login.
      * @param event_name string representation of event's name.
      * @param note Event_notice representation of notice.
      * @return true if notice was edited, false otherwise.
      */
     public synchronized boolean editEventNotice(String user_name, String event_name, Event_notice note) {
	  try {
	       for (Callendar_event e : getUser(user_name).get_callendar().get_events()) {
		    if (e.get_name().equals(event_name)) {
			 for (Event_notice n : e.get_notices()) {
			      //kazdy si moze editovat len svoje poznamky
			      if (n.get_date().equals(note.get_date()) && n.get_author().equals(note.get_author())) {
				   n.set_body(note.get_body());
				   return true;
			      }
			 }
		    }
	       }
	       System.out.println("Notice was not edited.");
	       return false;
	  } catch (Exception b) {
	       System.out.println("Notice was not edited.");
	       return false;
	  }
     }
      /**
      * Adds alert specified by parameter to event of user, both specified by parameter. 
      * @param user_name string representation of user's login.
      * @param event_name string representation of event's name.
      * @param alrt Event_alert representation of alert.
      * @return true if alert was added, false otherwise.
      */
     public synchronized boolean addEventAlert(String user_name, String event_name, Event_alert alrt) {
	  try {
	       for (Callendar_event e : getUser(user_name).get_callendar().get_events()) {
		    if (e.get_name().equals(event_name)) {
			 e.add_alert(alrt);
			 return true;
		    }
	       }
	       System.out.println("Alert was not added.");
	       return false;
	  } catch (Exception b) {
	       System.out.println("Alert was not added.");
	       return false;
	  }
     }
      /**
      * Removes alert specified by parameter from event of user, both specified by parameter. 
      * @param user_name string representation of user's login.
      * @param event_name string representation of event's name.
      * @param alrt Event_alert representation of alert.
      * @return true if alert was removed, false otherwise.
      */
     public synchronized boolean removeEventAlert(String user_name, String event_name, Event_alert alrt) {
	  try {
	       for (Callendar_event e : getUser(user_name).get_callendar().get_events()) {
		    if (e.get_name().equals(event_name)) {
			 e.remove_alert(alrt);
			 return true;
		    }
	       }
	       System.out.println("Alert was not deleted.");
	       return false;
	  } catch (Exception b) {
	       System.out.println("Alert was not deleted.");
	       return false;
	  }
     }
      /**
      * Adds group alert specified by parameter to event of group, both specified by parameter. 
      * @param user_name string representation of user's login.
      * @param  groupname string representation of group's name.
      * @param event_name string representation of event's name.
      * @param alrt Event_alert representation of alert.
      * @return true if alert was added, false otherwise.
      */
     public synchronized boolean addGroupAlert(String groupname, String event_name, Event_alert alrt) {
	  try {

	       //pridanie alertu
	       for (Callendar_event e : getGroup(groupname).get_callendar().get_events()) {
		    if (e.get_name().equals(event_name)) {
			 e.add_alert(alrt);
			 return true;
		    }
	       }

	       return false;
	  } catch (Exception b) {
	       return false;
	  }
     }
      /**
      * Removes group alert specified by parameter from event of group, both specified by parameter. 
      * @param user_name string representation of user's login.
      * @param  groupname string representation of group's name.
      * @param event_name string representation of event's name.
      * @param alrt Event_alert representation of alert.
      * @return true if alert was added, false otherwise.
      */
     public synchronized boolean removeGroupAlert(String groupname, String event_name, Event_alert alrt) {
	  try {

	       for (Callendar_event e : getGroup(groupname).get_callendar().get_events()) {
		    if (e.get_name().equals(event_name)) {
			 e.remove_alert(alrt);
			 return true;
		    }
	       }
	       System.out.println("Alert was not removed to group.");
	       return false;
	  } catch (Exception b) {
	       System.out.println("Alert was not removed to group.");
	       return false;
	  }
     }
      /**
      * Adds group notice specified by parameter to event of group, both specified by parameter. 
      * @param user_name string representation of user's login.
      * @param  groupname string representation of group's name.
      * @param event_name string representation of event's name.
      * @param note Event_notice representation of notice.
      * @return true if notice was added, false otherwise.
      */
     public synchronized boolean addGroupNotice(String groupname, String event_name, Event_notice note) {
	  try {

	       //clenstvo je overene, musim najst spravny event
	       for (Callendar_event c : getGroup(groupname).get_callendar().get_events()) {
		    if (c.get_name().equals(event_name)) {
			 c.add_notice(note);
			 return true;
		    }
	       }
	       return false;
	  } catch (Exception e) {
	       return false;
	  }
     }
      /**
      * Removes group notice specified by parameter from event of group, both specified by parameter. 
      * @param user_name string representation of user's login.
      * @param  groupname string representation of group's name.
      * @param event_name string representation of event's name.
      * @param note Event_notice representation of notice.
      * @return true if notice was removed, false otherwise.
      */
     public synchronized boolean removeGroupNotice(String groupname, String event_name, Event_notice note) {
	  try {
	       //clenstvo je overene, musim najst spravny event
	       for (Callendar_event c : getGroup(groupname).get_callendar().get_events()) {
		    if (c.get_name().equals(event_name)) {
			 c.remove_notice(note);
			 return true;
		    }
	       }
	       System.out.println("Notice was not added to group.");
	       return false;
	  } catch (Exception e) {
	       System.out.println("Notice was not added to group.");
	       return false;
	  }
     }
      /**
      * Edits group notice specified by parameter to event of group, both specified by parameter. 
      * @param user_name string representation of user's login.
      * @param  groupname string representation of group's name.
      * @param event_name string representation of event's name.
      * @param note Event_notice representation of notice.
      * @return true if notice was edited, false otherwise.
      */
     public synchronized boolean editGroupNotice(String groupname, String event_name, Event_notice note) {
	  try {

	       //clenstvo je overene, musim najst spravny event
	       for (Callendar_event c : getGroup(groupname).get_callendar().get_events()) {
		    if (c.get_name().equals(event_name)) {
			 for (Event_notice n : c.get_notices()) {
			      //mozu sa overovat len svoje poznamky
			      if (n.get_date().equals(note.get_date()) && n.get_author().equals(note.get_author())) {
				   n.set_body(note.get_body());
				   return true;
			      }
			 }
		    }
	       }
	       System.out.println("Notice was not added to group.");
	       return false;
	  } catch (Exception e) {
	       System.out.println("Notice was not added to group.");
	       return false;
	  }
     }
      /**
      * Returns singleton of local cache object.
      * @return singleton of local cache object.
      */
     public static synchronized LocalCache getDefaultCache() {
	  if (default_LocalCache == null) {
	       default_LocalCache = new LocalCache("./cusers", "./cgroups",true);
	  }
	  return default_LocalCache;
     }
}
