package kriz.util;

import java.io.Serializable;

public class LightState implements Serializable
{
	public enum LightStates { red, orange, green }
	public LightStates state;
	public LightState(LightStates state) {
		this.state = state;
	}
	@Override
	public String toString() {
		return this.state.toString();
	}	
	
}