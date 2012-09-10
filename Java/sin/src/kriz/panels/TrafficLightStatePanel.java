package kriz.panels;


import java.net.URL;

import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JPanel;

import kriz.TrafficLightAgent;
import kriz.util.DualLightState;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.GridBagLayout;
import java.awt.GridBagConstraints;
import java.awt.GridLayout;
import javax.swing.JLabel;
import javax.swing.JButton;
import java.awt.BorderLayout;
import java.awt.Dimension;

public class TrafficLightStatePanel extends JPanel {
	TrafficLightAgent myagent;

	public static final String baseDir = "road" ;	

    private static ImageIcon icon(String path) {
        URL resource = TrafficLightStatePanel.class.getResource(path);
        if(resource==null) {
            System.err.println("Resource "+path+" does not exist");
            return new ImageIcon();
        }
        return new ImageIcon(resource);
    }
	
	public static final ImageIcon road = icon(baseDir + "/road.png");
	private TrafficLightPanel trafficLightPanel;
	private TrafficLightPanel trafficLightPanel_1;
	private TrafficLightPanel trafficLightPanel_2;
	
	public TrafficLightStatePanel(TrafficLightAgent myagent) {
		this.myagent = myagent;
		setLayout(null);
		
		JLabel lblNewLabel = new JLabel(road);
		lblNewLabel.setBounds(10, 114, 370, 96);
		add(lblNewLabel);
		
		trafficLightPanel = new TrafficLightPanel(0, true, false);
		trafficLightPanel.setBounds(203, 7, 32, 96);
		add(trafficLightPanel);
		
		trafficLightPanel_1 = new TrafficLightPanel(1, true, false);
		trafficLightPanel_1.setBounds(245, 7, 32, 96);
		add(trafficLightPanel_1);
		
		trafficLightPanel_2 = new TrafficLightPanel(2, true, false);
		trafficLightPanel_2.setBounds(287, 7, 32, 96);
		add(trafficLightPanel_2);
		
	}
	
	public void setLightState( DualLightState dls )
	{
		trafficLightPanel.setState( dls.second );
		trafficLightPanel_1.setState( dls.first );
		trafficLightPanel_2.setState( dls.first );
		repaint();
	}
	
	@Override
	public void paint(Graphics g) {
		Graphics2D g2 = (Graphics2D) g;

		int agentstate = myagent.getMyid();
		if ( agentstate == 2 )
		{
		    g2.rotate( -Math.PI/2, 390/2, 390/2); //(myagent.getMyid()-1)*
		    //g2.translate(0, -40);		
		} else if ( agentstate == 3 )
		{
		    g2.rotate( Math.PI, 390/2, 390/2); //(myagent.getMyid()-1)*
		    g2.translate(0, 390/2 - 20);	
		} else if ( agentstate == 4 )
		{
		    g2.rotate( Math.PI/2, 390/2, 390/2); //(myagent.getMyid()-1)*
		    g2.translate(0, 390/2 - 20);	
		}
		

		super.paint(g);
	}



	@Override
	public Dimension getPreferredSize() {
		return getMinimumSize();
	}
	
	@Override
	public Dimension getMinimumSize() {
		if ( myagent.getMyid() % 2 == 0)
			return new Dimension( 222, 390);
		else
			return new Dimension( 390, 222);
	}

	public static void main(String[] args)
	{
		JFrame jfr = new JFrame();
		jfr.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		jfr.setContentPane(new TrafficLightStatePanel(null));
		jfr.pack();
		jfr.setVisible(true);
	}
	protected TrafficLightPanel getTrafficLightPanel() {
		return trafficLightPanel;
	}
	protected TrafficLightPanel getTrafficLightPanel_1() {
		return trafficLightPanel_1;
	}
	protected TrafficLightPanel getTrafficLightPanel_2() {
		return trafficLightPanel_2;
	}
}
