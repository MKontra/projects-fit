/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.PSOApplet.functions;

import org.jzy3d.maths.Range;
import org.jzy3d.plot3d.builder.Mapper;

/**
 *
 * @author Cukor
 */
public class MexicanHat implements ExampleFunctionInterface {

    public double evaluate(double x, double y) {
        double sigma = 10;
        return Math.exp( -(x*x+y*y) / sigma  )  *  Math.abs( Math.cos( 2 * Math.PI * ( x*x + y*y ) ) );
    }

    public Mapper getMapper() {
        return new Mapper(){
            public double f(double x, double y) {
                        return evaluate(x,y);
                    }
        };
    }

    public Range getRangeX() {
        return new Range(-.5, .5);
    }

    public Range getRangeY() {
        return new Range(-.5, .5);
    }

    public int getStepsX() {
        return 50;
    }

    public int getStepsY() {
        return 50;
    }

}
