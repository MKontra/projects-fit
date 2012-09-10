package COMMON;

//import Callendar_event; 
import java.util.*;
import org.jdom.*;


//obsah instancii tejto triedy bude zapisovany do suborov reprezentujucich kalendare
/**
 *  The class represents the calendar with specific events.
 * 
 */
public class Callendar {

     /**
      * Array of events sotred in concrete calendar. 
      */
     protected List<Callendar_event> events = new ArrayList<Callendar_event>();
     //protected List members <Callendar_members> = new ArrayList <Callendar_members> ();
     /**
      * Name asociated with calendar.
      */
     protected String name;
     /**
      * Creates new Callendar object.
      */
     public Callendar() {
	  name = "";
     }
     /**
      * Create new Callendar object with specified by parameter
      * @param n string representation of calendar name
      */
     public Callendar(String n) {
	  name = n;
     }

     /**
      * Returns list of events encapsulated in calendar object.
      */
     public List<Callendar_event> get_events() {
	  return events;
     }

     /**
      *Returns calendar`s associated name. 
      */
     public String get_name() {
	  return name;
     }

     /**
      * Sets the name of the calendar
      * @param n String to set name of calendar.
      */
     public void set_name(String n) {
	  name = n;
     }

     /**
      * Adds event to calendar.
      * @param e <Callendar_event> The instance of Callendar_event class, which represents event. 
      */
     public void add_event(Callendar_event e)throws Exception {
	  if (events.contains(e)) {
	       throw new Exception("Event was not added becouse it already exists.");
	  }
	  events.add(e);
     }

     /**
      * Removes the event from calendar.
      * @param e <Callendar_event> The instance of Callendar_event class, which represents event. 
      */
     public void remove_event(Callendar_event e) {
	  events.remove(e);
     }

     /**
      * Returns the XML <Element> representation of the calendar.
      * @return the calendar in xml element format.
      */
     public Element toXML() {

	  Element callendar = new Element("Callendar");
	  callendar.setAttribute("Name", this.get_name());
	  //Element events = new Element("Events");
	  //callendar.addContent(events);

	  for (Callendar_event e : this.get_events()) {

	       Element event = e.toXML();
	       callendar.addContent(event);
	  }
	  return callendar;
     }

     /**
      * Returns the instance of <Callendar> class which is generated from XML <Element> representation.
      * @param callendar Element which represents XML object.  
      * @return Instance of <Callendar> class is represented by this calendar.
      */
     public static Callendar fromXML(Element callendar) {

	  String name = new String(callendar.getAttributeValue("Name"));
	  Callendar CALLENDAR = new Callendar();
	  CALLENDAR.set_name(name);

	  //prebehnem eventami 
	  List events = callendar.getChildren();
	  Iterator i = events.iterator();
	  while (i.hasNext()) {
	       Element event = (Element) i.next();
	       Callendar_event EVENT = Callendar_event.fromXML(event);
	       try{
		    CALLENDAR.add_event(EVENT); //popridavam eventy do kalendara
	       }
	       catch (Exception e){
		    System.out.println("Event cannot be added.");
		    e.printStackTrace();
	       }
	       }
	  
	  return CALLENDAR;
     }

     /**
      * Writes content of <Callendar> object to standard output stream. 
      */
     public void print_content() {
	  String cal = this.get_name();
	  List evnts = this.get_events();

	  Iterator i = evnts.iterator();
	  while (i.hasNext()) {
	       Callendar_event e = (Callendar_event) i.next();
	       e.print_content();
	  }
     }
} 
