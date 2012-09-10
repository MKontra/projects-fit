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
import javax.swing.JOptionPane;


public class GroupMan extends javax.swing.JDialog {

    private LocalCache lc = LocalCache.getDefaultCache();
    private ClientController cc; 
    public GroupMan(java.awt.Frame parent, boolean modal, ClientController cc) {
        super(parent, modal);
        this.cc = cc;
        initComponents();
    }

  private class GroupListRenderer extends JLabel implements ListCellRenderer {
       public GroupListRenderer() {
           setOpaque(true);
       }
       
   public Component getListCellRendererComponent(JList list,
                                                     Object value,
                                                     int index,
                                                     boolean isSelected,
                                                     boolean cellHasFocus) {
  
                Group ds = (Group) value;
                
                if (isSelected)
                  setBackground(Color.lightGray );
                else
                  setBackground(Color.WHITE ) ;
                  
                  
              setText("GroupName: " + ds.get_group_name() + "  GroupLeader: " + ds.get_boss());
  
           return this;
       }
       
       
       
       }


    private class GroupList extends AbstractListModel
    {
           
          private LinkedList<Group> groups = new LinkedList<Group>();
           
          public int getSize() {
            return groups.size();
          }
          
          public void reload()
          {
            groups = new LinkedList<Group>();
            for (Group_membership m : lc.getUser(cc.getLogin()).get_groups()) 
            {
            

                groups.add(lc.getGroup(m.get_group_name()));
          

            
            }
            fireContentsChanged(this, 0, groups.size() );
          }
    
          public Object getElementAt(int index) {
            return groups.get(index);
          }    
    
    
    }


    private void initComponents() {
        java.awt.GridBagConstraints gridBagConstraints;

        jScrollPane1 = new javax.swing.JScrollPane();
        jList1 = new javax.swing.JList();
        jButton1 = new javax.swing.JButton();
        jButton2 = new javax.swing.JButton();
        jButton3 = new javax.swing.JButton();;

        setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);
        getContentPane().setLayout(new java.awt.GridBagLayout());

        gl = new GroupList();
        GroupListRenderer glr = new GroupListRenderer();

        jList1.setModel(gl);
        jList1.setCellRenderer(glr);
        gl.reload();
        jScrollPane1.setViewportView(jList1);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridheight = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.ipadx = 100;
        gridBagConstraints.ipady = 120;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        getContentPane().add(jScrollPane1, gridBagConstraints);

        jButton1.setText("Add Group");
        jButton1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton1ActionPerformed(evt);
            }
        });
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        getContentPane().add(jButton1, gridBagConstraints);

        jButton2.setText("Remove");
        jButton2.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton2ActionPerformed(evt);
            }
        });
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        getContentPane().add(jButton2, gridBagConstraints);

        jButton3.setText("Add member");
        jButton3.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton3ActionPerformed(evt);
            }
        });
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        getContentPane().add(jButton3, gridBagConstraints);

        pack();
    }// </editor-fold>

    private void jButton1ActionPerformed(java.awt.event.ActionEvent evt) {
        //Add group
        AddGroup ag = new AddGroup( new javax.swing.JFrame(), true );
        ag.setVisible(true);
        
        if (ag.getAction().equals("OK"))
        {
          try{
            cc.createGroup(ag.getGroupName());
          } catch (Exception e)
          {
          		    JOptionPane.showMessageDialog(new javax.swing.JFrame(), "Group creation failed", "Error", JOptionPane.ERROR_MESSAGE);
          }
        }
        try { Thread.sleep(200); } catch (Exception e){}
        gl.reload();
    }

    private void jButton2ActionPerformed(java.awt.event.ActionEvent evt) {
      Group g = (Group) jList1.getSelectedValue();
      
          try{
            cc.deleteGroup(g.get_group_name());
          } catch (Exception e)
          {
          		    JOptionPane.showMessageDialog(new javax.swing.JFrame(), "Deletion failed", "Error", JOptionPane.ERROR_MESSAGE);
          }      
        try { Thread.sleep(200); } catch (Exception e){}
        gl.reload();
    }
    
    private void jButton3ActionPerformed(java.awt.event.ActionEvent evt) {
      Group g = (Group) jList1.getSelectedValue();
         String response = JOptionPane.showInputDialog(null,
                      "Username to add",
                        "name",
                    JOptionPane.QUESTION_MESSAGE);
          
          try{
            cc.addMember(g.get_group_name(), response);
          } catch (Exception e)
          {
          		    JOptionPane.showMessageDialog(new javax.swing.JFrame(), "User addition failed", "Error", JOptionPane.ERROR_MESSAGE);
          		    e.printStackTrace();
          }      
        try { Thread.sleep(200); } catch (Exception e){}
        gl.reload();
    }

    // Variables declaration - do not modify
    
    private GroupList gl;
    
    private javax.swing.JButton jButton1;
    private javax.swing.JButton jButton2;
    private javax.swing.JButton jButton3;
    private javax.swing.JList jList1;
    private javax.swing.JScrollPane jScrollPane1;
    // End of variables declaration

}