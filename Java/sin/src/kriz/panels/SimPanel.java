package kriz.panels;

import java.awt.Dimension;

import javax.swing.JPanel;

import kriz.SimulatorAgent;
import javax.swing.JLabel;
import javax.swing.JSpinner;
import javax.swing.JButton;
import javax.swing.SpinnerNumberModel;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

public class SimPanel extends JPanel {

	/**
	 * Create the panel.
	 */
	
	private final SimulatorAgent myAg;
	private final JSpinner spinner_1;
	private final JSpinner spinner_2;
	private final JSpinner spinner_3;
	private final JSpinner spinner;
	
	public SimPanel(SimulatorAgent simag) {
		setLayout(null);
		myAg = simag;
		JLabel lblFromnorthaverage = new JLabel("fromNorthEvery");
		lblFromnorthaverage.setBounds(10, 11, 111, 14);
		add(lblFromnorthaverage);
		
		JLabel lblFromeastaverage = new JLabel("fromEastEvery");
		lblFromeastaverage.setBounds(10, 36, 111, 14);
		add(lblFromeastaverage);
		
		JLabel lblFromsouthaverage = new JLabel("fromSouthEvery");
		lblFromsouthaverage.setBounds(10, 61, 111, 14);
		add(lblFromsouthaverage);
		
		JLabel lblFromwestaverage = new JLabel("fromWestEvery");
		lblFromwestaverage.setBounds(10, 86, 111, 14);
		add(lblFromwestaverage);
		
		spinner = new JSpinner();
		spinner.setModel(new SpinnerNumberModel( new Long(myAg.getFromNorth().getOtTimeOut()/myAg.getUnit()), new Long(0), new Long(20), new Long(1)));
		spinner.setBounds(131, 8, 46, 20);
		add(spinner);
		
		spinner_1 = new JSpinner();
		spinner_1.setModel(new SpinnerNumberModel( new Long(myAg.getFromEast().getOtTimeOut()/myAg.getUnit()), new Long(0), new Long(20), new Long(1)));
		spinner_1.setBounds(131, 33, 46, 20);
		add(spinner_1);
		
		spinner_2 = new JSpinner();
		spinner_2.setModel(new SpinnerNumberModel(new Long(myAg.getFromSouth().getOtTimeOut()/myAg.getUnit()), new Long(0), new Long(20), new Long(1)));
		spinner_2.setBounds(131, 58, 46, 20);
		add(spinner_2);
		
		spinner_3 = new JSpinner();
		spinner_3.setModel(new SpinnerNumberModel( new Long(myAg.getFromWest().getOtTimeOut()/myAg.getUnit()), new Long(0), new Long(20), new Long(1)));
		spinner_3.setBounds(131, 83, 46, 20);
		add(spinner_3);
		
		JButton btnSet = new JButton("Set");
		btnSet.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				myAg.getFromNorth().setOtTimeOut( ((Long)spinner.getModel().getValue())*myAg.getUnit());
				myAg.getFromEast().setOtTimeOut( ((Long)spinner_1.getModel().getValue())*myAg.getUnit());
				myAg.getFromSouth().setOtTimeOut( ((Long)spinner_2.getModel().getValue())*myAg.getUnit());
				myAg.getFromWest().setOtTimeOut( ((Long)spinner_3.getModel().getValue())*myAg.getUnit());
				myAg.writeCarStat();
			}
		});
		btnSet.setBounds(211, 27, 89, 23);
		add(btnSet);
		
		JButton btnStop = new JButton("Stop");
		btnStop.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				System.exit(0);
			}
		});
		btnStop.setBounds(211, 57, 89, 23);
		add(btnStop);
	}
	@Override
	public Dimension getPreferredSize() {
		return getMinimumSize();
	}
	
	@Override
	public Dimension getMinimumSize() {
		return new Dimension( 328, 114);
	}
	protected JSpinner getSpinner_1() {
		return spinner_1;
	}
	protected JSpinner getSpinner_2() {
		return spinner_2;
	}
	protected JSpinner getSpinner_3() {
		return spinner_3;
	}
	protected JSpinner getSpinner() {
		return spinner;
	}
}
