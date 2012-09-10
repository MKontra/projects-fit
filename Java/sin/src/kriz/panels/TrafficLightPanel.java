package kriz.panels;

import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.BoxLayout;
import javax.swing.JLabel;
import javax.swing.border.LineBorder;

import java.awt.AlphaComposite;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.net.URL;

import kriz.util.LightState;

public class TrafficLightPanel extends JPanel {
	public static final int LEFT = 0;
	public static final int TOP = 1;
	public static final int RIGHT = 2;
	public static final int BOTTOM = 3;
	
	public static final int RED = 1;	
	public static final int ORANGE = 2;		
	public static final int GREEN = 3;		

	public static final String baseDir = "lightstates" ;	

    private static ImageIcon icon(String path) {
        URL resource = TrafficLightPanel.class.getResource(path);
        if(resource==null) {
            System.err.println("Resource "+path+" does not exist");
            return new ImageIcon();
        }
        return new ImageIcon(resource);
    }
	
	public static final ImageIcon leftGreen = icon(baseDir + "/leftGreen.png");
	public static final ImageIcon leftOrange = icon(baseDir + "/leftOrange.png");
	public static final ImageIcon leftRed = icon(baseDir + "/leftRed.png");
	
	public static final ImageIcon topGreen = icon(baseDir + "/topGreen.png");
	public static final ImageIcon topOrange = icon(baseDir + "/topOrange.png");
	public static final ImageIcon topRed = icon(baseDir + "/topRed.png");

	public static final ImageIcon rightGreen = icon(baseDir + "/rightGreen.png");
	public static final ImageIcon rightOrange = icon(baseDir + "/rightOrange.png");
	public static final ImageIcon rightRed = icon(baseDir + "/rightRed.png");	

	public static final ImageIcon icongrid[][] = new ImageIcon[][] { {leftGreen,topGreen,rightGreen},
																	 {leftOrange,topOrange,rightOrange}, 
																	 {leftRed,topRed,rightRed} };
	
	private boolean currentState;
	private boolean twoState;
	private int currentDirection;
	private LightLabel redLight;
	private LightLabel orangeLight;
	private LightLabel greenLight;
	
	public TrafficLightPanel(int direction, boolean twoState, boolean initialState)
	{

		if ( direction >= 0 && direction <= 4)
			currentDirection = direction;
		else
			currentDirection = 0;
		this.twoState = twoState;
		this.currentState = initialState;
		
		setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
		setBorder(null);
		
		redLight = new LightLabel(icongrid[2][direction], true);
		add(redLight);
		
		orangeLight = new LightLabel(icongrid[1][direction], false);
		if ( twoState == false )
		{			
			add(orangeLight);
		}
		greenLight = new LightLabel(icongrid[0][direction], false);
		add(greenLight);
		
		
	}
	
	public void setState( LightState ls )
	{
		switch (ls.state)
		{
			case green: redLight.setEnabled( false); orangeLight.setEnabled(false); greenLight.setEnabled(true); break;
			case orange: redLight.setEnabled( false); orangeLight.setEnabled(true); greenLight.setEnabled(false); break;
			case red: redLight.setEnabled( true); orangeLight.setEnabled(false); greenLight.setEnabled(false); break;
			default: break;
		}
		this.revalidate();
	}
	
	private class LightLabel extends JLabel
	{
		private ImageIcon icon;
		private boolean enabled;
		public LightLabel(ImageIcon icon, boolean enabled)
		{
			this.icon = icon;
			this.enabled = enabled;
			this.setText(null);
			this.setIcon(icon);
		}
		
		@Override
		public void paint(Graphics g) {
			// TODO Auto-generated method stub
			super.paint(g);
			if ( !enabled )
			{
				Graphics2D g2 = (Graphics2D)g;
				g2.setComposite(AlphaComposite.getInstance(AlphaComposite.SRC_OVER, 0.5f));
				Color c = Color.DARK_GRAY;
				g2.setPaint(c);
				g.fillRect(0, 0, getWidth(), getHeight());
			}
		}
		
		public boolean isEnabled() {
			return enabled;
		}
		public void setEnabled(boolean enabled) {
			this.enabled = enabled;
			repaint();		
		}
		
		
	}
	
	protected JLabel getLblNewLabel() {
		return redLight;
	}
	protected JLabel getLblNewLabel_1() {
		return orangeLight;
	}
	protected JLabel getLblNewLabel_2() {
		return greenLight;
	}

	public static void main(String[] args)
	{
		JFrame jfr = new JFrame();
		jfr.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		jfr.setContentPane(new TrafficLightPanel(2, false, true));
		jfr.pack();
		jfr.setVisible(true);
	}
	
}
