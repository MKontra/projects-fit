/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * GraphPanel.java
 *
 * Created on 26.4.2011, 8:15:25
 */

package org.PSOApplet;

import PSOlib.FitnessInterface;
import PSOlib.Particle;
import PSOlib.Swarm;
import PSOlib.UpdateCallback;
import java.awt.BorderLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.HashMap;
import java.util.List;
import javax.swing.Timer;
import org.PSOApplet.functions.ExampleFunctionInterface;
import org.jzy3d.chart.Chart;
import org.jzy3d.chart.controllers.mouse.ChartMouseController;
import org.jzy3d.colors.Color;
import org.jzy3d.colors.ColorMapper;
import org.jzy3d.colors.colormaps.ColorMapRainbow;
import org.jzy3d.maths.BoundingBox3d;
import org.jzy3d.maths.Coord3d;
import org.jzy3d.plot3d.builder.Builder;
import org.jzy3d.plot3d.builder.concrete.OrthonormalGrid;
import org.jzy3d.plot3d.primitives.Point;
import org.jzy3d.plot3d.primitives.Shape;
import org.jzy3d.plot3d.rendering.scene.Graph;
import org.jzy3d.plot3d.rendering.view.modes.ViewBoundMode;

/**
 *
 * @author Cukor
 */
public class GraphPanel extends javax.swing.JPanel implements UpdateCallback, ActionListener {

    /** Creates new form GraphPanel */
    private ExampleFunctionInterface current = null;
    private Chart graphui = new Chart("swing");
    private Swarm currswarm = null;
    private Timer updater;
    private HashMap<Particle, Point> partpoints;
    private HashMap<Particle, Point> oldpartpoints;
    public GraphPanel() {
        initComponents();
        ChartMouseController mouse = new ChartMouseController();
        graphui.addController(mouse);
        add((javax.swing.JComponent)graphui.getCanvas(), BorderLayout.CENTER);
    }

    public void setFunction(ExampleFunctionInterface efi)
    {
        if ( current != null )
        {
            graphui.getScene().setGraph(new Graph(graphui.getScene()));
        }
        current = efi;

        final Shape surface = (Shape)Builder.buildOrthonormal(new OrthonormalGrid(efi.getRangeX(),
                                                                                  efi.getStepsX(),
                                                                                  efi.getRangeY(),
                                                                                  efi.getStepsY()), efi.getMapper());
        graphui.getView().setBoundManual( new BoundingBox3d(surface.getBounds()) );
        surface.setColorMapper(new ColorMapper(new ColorMapRainbow(), surface.getBounds().getZmin(), surface.getBounds().getZmax()));
        surface.setFaceDisplayed(true);
        surface.setWireframeDisplayed(false);
        //surface.setWireframeColor(Color.BLACK);

        // Setup a colorbar for the surface object and add it to the scene
        graphui.getScene().getGraph().add(surface);
    }

    public void poolUpdated(List<Particle> pool, int iteration)
    {
        //throw new UnsupportedOperationException("Not supported yet.");
        System.out.println("poolUpdated");
        if ( currswarm == null )
        {
            //initiate pool
            partpoints = new HashMap<Particle, Point>();
            for ( Particle p: pool )
            {
                double pos[] = p.getPosition();
                Point np = new Point(new Coord3d(pos[0], pos[1], current.evaluate(pos[0], pos[1])), Color.BLACK );
                np.setWidth(3);
                partpoints.put(p, np);
                graphui.getScene().getGraph().add(np);
            }
        } else
        {
            for ( Particle p: pool )
            {
                double pos[] = p.getPosition();
                partpoints.get(p).setData( new Coord3d(pos[0], pos[1], current.evaluate(pos[0], pos[1])) );
            }
        }
        graphui.render();
    }

    public void actionPerformed(ActionEvent e)
    {
        if ( e.getSource() != updater )
            return;
        if ( currswarm != null )
        {
            System.out.println("updating swarm");
            currswarm.step();
        }
    }



    public void runPSO(int population, double omega, double phi_p, double phi_g, int delay)
    {
        FitnessInterface testingfi = new FitnessInterface()
          {
                public double evaluateParticle(Particle particle)
                {
                  double x = particle.getPosition()[0];
                  double y = particle.getPosition()[1];
                  return -current.evaluate(x, y);
                }
          };

        currswarm = null;
        Swarm tmp = new Swarm(2, population, testingfi, this,
                              new double[]{current.getRangeX().getMin(), current.getRangeY().getMin()},
                              new double[]{current.getRangeX().getMax(), current.getRangeY().getMax()},
                              omega, phi_p ,phi_g ,0);
       currswarm = tmp;
       updater = new Timer(delay, this);
       updater.start();
    }

    public void stopPSO()
    {
        if ( updater != null ) updater.stop();
        currswarm = null;
        for ( Particle p: partpoints.keySet() )
        {
            Point np = partpoints.get(p);
            graphui.getScene().getGraph().remove(np);
        }
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        setLayout(new java.awt.BorderLayout());
    }// </editor-fold>//GEN-END:initComponents

    public boolean isPSOrunning() {
        return currswarm != null;
    }

    public boolean pausePSO() {
        if (updater.isRunning())
        {
            updater.stop();
            return true;
        }
        else
        {
            updater.start();
            return false;
        }
    }


    // Variables declaration - do not modify//GEN-BEGIN:variables
    // End of variables declaration//GEN-END:variables

}
