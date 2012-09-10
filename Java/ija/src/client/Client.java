package client;

import dialogs.*;
import COMMON.*;
import java.io.*;
import java.util.*;
import java.util.concurrent.*;
import org.jdom.*;
import org.jdom.input.*;
import org.jdom.output.*;
import java.security.MessageDigest;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.table.*;

/**
 * Represents client and contains main metod.
 * @author Matus Kontra
 */
public class Client {

     private static SAXBuilder builder = new SAXBuilder();

     /**
      * Returns XML Element from string it's string representaiton.
      * @param in string representation of XML Element object.
      * @return XML Element object.
      * @throws java.lang.Exception
      */
     public static Element getStringAsRootElement(String in) throws Exception {
	  return builder.build(new ByteArrayInputStream(in.getBytes())).getRootElement();
     }
     private static XMLOutputter outputer = new XMLOutputter();

     /**
      * Returns string representation of XML Element specified by parameter.
      * @param e XML Element object.
      * @return string representation of XML Element object.
      * @throws java.lang.Exception
      */
     public static String getElementAsDocument(Element e) throws Exception {
	  return outputer.outputString(new Document(e));
     }

     /**
      * Main metod of Client class.
      * @param args array of string arguments of client's aplication.
      * @throws java.lang.Exception
      */
      
      
      
public static void main(String[] args) throws Exception {
     
  final LocalCache lc = LocalCache.getDefaultCache();
      
  	  SwingUtilities.invokeLater(new Runnable() {
  
  	       public void run() {
  	           boolean exit = false;
  	           final JFrame centparent = new JFrame();
              centparent.setLocation(300, 300); 
  	           while (!exit)
  	           {
  	              try {

                  ConnectionOptions lr = new ConnectionOptions(centparent, true);
                  lr.setLocationRelativeTo(centparent);
                  lr.setVisible(true);
                  
                  if (lr.getAction().equals("LOGIN") )
                  {
                    if ( lr.isOnlineLoggining() ) //prihlasenie na server
                    {
                      NetworkClient nc = new NetworkClient(lr.getServerName(), lr.getServerPort() );
                      nc.Connect();
                      ClientController operator = new ClientController(nc);
                      String mode = lr.sendDataToServerFirst() ? "to" : "from";
                      operator.LogIn(lr.getLogin(), lr.getPassword() , mode);
                      operator.waitForLog();
                      operator.showUI();
                      break;
                    } else        //pracuj lokalne
                    {
                      NetworkClient nc = new NetworkClient("", -1 );
                      //nc.Connect();
                      ClientController operator = new ClientController(nc);
                      operator.LogIn(lr.getLogin(), lr.getPassword() , "");
                      operator.showUI();
                      break;                    
                    }
                  } else if (lr.getAction().equals("REGISTER") )
                  {
                      Register rd = new Register(centparent, true);
                      rd.setLocationRelativeTo(centparent);
                      rd.setVisible(true);
                      
                      
                      if ( !rd.getAction().equals("CONFIRM") ) continue;
                      
                      
                      NetworkClient nc = new NetworkClient(lr.getServerName(), lr.getServerPort() );
                      nc.Connect();
                      ClientController operator = new ClientController(nc);
                      operator.Register( rd.getLogin(), rd.getName(),rd.getName(),rd.getPassword());
                      rd.dispose();
                      operator.waitForReg();
                      operator.showUI();
                      break;                  
                    
                  
                  } else {
                    exit = true;
                    System.exit(0); }
        
                  } catch (Exception e)
                  {
                    e.printStackTrace();
                    JOptionPane.showMessageDialog(centparent, "<html>Your attemt to log in or register caused an Exception.<br><br>" + 
                    "This may be due to bad password, already taken login during registration or <br>" +
                    " trying to work offline on computer where local copy of your account does not exists<html>", 
                    "Login or Registration failed", JOptionPane.ERROR_MESSAGE);
                  }

          

  		     
  		     
  	             }  	
  	             if (exit)
  	               System.exit(0);
              }       
  	  });

  }
}