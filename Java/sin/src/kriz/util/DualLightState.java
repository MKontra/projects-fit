package kriz.util;

import jade.util.leap.Serializable;
import kriz.util.LightState.LightStates;

public class DualLightState implements java.io.Serializable
{
	public LightState first;
	public LightState second;
	public DualLightState(LightState first, LightState second) {
		super();
		this.first = first;
		this.second = second;
	}
	
	public DualLightState(LightStates first, LightStates second) {
		super();
		this.first = new LightState(first);
		this.second = new LightState(second);
	}

	@Override
	public String toString() {
		return "first: "+first.toString() + " second: " + second.toString();
	}	
	
	
}