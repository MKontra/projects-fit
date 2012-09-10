package kriz.panels;

import java.awt.Dimension;

import javax.swing.JPanel;

import kriz.KrizControllerAgent;
import javax.swing.JLabel;
import javax.swing.JTextField;
import javax.swing.SwingConstants;

public class KrizPanel extends JPanel {

	/**
	 * Create the panel.
	 */
	private final KrizControllerAgent myAgent;
	private JTextField textField;
	private JTextField textField_1;
	private JTextField textField_2;
	private JTextField textField_3;
	private JLabel label;
	private JLabel lblTiming;
	private JLabel label_1;
	public KrizPanel(KrizControllerAgent ag) {
		setLayout(null);
		
		JLabel lblNewLabel = new JLabel("timing[0+1]");
		lblNewLabel.setHorizontalAlignment(SwingConstants.CENTER);
		lblNewLabel.setBounds(84, 11, 67, 14);
		add(lblNewLabel);
		
		textField = new JTextField();
		textField.setEditable(false);
		textField.setBounds(94, 26, 46, 20);
		add(textField);
		textField.setColumns(10);
		
		textField_1 = new JTextField();
		textField_1.setEditable(false);
		textField_1.setBounds(94, 99, 44, 20);
		add(textField_1);
		textField_1.setColumns(10);
		
		textField_2 = new JTextField();
		textField_2.setEditable(false);
		textField_2.setBounds(71, 57, 39, 25);
		add(textField_2);
		textField_2.setColumns(10);
		
		textField_3 = new JTextField();
		textField_3.setEditable(false);
		textField_3.setBounds(120, 57, 39, 25);
		add(textField_3);
		textField_3.setColumns(10);
		
		label = new JLabel("timing[0+1]");
		label.setHorizontalAlignment(SwingConstants.CENTER);
		label.setBounds(84, 121, 67, 14);
		add(label);
		
		lblTiming = new JLabel("timing[2+3]");
		lblTiming.setHorizontalAlignment(SwingConstants.CENTER);
		lblTiming.setBounds(10, 65, 67, 14);
		add(lblTiming);
		
		label_1 = new JLabel("timing[2+3]");
		label_1.setHorizontalAlignment(SwingConstants.CENTER);
		label_1.setBounds(164, 65, 67, 14);
		add(label_1);
		myAgent = ag;
	}
	@Override
	public Dimension getPreferredSize() {
		return getMinimumSize();
	}
	
	@Override
	public Dimension getMinimumSize() {
		return new Dimension( 240, 150);
	}	
	
	public void updateTimings()
	{
		textField.setText( myAgent.timings[0]+"" );
		textField_1.setText( myAgent.timings[0]+"" );
		textField_2.setText( myAgent.timings[2]+"" );
		textField_3.setText( myAgent.timings[2]+"" );		
	}
	
	protected JTextField getTextField_2() {
		return textField_2;
	}
	protected JTextField getTextField() {
		return textField;
	}
	protected JTextField getTextField_1() {
		return textField_1;
	}
	protected JTextField getTextField_3() {
		return textField_3;
	}
}
