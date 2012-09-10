/*
 * EventList.java
 *
 * Created on 27. duben 2009, 15:32
 */
package dialogs;

import java.util.*;
import client.*;
import COMMON.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.table.*;
import javax.swing.border.*;

/**
 *
 * @author  ivo
 */
public class EventList extends javax.swing.JDialog {

     private boolean granularity;
     private Date date;
     private ClientController cc;
      private LocalCache lc = LocalCache.getDefaultCache(); 
     
     /** Creates new form EventList */
     public EventList(java.awt.Frame parent, boolean modal, ClientController cc, boolean granularity, Date date) {
	  super(parent, modal);
	  	  this.cc = cc;
	  	  	  this.date = date;
	  this.granularity = granularity;
	  initComponents();
	  jLabel1.setText("Events list:");
	  jButton1.setText("Add event");
	  jButton2.setText("Remove event");
	  jButton3.setText("Copy event");
	  jButton4.setText("Add group event");
	  jButton5.setText("Remove group event");
	  this.jLabel2.setText("Group events list:");



	  pack();
     }


  private class UserEventsListRenderer extends JLabel implements ListCellRenderer {
       public UserEventsListRenderer() {
           setOpaque(true);
       }
       
   public Component getListCellRendererComponent(JList list,
                                                     Object value,
                                                     int index,
                                                     boolean isSelected,
                                                     boolean cellHasFocus) {
  
                Callendar_event ds = (Callendar_event) value;
                
                if (isSelected)
                  setBackground(Color.lightGray );
                else
                  setBackground(Color.WHITE ) ;
                  
                  
              setText(ds.get_name());
  
           return this;
       }
       
       
       
       }


     private class UserEventsList extends AbstractListModel {

	  java.util.List<Callendar_event> clientsEvents = new LinkedList<Callendar_event>();
	  
	  public void reload()
	  {
	   if  ( cc == null )
	     System.out.println("Zlykontroler");
	     else{
	     if ( granularity )
        clientsEvents = lc.listUserEventsOnHour(lc.getUser(cc.getLogin()),date);
      else
        clientsEvents = lc.listUserEventsOnDay(lc.getUser(cc.getLogin()),date);
      fireContentsChanged(this, 0, clientsEvents.size() );
    } }
	  
	  public int getSize() {
	       return clientsEvents.size();
	  }

	  public Object getElementAt(int arg0) {
	       return clientsEvents.get(arg0);
	  }
     }
     
    
  private class DoubleString{
    String s1;
    Callendar_event s2;
    DoubleString(String s1, Callendar_event s2)
    {
      this.s1 = s1;
      this.s2 = s2;
    }  
  }  

  private class GroupEventListRenderer extends JLabel implements ListCellRenderer {
       public GroupEventListRenderer() {
           setOpaque(true);
       }
       
   public Component getListCellRendererComponent(JList list,
                                                     Object value,
                                                     int index,
                                                     boolean isSelected,
                                                     boolean cellHasFocus) {
  
                DoubleString ds = (DoubleString) value;
                
                if (isSelected)
                  setBackground(Color.lightGray );
                else
                  setBackground(Color.WHITE ) ;
                  
                  
              setText(ds.s1 + " " + ds.s2.get_name());
  
           return this;
       }
       
       
       
       }
                     
      private class GroupsEventsList extends AbstractListModel {

	  java.util.List <DoubleString> ls = new LinkedList<DoubleString>();
	  
	  public void reload()
	  {


        
              ls =new LinkedList<DoubleString>();
            for (Group_membership m : lc.getUser(cc.getLogin()).get_groups()) 
            {
            

                if ( granularity )
                        for ( Callendar_event ce: lc.listGroupEventsOnHour(lc.getGroup(m.get_group_name()),date) )
                                      {ls.add(new DoubleString(m.get_group_name(), ce));}
               else
                        for ( Callendar_event ce: lc.listGroupEventsOnDay(lc.getGroup(m.get_group_name()),date) )
                                      {ls.add(new DoubleString(m.get_group_name(), ce));}
          }            

            
    
        
        
      fireContentsChanged(this, 0, ls.size() );
    } 
	  
	  
	  public int getSize() {
	       return ls.size();
	  }

	  public Object getElementAt(int arg0) {
	       return ls.get(arg0);
	  }
     }

     /** This method is called from within the constructor to
      * initialize the form.
      * WARNING: Do NOT modify this code. The content of this method is
      * always regenerated by the Form Editor.
      */
     // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
     private void initComponents() {
          java.awt.GridBagConstraints gridBagConstraints;

          jScrollPane1 = new javax.swing.JScrollPane();
          jList1 = new javax.swing.JList();
          jLabel1 = new javax.swing.JLabel();
          jButton1 = new javax.swing.JButton();
          jButton2 = new javax.swing.JButton();
          jButton3 = new javax.swing.JButton();
          jButton4 = new javax.swing.JButton();
          jButton5 = new javax.swing.JButton();
          jScrollPane2 = new javax.swing.JScrollPane();
          jList2 = new javax.swing.JList();
          jLabel2 = new javax.swing.JLabel();

          setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);
          setName("Form"); // NOI18N
          getContentPane().setLayout(new java.awt.GridBagLayout());

          jScrollPane1.setName("jScrollPane1"); // NOI18N

          uel = new UserEventsList();
          UserEventsListRenderer uelr = new UserEventsListRenderer();

          jList1.setModel(uel);
          jList1.setCellRenderer(uelr);
          uel.reload();
          jList1.setSelectionMode(javax.swing.ListSelectionModel.SINGLE_SELECTION);
          jList1.setName("jList1"); // NOI18N
          jList1.setPreferredSize(new java.awt.Dimension(300, 160));
          jList1.addMouseListener(new MouseAdapter(){
           public void mouseClicked(MouseEvent e){
              list2dc(e);
              }
           } );
 
          jScrollPane1.setViewportView(jList1);

          gridBagConstraints = new java.awt.GridBagConstraints();
          gridBagConstraints.gridx = 0;
          gridBagConstraints.gridy = 1;
          gridBagConstraints.gridheight = 7;
          gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
          gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHEAST;
          gridBagConstraints.weightx = 4.0;
          getContentPane().add(jScrollPane1, gridBagConstraints);


          jLabel1.setName("jLabel1"); // NOI18N
          gridBagConstraints = new java.awt.GridBagConstraints();
          gridBagConstraints.gridwidth = 2;
          gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
          gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTH;
          getContentPane().add(jLabel1, gridBagConstraints);

          jButton1.setName("jButton1"); // NOI18N
          jButton1.addActionListener(new java.awt.event.ActionListener() {
               public void actionPerformed(java.awt.event.ActionEvent evt) {
                    jButton1ActionPerformed(evt);
               }
          });
          gridBagConstraints = new java.awt.GridBagConstraints();
          gridBagConstraints.gridx = 1;
          gridBagConstraints.gridy = 1;
          gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
          gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
          gridBagConstraints.weightx = 1.0;
          getContentPane().add(jButton1, gridBagConstraints);

          jButton2.setName("jButton2"); // NOI18N
          jButton2.addActionListener(new java.awt.event.ActionListener() {
               public void actionPerformed(java.awt.event.ActionEvent evt) {
                    jButton2ActionPerformed(evt);
               }
          });          
          gridBagConstraints = new java.awt.GridBagConstraints();
          gridBagConstraints.gridx = 1;
          gridBagConstraints.gridy = 2;
          gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
          gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
          gridBagConstraints.weightx = 1.0;
          getContentPane().add(jButton2, gridBagConstraints);


          jButton3.setName("jButton3"); // NOI18N
          jButton3.addActionListener(new java.awt.event.ActionListener() {
               public void actionPerformed(java.awt.event.ActionEvent evt) {
                    jButton3ActionPerformed(evt);
               }
          }); 
          gridBagConstraints = new java.awt.GridBagConstraints();
          gridBagConstraints.gridx = 1;
          gridBagConstraints.gridy = 3;
          gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
          gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
          gridBagConstraints.weightx = 1.0;
          getContentPane().add(jButton3, gridBagConstraints);


          jButton4.setName("jButton4"); // NOI18N
          jButton4.addActionListener(new java.awt.event.ActionListener() {
               public void actionPerformed(java.awt.event.ActionEvent evt) {
                    jButton4ActionPerformed(evt);
               }
          }); 
          gridBagConstraints = new java.awt.GridBagConstraints();
          gridBagConstraints.gridx = 1;
          gridBagConstraints.gridy = 9;
          gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
          gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
          gridBagConstraints.weightx = 1.0;
          getContentPane().add(jButton4, gridBagConstraints);


          jButton5.setName("jButton5"); // NOI18N
          jButton5.addActionListener(new java.awt.event.ActionListener() {
               public void actionPerformed(java.awt.event.ActionEvent evt) {
                    jButton5ActionPerformed(evt);
               }
          }); 
          gridBagConstraints = new java.awt.GridBagConstraints();
          gridBagConstraints.gridx = 1;
          gridBagConstraints.gridy = 10;
          gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
          gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
          getContentPane().add(jButton5, gridBagConstraints);

          jScrollPane2.setName("jScrollPane2"); // NOI18N

          gel = new GroupsEventsList();
          
          GroupEventListRenderer gelr = new GroupEventListRenderer();

          jList2.setModel(gel);
          jList2.setCellRenderer(gelr);
          gel.reload();
          jList2.setName("jList2"); // NOI18N
          jList2.addMouseListener(new MouseAdapter(){
           public void mouseClicked(MouseEvent e){
              list1dc(e);
              }
           } );
          jScrollPane2.setViewportView(jList2);

          gridBagConstraints = new java.awt.GridBagConstraints();
          gridBagConstraints.gridx = 0;
          gridBagConstraints.gridy = 9;
          gridBagConstraints.gridheight = 7;
          gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
          getContentPane().add(jScrollPane2, gridBagConstraints);

          jLabel2.setName("jLabel2"); // NOI18N
          gridBagConstraints = new java.awt.GridBagConstraints();
          gridBagConstraints.gridx = 0;
          gridBagConstraints.gridy = 8;
          gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
          getContentPane().add(jLabel2, gridBagConstraints);

          pack();
     }
     
     private void jButton1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton1ActionPerformed

	  //Add event - otvorenie dialogu na pridavanie eventov.
	     AddEvent aev = new AddEvent(new javax.swing.JFrame(), true);
	     aev.setVisible(true);
	     
	     if (aev.getAction().equals("CONFIRM"))
	     {
              String eventaname = aev.getEventName();
              try{
              cc.addEvent(eventaname, new GregorianCalendar(aev.getEventYears() , aev.getEventMonths()-1 , aev.getEventDays() , aev.getEventHours() , 1).getTime() , aev.isRepeatable(), aev.getOftenDays()*24 + aev.getOftenHours(), aev.getCategory());
              } catch (Exception e)
              { e.printStackTrace();
              JOptionPane.showMessageDialog(new javax.swing.JFrame(), "Add event failed", "Error", JOptionPane.ERROR_MESSAGE); }
              try{ Thread.sleep(200);} catch(Exception e){}
              uel.reload();
              gel.reload();
       }
	     

     }//GEN-LAST:event_jButton1ActionPerformed

     private void jButton2ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton1ActionPerformed
              Callendar_event eventaname = (Callendar_event) jList1.getSelectedValue();
              try{
              cc.deleteEvent(eventaname.get_name());
              } catch (Exception e){ e.printStackTrace(); 		    JOptionPane.showMessageDialog(new javax.swing.JFrame(), "Delete event failed", "Error", JOptionPane.ERROR_MESSAGE);}
              try{ Thread.sleep(200);} catch(Exception e){}
              uel.reload();
              gel.reload();     
     }
     
     private void jButton3ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton1ActionPerformed
              Callendar_event eventaname = (Callendar_event) jList1.getSelectedValue();
              
              
              CopyEvent ce = new CopyEvent(new javax.swing.JFrame(), true);
              ce.setVisible(true);
              
              if ( ce.getAction().equals("CONFIRM") )
              {
                try{
                  cc.offerEvent(ce.getuser(), eventaname.get_name());
                } catch ( Exception e)
                {
                  JOptionPane.showMessageDialog(new javax.swing.JFrame(), "Copy event failed", "Error", JOptionPane.ERROR_MESSAGE);
                }
              }
              try{ Thread.sleep(200);} catch(Exception e){}
              uel.reload();
              gel.reload();       
     }
     
     private void jButton4ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton1ActionPerformed
      //add group event
      AddGroupEvent age= new AddGroupEvent( new javax.swing.JFrame(), true, cc.getLogin() );
      age.setVisible(true);
      
	     if (age.getAction().equals("CONFIRM"))
	     {
              String eventaname = age.getEventName();
              String grupname = age.getGroupName();
              try{
              cc.addGroupEvent(grupname, eventaname, new GregorianCalendar(age.getEventYears() , age.getEventMonths()-1 , age.getEventDays() , age.getEventHours() , 1).getTime() , age.isRepeatable(), age.getOftenDays()*24 + age.getOftenHours(), age.getCategory());
              } catch (Exception e)
              { e.printStackTrace();
              JOptionPane.showMessageDialog(new javax.swing.JFrame(), "Add Group Event Failed", "Error", JOptionPane.ERROR_MESSAGE); }
              try{ Thread.sleep(200);} catch(Exception e){}
              uel.reload();
              gel.reload();
       }      
      
     }

     private void jButton5ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton1ActionPerformed
     //delete event
           DoubleString ds = (DoubleString) jList2.getSelectedValue();
           if (ds != null){
              String eventaname = ds.s2.get_name();
              String grupname = ds.s1;
              try{
              cc.removeGroupEvent(grupname,eventaname);
              } catch (Exception e)
              { e.printStackTrace();
              JOptionPane.showMessageDialog(new javax.swing.JFrame(), "Remove Group Event Failed", "Error", JOptionPane.ERROR_MESSAGE); }
              }
              try{ Thread.sleep(200);} catch(Exception e){}
              uel.reload();
              gel.reload();
     }

    private void list2dc( MouseEvent e)
    {
       if(e.getClickCount() == 2){
         int index = jList1.locationToIndex(e.getPoint());
         ListModel dlm = jList1.getModel();
         Callendar_event eventaname = (Callendar_event) dlm.getElementAt(index);;
         jList1.ensureIndexIsVisible(index);

          ViewEvent ve = new ViewEvent(new javax.swing.JFrame(), true, cc, "", eventaname);
          ve.setVisible(true);

         }
      }
    

    
    private void list1dc( MouseEvent e)
    {
       if(e.getClickCount() == 2){
         int index = jList2.locationToIndex(e.getPoint());
         ListModel dlm = jList2.getModel();
         DoubleString item = (DoubleString) dlm.getElementAt(index);;
         jList2.ensureIndexIsVisible(index);

          ViewEvent ve = new ViewEvent(new javax.swing.JFrame(), true, cc, item.s1, item.s2);
          ve.setVisible(true);
         }
  }
    
       


     // Variables declaration - do not modify//GEN-BEGIN:variables
     private UserEventsList uel;
     private GroupsEventsList gel;
     
     private javax.swing.JButton jButton1;
     private javax.swing.JButton jButton2;
     private javax.swing.JButton jButton3;
     private javax.swing.JButton jButton4;
     private javax.swing.JButton jButton5;
     private javax.swing.JLabel jLabel1;
     private javax.swing.JLabel jLabel2;
     private javax.swing.JList jList1;
     private javax.swing.JList jList2;
     private javax.swing.JScrollPane jScrollPane1;
     private javax.swing.JScrollPane jScrollPane2;
     // End of variables declaration//GEN-END:variables
}
