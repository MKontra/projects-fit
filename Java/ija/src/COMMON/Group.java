/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package COMMON;

import java.io.File;
import org.jdom.*;
import org.jdom.input.*;
import org.jdom.output.*;
import java.util.*;
import java.text.*;
import java.io.*;
import java.lang.String.*;

/**
 *Represents group, which groups two or more users to such object instance.
 * @author Ivan Homoliak
 */
public class Group {

     /**
      * Represents name of group.
      */
     protected String group_name;
     /**
      * Represnets name of group's leader.
      */
     protected String boss;
     /**
      * Represents calendar asociated with group object.
      */
     protected Callendar callendar;
     /**
      * Create new Group object.
      */
     public Group() {
     }
     /**
      * Create new Group object with specified parameters.
      * @param s string representaiton of gorup name.
      * @param b string representaiton of leader's name.
      */
     public Group(String s, String b) {
	  group_name = s;
	  boss = b;
	  callendar = new Callendar();
     }
      /**
      * Create new Group object with specified parameters.
      * @param s string representaiton of gorup name.
      * @param b string representaiton of leader's name.
       * @param c Callendar object representing calendar.
      */
     public Group(String s, String b, Callendar c) {
	  group_name = s;
	  boss = b;
	  callendar = c;
     }

     /**
      * Returns callendar of this gorup object.
      * @return Calendar of Callendar type.
      */
     public Callendar get_callendar() {
	  return callendar;
     }

     /**
      * Sets the calendar of gorup object to calendar specified as parameter.
      * @param c Callendar object which represents calendar. 
      */
     public void set_callendar(Callendar c) {
	  this.callendar = c;
     }

     /**
      * Returns group name.
      * @return String representation of group.
      */
     public String get_group_name() {
	  return group_name;
     }

     /**
      * Returns name of gorup leader.
      * @return String representation of group leader.
      */
     public String get_boss() {
	  return boss;
     }

     /**
      * Sets the group's leader name to name speciefied in parameter.
      * @param b string representation of leader name.
      */
     public void set_boss(String b) {
	  boss = b;
     }

     /**
      * Sets the group name of group object.
      * @param n string representation of group name.
      */
     public void set_group_name(String n) {
	  group_name = n;
     }

     /**
      * Retruns XML Element representation of group object.
      * @return XML Element representation of group object.
      */
     public Element toXML() {

	  Element group = new Element("Group");
	  group.setAttribute("Name", this.get_group_name());
	  group.setAttribute("Boss", this.get_boss());
	  group.addContent(this.get_callendar().toXML());
	  return group;
     }

     /**
      * Returns Group object from XML Element representation of group object.
      * @param group XML Element.
      * @return Group object.
      */
     public static Group fromXML(Element group) {

	  String n = group.getAttributeValue("Name");
	  String b = group.getAttributeValue("Boss");
	  Element e = group.getChild("Callendar");
	  Callendar c = Callendar.fromXML(e);
	  return new Group(n, b, c);

     }

     /**
      * Prints the content of group object to standard output stream.
      */
     public void print_content() {
	  System.out.printf("\tGroup. Name: %s Boss: %s\n", this.get_group_name(), this.get_boss());
	  this.get_callendar().print_content();
     }
     //staticka metoda vrati instanciu triedy User tak ze z prislusneho xml extrahuje potrebne informacie a naplni nimi instanciu
     /**
      * Return Group object excracted from group database file.
      * @param groupname Name of group to extract.
      * @return Group object.
      * @throws Exception
      */
     public static Group get_group(String groupname) throws Exception {

	  File fd = new File("./src/COMMON/groups/" + groupname + ".xml");

	  if (!fd.exists()) {
	       System.out.println("Group file does not exists.");
	       throw new Exception("Group file does not exists.");
	  }
	  //samotna extrakcia
	  FileInputStream fin = new FileInputStream(fd);
	  SAXBuilder builder = new SAXBuilder(false);
	  Document doc = builder.build(fd);

	  // ziskam element grupy a extrahujem z neho potrebne atributy
	  Element root = doc.getRootElement();
	  return Group.fromXML(root);

     }

     //metoda zapise do grupu do xml suboru 
     /**
      * Writes to group's database Group object given in parameter.
      * @param g group object.
      * @throws Exception
      */
     public static void put_group(Group g) throws Exception {
	  File fd = new File("./src/COMMON/groups/" + g.group_name + ".xml");
	  if (!fd.exists()) {
	       fd.createNewFile();
	  } //ak neexistuje vytvori sa novy
	  FileOutputStream fos = new FileOutputStream(fd);


	  Element grupa = g.toXML();
	  Document doc = new Document(grupa);

	  //zapis do suboru
	  XMLOutputter serializer = new XMLOutputter(org.jdom.output.Format.getPrettyFormat());
	  serializer.output(doc, fos);
	  fos.close();
     }
     //metoda zapise do grupu do xml suboru 
     /**
      * Writes to group's database Group object given by parameters.
      * @param g group object.
      * @param fdd abstract pathname, which represent path to database.
      * @throws Exception
      */
     public static void put_group(Group g, File fdd) throws Exception {
	  File fd = new File(fdd.getPath() + "/" + g.group_name + ".xml");
	  if (!fd.exists()) {
	       fd.createNewFile();
	  } //ak neexistuje vytvori sa novy
	  FileOutputStream fos = new FileOutputStream(fd);


	  Element grupa = g.toXML();
	  Document doc = new Document(grupa);

	  //zapis do suboru
	  XMLOutputter serializer = new XMLOutputter(org.jdom.output.Format.getPrettyFormat());
	  serializer.output(doc, fos);
	  fos.close();
     }

     /**
      * Compares Group object with another.
      * @param o object of type Object.
      * @return true if objects are equal, false otherwise.
      */
     public boolean equals(Object o) {
	  if (o instanceof Group) {
	       Group ce = (Group) o;
	       return this.get_group_name().equals(ce.get_group_name());
	  } else {
	       return false;
	  }
     }

     /**
      * Returns hash code of Group object, which is delegated to hash code of group's name.
      * @return integer representation of hash code of Group object.
      */
     public int hashCode() {
	  return this.get_group_name().hashCode();
     }
}
