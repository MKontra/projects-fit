package COMMON;

import java.util.*;
import java.lang.String.*;
import org.jdom.*;

/**
 * Represents notice. Event class containts list of this object type.
 * @author ivo
 */
public class Event_notice {

     /**
      * Represents login of author of notice.
      */
     protected String author; //login uzivatela, ktory ju zadal
     /**
      * Represents date, when was notice entered. 
      */
     protected Date time_entered;
     /**
      * Represents body of notice.
      */
     protected String body;

     //public void Event_notice(){}
     /**
      * Creates new  Event_notice object with specified parameters
      * @param a string representation of author.
      * @param d Date representation of date.
      * @param b string representaiton of body event.
      */
     public Event_notice(String a, Date d, String b) {
	  author = a;
	  time_entered = d;
	  body = b;
     }

     /**
      * Sets the author of notice object, which is specified in parameter.
      * @param a
      */
     public void set_author(String a) {
	  author = a;
     }

     /**
      * Sets the date of entering notice to date specified in parameter.
      * @param a Date representation of date.
      */
     public void set_date(Date a) {
	  time_entered = a;
     }

     /**
      * Sets the body of notice object.
      * @param a string representation of notice body.
      */
     public void set_body(String a) {
	  body = a;
     }

     /**
      * Returns the author of notice object.
      * @return string representation of author of notice object
      */
     public String get_author() {
	  return author;
     }

     /**
      * Returns the body of notice.
      * @return string representation of notice body.
      */
     public String get_body() {
	  return body;
     }

     /**
      * Returns date, when was notice entered.
      * @return Date representaution of date, when was notice entered.
      */
     public Date get_date() {
	  return time_entered;
     }

     /**
      * Return Event_notice representation of XML Elemnet specified in parameter.
      * @param note XML Element.
      * @return Event_notice representation of notice.
      */
     public static Event_notice fromXML(Element note) {
	  String note_body = note.getText();
	  String note_author = note.getAttributeValue("Author");
	  Date note_time_entered = new Date(Long.parseLong(note.getAttributeValue("Time_entered")));
	  return new Event_notice(note_author, note_time_entered, note_body);
     }

     /**
      * Return XML Element representation of notice.
      * @return XML Element representation of notice.
      */
     public Element toXML() {

	  Element notice = new Element("Notice");
	  notice.setText(this.get_body());
	  notice.setAttribute("Author", this.get_author());
	  notice.setAttribute("Time_entered", Long.toString(this.get_date().getTime()));
	  return notice;
     }

     /**
      * Prints content of Event_notice object to standard output.
      */
     public void print_content() {
	  String au = this.get_author();
	  String bd = this.get_body();
	  Date dt = this.get_date();
	  System.out.printf("\t\tNotice. Author: %s Date: %s Body: %s\n", au, dt.toString(), bd);
     }

     /**
      * Compares Event_notice object with another.
      * @param o object of type Object.
      * @return true if objects are equal, false otherwise.
      */
     public boolean equals(Object o) {
	  if (o instanceof Event_notice) {
	       Event_notice ce = (Event_notice) o;
	       return this.author.equals(ce.author) && this.time_entered.equals(ce.time_entered);
	  } else {
	       return false;
	  }
     }

     /**
      * Returns hash code of Event_notice object.
      * @return integer representation of hash code of Event_notice object.
      */
     public int hashCode() {
	  return this.author.hashCode() - this.time_entered.hashCode() * this.author.hashCode() + this.time_entered.hashCode();
     }
} 
