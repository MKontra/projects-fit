package myserver;

import java.util.concurrent.*;
import java.util.*;
import java.net.*;
import java.io.*;
import javax.net.ssl.*;
import javax.security.cert.X509Certificate;
import java.security.KeyStore;

/**
 * Represents connection manager of the server object. 
 * @author Matus Kontra
 */
public class ConnectionManager {

     private Map<String, Connection> openconnections;
     private ServerController sc;

     /**
      * Represents single connection object of connection manager.
      */
     public class Connection implements Runnable {

	  private Socket sock;
	  private BufferedReader in;
	  private BufferedWriter out;
	  private String user;
	  private Thread listener;
	  private BlockingQueue<String> incoming;

	  /**
	   * Creates a connection object asociated with the specified socket and user.
	   * @param ssckt Socket object representation of socket.
	   * @param user string representation of user.
	   * @throws java.lang.Exception
	   */
	  public Connection(Socket ssckt, String user) throws Exception {
	       this.user = user;
	       this.sock = ssckt;
	       this.incoming = new LinkedBlockingQueue<String>();
	       this.in = new BufferedReader(new InputStreamReader(sock.getInputStream()));
	       this.out = new BufferedWriter(new OutputStreamWriter(sock.getOutputStream()));

	       (listener = new Thread(this)).start();

	  }

	  /**
	   * Returns user asociated with connection object.
	   * @return string representation of user's login.
	   */
	  public String getUser() {
	       return this.user;
	  }

	  /**
	   * Represents main loop of connection process, when packets are recieved and added to to que.
	   */
	  public void run() {
	       Thread thisThread = Thread.currentThread();
	      System.out.println("Server side of connection listening");
	       while (listener == thisThread) {
		    try {

			 String fromS = receiveFromHere();
			 if (fromS == null) {
			      throw new Exception();
			 }
			 incoming.put(fromS);
		    } catch (Exception e) {
			 e.printStackTrace();
			 try {
			      incoming.put("CLIENTOF ");
			      listener = null;
			 } catch (Exception ex) {
			      listener = null;
			      System.out.println("Cannot append to queue");
			 }
		    }

	       }
	  }

	  /**
	   * Returns the first packet from que of recieved packets.
	   * @return string representation of packet.
	   */
	  public String getNextPacket() {
	       String ret = null;
	       try {
		    ret = incoming.take();
	       } catch (Exception e) {

	       }
	       return ret;
	  }

	  /**
	   * Recieves data from input stream of socket asocited with connection object and returned them as a string.
	   * @return string representation of packet's data.
	   * @throws java.lang.Exception
	   */
	  public String receiveFromHere() throws Exception {
	       String s_pcklen = "";
	       while (true) {
		    int c = this.in.read();
		    if (c == -1) {
			 throw new Exception("Unexcepted close");
		    }
		    if (((char) c) == ' ') {
			 break;
		    }
		    s_pcklen = s_pcklen + (char) c;
	       }

	       int pcklen = Integer.parseInt(s_pcklen);

	       String contents = "";
	       while ((pcklen--) > 0) {
		    int c = this.in.read();
		    if (c == -1) {
			 throw new Exception("Unexcepted close");
		    }
		    contents = contents + (char) c;
	       }

	       return contents;

	  }

	  /**
	   * Writes data specified by parameter to output stream of socket of connection object. 
	   * @param data string representation of data.
	   * @throws java.lang.Exception
	   */
	  public void sendDataHere(String data) throws Exception {

	       String tosend = Integer.toString(data.length()) + " " + data;
	       out.write(tosend);
	       out.flush();

	  }

	  /**
	   * Sends packet of closing connection to client and close connection of connection object.
	   * Also closes input and output stream of conneciton object.
	   * @throws java.lang.Exception
	   */
	  public void closeConnection() throws Exception {
	       sendDataHere("SERVEROFF");
	       removeConnection(this.user);
	       in.close();
	       out.close();
	       sock.close();
	  }
     }

     /**
      * Creates new ConnectionManager object with default server controller.
      */
     public ConnectionManager() {
	  this.openconnections = Collections.synchronizedMap(new HashMap<String, Connection>());
	  this.sc = ServerController.getDefault();
     }

     /**
      *  Reads the packet from socket specified by parameter and returns its string representation.
      * @param sscket Socket object representation of socket.
      * @return string representaiton of read packet.
      * @throws java.lang.Exception
      */
     public static String readPacket(Socket sscket) throws Exception {

	  Reader in = new InputStreamReader(sscket.getInputStream());
	  String s_pcklen = "";
	  while (true) {
	       int c = in.read();
	       if (((char) c) == ' ') {
		    break;
	       }
	       s_pcklen = s_pcklen + (char) c;
	  }



	  int pcklen = Integer.parseInt(s_pcklen);

	  String contents = "";
	  while ((pcklen--) > 0) {
	       contents = contents + (char) in.read();
	  }
	  return contents;
     }

     /**
      * Sends the data to socket, both specified by parameters.
      * @param sscket Socket object representation of socket.
      * @param data string representaiton of read packet.
      * @throws java.lang.Exception
      */
     public static void sendPacket(Socket sscket, String data) throws Exception {
	  Writer out = new OutputStreamWriter(sscket.getOutputStream());
	  String tosend = Integer.toString(data.length()) + " " + data;
	  out.write(tosend);
	  out.flush();
     }

     /**
      * Adds connection and user who logged in to map of connection and users of connection manager.
      * @param user string representation of user's login.
      * @param con Connection object representation of connection.
      */
     public void addConnection(String user, Connection con) {
	  openconnections.put(user, con);
     }

     /**
      * Removes the connection from map of connections and users of connection manager. Removed by key - user's login.
      * @param user string representation of user's login.
      */
     public void removeConnection(String user) {
	  openconnections.remove(user);
     }

     /**
      * Returns Connection object excracted from map of connections and users according to key specified by parameter.
      * @param user string representation of user's login.
      * @return Connection object representation of connection.
      */
     public Connection getConnection(String user) {
	  return openconnections.get(user);
     }

     /**
      * Returns actual instance of connection manager object.
      * @return ConnectionManager object of actual instance.
      */
     public ConnectionManager getThis() {
	  return this;
     }

     /**
      * Allows add connection to connection manager based on loggining of user or registration of user.
      */
     private class ConnAdder implements Runnable {

	  private Socket src;

	  /**
	   * Create  ConnAdder object with the specified socket.
	   * @param src Socket object representation of socket.
	   */
	  public ConnAdder(Socket src) {
	       this.src = src;
	  }

	  /**
	   * Method performs registering of new user or loggining of existing user. It creates new conneciton and adds it to connection list.
	   */
	  public void run() {
	       try {
		    //sendPacket(this.src,"");
		    String fone = readPacket(this.src);
		    if (fone.substring(0, 5).equals("LOGIN")) {
			 //Check creditenials
			 // if ok create connection and register it and pass it to handler
			 String[] tkns = fone.substring(6).split(" ");
			 String username = tkns[0];
			 String pass = tkns[1];
			 String op = tkns[2];

			 System.out.println("login request" + username + " " + pass);

			 boolean success = sc.authenticateUser(username, pass);

			 if (success) {

			      if (op.equals("to")) {

				   sc.updateUser(fone.substring(6 + username.length() + 1 + pass.length() + 1 + op.length() + 1));
				   sendPacket(src, "ALLUINFO " + sc.getUserDataAndGroups(username));

			      } else if (op.equals("from")) {

				   sendPacket(src, "ALLUINFO " + sc.getUserDataAndGroups(username));

			      }   //wtf ?

			      Connection nw = new Connection(src, username);
			      nw.sendDataHere("LOGINOKK ");
			      addConnection(username, nw);
			      RequestHandler rh = new RequestHandler(nw, getThis());
			 }

		    } else if (fone.substring(0, 8).equals("REGISTER")) {
			 String[] tkns = fone.substring(9).split(" ");
			 String login = tkns[0];
			 String name = tkns[1];
			 String surname = tkns[2];
			 String pass = tkns[3];
			 System.out.println("registration" + login + " " + pass);

			 try {
			      sc.addUser(login, pass, name, surname);
			 } catch (Exception e) {
			      System.out.println("Reg failed");
			      throw e;
			 }
			 Connection nw = new Connection(src, login);
			 nw.sendDataHere("REGISTOK ");
			 addConnection(login, nw);
			 RequestHandler rh = new RequestHandler(nw, getThis());

		    } else {
			 src.close();
		    }

	       } catch (Exception e) {
		    System.out.println("During wait");
		    e.printStackTrace();
	       } finally {

		    try {
			 src.close();
		    } catch (Exception e) {
		    }

	       }

	  }
     }

     /**
      * Creates new instance of conneciton adder object and starts thread to process it.
      * @param sscket Socket representation of socket.
      * @throws java.lang.Exception
      */
     public void addConnection(Socket sscket) throws Exception {
	  new Thread(new ConnectionManager.ConnAdder(sscket)).start();
     }
}
