/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.PSOApplet.functions;

import java.util.HashMap;
import java.util.Set;

/**
 *
 * @author Cukor
 */
public class ExampleFunctions {
    HashMap<String, ExampleFunctionInterface> funcmap = new HashMap<String, ExampleFunctionInterface>();

    public ExampleFunctions()
    {
        funcmap.put("Mexican Hat", new MexicanHat());
        funcmap.put("Hills", new esincos());
        funcmap.put("sin(xx+yy)/(xx+yy)", new SinX2Y2hyp());
        //z= sin(x) * sin(x) + cos(y) * cos(y)
        //z=sin(x*2) + cos(y)*cos(y*3)+sin(y)
    }

    public Set<String> getFunctions()
    {
        return funcmap.keySet();
    }

    public ExampleFunctionInterface FuncForStr(String key)
    {
        return funcmap.get(key);
    }

}
