package client;

import COMMON.*;
import java.util.*;
import javax.swing.*;

public class Alerter implements Runnable
{
  private ClientGUI gui;
  private LocalCache lc = LocalCache.getDefaultCache();
  private Thread alertthread;
  private ClientController cc;
  
  private List <Callendar_event> evlist = new LinkedList<Callendar_event>(); //zoznam udalosti - implicitne oznamenia
  
  
  public Alerter( ClientGUI gui, ClientController cc) //gui to notify with dialogs
  {

    this.gui = gui;
    this.cc = cc;
    
    
  
  }
  
  public void startAlerting()
  {
      (alertthread = new Thread(this)).start();
  }
  
  
  public List<Callendar_event> fireThese( Date d )
  {

      //defaultne alerty na dobu vyskytu

      Calendar c = new GregorianCalendar();
      c.setTime(d);
      c.add(Calendar.HOUR , 1);
      Date plushour = c.getTime();

      List <Callendar_event> retval = lc.listUserEventsOnHour(lc.getUser(cc.getLogin()), d);
        
            for (Group_membership m : lc.getUser(cc.getLogin()).get_groups()) 
            {
              for ( Callendar_event ge: lc.listGroupEventsOnDay(lc.getGroup(m.get_group_name()), d))
              {
                if (!retval.contains(ge))
                    retval.add(ge);
              }

            
            }
          
    for ( Callendar_event ce: lc.listUserEventsAll(lc.getUser(cc.getLogin())) )
      for ( Event_alert ea: ce.get_alerts() )
      {
        
        if (ea.is_Repeatable())
        {
              if ( (ea.get_start_date().before(plushour)) && (ea.get_start_date().after(d)) )
              {
                if ( !retval.contains(ce) )
                  retval.add(ce);    
              } else {
                long hoursdelta = ea.get_interval();
                Date evinitsc = ea.get_start_date();
                 if (hoursdelta == 0) continue;               
                long h1 = evinitsc.getTime()/(1000 * 60 * 60);
                long h2 = d.getTime()/(1000 * 60 * 60);
                
                long delta = h2 - h1;
      
                if ( delta % hoursdelta == 0 )
                {
                  if ( !retval.contains(ce) )
                    retval.add(ce);          
                }
              }
        } else {
                if ( (ea.get_start_date().before(plushour)) && (ea.get_start_date().after(d)) )
                    {
                      if ( !retval.contains(ce) )
                        retval.add(ce);    
                    }
        }       
        
        
      }
      
          for (Group_membership m : lc.getUser(cc.getLogin()).get_groups()) 
            {
            

              for ( Callendar_event ce: lc.listGroupEventsOnHour(lc.getGroup(m.get_group_name()), d ))
                for ( Event_alert ea: ce.get_alerts() )
                {
                  
                  if (ea.is_Repeatable())
                  {
                        if ( (ea.get_start_date().before(plushour)) && (ea.get_start_date().after(d)) )
                        {
                          if ( !retval.contains(ce) )
                            retval.add(ce);    
                        } else {
                          long hoursdelta = ea.get_interval();
                          Date evinitsc = ea.get_start_date();
                          if (hoursdelta == 0) continue;                           
                          long h1 = evinitsc.getTime()/(1000 * 60 * 60);
                          long h2 = d.getTime()/(1000 * 60 * 60);
                          
                          long delta = h2 - h1;
                
                          if ( delta % hoursdelta == 0 )
                          {
                            if ( !retval.contains(ce) )
                              retval.add(ce);          
                          }
                        }
                  } else {
                          if ( (ea.get_start_date().before(plushour)) && (ea.get_start_date().after(d)) )
                              {
                                if ( !retval.contains(ce) )
                                  retval.add(ce);    
                              }
                  }       
                  
                  
                }
          

            
            }

    return retval;
  }
  
  //generuje oznamenia do gui
  public void run()
  {
	       Thread thisThread = Thread.currentThread();
	       while (alertthread == thisThread) {
           Calendar teraz = new GregorianCalendar();
           
           if ( teraz.get(Calendar.MINUTE) == 0 ) {
           //teraz.set(Calendar.MINUTE , 0);
           //teraz.set(Calendar.SECOND , 1);
           for ( final Callendar_event ce:fireThese(teraz.getTime()) )
           { 
            SwingUtilities.invokeLater( new Runnable() {  public void run(){  gui.showEvent(ce);  }  });
           } 
           
           try {
            Thread.sleep(60000);
           }  catch ( InterruptedException ie){}
           
           }
           try {
            Thread.sleep(500);
           }  catch ( InterruptedException ie)
           {}
	       
	       }
  }
  



}