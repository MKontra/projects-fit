package PSOlib;

public class pair<F, S>
{
  F f;
  S s;
  public pair ( F f, S s )
  {
    this.f = f;
    this.s = s;
  }
  
  public F first() { return f; }
  public S second() { return s; }
}
