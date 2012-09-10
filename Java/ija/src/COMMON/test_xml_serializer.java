package COMMON;

import java.util.*;
import java.io.*;
/**
 * Represents test class for COMMON package.
 * @author ivo
 */
public class test_xml_serializer {
     /**
      * Main metod of the test class.
      * @param argv array og string of arguments
      */
     public static void main(String[] argv) {
     try{
	  Event_notice poznamka1 = new Event_notice("xhomol11", new Date(), "bla bla ");
	  Event_notice poznamka2 = new Event_notice("xkontr00", new Date(), "dlheeeeeeeeeeeee telo poznamky2");
	  Callendar_event event1 = new Callendar_event("xhomol11", new Date(), false, 0, Callendar_event.MEETING);
	  event1.add_notice(poznamka1);
	  event1.add_notice(poznamka2);
	  event1.add_alert(new Event_alert(new Date(), 5, true));
	  event1.add_alert(new Event_alert(new Date(), 2, false));

	  Event_notice poznamka3 = new Event_notice("xbenco0011", new Date(), "dlheeeeeeeeeeeee telo poznamky");
	  Event_notice poznamka4 = new Event_notice("xbenco00", new Date(), "dlheeeeeeeeeeeee telo poznamky");
	  Callendar_event event2 = new Callendar_event("xhomol10", new Date(), false, 0, Callendar_event.ANNIVERSARY);
	  //event2.add_notice(poznamka3);
	  //event2.add_notice(poznamka4);
     }catch (Exception ex){
     ex.printStackTrace();
	  }
	  Callendar kalendar1 = new Callendar("Kalendar 1");
	  try {
	       //kalendar1.add_event(event1);
	       //kalendar1.add_event(event2);
	  } catch (Exception ex) {
	       ex.printStackTrace();
	  }

	  Group_membership group1 = new Group_membership("java developers", true);
	  Group_membership group2 = new Group_membership("acer users", false);

	  User user1 = new User("Ivan", "Homoliak", "xkontr00", "frer45t45t56y*898^$");
	  user1.set_callendar(kalendar1);
	  try {
	       user1.add_group(group1);
	       user1.add_group(group2);
	  } catch (Exception ex) {
	       ex.printStackTrace();
	  }

	  //serializacia
	  try {
	       User.put_User(user1); //vlozenie uzivatela do db - xml subor
	       User u = User.get_User("xkontr00"); //vytiahnutie atributov z db - xml suboru a naplnenie instancie User
	       u.show_users_content(); //vypisanie vnutorneho stavu objektu typu User
	       List<User> users = User.extract_users("./src/server/users");
	       Iterator q = users.iterator();
	       while (q.hasNext()) {
		    ((User) q.next()).show_users_content();//zobrazenie obsahu uzivatelov
	       }
	  } catch (Exception e) {
	       e.printStackTrace();
	  }
	  //testovanie ukladania grup
	  try {
	       Group g = new Group("Indiani kmen Apachov", "Winetou", kalendar1);
	       Group.put_group(g);
	       Group extracted = Group.get_group(g.get_group_name());
	       extracted.print_content();
	  } catch (Exception e) {
	       e.printStackTrace();
	  }
     }
} 
