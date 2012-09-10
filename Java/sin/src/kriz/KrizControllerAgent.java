package kriz;

import jade.core.AID;
import jade.core.Agent;
import jade.core.behaviours.Behaviour;
import jade.core.behaviours.CyclicBehaviour;
import jade.core.behaviours.OneShotBehaviour;
import jade.core.behaviours.ParallelBehaviour;
import jade.core.behaviours.SequentialBehaviour;
import jade.core.behaviours.SimpleBehaviour;
import jade.domain.DFService;
import jade.domain.FIPAException;
import jade.domain.FIPAAgentManagement.DFAgentDescription;
import jade.domain.FIPAAgentManagement.ServiceDescription;
import jade.lang.acl.ACLMessage;

import java.awt.Dimension;
import java.awt.Toolkit;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.util.logging.*;

import javax.swing.JFrame;

import kriz.panels.KrizPanel;
import kriz.util.DualLightState;
import kriz.util.LightState;
import kriz.util.LightState.LightStates;

class KrizControllerAgentUI extends JFrame
{
	private KrizControllerAgent myAgent;
	private KrizPanel krPanel;
	public KrizControllerAgentUI(KrizControllerAgent kragent) {
		super(kragent.getLocalName());
		myAgent = kragent;
		krPanel = new KrizPanel(myAgent);
		this.setContentPane( krPanel );
		addWindowListener(new	WindowAdapter() {
			public void windowClosing(WindowEvent e) {
				myAgent.doDelete();
			}
		} );				
	}
	
	public void showGui() {
		pack();
		Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
		int centerX = (int)screenSize.getWidth() / 2;
		int centerY = (int)screenSize.getHeight() / 2;
		setLocation(centerX - getWidth() / 2, centerY - getHeight() / 2);
		super.setVisible(true);
	}	
	
	public void updateTimings()
	{
		krPanel.updateTimings();	
	}	

}

public class KrizControllerAgent extends Agent {

	private AID sensorsTopRight[] = new AID[4];
	private AID sensorsLeft[] = new AID[4];
	private AID lights[] = new AID[4];	
	
	private KrizControllerAgentUI kui;
	
	private long[] lastUpdateCounts = new long[] {0,0,0,0};
	
    private static Logger logger = Logger.getLogger("kriz.KrizControllerAgent");
    private static FileHandler fh;

    FileOutputStream statfile; // declare a file output object
    PrintStream statprinter; // declare a print stream object    
    
    public enum state {
        north_south_topright ,
        north_south_left ,
        east_west_topright,
        east_west_left
    }

    private static long unit = 10;
    private static long timingsDelta = 1*unit;
    
    private static long minTiming = 10*unit;
    private static long maxTiming = 60*unit;
    private static long recalcDelta = 10*unit;
    private static double triggerRatio = 1.0;
    
    public long[] timings = { 30*unit, 30*unit, 30*unit, 30*unit };
    
    private state m_state = state.north_south_topright;
    
	@Override
	protected void setup() {
		//setup log
		try {
			fh = new FileHandler("controller.txt");
		} catch (IOException e) {
			System.out.println("Cannot open logfile");
			e.printStackTrace();
		}
		fh.setLevel(Level.ALL);
		fh.setFormatter(new SimpleFormatter());
		logger.addHandler(fh);
		logger.addHandler( new ConsoleHandler());
		
	    try {
			statfile = new FileOutputStream("KrizControllerStat", false);
		} catch (FileNotFoundException e1) {
			e1.printStackTrace();
		}
	    statprinter = new PrintStream(statfile)	;	
	    
		Object[] args = getArguments();
		try
		{
			sensorsTopRight = (AID[])args[0];
			sensorsLeft = (AID[])args[1];
			lights = (AID[])args[2];
		} catch (Exception e) {
			e.printStackTrace();
		}
		kui = new KrizControllerAgentUI (this);
		kui.showGui();
		kui.updateTimings();
		logger.info("Controller commences function");
		SequentialBehaviour seq = new SequentialBehaviour();
		seq.addSubBehaviour( new EnableComponents());
		ParallelBehaviour pbe = new ParallelBehaviour();
		pbe.addSubBehaviour( new SensorListenerBehaviour());
		pbe.addSubBehaviour( new TimingsRecalculationBehaviour(recalcDelta ) );
		pbe.addSubBehaviour( new LightManagingBehaviour());
		seq.addSubBehaviour( pbe );
		addBehaviour(seq);
	}

	private void writeTimings()
	{
	    statprinter.println(""+System.currentTimeMillis()+","+timings[0]+","+timings[1]+","+timings[2]+","+timings[3]);
	}
	
	private class EnableComponents extends OneShotBehaviour
	{

		@Override
		public void action() {
			ACLMessage st = new ACLMessage(ACLMessage.INFORM_REF);
			for (int i = 0; i < 4; i++) {
				st.addReceiver(sensorsTopRight[i]);
				st.addReceiver(sensorsLeft[i]);
				st.addReceiver(lights[i]);
			}
			try {
				st.setContentObject(getAID());
			} catch (IOException e) {
				logger.severe("Cannot send identification");
				e.printStackTrace();
			}
			send(st);
			
		}}
	
	private class SensorListenerBehaviour extends CyclicBehaviour
	{

		@Override
		public void action() {
			ACLMessage got = receive();
			if ( got != null )
			{
				//System.out.println("updating counts...");				
				String lname = got.getSender().getLocalName();
				//System.out.println(lname);	
				if ( !lname.startsWith("CarCountSensor")) return;
				int tlid = Integer.parseInt( lname.charAt(lname.length()-1)+"");
				tlid = tlid - 1;

				if ( tlid < 4 ) //topright
				{
					lastUpdateCounts[tlid]++;
				} else
				{
					lastUpdateCounts[tlid-4]++;
				}
				
			} else
				block();
			
		}
		
	}
	
	private class TimingsRecalculationBehaviour extends CyclicBehaviour
	{
		long timeout;
		long lastAction;
		
		public TimingsRecalculationBehaviour(long timeout) {
			this.timeout = timeout;
			this.lastAction = System.currentTimeMillis();
		}
		@Override
		public void action() {
			long currentTime = System.currentTimeMillis();
			if ( currentTime - lastAction >= timeout )
			{
				//System.out.println("Recealculating...");						
				//calculate relatives
				long northsouthtotal = lastUpdateCounts[0] + lastUpdateCounts[2];
				long eastwesttotal = lastUpdateCounts[1] + lastUpdateCounts[3];
				
				double newCarRatio = ((double)northsouthtotal) / eastwesttotal;
				double oldTimeRatio = ((double)timings[0]) / timings[2];
				
				//System.out.println(northsouthtotal);	
				//System.out.println(eastwesttotal);
				
				//timingsDelta = Math.round(newCarRatio);
				
				if ( true ) //Math.abs(newCarRatio/oldTimeRatio) > triggerRatio )
				{
					if ( newCarRatio > oldTimeRatio )
					{
						//System.out.println("Updating timing north+");
						if ( timings[0]+timingsDelta <= maxTiming && timings[2] -timingsDelta >= minTiming )
						{
							timings[0] = Math.min( timings[0] +timingsDelta,maxTiming) ;
							timings[2] = Math.max( timings[2] -timingsDelta,minTiming);
							timings[1] = Math.min( timings[1] +timingsDelta,maxTiming);
							timings[3] = Math.max( timings[3] -timingsDelta,minTiming);
						}
					} else if ( newCarRatio < oldTimeRatio )
					{
						if ( timings[2]+timingsDelta <= maxTiming && timings[0] -timingsDelta >= minTiming )
						{						
							//System.out.println("Updating timing east+");
							timings[0] = Math.max( timings[0] -timingsDelta,minTiming) ;
							timings[2] = Math.min( timings[2] +timingsDelta,maxTiming);
							timings[1] = Math.max( timings[1] -timingsDelta,minTiming);
							timings[3] = Math.min( timings[3] +timingsDelta,maxTiming);	
						}
					} else
					{
						//do nothing
					}
					kui.updateTimings();
					writeTimings();
				}
				
				//System.out.println( timings[0]);
				//System.out.println( timings[1]);
				//System.out.println( timings[2]);
				//System.out.println( timings[3]);
				
				for (int i = 0; i < lastUpdateCounts.length; i++) {
					lastUpdateCounts[i] = 0;
				}
				this.lastAction = currentTime;
				block (timeout);			
			} else
				block( timeout - (currentTime - lastAction) );
		}
		
	}
	
	private class LightManagingBehaviour extends Behaviour
	{
		long timeout;
		long lastAction;
		
		public LightManagingBehaviour() {
			this.timeout = 0;//timings[0];
			this.lastAction = System.currentTimeMillis();
		}

		@Override
		public void action() {
			//System.out.println("Executing light management");
			long currentTime = System.currentTimeMillis();
			if ( currentTime - lastAction >= timeout )
			{
				//System.out.println("Updating");
				//switch state
				switch (m_state)
				{
					case north_south_topright: m_state = state.north_south_left; timeout = timings[1]; break;
					case north_south_left: m_state = state.east_west_topright; timeout = timings[2]; break;
					case east_west_topright: m_state = state.east_west_left; timeout = timings[3]; break;
					case east_west_left: m_state = state.north_south_topright; timeout = timings[0]; break;
					default: break;
				}
				this.lastAction = currentTime;
				
				setTrafficLights( m_state );
				
				block (timeout);
			} else
				block( timeout - (currentTime - lastAction) );
			
		}

		public void setTrafficLights(state lstate)
		{
			ACLMessage to_1 = new ACLMessage(ACLMessage.INFORM);
			ACLMessage to_2 = new ACLMessage(ACLMessage.INFORM);
			ACLMessage to_3 = new ACLMessage(ACLMessage.INFORM);
			ACLMessage to_4 = new ACLMessage(ACLMessage.INFORM);
			
			to_1.addReceiver( lights[0] );
			to_2.addReceiver( lights[1] );
			to_3.addReceiver( lights[2] );
			to_4.addReceiver( lights[3] );	
			
			DualLightState lsto_1 = null;
			DualLightState lsto_2 = null;
			DualLightState lsto_3 = null;
			DualLightState lsto_4 = null;
			
			switch (m_state) 
			{
				case north_south_topright: 
					lsto_1 = new DualLightState( LightStates.green, LightStates.red );
					lsto_2 = new DualLightState( LightStates.red, LightStates.red );
					lsto_3 = new DualLightState( LightStates.green, LightStates.red );
					lsto_4 = new DualLightState( LightStates.red, LightStates.red );
					break;
				case north_south_left: 					
					lsto_1 = new DualLightState( LightStates.red, LightStates.green );
					lsto_2 = new DualLightState( LightStates.red, LightStates.red );
					lsto_3 = new DualLightState( LightStates.red, LightStates.green );
					lsto_4 = new DualLightState( LightStates.red, LightStates.red );
					break;
				case east_west_topright:
					lsto_1 = new DualLightState( LightStates.red, LightStates.red );
					lsto_2 = new DualLightState( LightStates.green, LightStates.red );
					lsto_3 = new DualLightState( LightStates.red, LightStates.red );
					lsto_4 = new DualLightState( LightStates.green, LightStates.red );
					break;
				case east_west_left:
					lsto_1 = new DualLightState( LightStates.red, LightStates.red );
					lsto_2 = new DualLightState( LightStates.red, LightStates.green );
					lsto_3 = new DualLightState( LightStates.red, LightStates.red );
					lsto_4 = new DualLightState( LightStates.red, LightStates.green );
					break;
				default: break;				
			}	
			try
			{
				to_1.setContentObject(lsto_1);
				to_2.setContentObject(lsto_2);
				to_3.setContentObject(lsto_3);
				to_4.setContentObject(lsto_4);
			} catch (Exception e) {
				e.printStackTrace();
			}
			
			send(to_1);send(to_2);send(to_3);send(to_4);
			
		}
		
		@Override
		public boolean done() {
			return false;
		}
		
	}
	
	
}
