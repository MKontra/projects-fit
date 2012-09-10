package myserver;

import java.util.*;
import java.util.concurrent.*;
import org.jdom.*;
import org.jdom.input.*;
import org.jdom.output.*;
import java.io.*;
import COMMON.*;

/**
 * Represents server controler of server. Inludes metodes for working with server cache. 
 * Singleton design pattern is used implementing this class.
 * @author Ivan Homoliak
 */
public class ServerController implements Runnable {

     /**
      * Represents singleton object of server controller.
      */
     private static ServerController default_controller;
     /**
      * Represent thread object of cache to disc synchronizer.
      */
     private Thread synchronizer = null;
     /**
      * Represents  map of users logins and users objects.
      */
     protected HashMap<String, User> users;
     /**
      * Represents map of users logins and groups objects.
      */
     protected HashMap<String, Group> groups;

     /**
      * Represents triplets of associated strings -  login, login, event name.  
      */
     private static class EventCopyRequest {

	  /**
	   * String representation of login of offerer.
	   */
	  protected String from_user;
	  /**
	   * String representation of login of accepter.
	   */
	  protected String to_user;
	  /**
	   * String representation of event name.
	   */
	  protected String event_name;

	  /**
	   * Creates a EventCopyRequest with specified users logins and event name.
	   * @param from_user String representation of user's login.
	   * @param to_user String representation of user's login.
	   * @param event_name String representation of event's name.
	   */
	  EventCopyRequest(String from_user, String to_user, String event_name) {
	       this.from_user = from_user;
	       this.to_user = to_user;
	       this.event_name = event_name;
	  }

	  /**
	   * Compares object with another.
	   * @param o object of type Object .
	   * @return true if objects are equals and false otherwise.
	   */
	  public boolean equals(Object o) {
	       EventCopyRequest ecr = (EventCopyRequest) o;
	       return (this.from_user.equals(ecr.from_user)) && (this.to_user.equals(ecr.to_user)) && (this.event_name.equals(ecr.event_name));
	  }
     }

     /**
      * Represents memberhip association of user and group.
      */
     private static class MembershipRequest {

	  public String from_group;
	  public String to_user;

	  /**
	   * Creates MembershipRequest object with specified group name and user name.
	   * @param from_group String representation of gruop's name.
	   * @param to_user String representation of user's login.
	   */
	  MembershipRequest(String from_group, String to_user) {
	       this.from_group = from_group;
	       this.to_user = to_user;
	  }

	  /**
	   * Compares object with another.
	   * @param o object of type Object .
	   * @return true if objects are equals and false otherwise.
	   */
	  public boolean equals(Object o) {
	       MembershipRequest ecr = (MembershipRequest) o;
	       return (this.from_group.equals(ecr.from_group)) && (this.to_user.equals(ecr.to_user));
	  }
     }
     private LinkedList<EventCopyRequest> offeredCopies; //...
     private LinkedList<MembershipRequest> offeredMemberships; //...
     /**
      * Represents File object of users database.
      */
     protected File userdict;
       /**
      * Represents File object of groups database.
      */
     protected File gropudict;

     /**
      * Create ServerController object with specified paths to users and group folder with specified synchronization.
      * @param users string representation of direcrtory with users files.
      * @param groups string representation of direcrtory with groups files.
      * @param autosync boolean representation of synchronization mode.
      */
     public ServerController(String users, String groups, boolean autosync) {
	  this.users = new HashMap<String, User>();
	  this.groups = new HashMap<String, Group>();

	  this.offeredCopies = new LinkedList<EventCopyRequest>();
	  this.offeredMemberships = new LinkedList<MembershipRequest>();

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
		    System.out.println("Loading file:" +  users + "/" + fname );
		    User u = User.fromXML(builder.build(fis).getRootElement());
		    this.users.put(u.get_login(), u);
		    fis.close();
	       } catch (Exception e) {
		    e.printStackTrace();
		    System.out.println("Users directory contains non xml files");
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
	       }
	  }
	  if (autosync) {
	       (synchronizer = new Thread(this)).start();
	  }

     }

     /**
      * Synchronization's run metod. Calls syncToDisc in periodical intervals.
      */
     public void run() {

	  //periodicka synchronizacia na disk
	  Thread thisThread = Thread.currentThread();
	  while (synchronizer == thisThread) {
	       try {
		    Thread.sleep(5000);
		    syncToDisc();
	       } catch (Exception e) {
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
      * Returns User object from maps of users of server's cache accordning to user's login specified by parameter.
      * @param login string represention of user's login.
      * @return User object.
      */
     public User getUser(String login) {
	  return users.get(login);
     }

     /**
      * Retruns Group from maps of groups in server's cache according to group name specified by parameter.
      * @param groupname string represention of group's name.
      * @return Group object.
      */
     public Group getGroup(String groupname) {
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
      * Returns list of groups, where is user specified by parameter member.
      * @param u User object representation of user.
      * @return List of Group objects.
      */
     public List<Group> getUserGroups(User u) {
	  List<Group> ret = new LinkedList<Group>();
	  for (Group_membership gg : u.get_groups()) {
	       if (getGroup(gg.get_group_name()) != null) {
		    ret.add(getGroup(gg.get_group_name()));
	       }
	  }
	  return ret;

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
      * Retuns list of users, whose are member of group specified by parameter.
      * @param g Group object represention of group.
      * @return List of Group objects.
      */
     public synchronized List<User> listMembers(Group g) {
	  List<User> ulist = new ArrayList<User>();
	  for (User u : users.values()) {
	       for (Group_membership m : u.get_groups()) {
		    if (m == null) {
			 System.out.println("je null");
		    } else {
			 m.print_content();
			 if (m.get_group_name().equals(g.get_group_name())) {
			      ulist.add(u);
			 }
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
	       if (u.compareTo(username) == 0 && users.get(u).get_password().compareTo(password) == 0) {
		    return true;
	       }
	  }
	  return false;
     }

     /**
      * Adds user to map of users in server cache.
      * @param login  string representation of user's login.
      * @param password  string representation of user's password hash.
      * @param name  string representation of user's name.
      * @param surname  string representation of user's surname.
      * @throws java.lang.Exception
      */
     public synchronized void addUser(String login, String password, String name, String surname) throws Exception {
	  for (User u : users.values()) {
	       if (u.get_login().equals(login)) {
		    throw new Exception("Login exists");
	       }
	  }
	  User created_usr = new User(name, surname, login, password);
	  this.users.put(login, created_usr);
     }

     /**
      * Deletes user from server cache.
      * @param username  string representation of user's login.
      * @throws java.lang.Exception
      */
     public synchronized void deleteUser(String username) throws Exception {
	  User u;
	  if ((u = users.get(username)) == null) {
	       System.out.println("Error during deleting client file on server, user does not exits.");
	       throw new Exception("User does not exists");
	  }
	  File fd = new File(userdict.getPath() + "/" + u.get_login() + ".xml");
	  if (!fd.exists()) {
	       System.out.println("Error during deleting client file on server, file does not exits.");
	       throw new Exception("File does not exists");
	  }
	  if (fd.delete() == false) {
	       System.out.println("Error during deleting client file on server");
	       throw new Exception();
	  }
	  users.remove(username);
     }

     /**
      * Creates a new group and adds it to server's cache.
      * @param groupname  string representation of group's name.
      * @param leader  string representation of user's login.
      * @return true if group was created, false otherwise.
      */
     public synchronized boolean createGroup(String groupname, String leader) {
	  try {
	       if (this.groups.containsKey(groupname)) {
		    throw new Exception("Skupina uz existuje");
	       }
	       this.groups.put(groupname, new Group(groupname, leader));
	       getUser(leader).add_group(new Group_membership(groupname, true));
	       syncToDisc();
	       return true;
	  } catch (Exception e) {
	       System.out.println("Group created");
	       e.printStackTrace();
	       return false;
	  }
     }

     /**
      * Deletes group from server's cache.
      * @param groupname  string representation of group's name.
      * @param user_name  string representation of user's login.
      * @return true if group was deleted, false otherwise.
      */
     public synchronized boolean deleteGroup(String groupname, String user_name) {
	  Group g;
	  if ((g = groups.get(groupname)) == null) {
	       System.out.println("No such Group in List: Error during deleting client file on server, user does not exits.");
	       return false;
	  }
	  if (getGroup(groupname).get_boss().equals(user_name) != true) {
	       return false;
	  }

	  File fd = new File(gropudict.getPath() + "/" + g.get_group_name() + ".xml");
	  if (!fd.exists()) {
	       System.out.println("File does not exists: Error during deleting group file on server, file does not exits.");
	       return false;
	  }
	  if (fd.delete() == false) {
	       System.out.println("Cannot delete: Error during deleting client file on server");
	       return false;
	  }
	  //je potrebne vymazat clenstvo v skupine aj u ostatnych uzivateloch
	  for (User usr : users.values()) {
	       removeGroupMembership(groupname, usr.get_login());
	  }
	  groups.remove(groupname);
	  return true;
     }
     //metoda maze clenstvo v danej skupine u daneho uzivatela
     /**
      * Removes group membership in group specified by parameter at user specified by parameter.
      * @param group_name   string representation of group's name.
      * @param user_name  string representation of user's login.
      * @return true if mebership was deleted, false otherwise.
      */
     public synchronized boolean removeGroupMembership(String group_name, String user_name) {
	  User u = getUser(user_name);
	  for (Group_membership m : u.get_groups()) {
	       if (m.get_group_name().equals(group_name)) {
		    u.remove_group(m);
		    return true;
	       }
	  }
	  return false;
     }

     /**
      *  Returns event specified by parameter of user specified by parameter.
      * @param event_name  string representation of group's name.
      * @param user  string representation of user's login.
      * @return true if everything was ok, false otherwise.
      */
     public synchronized Callendar_event getEvent(String event_name, String user) {
	  try {
	       for (Callendar_event e : users.get(user).get_callendar().get_events()) {
		    if (e.get_name().compareTo(event_name) == 0) {
			 return e;
		    }
	       }
	       System.out.println("Event was not found.");
	       return null;
	  } catch (Exception e) {
	       System.out.println("Event was not found.");
	       return null;
	  //throw e;
	  }
     //throw new Exception();
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
	       System.out.println("Event was not added to user.");
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
	       for (Callendar_event e : user.get_callendar().get_events()) {
		    if (e.get_name().compareTo(event_name) == 0) {
			 user.get_callendar().remove_event(e);
			 return true;
		    }
	       }
	       return false;
	  } catch (Exception e) {
	       System.out.println("Event was not romeved from user.");
	       return false;
	  }
     }

     /**
      * Adds group event to server cache.
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
	       try {
		    g.get_callendar().add_event(e);
	       } catch (Exception ee) {
		    ee.printStackTrace();
		    System.out.println("Didnt add group event");
		    return false;
	       }
	       return true;
	  } else {
	       System.out.println("Event was not added to group.");
	       return false;
	  }
     }

     /**
      * Removes group event from server cache.
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
      * Offer copy of event specified by parameter from user specified by parameter to user specified by parameter.
      * @param event_name string representation of event's name
      * @param from string representation of user's login.
      * @param to string representation of usert's login.
      * @return true if no errors occured, false othewise.
      */
     public synchronized boolean offerCopy(String event_name, String from, String to) {
	  try {
	       offeredCopies.add(new EventCopyRequest(event_name, from, to));
	       return true;
	  } catch (Exception e) {
	       System.out.println("Copy was added to offer list.");
	       return false;
	  }
     }

     /**
      * Accept copy of event specified by parameter from user specified by parameter to user specified by parameter.
      * @param event_name string representation of event's name
      * @param from string representation of user's login.
      * @param to string representation of usert's login.
      * @return true if no errors occured, false othewise.
      */
     public synchronized boolean acceptCopy(String event_name, String from, String to) {
	  EventCopyRequest ref = new EventCopyRequest(event_name, from, to);
	  try {
	       if (offeredCopies.contains(ref)) {
		    User recv = getUser(to);
		    recv.get_callendar().add_event(getEvent(event_name, from));
		    offeredCopies.remove(ref);
		    return true;
	       } else {
		    System.out.println("Copy was not accepted.");
		    return false;
	       }
	  } catch (Exception e) {
	       e.printStackTrace();
	       System.out.println("Copy was not accepted.");
	       return false;
	  }
     }

     /**
      * Offer membership from group specified by parameter to user specified by parameter.
      * @param g  string representation of group's name.
      * @param to_user  string representation of user's login.
      * @return true if mebership was offered, false otherwise.
      */
     public synchronized boolean offerMembership(String g, String to_user) {
	  try {
	       //doplnit ci uz je clenom
	       if (isMember(getUser(to_user), getGroup(g))) {
		    return false;
	       }
	       offeredMemberships.add(new MembershipRequest(g, to_user));
	       return true;
	  } catch (Exception e) {
	       System.out.println("Membership was not aded to offer list.");
	       return false;
	  }
     }

     /**
      * Accept membership from group specified by parameter to user specified by parameter.
      * @param g  string representation of group's name.
      * @param to_user  string representation of user's login.
      * @return true if mebership was accepted, false otherwise.
      */
     public synchronized boolean acceptMembership(String g, String to_user) {
	  MembershipRequest ref = new MembershipRequest(g, to_user);
	  if (offeredMemberships.contains(ref)) {
	       Group recv = getGroup(g);
	       User u = getUser(to_user);
	       try {
		    u.add_group(new Group_membership(recv.get_group_name(), false));
	       } catch (Exception ee) {
		    ee.printStackTrace();
		    System.out.println("Already member");
	       }
	       offeredMemberships.remove(ref);
	       return true;
	  } else {
	       System.out.println("Membership was not accepted");
	       return false;
	  }
     }

     /**
      * Refuses membership from group specified by parameter to user specified by parameter.
      * @param g  string representation of group's name.
      * @param to_user  string representation of user's login.
      * @return true if mebership was refused, false otherwise.
      */
     public synchronized boolean refuseMembership(String g, String to_user) {
	  MembershipRequest ref = new MembershipRequest(g, to_user);
	  if (offeredMemberships.contains(ref)) {
	       offeredMemberships.remove(ref);
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
     public synchronized boolean addGroupAlert(String user_name, String groupname, String event_name, Event_alert alrt) {
	  try {
	       //zistim ze ci je uzivatel clenom skupiny a aj sefom
	       boolean is_b = false;
	       for (Group_membership m : getUser(user_name).get_groups()) {
		    if (m.get_group_name().equals(groupname)) {
			 if (m.is_boss()) {
			      is_b = true;
			 }
		    }
	       }
	       if (!is_b) {
		    return false;
	       }

	       //pridanie alertu
	       for (Callendar_event e : getGroup(groupname).get_callendar().get_events()) {
		    if (e.get_name().equals(event_name)) {
			 e.add_alert(alrt);
			 return true;
		    }
	       }
	       System.out.println("Alert was not added to group.");
	       return false;
	  } catch (Exception b) {
	       System.out.println("Alert was not added to group.");
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
     public synchronized boolean removeGroupAlert(String user_name, String groupname, String event_name, Event_alert alrt) {
	  try {
	       //zistim ze ci je uzivatel clenom skupiny a aj sefom
	       boolean is_b = false;
	       for (Group_membership m : getUser(user_name).get_groups()) {
		    if (m.get_group_name().equals(groupname)) {
			 if (m.is_boss()) {
			      is_b = true;
			 }
		    }
	       }
	       if (!is_b) {
		    return false;
	       }
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
     public synchronized boolean addGroupNotice(String user_name, String groupname, String event_name, Event_notice note) {
	  try {
	       //musim overit clenstvo v skupine
	       boolean is_member = false;
	       for (Group_membership m : getUser(user_name).get_groups()) {
		    if (m.get_group_name().equals(groupname)) {
			 is_member = true;
		    }
	       }
	       if (!is_member) {
		    return false;
	       }
	       //clenstvo je overene, musim najst spravny event
	       for (Callendar_event c : getGroup(groupname).get_callendar().get_events()) {
		    if (c.get_name().equals(event_name)) {
			 c.add_notice(note);
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
      * Removes group notice specified by parameter from event of group, both specified by parameter. 
      * @param user_name string representation of user's login.
      * @param  groupname string representation of group's name.
      * @param event_name string representation of event's name.
      * @param note Event_notice representation of notice.
      * @return true if notice was removed, false otherwise.
      */
     public synchronized boolean removeGroupNotice(String user_name, String groupname, String event_name, Event_notice note) {
	  try {
	       //musim overit clenstvo v skupine
	       boolean is_member = false;
	       for (Group_membership m : getUser(user_name).get_groups()) {
		    if (m.get_group_name().equals(groupname)) {
			 is_member = true;
		    }
	       }
	       if (!is_member) {
		    return false;
	       }
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
     public synchronized boolean editGroupNotice(String user_name, String groupname, String event_name, Event_notice note) {
	  try {
	       //musim overit clenstvo v skupine
	       boolean is_member = false;
	       for (Group_membership m : getUser(user_name).get_groups()) {
		    if (m.get_group_name().equals(groupname)) {
			 is_member = true;
		    }
	       }
	       if (!is_member) {
		    return false;
	       }
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
      * Updates user's information according to specified parameter.
      * @param userdata string representation of XML user informations.
      * @throws java.lang.Exception
      */
     public synchronized void updateUser(String userdata) throws Exception {
	  StringBuffer length = new StringBuffer();
	  int pos = 0;
	  while (true) {
	       if (userdata.charAt(pos) == ' ') {
		    break;
	       }

	       length.append(userdata.charAt(pos));
	       pos++;

	  }

	  pos++;

	  StringBuffer userxmldata = new StringBuffer();
	  int toread = 0;
	  int max = Integer.parseInt(length.toString());
	  while (toread < max) {
	       userxmldata.append(userdata.charAt(pos));
	       pos++;
	       toread++;
	  }
	  pos++;

	  User u = User.fromXML(RequestHandler.getStringAsRootElement(userxmldata.toString()));

	  users.remove(u.get_login());
	  users.put(u.get_login(), u);

     }

     /**
      * Returns string representation of all information about user  specified by parameter exctracted from server's cache. 
      * @param username string representation of user's login.
      * @return string representaiton of all user's information.
      * @throws java.lang.Exception
      */
     public synchronized String getUserDataAndGroups(String username) throws Exception {

	  String tosend = "";
	  String userdata = RequestHandler.getElementAsDocument(getUser(username).toXML());
	  //dlzka kolko sa ma nacitat
	  tosend = tosend + Integer.toString(userdata.length()) + " " + userdata + " ";

	  List<Group> grps = getUserGroups(getUser(username));
	  for (Group g : grps) {
	       String grp = RequestHandler.getElementAsDocument(g.toXML());
	       tosend = tosend + Integer.toString(grp.length()) + " " + grp + " ";
	  }

	  return tosend;

     }

     /**
      * Returns owner of event spesified by parameter.
      * @param e Callendar_event representation of event.
      * @return User object.
      */
     public synchronized User getEventOwner(Callendar_event e) {
	  return null; //???????????????????
     }

     /**
      * Returns singleton of server sontroler object.
      * @return singleton of server sontroler object.
      */
     public static synchronized ServerController getDefault() {
	  if (default_controller == null) {
	       default_controller = new ServerController("./users", "./groups", true);
	  }
	  return default_controller;
     }
}
