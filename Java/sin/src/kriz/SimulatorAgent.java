package kriz;

import java.awt.Dimension;
import java.awt.Toolkit;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.util.PriorityQueue;

import jade.core.Agent;
import jade.core.behaviours.*;

import jade.core.AID;
import jade.wrapper.AgentContainer;
import jade.wrapper.AgentController;

import jade.lang.acl.*;

import java.util.*;

import javax.swing.JFrame;

import kriz.KrizControllerAgent.state;
import kriz.panels.SimPanel;
import kriz.panels.TrafficLightStatePanel;
import kriz.util.DualLightState;
import kriz.util.LightState.LightStates;

class SimulatorAgentUI extends JFrame
{
	private SimulatorAgent myAgent;
	private SimPanel simPanel;
	public SimulatorAgentUI(SimulatorAgent simagent) {
		super(simagent.getLocalName());
		myAgent = simagent;
		simPanel = new SimPanel(simagent);
		this.setContentPane( simPanel );
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

}

public class SimulatorAgent extends Agent {
	
	private static long cid = 0;
	
	private static class SimCar implements Comparable
	{
		public long cid;
		public long timeEntered;
		public SimCar(long cid, long timeEntered) {
			this.cid = cid;
			this.timeEntered = timeEntered;
		}
		
		public static void getUniqueCarId()
		{
			
		}
		
		@Override
		public int compareTo(Object arg0) {
			SimCar sc = null;
			sc = (SimCar) arg0;
			if ( sc.timeEntered < this.timeEntered )
			{
				return 0;
			} else if ( sc.timeEntered > this.timeEntered )
			{
				return 0;
			} else
				return 0;
		}
		
	}
	
	private AID[] sensorTopRight = new AID[] {
			new AID( "CarCountSensor1", AID.ISLOCALNAME ), 
			new AID( "CarCountSensor2", AID.ISLOCALNAME ),
			new AID( "CarCountSensor3", AID.ISLOCALNAME ),
			new AID( "CarCountSensor4", AID.ISLOCALNAME )
	};
	
	private PriorityQueue<SimCar>[] ccstr = new PriorityQueue[]{ 
			new PriorityQueue<SimCar>(),
			new PriorityQueue<SimCar>(),
			new PriorityQueue<SimCar>(),
			new PriorityQueue<SimCar>()};
	
	private AID[] sensorLeft = new AID[] {
			new AID( "CarCountSensor5", AID.ISLOCALNAME ), 
			new AID( "CarCountSensor6", AID.ISLOCALNAME ),
			new AID( "CarCountSensor7", AID.ISLOCALNAME ),
			new AID( "CarCountSensor8", AID.ISLOCALNAME )
	};
	
	private PriorityQueue<SimCar>[] ccsl = new PriorityQueue[]{ 
			new PriorityQueue<SimCar>(),
			new PriorityQueue<SimCar>(),
			new PriorityQueue<SimCar>(),
			new PriorityQueue<SimCar>()};	
	
	private AID[] trafficLights = new AID[] {
			new AID( "TrafficLight1", AID.ISLOCALNAME ), 
			new AID( "TrafficLight2", AID.ISLOCALNAME ),
			new AID( "TrafficLight3", AID.ISLOCALNAME ),
			new AID( "TrafficLight4", AID.ISLOCALNAME )
	};
	
	private AID controller = new AID( "KrizController", AID.ISLOCALNAME );
	
	private Random r;

    public enum state {
    	unknown,
        north_south_topright ,
        north_south_left ,
        east_west_topright,
        east_west_left
    }	
	
    private static long unit = 10;

    
    public static long getUnit() {
		return unit;
	}


	private static long carTransitDelta = 2*unit; 
    private static long sensorToLightQueueDelay = 30*unit;
    
    private static long fromNorthAverageDelay = 3*unit;    
    private static long fromEastAverageDelay = 7*unit;    
    private static long fromSouthAverageDelay = 3*unit;   
    private static long fromWestAverageDelay = 7*unit;   
    
    protected CarGeneratingBehaviour fromNorth;
    protected CarGeneratingBehaviour fromEast;
    protected CarGeneratingBehaviour fromSouth;
    protected CarGeneratingBehaviour fromWest;
    
    protected SimulatorAgentUI myGui;

    FileOutputStream statfile; // declare a file output object
    PrintStream statprinter; // declare a print stream object      
 
    FileOutputStream carstatfile; // declare a file output object
    PrintStream carstatprinter; // declare a print stream object      
    
	@Override
	protected void setup() {
		r = new Random( System.currentTimeMillis());
		AgentContainer cc = getContainerController();
        try {
            AgentController a = cc.createNewAgent( "CarCountSensor1", "kriz.CarCountSensorAgent", new Object[] {"1", "simul", this.getAID()} );
            AgentController b = cc.createNewAgent( "CarCountSensor2", "kriz.CarCountSensorAgent", new Object[] {"2", "simul", this.getAID()} );
            AgentController c = cc.createNewAgent( "CarCountSensor3", "kriz.CarCountSensorAgent", new Object[] {"3", "simul", this.getAID()} );
            AgentController d = cc.createNewAgent( "CarCountSensor4", "kriz.CarCountSensorAgent", new Object[] {"4", "simul", this.getAID()} );
            AgentController e = cc.createNewAgent( "CarCountSensor5", "kriz.CarCountSensorAgent", new Object[] {"5", "simul", this.getAID()} );
            AgentController f = cc.createNewAgent( "CarCountSensor6", "kriz.CarCountSensorAgent", new Object[] {"6", "simul", this.getAID()} );
            AgentController g = cc.createNewAgent( "CarCountSensor7", "kriz.CarCountSensorAgent", new Object[] {"7", "simul", this.getAID()} );
            AgentController h = cc.createNewAgent( "CarCountSensor8", "kriz.CarCountSensorAgent", new Object[] {"8", "simul", this.getAID()} );
            
            AgentController i = cc.createNewAgent( "TrafficLight1", "kriz.TrafficLightAgent", new Object[] {"1", "simul", this.getAID()} );
            AgentController j = cc.createNewAgent( "TrafficLight2", "kriz.TrafficLightAgent", new Object[] {"2", "simul", this.getAID()} );
            AgentController k = cc.createNewAgent( "TrafficLight3", "kriz.TrafficLightAgent", new Object[] {"3", "simul", this.getAID()} );
            AgentController l = cc.createNewAgent( "TrafficLight4", "kriz.TrafficLightAgent", new Object[] {"4", "simul", this.getAID()} );  
            
            AgentController x = cc.createNewAgent( "KrizController", "kriz.KrizControllerAgent", new Object[] {sensorTopRight, sensorLeft, trafficLights} );           
            
            a.start();b.start();c.start();d.start();e.start();f.start();g.start();h.start();
            i.start();j.start();k.start();l.start();
            
            //Thread.sleep(5000);
            
            x.start();
        }
        catch (Exception e){e.printStackTrace();}

        ParallelBehaviour pbe = new ParallelBehaviour();
        pbe.addSubBehaviour( new CarProcessingBehaviour(carTransitDelta, sensorToLightQueueDelay) );
        
        fromNorth = new CarGeneratingBehaviour(1, fromNorthAverageDelay);
        fromEast = new CarGeneratingBehaviour(2, fromEastAverageDelay);
        fromSouth = new CarGeneratingBehaviour(3, fromSouthAverageDelay);
        fromWest = new CarGeneratingBehaviour(4, fromWestAverageDelay);
        
        
        
        pbe.addSubBehaviour( fromNorth );
        pbe.addSubBehaviour( fromEast );
        pbe.addSubBehaviour( fromSouth );
        pbe.addSubBehaviour( fromWest );
        
		myGui = new SimulatorAgentUI(this);        
        myGui.showGui();
	    try {
			statfile = new FileOutputStream("CarGenStat", false);
		} catch (FileNotFoundException e1) {
			e1.printStackTrace();
		}
	    statprinter = new PrintStream(statfile)	;	
	    writeCarStat();        
	    try {
			carstatfile = new FileOutputStream("CarProcStat", false);
		} catch (FileNotFoundException e1) {
			e1.printStackTrace();
		}
	    carstatprinter = new PrintStream(carstatfile)	;	       
        addBehaviour(pbe);
	}
	
	public void writeCarStat()
	{
	    statprinter.println(""+System.currentTimeMillis()+","+getFromNorth().getOtTimeOut()+","+getFromEast().getOtTimeOut()
	    		+","+getFromSouth().getOtTimeOut()+","+getFromWest().getOtTimeOut());
	}	

	public CarGeneratingBehaviour getFromNorth() {
		return fromNorth;
	}

	public void setFromNorth(CarGeneratingBehaviour fromNorth) {
		this.fromNorth = fromNorth;
	}

	public CarGeneratingBehaviour getFromEast() {
		return fromEast;
	}

	public void setFromEast(CarGeneratingBehaviour fromEast) {
		this.fromEast = fromEast;
	}

	public CarGeneratingBehaviour getFromSouth() {
		return fromSouth;
	}

	public void setFromSouth(CarGeneratingBehaviour fromSouth) {
		this.fromSouth = fromSouth;
	}

	public CarGeneratingBehaviour getFromWest() {
		return fromWest;
	}

	public void setFromWest(CarGeneratingBehaviour fromWest) {
		this.fromWest = fromWest;
	}

	@Override
	protected void takeDown() {
		// TODO Auto-generated method stub
		super.takeDown();
	}

	public class CarProcessingBehaviour extends CyclicBehaviour
	{
		
		private ACLMessage[] rcvdset = new ACLMessage[4];
		
		long lastSwitchTime;
	    
	    private state m_state = state.unknown;		
	    
	    private long carTransitDelta;
	    private long carEnterToQueuedDelay;
		
		public CarProcessingBehaviour(long carTransitDelta, long carEnterToQueuedDelay) {
			lastSwitchTime = System.currentTimeMillis();
			this.carTransitDelta = carTransitDelta;
			this.carEnterToQueuedDelay = carEnterToQueuedDelay;
		}

		@Override
		public void action() {
			ACLMessage got = receive();		
			if ( got != null )
			{
				//System.out.println("got message");
				long currentTime = System.currentTimeMillis();
				int tlid = -1;
				try {
					String lname = got.getSender().getLocalName();
					tlid = Integer.parseInt( lname.charAt(lname.length()-1)+"");
					tlid = tlid - 1;
				} catch (NumberFormatException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
				rcvdset[tlid] = got;
				boolean done = true;
				for (int i = 0; i < rcvdset.length; i++) {
					if ( rcvdset[i] == null ) done = false;
				}
				//System.out.println("processed message");
				if ( done )
				{
					long delta = currentTime - lastSwitchTime;
					if ( m_state != state.unknown )
					{
						//opened for
						switch ( m_state )
						{
						case north_south_topright:
							pumpQueueAndProcessStats(ccstr[0], delta, currentTime);
							pumpQueueAndProcessStats(ccstr[2], delta, currentTime);
							break;
							
						case north_south_left:
							pumpQueueAndProcessStats(ccsl[0], delta, currentTime);
							pumpQueueAndProcessStats(ccsl[2], delta, currentTime);
							break;
							
						case east_west_topright:
							pumpQueueAndProcessStats(ccstr[1], delta, currentTime);
							pumpQueueAndProcessStats(ccstr[3], delta, currentTime);
							break;
							
						case east_west_left:
							pumpQueueAndProcessStats(ccsl[0], delta, currentTime);
							pumpQueueAndProcessStats(ccsl[2], delta, currentTime);
							break;
							
						default: break;
						}
					}
					//infer next state
					DualLightState dls1 = null;
					DualLightState dls2 = null;		
					DualLightState dls3 = null;	
					DualLightState dls4 = null;	
					try {
						dls1 = ((DualLightState)rcvdset[0].getContentObject());
						dls2 = ((DualLightState)rcvdset[1].getContentObject());
						dls3 = ((DualLightState)rcvdset[2].getContentObject());
						dls4 = ((DualLightState)rcvdset[3].getContentObject());
					} catch (UnreadableException e) {
						e.printStackTrace();
					}
					if ( dls1.first.state == LightStates.green )
					{
						m_state = state.north_south_topright;
					} else if ( dls1.second.state == LightStates.green )
					{
						m_state = state.north_south_left;
					} else if ( dls2.first.state == LightStates.green )
					{
						m_state = state.east_west_topright;
					} else
					{
						m_state = state.east_west_left;
					}
					//System.out.println(dls1.toString());
					//System.out.println(dls2.toString());
					//System.out.println(dls3.toString());
					//System.out.println(dls4.toString());
					//System.out.println(m_state.toString());
					
					for (int i = 0; i < rcvdset.length; i++) {
						rcvdset[i] = null;
					}		
					lastSwitchTime = currentTime;
				}
			} else
				block();

		}
		
		public void pumpQueueAndProcessStats(PriorityQueue<SimCar> que, long delta, long currentTime)
		{
			//long lastOne = currentTime - delta;
			
			SimCar top = que.peek();
			if ( top == null )
				return;

			long pumpedCount = ( delta / carTransitDelta ) + 1;
			
			long i = 0;
			SimCar scurr = null;
			while ( (scurr = que.peek()) != null && i < pumpedCount )
			{
				if ( scurr.timeEntered - currentTime > carEnterToQueuedDelay-delta )
					break;
				carstatprinter.println(currentTime+","+scurr.cid+","+(currentTime - scurr.timeEntered));
				//System.out.println("Car processed");
				//System.out.println(scurr.cid);
				//System.out.println(scurr.timeEntered + carEnterToQueuedDelay);
				//System.out.println(currentTime);
				//System.out.println(currentTime - scurr.timeEntered );
				que.poll();
				i++;
			}
			//System.out.println("Queue size: " + que.size());
		}

	}

	
	public class CarGeneratingBehaviour extends CyclicBehaviour
	{

		long timeout;
		long lastTime;
		
		long otTimeOut;
		int direction;
		
		public CarGeneratingBehaviour(int direction, long otTimeOut) {
			this.direction = direction;
			this.otTimeOut = otTimeOut;
			lastTime = System.currentTimeMillis();
			timeout = otTimeOut;
		}

		
		
		public long getOtTimeOut() {
			return otTimeOut;
		}



		public void setOtTimeOut(long otTimeOut) {
			this.otTimeOut = otTimeOut;
		}



		@Override
		public void action() {
			
			long currentTime = System.currentTimeMillis();
			if ( currentTime - lastTime >= timeout )
			{
				boolean to = r.nextBoolean(); //true right, false left
				if ( to )
				{
					ccstr[direction-1].add( new SimCar(cid++, currentTime));
					ACLMessage acm = new ACLMessage(ACLMessage.INFORM);
					acm.addReceiver(sensorTopRight[direction-1]);
					send(acm);
				}else
				{
					ccsl[direction-1].add( new SimCar(cid++, currentTime));	
					ACLMessage acm = new ACLMessage(ACLMessage.INFORM);
					acm.addReceiver(sensorLeft[direction-1]);
					send(acm);
				}
				
				//System.out.println("added car from");
				//System.out.println(direction);
				//System.out.println(to);
				
				this.lastTime = currentTime;
				timeout = otTimeOut;//r.nextInt(10);
				block (timeout);
			} else
			{
				block ( Math.max(0, timeout - (currentTime - lastTime)) );
			}

		}

	}
}
