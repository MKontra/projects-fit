package PSOlib;

import java.util.List;

public interface UpdateCallback
{
  public void poolUpdated(List<Particle> pool, int iteration);
} 
