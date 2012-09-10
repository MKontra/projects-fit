package myserver;

import java.util.*;
import java.net.*;
import java.io.*;
import javax.net.ssl.*;
import javax.security.cert.X509Certificate;
import java.security.KeyStore;

/**
 * Represents server of calendar aplication.
 * @author Matus Kontra
 */
public class Server implements Runnable {
     /**
      * Represent the primary thread of server aplication.
      */
     private Thread serverthread;
    /**
     * Represents ssl server socket.
     */
     private SSLServerSocket sslser;
     /**
      * Represents conection manager of whole server.
      */
     private ConnectionManager cm;
     int port;
     /**
      * Creates a server object with the specified listening port.
      * @param port integer representation of port.
      */
     public Server(int port) {
	  //test...
	  System.setProperty("javax.net.ssl.keyStore", "mySrvKeystore");
	  System.setProperty("javax.net.ssl.keyStorePassword", "kalendar");
	  System.setProperty("javax.net.ssl.trustStore", "mySrvKeystore");
	  System.setProperty("javax.net.ssl.trustStorePassword", "kalendar");
	  this.port = port;
	  this.cm = new ConnectionManager();
     }

     //main server loop
     /**
      * Represents main loop of server where listening is performed. Starts a thread when there is a new connection accepted.
      */
     public void run() {
	  SSLSocket ssl_socket;
	  Thread thisThread = Thread.currentThread();
	  while (serverthread == thisThread) {
	       try {
		    ssl_socket = (SSLSocket) this.sslser.accept();


		    cm.addConnection(ssl_socket);
	       } catch (Exception e) {
		    e.printStackTrace();
	       }
	  }


     }
     /**
      * Starts the server which Server object represents .
      * @throws java.lang.Exception
      */
     public void startServer() throws Exception {
	  SSLServerSocketFactory sslserversocketfactory = (SSLServerSocketFactory) SSLServerSocketFactory.getDefault();
	  this.sslser = (SSLServerSocket) sslserversocketfactory.createServerSocket(this.port);

	  this.serverthread = new Thread(this);
	  this.serverthread.start();

     }
     /**
      * Stops the server which server object represents.
      * @throws java.lang.Exception
      */
     public void stopServer() throws Exception {
	  serverthread = null;
	  sslser.close();
     }
     /**
      * Main metod of server aplication.
      * @param args array of strings.
      * @throws java.lang.Exception
      */
     public static void main(String[] args) throws Exception {
     int portnum = 11111;
     try{
      portnum = Integer.parseInt(args[0]); 
      } catch (Exception e) { portnum = 11111; }
	  Server s = new Server(portnum);


	  s.startServer();
     }
}
