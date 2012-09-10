package kriz;
import java.awt.Dimension;
import java.awt.Toolkit;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.IOException;

import javax.swing.JFrame;
import javax.swing.SwingUtilities;

import jade.core.AID;
import jade.core.Agent;
import jade.core.behaviours.CyclicBehaviour;
import jade.core.behaviours.OneShotBehaviour;
import jade.core.behaviours.SequentialBehaviour;
import jade.domain.DFService;
import jade.domain.FIPAException;
import jade.domain.FIPAAgentManagement.DFAgentDescription;
import jade.domain.FIPAAgentManagement.ServiceDescription;
import jade.lang.acl.ACLMessage;
import jade.lang.acl.MessageTemplate;
import jade.lang.acl.UnreadableException;
import kriz.panels.SensorPanel;
import kriz.panels.TrafficLightStatePanel;
import kriz.util.DualLightState;

class TrafficLightUI extends JFrame
{
	private TrafficLightAgent myAgent;
	private TrafficLightStatePanel trafficLightPanel;
	public TrafficLightUI(TrafficLightAgent trafficLight) {
		super(trafficLight.getLocalName());
		myAgent = trafficLight;
		trafficLightPanel = new TrafficLightStatePanel(trafficLight);
		this.setContentPane( trafficLightPanel );
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
	
	public void setState( DualLightState dls )
	{
		trafficLightPanel.setLightState(dls);
		this.repaint();
	}

}

public class TrafficLightAgent extends Agent 
{
	private TrafficLightUI myGui;
	
	private AID simulator = null;
	
	protected int myid;
	
	public int getMyid() {
		return myid;
	}

	protected AID contrl = null;
	@Override
	protected void setup() {
		Object[] args = getArguments();
		if ( args.length < 1 )
			throw new IllegalArgumentException("Missing light identifier");
		
		if ( args.length == 3 && ((String)args[1]).equals("simul"))
		{
			this.simulator = (AID)args[2];
		}
		
		myid = Integer.parseInt( (String)args[0]);
		
		myGui = new TrafficLightUI(this);
		myGui.showGui();
        DFAgentDescription dfd = new DFAgentDescription();
        dfd.setName( getAID() ); 
        ServiceDescription sd  = new ServiceDescription();
        sd.setType( "traffic-light" );
        sd.setName( Integer.toString(myid) );
        dfd.addServices(sd);
        
        try {
        	DFAgentDescription list[] = DFService.search( this, dfd );
			if ( list.length>0 ) 
            	DFService.deregister(this);
            	
            dfd.addServices(sd);
			DFService.register(this,dfd);
		}
        catch (FIPAException fe) { fe.printStackTrace(); }
        
        SequentialBehaviour seq = new SequentialBehaviour();
        seq.addSubBehaviour( new EnableLightBehaviour() );
        seq.addSubBehaviour( new UpdateLights() );
        addBehaviour(seq);
	}
	
    protected void takeDown() 
    {
       try { DFService.deregister(this); }
       catch (Exception e) { e.printStackTrace(); }
    }
    
	protected void setState(DualLightState state)
	{
		if ( state == null ) return;
		final DualLightState tmp = state;
		Runnable updateUiState = new Runnable() {
		    public void run() { myGui.setState(tmp); }
		};
		SwingUtilities.invokeLater(updateUiState);
	}
	
	private class EnableLightBehaviour extends OneShotBehaviour
	{

		@Override
		public void action() {
			// TODO Auto-generated method stub
			ACLMessage got = null;
			got = blockingReceive(MessageTemplate.MatchPerformative(ACLMessage.INFORM_REF));
			/*
			while ( (got = blockingReceive()) != null )
			{
				System.out.println("waiting for message");
				if ( MessageTemplate.MatchContent("enable").match(got))
					break;
			}*/
			try {
				contrl = (AID)got.getContentObject();
			} catch (UnreadableException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			//System.out.println("Having controller reference");
			//myGui.setControllerRunning();
		}
		
	}
	
	private class UpdateLights extends CyclicBehaviour
	{
		@Override
		public void action()
		{
			ACLMessage got = receive();
			if ( got != null )
			{
				if ( !got.getSender().equals(contrl)) return;
				DualLightState state = null;
				try {
					state = (DualLightState)got.getContentObject();
				} catch (UnreadableException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
					System.out.println(got.getSender().getLocalName());
					System.out.println( ACLMessage.getPerformative(got.getPerformative()));
					System.out.println( got.getContent() );
				}
				//System.out.println("Got light settings message");
				setState(state);
				if ( simulator != null )
				{
					ACLMessage simmsg = new ACLMessage( ACLMessage.INFORM );
					simmsg.addReceiver(simulator);
					try {
						simmsg.setContentObject( state);
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
					send(simmsg);
				}
			} else
				block();
		}
		
	}
	
}
