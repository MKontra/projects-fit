package client;

import COMMON.*;
import java.io.*;
import java.util.*;
import java.util.concurrent.*;
import org.jdom.*;
import org.jdom.input.*;
import org.jdom.output.*;
/**
 * Represents object which handles server's responses. Uses services of client's controller to perform requested operations.
 * @author Matus Kontra
 */
public class CommunicationHandler implements Runnable {

     private Thread response_processor;
     private NetworkClient nc;
     private ClientController cc;
     private LocalCache lc = LocalCache.getDefaultCache();
     /**
      * Creates new CommunicationHandler objects with specified NetworkClient object and ClientController object.
      * @param nc NetworkClient object asociated with actual comunication.
      * @param cc ClientController object asociated with actual comunication.
      */
     public CommunicationHandler(NetworkClient nc, ClientController cc) {
	  this.nc = nc;
	  this.cc = cc;

	  (response_processor = new Thread(this)).start();

     }
     /**
      * Represents main loop of recieving responses from server. 
      */
     public void run() {
	  Thread thisThread = Thread.currentThread();
	  try {
	       while (response_processor == thisThread) {
		    String pck = nc.getNextResponse();

		    System.out.println("Client Received pck----------");
		    System.out.println(pck);
		    System.out.println("-----------------------------");
		    
		    if (pck.length() == 0) {
			 continue;
		    }

		    String operation = pck.substring(0, 8);
		    String rest = pck.substring(9, pck.length());

		    if (operation.compareTo("EVEADDOK") == 0) //server potvrdil prijatie udalosti
		    {
			 cc.confirmEventAddition(rest);
		    } else if (operation.compareTo("EVEADDFL") == 0)//zlyhalo pridanie udalosti na servri
		    {
			 cc.refuseEventAddition(rest);

		    } else if (operation.compareTo("EVEDELOK") == 0) //server potvrdil zrusenie udalosti
		    {
			 cc.confirmEventRemoval(rest);
		    } else if (operation.compareTo("EVEDELFL") == 0)//zlyhalo odobranie udalosti
		    {
		    //cc.refuseEventRemoval(rest);

		    } else if (operation.compareTo("GRPCRTOK") == 0) //potvrd vytvorenie skupiny
		    {
			 //group creation ?
			 cc.confirmGroupCreation(rest);

		    } else if (operation.compareTo("GRPCRTFL") == 0) //odmietni vytvorenie skupiny
		    {
			 //removal ?
			 cc.refuseEventAddition(rest);

		    } else if (operation.compareTo("GRPDELOK") == 0) //zmazanie grupy prebehlo
		    {
		    //   


		    } else if (operation.compareTo("GRPDELFL") == 0) //zmazanie grupy zlyhalo
		    {


		    } else if (operation.compareTo("GRPMBSRV") == 0) //zrusenie clenstva v skupine
		    {
			 cc.confirmGroupDeletion(rest);

		    } else if (operation.compareTo("CPYOFFER") == 0) //ponuka zdielania udalosti
		    {

			 String[] tkns = rest.split(" ");
			 String fromuser = tkns[0];
			 cc.receiveEvent(fromuser, rest.substring(fromuser.length() + 1));

		    } else if (operation.compareTo("CPYEVEOK") == 0) //ponuka prebehla ok
		    {


		    } else if (operation.compareTo("CPYEVEFL") == 0) //ponuka zlyhala
		    {


		    } else if (operation.compareTo("MBSOFFER") == 0) //ponuka na clenstvo
		    {
			 String[] tkns = rest.split(" ");
			 String groupname = tkns[0];
			 cc.offerMembership(groupname, rest.substring(groupname.length() + 1));

		    } else if (operation.compareTo("MBSOFFOK") == 0) //ponuka ok
		    {


		    } else if (operation.compareTo("MBSOFFFL") == 0) //ponuka fail
		    {


		    } else if (operation.compareTo("ADGREVOK") == 0) // pridanie skupinovej udalosti zlyhalo
		    {


		    } else if (operation.compareTo("GRPEVADD") == 0) //prisla skupinova udalost
		    {

			 String[] tkns = rest.split(" ");
			 String groupname = tkns[0];
			 cc.receiveAddGroupEvent(groupname, rest.substring(groupname.length() + 1));


		    } else if (operation.compareTo("ADGREVFL") == 0) //pridanie skupinovej udalosti zlyhalo
		    {


		    } else if (operation.compareTo("RMGREVOK") == 0) //odstranenie skupinovej udalost ok
		    {


		    } else if (operation.compareTo("GRPEVERM") == 0) // skupinova udalost odstranena
		    {

			 String[] tkns = rest.split(" ");
			 String groupname = tkns[0];
			 cc.receiveRemoveGroupEvent(groupname, rest.substring(groupname.length() + 1));

		    } else if (operation.compareTo("RMGREVFL") == 0) //zlyhalo odstranenie skupinovej udalosti
		    {


		    } else if (operation.compareTo("ADGRALOK") == 0) //pridanie skupinoveho upozornenia ok
		    {


		    } else if (operation.compareTo("ADGRPALT") == 0) // notifikacia o pridani skupinoveho upozornenia
		    {

			 //???
			 String[] tkns = rest.split(" ");
			 String groupname = tkns[0];
			 String eventname = tkns[1];
			 cc.groupAlertAddResponse(groupname, eventname, rest.substring(groupname.length() + 1 + eventname.length() + 1));


		    } else if (operation.compareTo("ADGRALFL") == 0) //pridanie skupinoveho upozornenia zlyhalo
		    {


		    } else if (operation.compareTo("RMGRALOK") == 0) //remove group alert OK
		    {


		    } else if (operation.compareTo("GREVNTRM") == 0) //oznamenie o zruseni upozornenia
		    {

			 String[] tkns = rest.split(" ");
			 String groupname = tkns[0];
			 String eventname = tkns[1];
			 cc.groupAlertRemoveResponse(groupname, eventname, rest.substring(groupname.length() + 1 + eventname.length() + 1));


		    } else if (operation.compareTo("RMGRALFL") == 0) //remove group alert Failed
		    {


		    } else if (operation.compareTo("ADGRNTOK") == 0) //pridanie skupinovej poznamky ok
		    {


		    } else if (operation.compareTo("ADDGRPNT") == 0) //oznamenie o pridani skupinovej poznamky
		    {

			 String[] tkns = rest.split(" ");
			 String groupname = tkns[0];
			 String eventname = tkns[1];
			 cc.groupNoteAddResponse(groupname, eventname, rest.substring(groupname.length() + 1 + eventname.length() + 1));


		    } else if (operation.compareTo("ADGRNTFL") == 0) //pridanie poznamky zlyhalo
		    {


		    } else if (operation.compareTo("RMGRNTOK") == 0) //odobranie skupinovej poznamky ok
		    {


		    } else if (operation.compareTo("REMGRPNT") == 0) //oznamenie o odobrani skupinovej poznamky
		    {

			 String[] tkns = rest.split(" ");
			 String groupname = tkns[0];
			 String eventname = tkns[1];
			 cc.groupNoteRemoveResponse(groupname, eventname, rest.substring(groupname.length() + 1 + eventname.length() + 1));


		    } else if (operation.compareTo("RMGRNTFL") == 0) //odobranie  skupinovej poznamky zlyhalo
		    {


		    } else if (operation.compareTo("EDGRNTOK") == 0) //uprava skupinovej poznamky ok
		    {


		    } else if (operation.compareTo("EDTGRPNT") == 0) //oznamenie o upraveni skupinovej poznamky
		    {

			 String[] tkns = rest.split(" ");
			 String groupname = tkns[0];
			 String eventname = tkns[1];
			 cc.groupNoteEditResponse(groupname, eventname, rest.substring(groupname.length() + 1 + eventname.length() + 1));


		    } else if (operation.compareTo("EDGRNTFL") == 0) //uprava poznamky zlyhalo
		    {


		    } else if (operation.compareTo("ALLUINFO") == 0) {
          cc.LogDone(rest);

		    } else if (operation.compareTo("SERVEROF") == 0) {


		    }
		    else if(operation.compareTo("REGISTOK") == 0){ //registracia bola akceptovana, vytvor usra do lc
			 cc.RegisterAccepted();
		    }

	       }
	  } catch (Exception e) {
	       System.out.println("Vynimka pri pocuvani odservra");
	       e.printStackTrace();
	  }
     }
     /**
      * Stops  handling of responses from server perfomed by thread of this object.
      */
     public void stopProcessing() {
	  response_processor = null;
     }
}
