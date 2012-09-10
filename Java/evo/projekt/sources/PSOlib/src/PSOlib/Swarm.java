package PSOlib;

import java.util.*;

public class Swarm
{
  
  private int dimensions;
  private int population;
  
  private FitnessInterface fi;
  private UpdateCallback uc;
  
  private double lowerBoundaries[];
  private double upperBoundaries[];
  
  private double omega;
  private double phip;
  private double phig;
  private int iterations;
  
  private List<Particle> populationPool;
  private double[] swarmBestPosition;
  private double swarmBestValue;

  public double getOmega() { return omega; }
  public double getPhip() { return phip; }
  public double getPhig() { return phig; }
  public int getDimensions() { return dimensions; }
  public double[] getSwarmBest() { return swarmBestPosition; }
  public FitnessInterface getFitnessInterface() { return fi; }
  public double getSwarmBestValue() { return swarmBestValue; }

  public List<Particle> getPopulationPool() {
      return populationPool;
  }


  public double[] getRandomInitialPos()
  {
    double retval[] = new double[dimensions];
    for ( int i = 0; i < dimensions; i++ )
    {
      retval[i] = (Math.random()*(upperBoundaries[i]-lowerBoundaries[i])) + lowerBoundaries[i];
    }
    return retval;
  }
  
  public double[] getRandomInitialVel()
  {
    double retval[] = new double[dimensions];
    for ( int i = 0; i < dimensions; i++ )
    {
      retval[i] = (Math.random()
                  *(Math.abs(upperBoundaries[i]-lowerBoundaries[i])-(-Math.abs(upperBoundaries[i]-lowerBoundaries[i])))) 
                  +( -Math.abs(upperBoundaries[i]-lowerBoundaries[i]) );
    }
    return retval;
  }
      
  public void updateBestValueAndPosition(double newfit, double newpos[] )
  {
    swarmBestPosition = newpos.clone();
    swarmBestValue = newfit;
  }
  
  public Swarm( int dimensions, int population,
                FitnessInterface fi, UpdateCallback uc,
                double lowerBoundaries[], double upperBoundaries[],
                double omega, double phip, double phig, int iterations )
  {
    this.fi = fi;
    this.uc = uc;
    this.dimensions = dimensions;
    this.population = population;
    
    this.lowerBoundaries = lowerBoundaries.clone();
    this.upperBoundaries = upperBoundaries.clone();
    
    this.omega = omega;
    this.phip = phip;
    this.phig = phig;
    this.iterations = iterations;
    
    //pool initialization
    populationPool = new ArrayList<Particle>(population);
    Particle fone = new Particle(this);
    populationPool.add(fone);
    swarmBestPosition = fone.getPosition().clone();
    swarmBestValue = fi.evaluateParticle(fone);
    for ( int i = 1; i < population; i++ )
    {
      Particle toadd = new Particle(this);
      populationPool.add(toadd);
      double toaddval = fi.evaluateParticle(toadd);
      if ( toaddval < swarmBestValue )
      {
        swarmBestPosition = toadd.getPosition();
        swarmBestValue = toaddval;
      }
    }
    if ( uc != null )
        uc.poolUpdated(populationPool, 1);
  }
  
  public void step()
  {
    for ( Particle p: populationPool )
    {
      p.update();
    }
    if ( uc != null )
        uc.poolUpdated(populationPool, 1); 
  }
  
  public void printParticleInfo()
  {
    for ( Particle p: populationPool )
    {
      System.out.println(p);
    }
  }
  
  public static void main(String[] args)
  {
    FitnessInterface testingfi = new FitnessInterface()
      { 
            public double evaluateParticle(Particle particle) 
            {
              double retval = 0;
              double x = particle.getPosition()[0];
              double y = particle.getPosition()[1];
              retval =-x*y;
              return retval;
            } 
      };
    UpdateCallback testinguc = new UpdateCallback(){ public void poolUpdated(List<Particle> pool, int iteration){} };
    Swarm testf = new Swarm(2, 200, testingfi, testinguc, 
                            new double[]{0, 0}, new double[]{2.0, 2.0}, 0.1, 0.5 ,1.0 ,0);
    for ( int i = 0; i < 10000; i++ )
    {
    testf.step();
    }
    testf.printParticleInfo();
  }
}
