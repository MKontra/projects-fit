package client;

import COMMON.*;
import java.io.*;
import java.util.*;
import java.util.concurrent.*;
import org.jdom.*;
import org.jdom.input.*;
import org.jdom.output.*;
import java.security.MessageDigest;

/**
 * Represents controller of client network aplication. Offers metodes which makes loggining to server and edits client's database.
 *  These actions are performed by communication handler of client. 
 * @author Matus Kontra
 */
public class ClientController {

     /**
      * Represents login of user who is logged in.
      */
     private String user;
     /**
      * Represents graphical user interface of client's aplication.
      */
     private ClientGUI gui;
     
     /**
      * Comonent which is processing incoming requests
      */     
     private CommunicationHandler ch;
     
     /**
      * Represents network parts of client's comunication with server.
      */
     private NetworkClient nc;
     /**
      * Represents status of client's user , whether he is logged in or not. 
      */
     private Boolean loggedIn; //flag koli obmedzeniu viacnasobneho pripojenia jedneho uzivatela
     /**
      * Represents local cache of client's side. 
      */
     private LocalCache lc = LocalCache.getDefaultCache();

     public ClientController(NetworkClient nc) {
	  this.ch = new CommunicationHandler(nc, this);
	  this.nc = nc;
	  this.loggedIn = false;
     }
     private String cmode;
     
     public void showUI()
     {
     System.out.println(user);
     	this.gui = new ClientGUI(this);
      this.gui.showMainWindow();
     }

     /**
      * Login user to server or to local cache according to status of connection.
      * @param username string representation of user's login.
      * @param password username string representation of user's password.
      * @param mode username string representation of recieveing and sending data to and from server.
      * @throws java.lang.Exception
      */
     public void LogIn(String username, String password, String mode) throws Exception {
	  //musi sa zisit ci je klient pripojeny na zaklade flagu v network klientovi
	  //????????? ROZLISIT CI JE KLIENT ON LINE ALEBO OFFLINE A PODLA TOHO SA PRIHLASIT DO LOCAL CACHE ALEBO NA SERVER????
	  if (!loggedIn) {
	       this.user = username;
	       String tosend = "";
	       cmode = mode;
	       if (nc.isOnline()) {
		    if (mode.equals("to")) {
			 String userdata = Client.getElementAsDocument(lc.getUser(username).toXML());
			 tosend = tosend + Integer.toString(userdata.length()) + " " + userdata;
			 nc.send("LOGIN " + username + " " + password + " to " + tosend);
		    } else if (mode.equals("from")) {

			 nc.send("LOGIN " + username + " " + password + " from");

		    }
	       } else {
		    //je potrebne overit ci sa uzivatel nachadza v lokalnej cachi, ak nie prihlasenie mu nebude umoznene
		    if (lc.users.containsKey(username)) {

			 if (lc.authenticateUser(username, password)) {
			      this.user = username;
			      this.loggedIn = true;
			 } else {
			   throw new Exception();
			 }
		    } else {
			   throw new Exception();
		    }
	       }


	  }
     }
     
     public String getLogin()
     {
      return user;
     }
     
     public boolean onlineMode()
     {
      return nc.isOnline();
     }

     /**
      * Performs actions after server mirroring or client's cache refreshing to server according to incomming mode included in server's response.
      * @param userdata string representaiton of data which includes server's response.
      * @throws java.lang.Exception
      */
      
    Object loglock = new Object();  
      
    public void waitForLog()
    {
    synchronized (loglock) {
      while ( loggedIn == Boolean.FALSE )
      {
        try{
          loglock.wait();
        } catch (InterruptedException ie)
        {}
      }
    }
    }
      
     public void LogDone(String userdata) throws Exception {



	  if (cmode.equals("to")) {

	       setUserDataAndGroups(userdata, false);


	  } else if (cmode.equals("from")) {

	       setUserDataAndGroups(userdata, true);

	  }
	  


    synchronized (loglock) {
      loggedIn = true;
      loglock.notify();
    }    


     }
     private String stored_login;
     private String stored_name;
     private String stored_surname;
     private String stored_password;

     /**
      * Notify server about registering user.
      * @param login string representaiton of user's login.
      * @param name string representaiton of user's name.
      * @param surname string representaiton of user's surname.
      * @param password string representaiton of user's password.
      * @throws java.lang.Exception
      */
     public void Register(String login, String name, String surname, String password) throws Exception {
	  if (!loggedIn && nc.isOnline()) {

	       nc.send("REGISTER " + login + " " + name + " " + surname + " " + password);
	       stored_login = login;
	       stored_name = name;
	       stored_surname = surname;
	       stored_password = password;
	       loggedIn = false;
	  } else {
	       throw new Exception("Already logged in or not online.");
	  }
     }

    public void waitForReg()
    {
    synchronized (loglock) {
      while ( loggedIn == Boolean.FALSE )
      {
        try{
          loglock.wait();
        } catch (InterruptedException ie)
        {}
      }
    }
    }


     /**
      * Performs register action at client's side according to server's response to register request.
      */
     public void RegisterAccepted() {
	  //registracia bola akceptovana na servri a preto sa moze vytvorit uzivatel v local cachi
	  lc.addUser(stored_login, stored_password, stored_name, stored_surname);
	  synchronized (loglock) {
      loggedIn = true;
      loglock.notify();
    }    
    	       this.user = stored_login;
     }

     /**
      * Performs actions such as adding user and his groups to local cache after loginnig to server if his database did not existed.
      * @param userdata string representation of data which server sends to as respond.
      * @param setuser if true, user will be added to local cache after loggining when his database file did not exists. 
      * If false his database file have already exists so information about him and his calendar were used from it and there is no need to
      * download them from server.
      * @throws java.lang.Exception
      */
     public void setUserDataAndGroups(String userdata, boolean setuser) throws Exception {
	  System.out.println("raw length " + userdata.length());

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

	  if (setuser) {
	       User u = User.fromXML(Client.getStringAsRootElement(userxmldata.toString()));

	       lc.addUser(u);
	  }

	  LinkedList<Group> skpiny = new LinkedList<Group>();

	  System.out.println("Parsing grups position " + pos);

	  while (pos < userdata.length()) {
	       StringBuffer grlen = new StringBuffer();
	       while (true) {
		    if (userdata.charAt(pos) == ' ') {
			 break;
		    }

		    grlen.append(userdata.charAt(pos));
		    pos++;
	       }
	       pos++;

	       StringBuffer grpdata = new StringBuffer();
	       toread = 0;
	       max = Integer.parseInt(grlen.toString());
	       while (toread < max) {
		    grpdata.append(userdata.charAt(pos));
		    pos++;
		    toread++;
	       }
	       pos++;
	       System.out.println("finished group len: " + grpdata.toString().length() + " " + grpdata.toString());
	       skpiny.add(Group.fromXML(Client.getStringAsRootElement(grpdata.toString())));
	  }
	  lc.addGroups(skpiny);

     }
     List<Callendar_event> pending_events_additions = new LinkedList<Callendar_event>();

     //metoda posle udalost fo forme xml metodou send
     /**
      * Sends the event addition request to server or adds it to local cache according to connection status. 
      * Adds event to pending event' additions list.
      * @param eventname string representation of event's name.
      * @param date Date representation of date.
      * @param repeating boolean representation event's repeating.
      * @param often integer representation of numbers of hours to repeat event. 
      * @param category integer representation of event's category
      * @throws java.lang.Exception
      */
     public void addEvent(String eventname, Date date, boolean repeating, int often, int category) throws Exception {
System.out.println(loggedIn + " " + nc.isOnline() );
	  if (loggedIn) {
	       Callendar_event ev = new Callendar_event(eventname, date, repeating, often, category);
	       if (nc.isOnline()) {
		    nc.send("ADDEVENT " + Client.getElementAsDocument(ev.toXML())); ///???????? .toString, prekladac nehubuje
		    pending_events_additions.add(ev);
	       } else {
		           lc.addEvent(lc.getUser(user), ev);
	       }

	  }
     }

     /**
      * Confirms addition of event on server's side and adds event to local cache. Removes event from list of pending events addition.
      * @param eventname  string representation of event's name.
      */
     public void confirmEventAddition(String eventname) {
	  for (Callendar_event ce : pending_events_additions) {
	       if (ce.get_name().equals((eventname))) {
		    //prislo potvrdenie od servra, tympadom moze byt pridane do lok cache
		    lc.addEvent(lc.getUser(user), ce);
		    pending_events_additions.remove(ce);
		    break;
	       }
	  }
     }

     /**
      * Refuse event's addition on server's side and removes it from pending events addition list.
      * @param eventname string representation of event's name.
      */
     public void refuseEventAddition(String eventname) {
	  for (Callendar_event ce : pending_events_additions) {
	       if (ce.get_name().equals((eventname))) {
		    pending_events_additions.remove(ce);
		    break;
	       }
	  }
     }

     /**
      * Sends request of event deletion to server or removes it from local cache according to status of connection.
      * @param eventname string representation of event's name.
      * @throws java.lang.Exception
      */
     public void deleteEvent(String eventname) throws Exception {
	  if (loggedIn) {
	       if (nc.isOnline()) {
		    nc.send("DELEVENT " + eventname);
	       }
	   else {
	       lc.removeEvent(lc.getUser(user), eventname);
	  }
     }
     }

     /**
      * Confirms event removal on server and removes event from local cache.
      * @param eventname string representation of event's name.
      */
     public void confirmEventRemoval(String eventname) {
	  for (Callendar_event ce : lc.getUser(user).get_callendar().get_events()) {
	       if (ce.get_name().equals(eventname)) {
		    //prislo potvrdenie od servra, tympadom moze byt odobrate z local cache
		    lc.removeEvent(lc.getUser(user), eventname);
		    break;
	       }
	  }
     }
     private List<Group> pendingGroupCreations = new ArrayList<Group>();

     /**
      * Sends request to create group to server or creates group according to status of connection. 
      * Also adds group creation to pending group creation list.
      * @param groupname string representation of group's name.
      * @throws java.lang.Exception
      */
     public void createGroup(String groupname) throws Exception {

	  if (loggedIn) {
	       Group g = new Group(groupname, user);
	       if (nc.isOnline()) {
		    nc.send("CRTGROUP " + groupname);
		    pendingGroupCreations.add(g);
	       } else {
          throw new Exception("Cannot delete in offline mode");
	       }
	  }
     }

     /**
      * Confirms group creation on server and create gorup in local cache. Also removes group creation from pending group creation list.
      * @param groupname string representation of group's name.
      */
     public void confirmGroupCreation(String groupname) {
	  for (Group ce : pendingGroupCreations) {
	       if (ce.get_group_name().equals((groupname))) {
		    //prislo potvrdenie od servra, tympadom moze byt pridane do lok cache
		    lc.createGroup(groupname, user);
		    pendingGroupCreations.remove(ce);
		    break;
	       }
	  }
     }

     /**
      * Refuses group creation on server's side and removes it from pending group creation list. 
      * @param eventname string representation of group's name
      */
     public void refuseGroupAddition(String eventname) {
	  for (Group ce : pendingGroupCreations) {
	       if (ce.get_group_name().equals((eventname))) {
		    pendingGroupCreations.remove(ce);
		    break;
	       }
	  }
     }

     /**
      * Send delete group request to server only in online mode.
      * @param groupname string representation of group's name
      * @throws java.lang.Exception
      */
     public void deleteGroup(String groupname) throws Exception {

	  if (loggedIn) {
	       if (nc.isOnline()) {
		    nc.send("DELGROUP " + groupname);
	       } else
	       {
          throw new Exception("Cannot delete in offline mode");
         }
	  } else {
	  //lc.removeGroup(groupname);
	  }
     }

     /**
      * Removes grup's event from local cache.
      * @param groupname  string representation of group's name
      */
     public void deleteGroupResponse(String groupname) {
	  synchronized (lc) {
	       lc.removeGroup(groupname);
	  }
     }

     /**
      * Deletes user's membership of group in local cache according to group name parameter.
      * @param groupname  string representation of group's name
      */
     public void confirmGroupDeletion(String groupname) {
	  for (Group_membership m : new LinkedList<Group_membership>(lc.getUser(user).get_groups())) {
	       if (m.get_group_name().equals(groupname)) {
		    lc.getUser(user).remove_group(m);
	       }
	  }
     }

     /**
      * Represents association of user and callendar event, which can be offered or recieved from and to another user.
      */
     private static class CopyRequest {

	  /**
	   * Represents event to offer.
	   */
	  Callendar_event ce;
	  /**
	   * Reprsents login of user who offers.
	   */
	  String user;

	  CopyRequest(String user, Callendar_event ce) {
	       this.ce = ce;
	       this.user = user;
	  }

	  /**
	   * Campares CopyRequest object with another object. 
	   * @param cr
	   * @return true if object is equal with object specified in parameter, false otherwise.
	   */
	  public boolean equals(Object cr) {
	       if (cr instanceof CopyRequest) {
		    CopyRequest c = (CopyRequest) cr;
		    return (this.ce.equals(c.ce)) && (c.user.equals(this.user));
	       }
	       return false;
	  }

	  /**
	   * Returns integer representation of CopyRequest object.
	   * @return integer representation of CopyRequest object.
	   */
	  public int hashCode() {
	       return user.hashCode() - ce.hashCode() * user.hashCode() + user.hashCode();
	  }
     }
     private List<CopyRequest> event_confirmations_list = new ArrayList<CopyRequest>();
     //ok?????????????????????
     /**
      * Offers event copy specified by parameter to user specfied by parameter. 
      * @param user string representation of user's login.
      * @param eventname string representation of event name.
      * @throws java.lang.Exception
      */
     public void offerEvent(String user, String eventname) throws Exception {
	  if (loggedIn) {
	       if (nc.isOnline()) {
		    nc.send("CPYEVENT " + user + " " + eventname);
	       } else
	       {
         	   throw new Exception("You are in offline mode");
         }
	  } else {
	  


	  }
     }

     /**
      * Notify gui, when event offer arrives and add event to confirmation list.
      * @param user_from string representation of sender's login.
      * @param eventXMLcode string representation of event which has arrived from sender.
      * @throws java.lang.Exception
      */
     public void receiveEvent(String user_from, String eventXMLcode) throws Exception {

	  if (loggedIn) {
	       if (nc.isOnline()) {
		    Callendar_event nw = Callendar_event.fromXML(Client.getStringAsRootElement(eventXMLcode));
		    event_confirmations_list.add(new CopyRequest(user_from, nw));
		    gui.EventAdded(user_from, nw);
	       }
	  } else {

	  }
     }

     /**
      * Confirms accepting of event offer, adding event to local cache and also notify server, that event was accepted. Removes event from event confirmation list.
      * @param user_from string representation of sender's login.
      * @param nw XML Element representation of event which was accepted.
      * @throws java.lang.Exception
      */
     public void confirmEventRecieving(String user_from, Callendar_event nw) throws Exception {

	  if (loggedIn) {
	       if (nc.isOnline()) {
		    CopyRequest ref = new CopyRequest(user_from, nw);
		    for (CopyRequest ce : event_confirmations_list) {
			 if (ce.equals(ref)) {
			      //prislo potvrdenie od servra, tympadom moze byt pridane do lok cache
			      lc.addEvent(lc.getUser(user), ce.ce);
			      event_confirmations_list.remove(ce);
			      nc.send("ACCEVENT " + ref.user + " " + ce.ce.get_name());
			      gui.reloadui();
			      break;
			 }
		    }
	       }
	  } else {

	  }
     }

     /**
      * Refuses event offer from other user based on user's decision. Removes event from event confirmation list.
      * @param user_from string representation of sender's login.
      * @param nw XML Element representation of event which was refused.
      * @throws java.lang.Exception
      */
     public void refuseEvent(String user_from, Callendar_event nw) throws Exception {

	  if (loggedIn) {
	       if (nc.isOnline()) {
		    CopyRequest ref = new CopyRequest(user_from, nw);
		    for (CopyRequest ce : event_confirmations_list) {
			 if (ce.equals(ref)) {
			      event_confirmations_list.remove(ce);
			      nc.send("RFSEVENT " + ref.user + " " + ce.ce.get_name());
			      break;
			 }
		    }
	       }
	  } else {

	  }
     }
     
     
     /**
      * Sends request of adding new member of group to server. 
      * @param groupname string representation of group's name.
      * @param username string representation of user's name
      * @throws java.lang.Exception
      */
     public void addMember(String groupname, String username) throws Exception
     {
     
	  if (loggedIn) {
      	       if (nc.isOnline())
      	       {
      	       			      nc.send("OFFERMBS " + username + " " + groupname);
      	       	} else
      	       	{ }
	       
	  } else {

	  }     
     
     }
      /**
      * Sends request of deleting member of group to server. 
      * @param groupname string representation of group's name.
      * @param username string representation of user's name
      * @throws java.lang.Exception
      */
     public void removeMember(String groupname, String username) throws Exception
     {
	  if (loggedIn) {
      	       if (nc.isOnline()) {}
      	       			      nc.send("DELMEMBR " + username + " " + groupname);
	       
	  } else {

	  }      
     }
     
     
     
     
     private List<Group> pending_membership_offers = new LinkedList<Group>();

     //metoda posle udalost fo forme xml metodou send
     //
     //---------------------------------------------------------odtialto
     //
     /**
      * Notify gui, when group membership was offered. Add group to pending groups list.
      * @param groupname  string representation of  group name.
      * @param groupXMLcode XML Element representation of group.
      * @throws java.lang.Exception                  
      */
     public void offerMembership(String groupname, String groupXMLcode) throws Exception {
	  if (loggedIn) {
	       if (nc.isOnline()) {
		    Group g = Group.fromXML(Client.getStringAsRootElement(groupXMLcode));
		    pending_membership_offers.add(g);
	      gui.MembershipAdded(g);
	       } else {
	       
	       
	       
	       }

	  }
     }

     /**
      * Confirms membership, adds group and membership to local cache, send notification to server and also removes group from pending groups list.
      * @param groupname  string representation of  group name.
      * @throws java.lang.Exception
      */
     public void confirmMembership(String groupname) throws Exception {
	  for (Group g : pending_membership_offers) {
	       if (g.get_group_name().equals((groupname))) {
		    try {
			 //ak nohodou v lokalnej cachi bola skupina predtym tak sa skusi vymazat
			 lc.removeGroup(groupname);
		    } catch (Exception e) {
		    }
		    lc.addGroup(g);
		    lc.addMembership(groupname, user);
		    pending_membership_offers.remove(g);
		    nc.send("ACPTMBSH " + groupname);
		    			      gui.reloadui();
		    break;
	       }
	  }
     }

     /**
      * Refuses group membership offer. Removes it from pending group membership list and also sends notify to server.
      * @param groupname  string representation of  group name.
      * @throws java.lang.Exception
      */
     public void refuseMembership(String groupname) throws Exception {
	  for (Group g : pending_membership_offers) {
	       if (g.get_group_name().equals((groupname))) {
		    pending_membership_offers.remove(g);
		    nc.send("REFUSMBS " + groupname);
		    break;
	       }
	  }
     }

     /**
      * Try to add gorup event to group, so sends notify to server.
      * @param groupname  string representation of  group name.
      * @param ce XML Element representation of event.
      * @throws java.lang.Exception
      */
     public void addGroupEvent(String groupname, String eventname, Date date, boolean repeating, int often, int category) throws Exception {
	  if (loggedIn) {
	       if (nc.isOnline()) {
          	       Callendar_event ev = new Callendar_event(eventname, date, repeating, often, category);
		    nc.send("ADDGRPEV " + groupname + " " + Client.getElementAsDocument(ev.toXML()));

	       } else {   throw new Exception("Cannot add event"); }
	  } else {
	  throw new Exception("Cannot add event");
	  }
     }

     /**
      * Adds group's event to local cache when server confirms adding of it.
      * @param groupname string representation of  group name.
      * @param groupevent XML Element representation of event.
      * @throws java.lang.Exception
      */
     public void receiveAddGroupEvent(String groupname, String groupevent) throws Exception {
	  if (loggedIn) {
	       if (nc.isOnline()) {

		    lc.addGroupEvent(lc.getGroup(groupname), Callendar_event.fromXML(Client.getStringAsRootElement(groupevent)));
         gui.reloadui();
	       //notify gui
	       }
	  } else {
	  //notify gui
	  }
     }

     /**
      * Try to remove group event, so sends request to server.
      * @param groupname string representation of  group name.
      * @param ce XML Element representation of event.
      * @throws java.lang.Exception
      */
     public void removeGroupEvent(String groupname, String eventname) throws Exception {
	  if (loggedIn) {
	       if (nc.isOnline()) {

		    nc.send("REMGRPEV " + groupname + " " + eventname);


	       }  else {   throw new Exception("Cannot remove event"); }
	  } else {
	  //notif gui
	  }
     }

     /**
      * Removes group event from loccal cache based on server's confirming of it.
      * @param groupname string representation of  group name.
      * @param eventname XML Element representation of event.
      */
     public void receiveRemoveGroupEvent(String groupname, String eventname) {
	  if (loggedIn) {
	       if (nc.isOnline()) {

		    lc.removeGroupEvent(lc.getGroup(groupname), eventname);

      gui.reloadui();
	       }
	  } else {
	  //notify gui
	  }
     }

     /**
      * Adds group's notice of event to local cache. 
      * @param groupname string representation of  group name.
      * @param eventname string representation of  event name.
      * @param noticeXMLCode XML Element representation of notice.
      * @throws java.lang.Exception
      */
     public void receivedEventNotice(String groupname, String eventname, String noticeXMLCode) throws Exception {

	  lc.addGroupNotice(groupname, eventname, Event_notice.fromXML(Client.getStringAsRootElement(noticeXMLCode)));

     }

     /**
      * Adds notice to event in local cache and notify server about it according to status of connection.
      * @param eventname string representation of  event name.
      * @param text string representation of  body of notice.
      * @throws java.lang.Exception
      */
     public void addEventNotice(String eventname, String text) throws Exception {

	  if (loggedIn) {
	       Event_notice en = new Event_notice(user, new Date(), text);
	       if (nc.isOnline()) {

		    nc.send("ADDEVNTC " + eventname + " " + Client.getElementAsDocument(en.toXML()));
		    System.out.println("pridavam poznamku: " + lc.addEventNotice(user, eventname, en));
	       } else {
		    lc.addEventNotice(user, eventname, en);
	       }
	  }
     }

     /**
      * Removes notice from event in local cache and notify server about it according to status of connection.
      * @param eventname string representation of  event name.
      * @param note_in_xml  XML Element representation of notice.
      * @throws java.lang.Exception
      */
     public void removeEventNotice(String eventname, Event_notice note) throws Exception {

	  if (loggedIn) {
	       if (nc.isOnline()) {
		    nc.send("REMEVNTC " + eventname + " " + Client.getElementAsDocument(note.toXML()));
		    lc.removeEventNotice(user, eventname, note);
	       } else {
		    lc.removeEventNotice(user, eventname, note);
	       }
	  }
     }

     /**
      * Edits event's note in local cache and sends notification about it to server.
      * @param eventname string representation of  event name.
      * @param note_in_xml XML Element representation of notice.
      * @throws java.lang.Exception
      */
     public void editEventNotice(String eventname, Event_notice note) throws Exception {

	  if (loggedIn) {

	       if (nc.isOnline()) {

		    nc.send("EDITNOTE " + eventname + " " + Client.getElementAsDocument(note.toXML()));
		    lc.editEventNotice(user, eventname, note);
	       } else {
		    lc.editEventNotice(user, eventname, note);
	       }
	  }
     }

     /**
      * Adds alert to event in local cache and sends the notification to server about it.
      * @param eventname string representation of  event name.
      * @param repeatable boolean representation of  repeatability.
      * @param interval integer representation of hours to repeat.
      * @throws java.lang.Exception
      */
     public void addEventAlert(String eventname, Event_alert alert) throws Exception {

	  if (loggedIn) {
	       if (nc.isOnline()) {
		    nc.send("ADDALERT " + eventname + " " + Client.getElementAsDocument(alert.toXML()));
		    lc.addEventAlert(user, eventname, alert);
	       } else {
		    lc.addEventAlert(user, eventname, alert);
	       }
	  }
     }

     /**
      * Removes alert from event in local cache and sends notification to server about it.
      * @param eventname string representation of  event name.
      * @param alert_in_xml_text XML Element representation of alert.
      * @throws java.lang.Exception
      */
     public void removeEventAlert(String eventname, Event_alert alert) throws Exception {

	  if (loggedIn) {
	       if (nc.isOnline()) {
		    nc.send("REMALERT " + eventname + " " + Client.getElementAsDocument(alert.toXML()));
		    lc.removeEventAlert(user, eventname, alert);
	       } else {
		    lc.removeEventAlert(user, eventname, alert);
	       }
	  }
     }

     /**
      * Send request of adding gorup alert to server.
      * @param group string representation of group name.
      * @param eventname string representation of  event name.
      * @param alert_in_xml_text XML Element representation of alert.
      * @throws java.lang.Exception
      */
     public void addGroupAlert(String group, String eventname, Event_alert alert) throws Exception {
	  if (loggedIn) {
	       if (nc.isOnline()) {
		    nc.send("ADDGRPAL " + group + " " + eventname + " " + Client.getElementAsDocument(alert.toXML()));
	       //lc.addGroupAlert(user, group, eventname, Event_alert.fromXML(Client.getStringAsRootElement(alert_in_xml_text)));
	       } else {
	       //notify gui
	       }
	  }
     }

     /**
      * Adds alert to group in local cache according to server's response.
      * @param group string representation of group name.
      * @param eventname  string representation of  event name.
      * @param alert_in_xml_text XML Element representation of alert.
      * @throws java.lang.Exception
      */
     public void groupAlertAddResponse(String group, String eventname, String alert_in_xml_text) throws Exception {
	  lc.addGroupAlert(group, eventname, Event_alert.fromXML(Client.getStringAsRootElement(alert_in_xml_text)));
     //guintfy
     }

     /**
      * Send request of removing gorup alert to server.
      * @param group string representation of group name.
      * @param eventname string representation of  event name.
      * @param alert_in_xml_text XML Element representation of alert.
      * @throws java.lang.Exception
      */
     public void removeGroupAlert(String group, String eventname, Event_alert alert) throws Exception {
	  if (loggedIn) {
	       if (nc.isOnline()) {
		    nc.send("REMGRPAL " + group + " " + eventname + " " + Client.getElementAsDocument(alert.toXML()));

	       } else {
	       //notify gui
	       }
	  }
     }

     /**
      * Removes alert from group in local cache according to server's response.
      * @param group string representation of group name.
      * @param eventname  string representation of  event name.
      * @param alert_in_xml_text XML Element representation of alert.
      * @throws java.lang.Exception
      */
     public void groupAlertRemoveResponse(String group, String eventname, String alert_in_xml_text) throws Exception {
	  lc.removeGroupAlert(group, eventname, Event_alert.fromXML(Client.getStringAsRootElement(alert_in_xml_text)));
     }

     /**
      * Send request of adding group notice to server.
      * @param group string representation of group name.
      * @param eventname  string representation of  event name.
      * @param note_in_xml_text Element representation of notice.
      * @throws java.lang.Exception
      */
     public void addGroupNote(String group, String eventname, String text) throws Exception {
	  if (loggedIn) {
	     	 Event_notice en = new Event_notice(user, new Date(), text);
	       if (nc.isOnline()) {
		    nc.send("ADDGRPNT " + group + " " + eventname +  " " + Client.getElementAsDocument(en.toXML()));

	       } else {
	       //notify gui
	       }
	  }
     }

     /**
      * Adds group's notice to local cache.
      * @param group  string representation of group name.
      * @param eventname  string representation of  event name.
      * @param note_in_xml_text XML Element representation of notice.
      * @throws java.lang.Exception
      */
     public void groupNoteAddResponse(String group, String eventname, String note_in_xml_text) throws Exception {
	  lc.addGroupNotice(group, eventname, Event_notice.fromXML(Client.getStringAsRootElement(note_in_xml_text)));
     }

     /**
      * Send request of removing group notice to server.
      * @param group string representation of group name.
      * @param eventname string representation of  event name.
      * @param note_in_xml_text  XML Element representation of notice.
      * @throws java.lang.Exception
      */
     public void removeGroupNote(String group, String eventname, Event_notice en) throws Exception {
	  if (loggedIn) {
	       if (nc.isOnline()) {
		    nc.send("DELGRPNT " + group + " " + eventname + " " + Client.getElementAsDocument(en.toXML()));

	       } else {
	       //notify gui
	       }
	  }
     }

     /* * Removes group's note from local cache.
      * @param group string representation of group name.
      * @param eventname string representation of  event name.
      * @param note_in_xml_text XML Element representation of notice.
      * @throws java.lang.Exception
      */
     public void groupNoteRemoveResponse(String group, String eventname, String note_in_xml_text) throws Exception {
	  lc.removeGroupNotice(group, eventname, Event_notice.fromXML(Client.getStringAsRootElement(note_in_xml_text)));
     }

     /** Send request of editing group's notice to server.
      * @param group string representation of group name.
      * @param eventname string representation of  event name.
      * @param note_in_xml_text XML Element representation of notice.
      * @throws java.lang.Exception
      */
     public void editGroupNote(String group, String eventname, Event_notice en) throws Exception {
	  if (loggedIn) {
	       if (nc.isOnline()) {
		    nc.send("EDTGRPNT " + group + " " + eventname + " " + Client.getElementAsDocument(en.toXML()));

	       } else {
	       //notify gui
	       }
	  }
     }

     /**
      * Edits group's notice in local cache.
      * @param group string representation of group name.
      * @param eventname string representation of  event name.
      * @param note_in_xml_text XML Element representation of notice.s
      * @throws java.lang.Exception
      */
     public void groupNoteEditResponse(String group, String eventname, String note_in_xml_text) throws Exception {
	  lc.editGroupNotice(group, eventname, Event_notice.fromXML(Client.getStringAsRootElement(note_in_xml_text)));
     }
}













