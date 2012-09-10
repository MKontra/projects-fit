/**
 * 
 */
package kriz;

import java.awt.Dimension;
import java.awt.HeadlessException;
import java.awt.Toolkit;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

import javax.swing.JFrame;

import jade.core.Agent;
import jade.core.AID;
import jade.core.behaviours.*;
import jade.lang.acl.ACLMessage;
import jade.lang.acl.MessageTemplate;
import jade.lang.acl.UnreadableException;
import jade.domain.DFService;
import jade.domain.FIPAException;
import jade.domain.FIPAAgentManagement.DFAgentDescription;
import jade.domain.FIPAAgentManagement.ServiceDescription;
import kriz.panels.SensorPanel;
/**
 * @author Administrator
 *
 */

class CarCountSensorGui extends JFrame
{
	private CarCountSensorAgent myAgent;

	public CarCountSensorGui(CarCountSensorAgent myagent) throws HeadlessException {
		super(myagent.getLocalName());
		this.myAgent = myagent;
		this.setContentPane(new SensorPanel(myagent));
		addWindowListener(new WindowAdapter() {
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

public class CarCountSensorAgent extends Agent
{
	protected CarCountSensorGui myGui;
	protected int myid;
	
	protected AID contrl = null;

	private AID simulator = null;	
	
	@Override
	protected void setup() {
		Object[] args = getArguments();
		if ( args.length < 1 )
			throw new IllegalArgumentException("Missing sensor identifier");
		
		myid = Integer.parseInt( (String)args[0]);

		if ( args.length == 3 && ((String)args[1]).equals("simul"))
		{
			this.simulator = (AID)args[2];
		}
		
		myid = Integer.parseInt( (String)args[0]);		
		
		//myGui = new CarCountSensorGui(this);
		//myGui.showGui();
        DFAgentDescription dfd = new DFAgentDescription();
        dfd.setName( getAID() ); 
        ServiceDescription sd  = new ServiceDescription();
        sd.setType( "count-sensor" );
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
        seq.addSubBehaviour( new EnableSensorBehaviour() );
        seq.addSubBehaviour( new SimulatorForwardingBehaviour() );
        addBehaviour(seq);        
	}
	
    protected void takeDown() 
    {
       try { DFService.deregister(this); }
       catch (Exception e) { e.printStackTrace(); }
    }
	
	private class EnableSensorBehaviour extends OneShotBehaviour
	{

		@Override
		public void action() {
			// TODO Auto-generated method stub
			ACLMessage got = null;
			got = blockingReceive(MessageTemplate.MatchPerformative(ACLMessage.INFORM_REF));
			/**
			while ( (got = blockingReceive()) != null )
			{
				//System.out.println(got.getContent());
				if ( MessageTemplate.MatchContent("enable").match(got))
					break;
			}**/
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
	
	private class SimulatorForwardingBehaviour extends CyclicBehaviour
	{

		@Override
		public void action() {
			ACLMessage got = receive();
			if ( got != null )
			{
				//System.out.println("wahaaa");
				if ( got.getSender().equals(simulator) )
				
				{
					//System.out.println(got.getPerformative(got.getPerformative()));
					//System.out.println(got.getContent());
					ACLMessage snt = new ACLMessage(ACLMessage.INFORM);
					snt.addReceiver(contrl);
					send(snt);
				}
			} else
				block();	
		}
		
	}

}
