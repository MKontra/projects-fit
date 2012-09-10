package client;

import java.util.concurrent.*;
import java.util.*;
import java.net.*;
import java.io.*;
import javax.net.ssl.*;
import javax.security.cert.X509Certificate;
import java.security.KeyStore;

/**
 * Represents network parts of the client's aplication. SSL comunication is used.
 * @author Matus Kontra
 */
public class NetworkClient {

     /**
      * Represents host name of the remote server, which will be used to connect on it.
      */
     protected String hostname;
     /**
      * Represents listening port of the remote server, which will be used to connect on it.
      */
     protected int port;
     /**
      * Identifies, whether connection is established. 
      */
     private boolean established;
     private BufferedReader in;
     private BufferedWriter out;
     private BlockingQueue<String> incoming;
     private Thread listener;
     private boolean online = false;

     /**
      * Crrate the NetworkClient object with specified parameters.
      * @param hostname represent network name of the remote server to connect.
      * @param port represent of the remote server to connect.
      */
     public NetworkClient(String hostname, int port) {
	  //for testing purposes
	  System.setProperty("javax.net.ssl.keyStore", "mySrvKeystore");
	  System.setProperty("javax.net.ssl.keyStorePassword", "kalendar");
	  System.setProperty("javax.net.ssl.trustStore", "mySrvKeystore");
	  System.setProperty("javax.net.ssl.trustStorePassword", "kalendar");

	  this.port = port;
	  this.hostname = hostname;
	  incoming = new LinkedBlockingQueue<String>();
	  //this.loggedIn = false;
	  this.online = false;
	  this.established = false;
     }

     /**
      * Connects client's side to server using SSL.
      * @throws java.lang.Exception
      */
     public synchronized void Connect() throws Exception {

	  if (!established) {
	       try {
		    SSLSocketFactory socketFactory = (SSLSocketFactory) SSLSocketFactory.getDefault();
		    SSLSocket socket = (SSLSocket) socketFactory.createSocket(this.hostname, this.port);

		    this.in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
		    this.out = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()));
		    this.established = true;

		    this.online = true;
		    (listener = new Thread(new PacketReceiverThread())).start();
	       } catch (Exception e) {
		    e.printStackTrace();
		    this.online = false;
	       }
	  } else {
	       throw new Exception("Already established connection");
	  }
     }

     /**
      * Finds out, whether client is online or no.
      * @return true if NetworkClient object is online, false otherwise.
      */
     public boolean isOnline() {
	  return online;
     }

     /**
      * Sets the flag, which represents status of client's comunication.
      * @param val boolean representation of client's connection status.
      */
     public void setOnlineFlag(boolean val) {
	  this.online = val;
     }

     /**
      * Reads packet from output stream of the communication's channel.
      * @return string representation of packet's content.
      * @throws java.lang.Exception
      */
     private String readPacket() throws Exception {
	  synchronized (this.in) {
	       if (established) {
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
	       } else {
		    throw new Exception("Connection not established");
	       }
	  }
     }

     /**
      * Sends packet to output stream of the communication channel.
      * @param str string representation of data to send.
      * @throws java.lang.Exception
      */
     private void sendPacket(String str) throws Exception {
	  synchronized (this.out) {
	       if (established) {
		    String tosend = Integer.toString(str.length()) + " " + str;
		    this.out.write(tosend);
		    this.out.flush();
	       }
	  }
     }

     /**
      * Sends data to output stream of the communication channel.
      * @param data string representation of data to send.
      * @throws java.lang.Exception
      */
     public void send(String data) throws Exception {
	  sendPacket(data);
     }

     /**
      * Returns first packet from que of packets.
      * @return string representation of packet. 
      */
     public String getNextResponse() {
	  String ret = null;
	  try {
	       ret = incoming.take();
	  } catch (Exception e) {
	       e.printStackTrace();
	  }
	  return ret;
     }
     /**
      * Closes the communication channel of the NetworkClient Object. Before it sends the packet to server.
      */
     public void close() {
	  try {
	       sendPacket("CLIENTOFF");
	       listener = null;
	       established = false;
	       in.close();
	       out.close();
	  } catch (Exception e) {
	       System.out.println("Error closing socket");
	  }
     }
     /**
      * Returns thread which is processing accepts of packets from server's side. 
      * @return Thread object representing thread.
      */
     public Thread getListener() {
	  return listener;
     }
     /**
      * Represents object which is recieving packet from server. 
      */
     private class PacketReceiverThread implements Runnable {

	  public void run() {
	       Thread thisThread = Thread.currentThread();
	       while (listener == thisThread) {
		    try {
			 String fromS = readPacket();
			 System.out.println("fromserver:" + fromS);
			 if (fromS == null) {
			      throw new Exception();
			 }
			 incoming.put(fromS);
		    } catch (Exception e) {
			 e.printStackTrace();
			 try {
			      incoming.put("SERVEROFF");
			      listener = null;
			 } catch (Exception ex) {
			      listener = null;
			      System.out.println("Cannot append to queue");
			 }
		    }

	       }
	  }
     }
     /**
     public static void main(String[] arg) throws Exception
     {
     NetworkClient nc = new NetworkClient("localhost", 1103);
     nc.logInConnect("xkontr00","hovado");
     nc.send("LOGIN xkontr00 ff");
     NetworkClient ncz = new NetworkClient("localhost", 1103);
     ncz.logInConnect("xkontr00","hovado");
     ncz.send("LOGIN xhomol00 ff");
     }**/
}
