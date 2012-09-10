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
 * Represents user of aplication Calendar2009.
 * @author Ivan Homoliak
 */
public class User {

     /**
      * Represents the name of user.
      */
     protected String name = "";
     /**
      * Represents the surname of user.
      */
     protected String surname = "";
     /**
      * Represents the login of user.
      */
     protected String login = "";
     /**
      * Represents the password hash of user.
      */
     protected String password_hash = ""; //v kryptovanej podobe
     /**
      * Represents list of memberships of user object.
      */
     protected List<Group_membership> list_of_groups = new LinkedList<Group_membership>();
     /**
      * Represents the calendar of user.
      */
     protected Callendar callendar = new Callendar(); //kalendar ktory ma uzivatel k dispozicii

     public User() {
     }

     public User(String n, String s, String l, String p) {
	  name = n;
	  surname = s;
	  login = l;
	  password_hash = p;
     }
     //metoda zobrazuje obsah uzivatela na stdout
     /**
      * Prints content of user to standard output stream.
      */
     public void show_users_content() {
	  User u = this;
	  String log = u.get_login();
	  String nam = u.get_name();
	  String sur = u.get_surname();
	  String passwd = u.get_password();
	  System.out.printf("Login: %s Meno: %s Priezvisko: %s Heslo: %s\n", log, nam, sur, passwd);

	  Callendar c = u.get_callendar();
	  c.print_content();

	  //prejdenie skupinami
	  List sks = u.get_groups();
	  Iterator y = sks.iterator();
	  while (y.hasNext()) {
	       Group_membership g = (Group_membership) y.next();
	       g.print_content();
	  }
     }
     //vrati zoznam vsetkych uzivatelov, ktorych vyextrahuje z databazy - parameter je zlozka z ktorej sa extrahuje
     /**
      * Returns list of all users exctracted from database given by parameter.
      * @param folder string representation of folder where is database located.
      * @return list of User representation of users.
      * @throws java.lang.Exception
      */
     public static List<User> extract_users(String folder) throws Exception {
	  File f = new File(folder);
	  String[] subory = f.list();

	  //do zoznamu popridavam uz vyextrahovanych uzivatelov
	  List<User> users = new ArrayList<User>();
	  for (String s : subory) {
	       if (s.substring(0, 1).compareTo(".") == 0) {
		    continue;
	       } //preskocenie skrytych suborov zo zoznamu -- especialy .svn
	       //System.out.println(s.substring(0,s.length() - 4));
	       User usr;
	       if ((usr = get_User(s.substring(0, s.length() - 4))).is_empty()) {
		    System.out.printf("Subor \"%s\" nema spravny format xml alebo neexistuje.\n", s);
		    throw new Exception("Subor nema spravny format xml alebo neexistuje.");
	       }
	       users.add(usr);
	  }
	  return users;
     }
     //staticka metoda vrati instanciu triedy User tak ze z prislusneho xml extrahuje potrebne informacie a naplni nimi instanciu
     /**
      * Returns the User object which is extracted from database according to login given in parameter.
      * @param l string representation of login of user.
      * @return User representation of user.
      * @throws java.lang.Exception
      */
     public static User get_User(String l) throws Exception {

	  File fd = new File("./src/COMMON/users/" + l + ".xml");
	  if (!fd.exists()) {
	       System.out.println("User file does not exists.");
	       throw new Exception("User file does not exists.");
	  }
	  //samotna extrakcia
	  FileInputStream fin = new FileInputStream(fd);
	  SAXBuilder builder = new SAXBuilder(false);
	  Document doc = builder.build(fd);

	  // ziskam uzivatelov element a extrahujem z neho potrebne atributy
	  Element root = doc.getRootElement();
	  return User.fromXML(root);

     }
     //metoda zapise do uzivatela xml 
     /**
      * Writes user given in parameter to database.
      * @param u User representation of user.
      * @throws java.lang.Exception
      */
     public static void put_User(User u) throws Exception {
	  File fd = new File("./src/COMMON/users/" + u.get_login() + ".xml");
	  if (!fd.exists()) {
	       fd.createNewFile();
	  } //ak neexistuje vytvori sa novy
	  FileOutputStream fos = new FileOutputStream(fd);


	  Element user = u.toXML();
	  Document doc = new Document(user);

	  //zapis do suboru
	  XMLOutputter serializer = new XMLOutputter(org.jdom.output.Format.getPrettyFormat());
	  serializer.output(doc, fos);
	  fos.close();
     }
     //metoda zapise do uzivatela xml 
     /**
      * Store user given by first parameter to database file given by second parameter.
      * @param u user representation of user
      * @param fdd abstract pathaname representation of path to database of users.
      * @throws java.lang.Exception
      */
     public static void put_User(User u, File fdd) throws Exception {
	  File fd = new File(fdd.getPath() + "/" + u.get_login() + ".xml");
	  if (!fd.exists()) {
	       fd.createNewFile();
	  } //ak neexistuje vytvori sa novy
	  FileOutputStream fos = new FileOutputStream(fd);


	  Element user = u.toXML();
	  Document doc = new Document(user);

	  //zapis do suboru
	  XMLOutputter serializer = new XMLOutputter(org.jdom.output.Format.getPrettyFormat());
	  serializer.output(doc, fos);
	  fos.close();
     }

     /**
      * Return User representaiton of user from XML Element representation of user given by parameter. 
      * @param root XML Element representaiton of user.
      * @return User representation of user.
      */
     public static User fromXML(Element root) {

	  User human = new User(root.getAttributeValue("Name"), root.getAttributeValue("Surname"), root.getAttributeValue("Login"), root.getAttributeValue("Password_hash"));

	  List level_1_children = root.getChildren();
	  Element callendar = (Element) level_1_children.get(0); //pozriem sa na kalendar daneho uzivatela a natiahnem do neho 	atributy	
	  Callendar CALLENDAR = Callendar.fromXML(callendar); //sparsujem kalendar
	  human.set_callendar(CALLENDAR);

	  //musia sa este vyriesit skupiny
	  if (level_1_children.size() == 2) {
	       Element groups = (Element) level_1_children.get(1);
	       List groupy = groups.getChildren();

	       //prejdem vsetkymi skupinami
	       Iterator b = groupy.iterator();
	       while (b.hasNext()) {
		    Element group = (Element) b.next();
		    Group_membership group_to_put = Group_membership.fromXML(group);
		    try{
			 human.add_group(group_to_put);
		    }catch (Exception ex){
			 ex.printStackTrace();
		    }
	       }
	  }
	  return human;
     }

     /**
      * Returns XML Element representation of object User .
      * @return XML Element representation of user.
      */
     public Element toXML() {

	  Element root = new Element("User");
	  root.setAttribute("Login", this.get_login());
	  root.setAttribute("Name", this.get_name());
	  root.setAttribute("Surname", this.get_surname());
	  root.setAttribute("Password_hash", this.get_password());

	  Element calendar = this.get_callendar().toXML();
	  root.addContent(calendar);

	  //je potrebne este pridat vsetky skupiny daneho uzivatela do xml
	  if (!this.get_groups().isEmpty()) {
	       Element groups = new Element("Groups");
	       root.addContent(groups);

	       for (Group_membership g : this.get_groups()) {
		    Element group = g.toXML();
		    groups.addContent(group);
	       }
	  }
	  return root;
     }

     /**
      * Checks whether user object is empty or no.
      * @return boolean representation of user's content.
      */
     public boolean is_empty() {
	  if (name.isEmpty() || surname.isEmpty() || login.isEmpty() || password_hash.isEmpty()) {
	       return true;
	  } else {
	       return false;
	  }
     }

     /**
      * Return Callendar object of user which represents user's calendar.
      * @return Callendar object which represents user's calendar.
      */
     public Callendar get_callendar() {
	  return callendar;
     }
     /**
      * Returns the name of user.
      * @return string representation of user's name.
      */
     public String get_name() {
	  return name;
     }
     /**
      * Returns user's surname.
      * @return string representation of user's surname.
      */
     public String get_surname() {
	  return surname;
     }
     /**
      * Returns login of user.
      * @return string representation of user's login.
      */
     public String get_login() {
	  return login;
     }
     /**
      * Returns password hash of user.
      * @return string representation of password has of user.
      */
     public String get_password() {
	  return password_hash;
     }
     /**
      * Returns list of group memberships of user.
      * @return list of Group_membership representation of user's memberships in groups.
      */
     public List<Group_membership> get_groups() {
	  return list_of_groups;
     }
     /**
      * Adds membership to list of user's memberships
      * @param g Group_membership representation of group membership.
      */
     public void add_group(Group_membership g)throws Exception {
	  if(list_of_groups.contains(g)) throw new Exception("Group membership cannot be added, becouse it allready exicts.");
	  list_of_groups.add(g);
     }
   /**
      * Removes membership from list of user's memberships
      * @param g Group_membership representation of group membership.
      */
     public void remove_group(Group_membership g) {
	  list_of_groups.remove(g);
     }
     /**
      * Sets the surname of user.
      * @param n string representation of surname.
      */
     public void set_surname(String n) {
	  surname = n;
     }
     /**
      * Set user's calendar to calendar given by parameter.
      * @param c Callendar representation of calendar.
      */
     public void set_callendar(Callendar c) {
	  callendar = c;
     }
     /**
      * Sets user's login to login given by parameter.
      * @param l string representation of user's login.
      */
     public void set_login(String l) {
	  login = l;
     }
     /**
      * Sets user's name to name given by parameter.
      * @param n string representation of name.
      */
     public void set_name(String n) {
	  name = n;
     }
     /**
      * Sets user's password hash to password hash given by parameter.
      * @param pass string representation of passeword hash 
      */
     public void set_password(String pass) {
	  password_hash = pass;
     }
      /**
      * Compares User object with another.
      * @param o object of type Object.
      * @return true if objects are equal, false otherwise.
      */
     public boolean equals(Object o) {
	  if (o instanceof User) {
	       User ce = (User) o;
	       return this.get_login().equals(ce.get_login());
	  } else {
	       return false;
	  }
     }

     /**
      * Returns hash code of User object, which is delegated to hash code of user's name.
      * @return integer representation of hash code of User object.
      */
     public int hashCode() {
	  return this.get_login().hashCode();
     }
}
