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
public interface ExampleFunctionInterface {
    public Range getRangeX();
    public Range getRangeY();
    public int getStepsX();
    public int getStepsY();

    public Mapper getMapper();
    public double evaluate(double x, double y);
}
