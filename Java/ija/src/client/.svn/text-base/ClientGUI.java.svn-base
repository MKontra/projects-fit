package client;

import COMMON.*;
import java.io.*;
import dialogs.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.table.*;
import javax.swing.border.*;
import edu.stanford.ejalbert.BrowserLauncher;

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * MainFrame.java
 *
 * Created on Apr 22, 2009, 3:28:11 PM
 */

/**
 *
 * @author Cukor
 */
public class ClientGUI extends javax.swing.JFrame {

    /** Creates new form MainFrame */
    private LocalCache lc = LocalCache.getDefaultCache(); 
    private Date current = new Date();
    private Calendar c = Calendar.getInstance();
    private ClientController cc;
    private Alerter al;
    
    public ClientGUI(ClientController cc) {
        this.cc = cc;
        this.al = new Alerter(this, cc);
    }
    
    public void showMainWindow()
    {
        this.initComponents();
        this.setVisible(true);
        this.al.startAlerting();
    }
     
     //dialog na login a registraciu
     // + moznost z a na server
     // + 


    //dialog zobrazenia udalosti na den
    
    //dialog poznamok a upozorneni udalosti
    
    //jednoduche dialogy na pridanie a odobranie udalosti poznamok a upozorneni
    
    //zoznam pre agendu
    
    //okno upozornenia

    //private class WeekTableModel extends AbstractTableModel 
    //{}
    
    private class SimpleCell
    {
      Calendar c;
      int eventcount;
      SimpleCell(Calendar c, int eventcount)
      {
        this.c = c;
        this.eventcount = eventcount;
      }
      
      Calendar getCalendar()
      {
        return c;
      }
      
      int getEventcount()
      {
        return eventcount;
      }
      
    }
    
    
    private class EventSet
    {
      private Map<Callendar_event, Group > groupevents;
      private java.util.List<Callendar_event> userevents;
      EventSet(){}
      
      void addGroupEvent(Group g, Callendar_event ce)
      {
        groupevents.put(ce, g);
      }
      
      void addUserEvent(Callendar_event ce)
      {
       userevents.add(ce);
      }
      
      Map<Callendar_event, Group> getGroupEvents()
      {
        return groupevents;
      }
      
      java.util.List<Callendar_event> getUserEvents()
      {
        return userevents;
      }
      
    }

    private class AgendaGroupListModel extends AbstractListModel
    {
           
          private LinkedList<String> events = new LinkedList<String>();
           
          public int getSize() {
            return events.size();
          }
          
          public void reload()
          {
            events = new LinkedList<String>();
            for (Group_membership m : lc.getUser(cc.getLogin()).get_groups()) 
            {
            
              for ( Callendar_event ce: lc.getGroup(m.get_group_name()).get_callendar().get_events() )
              {
                events.add("Skupina: " + m.get_group_name() + ": " + ce.get_name() + " " +  ce.get_Event_date_and_time().toString());
              }            

            
            }
            fireContentsChanged(this, 0, events.size() );
          }
    
          public Object getElementAt(int index) {
            return events.get(index);
          }    
    
    
    }

    
    private class AgendaUserListModel extends AbstractListModel
    {
     
    
          private LinkedList<String> events = new LinkedList<String>(); 
           
          public int getSize() {
            return events.size();
          }
          
          public void reload()
          {
            events = new LinkedList<String>();
              for ( Callendar_event ce: lc.getUser(cc.getLogin()).get_callendar().get_events() )
              {
                events.add(ce.get_name() + " " + ce.get_Event_date_and_time().toString());
              }

            fireContentsChanged(this, 0, events.size() );
          }
    
          public Object getElementAt(int index) {
            return events.get(index);
          }    
    
    
    }
    
    private class MonthTableRenderer extends JLabel implements TableCellRenderer {
    private Border border = LineBorder.createBlackLineBorder() ;
    private Border old;
    public MonthTableRenderer() {
        setOpaque(true); //MUST do this for background to show up.
        setSize(50, 50);
        old = getBorder();
    }

    public Component getTableCellRendererComponent(
                            JTable table, Object color,
                            boolean isSelected, boolean hasFocus,
                            int row, int column) {
        SimpleCell sc = (SimpleCell) color;
        int cday = c.get(Calendar.DAY_OF_WEEK);
        int wk = c.get(Calendar.WEEK_OF_MONTH );
        if ( (column == cday-1) && (wk == row+1) )
                  setBorder(border);
        else
                   setBorder(old);        
        
        String toshow = "<html>";
        Calendar t = sc.getCalendar();
        t.setTime(c.getTime());
        t.set(Calendar.DAY_OF_WEEK , column+1);
        t.set(Calendar.WEEK_OF_MONTH, row + 1);
        t.set(Calendar.HOUR_OF_DAY, 0);
        t.set(Calendar.MINUTE , 0);
        
        if (t.get(Calendar.MONTH ) == c.get(Calendar.MONTH) )
                  setBackground(Color.YELLOW );
        else
                  setBackground(Color.lightGray )   ;
                  
        toshow = toshow + t.get(Calendar.DAY_OF_MONTH) + "." +  (t.get(Calendar.MONTH )+1) + "  " + t.get(Calendar.YEAR ) + "<br>";
        
        if ( sc.getEventcount() == 0 )
          toshow = toshow + "";
        else 
          toshow = toshow + sc.getEventcount()+ " events";
        setText(toshow);
        return this;
    }
}



  
    private class MonthTableModel extends AbstractTableModel 
    {
 
        private String[] columnNames = {
                                        "Sunday",
                                        "Monday",
                                        "Tuesday",
                                        "Wenesday",
                                        "Thursday",
                                        "Freeday",
                                        "Saturday"
                                        };
 
    
      public String getColumnName(int col) {
      
          return columnNames[col];
      }
      
      public int getRowCount() { return 5; }
      public int getColumnCount() { return columnNames.length; }
      
      public void reload()
      {
        fireTableDataChanged();
        fireTableStructureChanged(); 
      }
      
      
      public Object getValueAt(int row, int col) {
      
        Calendar t = new GregorianCalendar();
        t.setTime(c.getTime());
        t.set(Calendar.DAY_OF_WEEK , col+1);
        t.set(Calendar.WEEK_OF_MONTH, row + 1);
        t.set(Calendar.HOUR_OF_DAY, 0);
        t.set(Calendar.MINUTE , 0);
        int retval = lc.listUserEventsOnDay(lc.getUser(cc.getLogin()), t.getTime()).size();
        
            for (Group_membership m : lc.getUser(cc.getLogin()).get_groups()) 
            {
            
              retval = retval + lc.listGroupEventsOnDay(lc.getGroup(m.get_group_name()), t.getTime()).size();   

            
            }        
        
        return new SimpleCell(t, retval);
        

      }
      
      
      public boolean isCellEditable(int row, int col)
          { return false; }

    
    }    
    
    private class WeekTableRenderer extends JLabel implements TableCellRenderer {

    public WeekTableRenderer() {
        setOpaque(true); //MUST do this for background to show up.
    }

    public Component getTableCellRendererComponent(
                            JTable table, Object color,
                            boolean isSelected, boolean hasFocus,
                            int row, int column) {

        SimpleCell sc = (SimpleCell) color;
        
        int cday = c.get(Calendar.DAY_OF_WEEK);
        if ( column == cday-1 )
                  setBackground(Color.YELLOW );
        else
                  setBackground(Color.lightGray )   ;
        
        if ( sc.getEventcount() == 0 )
          setText("");
        else 
          setText(sc.getEventcount()+ " events");
        
        //setToolTipText(...); //Discussed in the following section
        return this;
    }
}

  
    private class WeekTableModel extends AbstractTableModel 
    {
 
        private String[] columnNames = {
                                        "Sunday",
                                        "Monday",
                                        "Tuesday",
                                        "Wenesday",
                                        "Thursday",
                                        "Freeday",
                                        "Saturday"
                                        };
 
    
      public String getColumnName(int col) {
      
        Calendar t = new GregorianCalendar();
        t.setTime(c.getTime());
        t.set(Calendar.DAY_OF_WEEK , col+1);
      
          return columnNames[col] + " " + t.get(Calendar.DATE); 
      }
      
      public int getRowCount() { return 24; }
      public int getColumnCount() { return columnNames.length; }
      
      public void reload()
      {
        fireTableDataChanged();
        fireTableStructureChanged(); 
      }
      
      
      public Object getValueAt(int row, int col) {
      
        Calendar t = new GregorianCalendar();
        t.setTime(c.getTime());
        t.set(Calendar.DAY_OF_WEEK , col+1);
        t.set(Calendar.HOUR_OF_DAY, row);
        t.set(Calendar.MINUTE , 0);
        int retval = lc.listUserEventsOnHour(lc.getUser(cc.getLogin()), t.getTime()).size();
        
        for (Group_membership m : lc.getUser(cc.getLogin()).get_groups()) 
            {
            
              retval = retval + lc.listGroupEventsOnHour(lc.getGroup(m.get_group_name()), t.getTime()).size();   

            
            }   

        return new SimpleCell(t, retval);
        

      }
      
      
      public boolean isCellEditable(int row, int col)
          { return false; }

    
    }
        
    private class WeekTableRowHeaderListRenderer extends JLabel implements ListCellRenderer {
    JTable tref;
    WeekTableRowHeaderListRenderer(JTable table) {
      tref = table;
      JTableHeader header = table.getTableHeader();
      setOpaque(true);
      setBorder(UIManager.getBorder("TableHeader.cellBorder"));
      setHorizontalAlignment(CENTER);
      setForeground(header.getForeground());
      setBackground(header.getBackground());
      setFont(header.getFont());
    }
  
    public Component getListCellRendererComponent(JList list, Object value,
        int index, boolean isSelected, boolean cellHasFocus) {
        //setSize(50, tref.getRowHeight());
      setText((value == null) ? "" : value.toString());
      return this;
    }
  }    

    private void initComponents() {
    
        try {
          UIManager.setLookAndFeel(
              UIManager.getSystemLookAndFeelClassName());
        } catch (Exception e)
        {}

        jTabbedPane1 = new javax.swing.JTabbedPane();
        jButton1 = new javax.swing.JButton();
        jButton2 = new javax.swing.JButton();
        jButton3 = new javax.swing.JButton();
        jLabel1 = new javax.swing.JLabel();
        jMenuBar1 = new javax.swing.JMenuBar();
        jMenu1 = new javax.swing.JMenu();
        jMenu2 = new javax.swing.JMenu();
        jMenu3 = new javax.swing.JMenu();
        jMenu4 = new javax.swing.JMenu();
        
        setTitle(cc.getLogin() + " : " + (cc.onlineMode() ? "ONLINE":"OFFLINE")); 
        
        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);

        tablemodel = new WeekTableModel();
        
        
        ListModel lm = new AbstractListModel() {
          String headers[] = { "00:00", "01:00", "02:00", "03:00", "04:00", "05:00", "06:00", "07:00", "08:00",
                               "09:00", "10:00", "11:00", "12:00", "13:00", "14:00", "15:00", "16:00", "17:00",
                               "18:00", "19:00", "20:00", "21:00", "22:00", "23:00" };
    
          public int getSize() {
            return headers.length;
          }
    
          public Object getElementAt(int index) {
            return headers[index];
          }
        };
        
        table = new JTable(tablemodel);//WeekTableRenderer
        table.setDefaultRenderer( Object.class , new WeekTableRenderer());
        table.setRowSelectionAllowed(false);
        table.setColumnSelectionAllowed(false);
        table.setCellSelectionEnabled(false);
  
        table.setAutoResizeMode(JTable.AUTO_RESIZE_OFF);
        table.setRowHeight(35);
    
        table.addMouseListener(new MouseAdapter(){
         public void mouseClicked(MouseEvent e){
            tableClicked(e);
            }
         } );
  

    
    
        JList rowHeader = new JList(lm);
        rowHeader.setFixedCellWidth(100);
    
        rowHeader.setFixedCellHeight(table.getRowHeight());
           // + table.getRowMargin()  + table.getIntercellSpacing().height);
        rowHeader.setCellRenderer(new WeekTableRowHeaderListRenderer(table));
    
        //JScrollPane scroll = new JScrollPane(table);
        jScrollPane1 = new javax.swing.JScrollPane(table);
        jScrollPane1.setRowHeaderView(rowHeader);        

        //--------------------------------
        //Tabulka mesiaca
        monthmodel = new MonthTableModel();
        month = new JTable(monthmodel);
        month.setDefaultRenderer( Object.class , new MonthTableRenderer());
        month.setRowSelectionAllowed(false);
        month.setColumnSelectionAllowed(false);
        month.setCellSelectionEnabled(false);
        month.setAutoResizeMode(JTable.AUTO_RESIZE_OFF);
        month.setRowHeight(50);
        
        month.addMouseListener(new MouseAdapter(){
         public void mouseClicked(MouseEvent e){
            monthClicked(e);
            }
         } );        
        
        jScrollPane2 = new javax.swing.JScrollPane(month);
   
   
        aglm = new AgendaGroupListModel();
        aulm = new AgendaUserListModel ();
        
        JPanel jp = new JPanel( new FlowLayout() );
        
        JList useragenda = new JList(aulm); 
        aulm.reload();
        jp.add(useragenda);
        
        JList groupagenda = new JList(aglm); 
        aglm.reload();
        jp.add(groupagenda);
        
        jScrollPane3 = new javax.swing.JScrollPane(jp);
        

        jTabbedPane1.addTab("Week", jScrollPane1);
        jTabbedPane1.addTab("Month", jScrollPane2);
        jTabbedPane1.addTab("Events", jScrollPane3);

        jButton1.setText("Previous week");
          jButton1.addActionListener(new java.awt.event.ActionListener() {
               public void actionPerformed(java.awt.event.ActionEvent evt) {
                    jButton1ActionPerformed(evt);
               }
          });

        jButton2.setText("Today");
          jButton2.addActionListener(new java.awt.event.ActionListener() {
               public void actionPerformed(java.awt.event.ActionEvent evt) {
                    jButton2ActionPerformed(evt);
               }
          });
          
        jButton3.setText("Next week");
          jButton3.addActionListener(new java.awt.event.ActionListener() {
               public void actionPerformed(java.awt.event.ActionEvent evt) {
                    jButton3ActionPerformed(evt);
               }
          });
          
          
        jLabel1.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        jLabel1.setText("Day: " + c.get(Calendar.DAY_OF_MONTH) + " Week: " + c.get(Calendar.	WEEK_OF_MONTH) + " Month: " + (c.get(Calendar.MONTH )+1) + " Year: " + c.get(Calendar.YEAR ) );

        JMenuItem menuItem;

        jMenu1.setText("File");
        
        menuItem = new JMenuItem("Exit");
        menuItem.addActionListener(new java.awt.event.ActionListener() {
               public void actionPerformed(java.awt.event.ActionEvent evt) {
                    menuExit(evt);
               }
          });  
        jMenu1.add(menuItem);       
        jMenuBar1.add(jMenu1);

        jMenu2.setText("Groups");
        
        menuItem = new JMenuItem("View groups");
        menuItem.addActionListener(new java.awt.event.ActionListener() {
               public void actionPerformed(java.awt.event.ActionEvent evt) {
                    listGroups(evt);
               }
          }); 

        jMenu2.add(menuItem);                
        jMenuBar1.add(jMenu2);
        


        jMenu3.setText("Tools");
        menuItem = new JMenuItem("Settings");
        menuItem.addActionListener(new java.awt.event.ActionListener() {
               public void actionPerformed(java.awt.event.ActionEvent evt) {
                    settingsDialog(evt);
               }
          });         
        jMenu3.add(menuItem);        
        jMenuBar1.add(jMenu3);

        jMenu4.setText("Help");
        menuItem = new JMenuItem("Show help");
        menuItem.addActionListener(new java.awt.event.ActionListener() {
               public void actionPerformed(java.awt.event.ActionEvent evt) {
                    showHelp(evt);
               }
          }); 
        jMenu4.add(menuItem);
        jMenuBar1.add(jMenu4);

        setJMenuBar(jMenuBar1);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(jButton1)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jButton2)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jButton3)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jLabel1, javax.swing.GroupLayout.DEFAULT_SIZE, 320, Short.MAX_VALUE))
            .addComponent(jTabbedPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 555, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addComponent(jTabbedPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 445, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jButton1)
                    .addComponent(jButton2)
                    .addComponent(jButton3)
                    .addComponent(jLabel1)))
        );

        pack();
    }
    
    public void tableClicked( MouseEvent e)
    {
    
      if (e.getClickCount() == 2){
          int row = table.rowAtPoint(e.getPoint() );
          int col = table.columnAtPoint(e.getPoint() );
          SimpleCell value = (SimpleCell)table.getValueAt(row,col);
          
         	   if  ( cc == null )
	     System.out.println("Zlykontrolerv ClientGI"); 
          
          EventList el = new EventList(this, true, this.cc, true, value.getCalendar().getTime() );
          el.setVisible(true);
          
      tablemodel.reload();      monthmodel.reload();
      aglm.reload();
      aulm.reload();
        }
    }
    
    public void monthClicked( MouseEvent e)
    {
    
      if (e.getClickCount() == 2){
          int row = month.rowAtPoint(e.getPoint() );
          int col = month.columnAtPoint(e.getPoint() );
          SimpleCell value = (SimpleCell)month.getValueAt(row,col);
          
         	   if  ( cc == null )
	     System.out.println("Zlykontrolerv ClientGI"); 
          
          EventList el = new EventList(this, true, this.cc, false, value.getCalendar().getTime() );
          el.setVisible(true);
          
      tablemodel.reload();      monthmodel.reload();
      aglm.reload();
      aulm.reload();
        }
    }    

    public void jButton1ActionPerformed(java.awt.event.ActionEvent evt)//prev
    {
      c.add(Calendar.	WEEK_OF_MONTH, -1);
      jLabel1.setText("Day: " + c.get(Calendar.DAY_OF_MONTH) + " Week: " + c.get(Calendar.	WEEK_OF_MONTH) + " Month: " + (c.get(Calendar.MONTH )+1) + " Year: " + c.get(Calendar.YEAR ) );
      tablemodel.reload();      monthmodel.reload();
      aglm.reload();
      aulm.reload();
    }  
    
    public void jButton2ActionPerformed(java.awt.event.ActionEvent evt)//today
    {
      c = Calendar.getInstance();
            jLabel1.setText("Day: " + c.get(Calendar.DAY_OF_MONTH) + " Week: " + c.get(Calendar.	WEEK_OF_MONTH) + " Month: " + (c.get(Calendar.MONTH )+1) + " Year: " + c.get(Calendar.YEAR ) );
      tablemodel.reload();      monthmodel.reload();
          aglm.reload();
      aulm.reload();  
    }  
    
    public void jButton3ActionPerformed(java.awt.event.ActionEvent evt)//next
    {
      c.add(Calendar.	WEEK_OF_MONTH, 1);
      jLabel1.setText("Day: " + c.get(Calendar.DAY_OF_MONTH) + " Week: " + c.get(Calendar.	WEEK_OF_MONTH) + " Month: " + (c.get(Calendar.MONTH )+1) + " Year: " + c.get(Calendar.YEAR ) );
      tablemodel.reload(); 
      monthmodel.reload();
      aglm.reload();
      aulm.reload();
    }  
    
//menu actions    

    public void menuExit(java.awt.event.ActionEvent evt)//next
    {
        System.exit(0);
    } 

    public void listGroups(java.awt.event.ActionEvent evt)//next
    {
        GroupMan gm = new GroupMan(this, true, cc);
        gm.setVisible(true);
    } 
 
    
    public void settingsDialog(java.awt.event.ActionEvent evt)//next
    {

    } 
    
    public void showHelp(java.awt.event.ActionEvent evt)//next
    {
      try{
                File hlpfile = new File("help.html");
                BrowserLauncher launcher = new BrowserLauncher();
                launcher.openURLinBrowser("file://" + hlpfile.getAbsolutePath());
        } catch(Exception e)
        {
              e.printStackTrace();
              JOptionPane.showMessageDialog(new javax.swing.JFrame(), "Cannot find helpfile", "Error", JOptionPane.ERROR_MESSAGE);        
        
        }

    }   
  
    public void threadSaveEventAdded( final String user, final Callendar_event ce )
    {
      //malo by sa volat z EDT
      final ClientGUI tref = this;
      SwingUtilities.invokeLater( new Runnable() {  public void run(){ tref.EventAdded(user, ce);  }  });
    }
  
    public void EventAdded( String user, Callendar_event ce )  
    {
      
        Object[] options = {"Accept event",
                            "Refuse"};
                            
        int n = JOptionPane.showOptionDialog(this,
        user + " offered you an event " + ce.get_name() + " " + ce.get_Event_date_and_time().toString(),
        "Remote user offered you an Event",
        JOptionPane.YES_NO_OPTION,
        JOptionPane.QUESTION_MESSAGE,
         null,  
          options,  
        options[0]);

        try {
        if ( n == 0 )
          cc.confirmEventRecieving(user,  ce);
        else
          cc.refuseEvent(user,  ce);
        } catch (Exception e)
        {
          e.printStackTrace();
          JOptionPane.showMessageDialog(this, "Event confirmation failed", "Error", JOptionPane.ERROR_MESSAGE);
        }
      tablemodel.reload(); 
      monthmodel.reload();
      aglm.reload();
      aulm.reload();   

    }
    
    public void MembershipAdded( Group g )  
    {
        Object[] options = {"Accept membership",
                            "Refuse"};
                            
        int n = JOptionPane.showOptionDialog(this,
        "You ere offered to join group " + g.get_group_name() + " with leader " + g.get_boss(),
        "Membership",
        JOptionPane.YES_NO_OPTION,
        JOptionPane.QUESTION_MESSAGE,
         null,  
          options,  
        options[0]);

        try {
        if ( n == 0 )
          cc.confirmMembership(g.get_group_name());
        else
          cc.refuseMembership(g.get_group_name());
        } catch (Exception e)
        {
          e.printStackTrace();
          JOptionPane.showMessageDialog(this, "Membership confirmation failed", "Error", JOptionPane.ERROR_MESSAGE);
        }
              tablemodel.reload(); 
      monthmodel.reload();
      aglm.reload();
      aulm.reload();

    }   
    
    public void reloadui()
    {
      SwingUtilities.invokeLater( new Runnable() {  public void run(){   
      tablemodel.reload(); 
      monthmodel.reload();
      aglm.reload();
      aulm.reload(); 

      }  });
    } 
  
    public void showEvent( Callendar_event ce )
    {
		  JOptionPane.showMessageDialog(this, ce.get_name() + " ---  " + 
      ce.get_Event_date_and_time().toString() , "Event occured", JOptionPane.INFORMATION_MESSAGE); 
    }  
    
    public void showInfo(String txt)
    {
    
		  JOptionPane.showMessageDialog(this, txt, "Info", JOptionPane.INFORMATION_MESSAGE);      
      tablemodel.reload(); 
      monthmodel.reload();
      aglm.reload();
      aulm.reload();    
    }
    
        
    private WeekTableModel tablemodel;
    private MonthTableModel monthmodel;
    
    private AgendaGroupListModel aglm;
    private AgendaUserListModel aulm;
    
    private javax.swing.JButton jButton1;
    private javax.swing.JButton jButton2;
    private javax.swing.JButton jButton3;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JMenu jMenu1;
    private javax.swing.JMenu jMenu2;
    private javax.swing.JMenu jMenu3;
    private javax.swing.JMenu jMenu4;
    private javax.swing.JMenuBar jMenuBar1;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JScrollPane jScrollPane2;
    private javax.swing.JScrollPane jScrollPane3;
    private javax.swing.JTabbedPane jTabbedPane1;
    private javax.swing.JTable table;
    private javax.swing.JTable month;
    // End of variables declaration

}