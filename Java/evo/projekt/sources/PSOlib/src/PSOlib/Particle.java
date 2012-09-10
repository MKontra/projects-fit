package PSOlib;

public class Particle
{
  private Swarm parentSwarm;
  private double position[];
  private double velocity[];
  private double particleBestVal;
  private double particleBest[];
  
  public Particle( double initialPos[], double initialVel[] , Swarm parentSwarm )
  {
    position = initialPos.clone();
    particleBest = initialPos.clone();
    velocity = initialVel.clone();
    this.parentSwarm = parentSwarm;
  }
  
  public Particle( Swarm parentSwarm )
  {
    position = parentSwarm.getRandomInitialPos();
    particleBest = position.clone();
    velocity = parentSwarm.getRandomInitialVel();
    this.parentSwarm = parentSwarm;
    particleBestVal = parentSwarm.getFitnessInterface().evaluateParticle(this);
  }
  
  public double[] getBestPos()
  {
    return particleBest;
  }
  
  public double[] getPosition()
  {
    return position;
  }
  
  public void update()
  {
    double rp = Math.random();
    double rg = Math.random();
    
    for ( int i = 0; i < parentSwarm.getDimensions(); i++ )
    {
      double v = velocity[i];
      double newvel = parentSwarm.getOmega()* v + 
            parentSwarm.getPhip() * rp * (particleBest[i] - position[i]) +
            parentSwarm.getPhig() * rg * ( parentSwarm.getSwarmBest()[i] - position[i] );
      velocity[i] = newvel;
    }
    
    for ( int i = 0; i < parentSwarm.getDimensions(); i++ )
    {
      double p = position[i];
      double newpos = p + velocity[i];
      position[i] = newpos;
    }
    
    double fitness= parentSwarm.getFitnessInterface().evaluateParticle(this);
    if ( fitness < particleBestVal )
    {
      particleBestVal = fitness;
      particleBest = position.clone();
    }
    
    if ( fitness < parentSwarm.getSwarmBestValue() )
    {
      parentSwarm.updateBestValueAndPosition(fitness, position);
    }
    
  }
  
  public String toString()
  {
    String retval = super.toString() + " : " ;
    for ( double d: position )
    {
      retval = retval + Double.toString(d) + " ";
    }
    return retval;
  }
  
}