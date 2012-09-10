package kriz.panels;

import javax.swing.JPanel;
import java.awt.BorderLayout;
import javax.swing.JButton;
import java.awt.FlowLayout;
import javax.swing.JLabel;
import javax.swing.SwingConstants;
import javax.swing.JTextField;

import kriz.CarCountSensorAgent;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

public class SensorPanel extends JPanel {
	private JTextField textField;
	private CarCountSensorAgent myagent;
	/**
	 * Create the panel.
	 * @param myagent 
	 */
	public SensorPanel(CarCountSensorAgent myagent) {
		this.myagent = myagent;
		setLayout(new BorderLayout(0, 0));
		
		JPanel panel = new JPanel();
		FlowLayout flowLayout = (FlowLayout) panel.getLayout();
		flowLayout.setAlignment(FlowLayout.RIGHT);
		add(panel, BorderLayout.SOUTH);
		
		JButton btnNewButton = new JButton("Start");
		btnNewButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				//SensorPanel.this.myagent.startGeneration(Integer.parseInt(textField.getText()));
			}
		});
		panel.add(btnNewButton);
		
		JButton btnNewButton_1 = new JButton("Stop");
		btnNewButton_1.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				//SensorPanel.this.myagent.stopGeneration();
			}
		});
		panel.add(btnNewButton_1);
		
		JLabel lblNewLabel = new JLabel("<html>Sensor Settings<br>\r\nEnter Cars Per Minute Value:</html>");
		lblNewLabel.setHorizontalAlignment(SwingConstants.CENTER);
		add(lblNewLabel, BorderLayout.NORTH);
		
		JPanel panel_1 = new JPanel();
		add(panel_1, BorderLayout.CENTER);
		
		textField = new JTextField();
		panel_1.add(textField);
		textField.setColumns(10);
		
		JButton btnNewButton_2 = new JButton("Set");
		btnNewButton_2.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent arg0) {
				//SensorPanel.this.myagent.startGeneration(Integer.parseInt(textField.getText()));
			}
		});
		panel_1.add(btnNewButton_2);

	}

}
