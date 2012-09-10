package COMMON;

import java.util.*;
import org.jdom.*;
import java.text.*;

// REPREZENTUJE CLENSTVO V SKUPINE
/**
 *  Represents user's membership in group and his function. Respectively whether he is gorup leader or not.
 * @author Ivan Homoliak
 *
 */
public class Group_membership {

     /**
      * Name of group.
      */
     protected String group_name;
     /**
      * Identify whether owner of this object is leader in this group or no.	
      */
     protected Boolean boss = new Boolean(false);

     public Group_membership() {
     }
     /**
      * Creates new group membership object with specified parameters.
      * @param s string representation of group name.
      * @param b boolean representation of whether is owner of this object leader or not.
      */
     public Group_membership(String s, boolean b) {
	  group_name = s;
	  boss = b;
     }

     /**
      * Returns group name of membership object.
      * @return Name of group.
      */
     public String get_group_name() {
	  return group_name;
     }

     /**
      * Identify whether owner of this membership is leader of group or not. 
      * @return true if owner of this gorup is leader of group, otherwise false.
      */
     public boolean is_boss() {
	  return boss;
     }

     /**
      * Set function of owner of this object to leader or no.
      * @param true if owner is leader, false otherwise.
      */
     public void set_boss(boolean b) {
	  boss = b;
     }

     /**
      * Sets the name of group associated with membership object.
      * @param n Represents name of gorup.
      */
     public void set_group_name(String n) {
	  group_name = n;
     }

     /**
      * Returns XML Element representation of membership object.
      * @return Element representaiton of membership object.
      */
     public Element toXML() {
	  Element group = new Element("Group");
	  group.setAttribute("Name", this.get_group_name());
	  group.setAttribute("Boss", Boolean.toString(this.is_boss()));
	  return group;
     }

     /**
      * Returns Group_membership object from XML membership represention Element
      * @param <Element>   group XML Element filled with required structure.
      * @return Object of this class.
      */
     public static Group_membership fromXML(Element group) {

	  String n = group.getAttributeValue("Name");
	  Boolean b = Boolean.parseBoolean(group.getAttributeValue("Boss"));
	  return new Group_membership(n, b);
     }
     /**
      * Prints the content of this object to standard output.
      */
     public void print_content() {
	  System.out.printf("\tGroup. Name: %s Boss: %b\n", this.get_group_name(), this.is_boss());
     }
     /**
      * Compares Group_membership object with another.
      * @param o object of type Object.
      * @return true if objects are equal, false otherwise.
      */
     public boolean equals(Object o) {
	  if (o instanceof Group_membership) {
	       Group_membership ce = (Group_membership) o;
	       return this.group_name.equals(ce.get_group_name());
	  } else {
	       return false;
	  }
     }

     /**
      * Returns hash code of Group_membership object.
      * @return integer representation of hash code of Group_membership object.
      */
     public int hashCode() {
	  return this.group_name.hashCode();
     }
} 
