/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.PSOApplet.functions;

import org.jzy3d.maths.Range;
import org.jzy3d.plot3d.builder.Mapper;

import static java.lang.Math.*;

/**
 *
 * @author Cukor
 */
public class esincos implements ExampleFunctionInterface {

    public double evaluate(double x, double y) {
        return exp( -0.2*sqrt(pow(x,2)+pow(y,2)) + 3*(cos(2*x) + sin(2*y)) );
    }

    public Mapper getMapper() {
        return new Mapper(){
            public double f(double x, double y) {
                        return evaluate(x,y);
                    }
        };
    }

    public Range getRangeX() {
        return new Range(-10, 10);
    }

    public Range getRangeY() {
        return new Range(-10, 10);
    }

    public int getStepsX() {
        return 80;
    }

    public int getStepsY() {
        return 80;
    }

}
