package edu.oregonstate.cartography.geometry;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.geom.Ellipse2D;
import java.awt.geom.Rectangle2D;

/**
 *
 * @author marstonb
 */
public class Point extends Geometry {

    private double x;
    private double y;
    private double z;

    public Point(double valueX, double valueY) {
        this.x = valueX;
        this.y = valueY;
    }
    
    public Point(double valueX, double valueY, double valueZ) {
        this.x = valueX;
        this.y = valueY;
        this.z = valueZ;
    }
    
    /**
     * @return the x
     */
    public double getX() {
        return x;
    }

    /**
     * @param x the x to set
     */
    public void setX(double x) {
        this.x = x;
    }

    /**
     * @return the y
     */
    public double getY() {
        return y;
    }

    /**
     * @param y the y to set
     */
    public void setY(double y) {
        this.y = y;
    }

     /**
     * @return the z
     */
    public double getZ() {
        return z;
    }

    /**
     * @param z the z to set
     */
    public void setZ(double z) {
        this.z = z;
    }
    
    @Override
    public String toString() {
        
        return getX() + " " + getY() + " " + getZ();
    }

    @Override
    public void paint(Graphics2D g2d) {
        //Sets color of points
        g2d.setColor(Color.BLUE);
        //Creates a double precision ellipse centered on (6,6)
        Ellipse2D.Double point = new Ellipse2D.Double(x - 3, y - 3, 6, 6);
        //Paints the point
        g2d.fill(point);

    }

    //Returns the bounding box of a 2D rectangle with height and width equal to 0
    @Override
    public Rectangle2D getBoundingBox() {
        Rectangle2D rectangle = new Rectangle2D.Double(x, y, 0, 0);
        return rectangle;
    }

    @Override
    public int hashCode() {
        int hash = 5;
        hash = 17 * hash + (int) (Double.doubleToLongBits(this.x) ^ (Double.doubleToLongBits(this.x) >>> 32));
        hash = 17 * hash + (int) (Double.doubleToLongBits(this.y) ^ (Double.doubleToLongBits(this.y) >>> 32));
        return hash;
    }

    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final Point other = (Point) obj;
        if (Double.doubleToLongBits(this.x) != Double.doubleToLongBits(other.x)) {
            return false;
        }
        if (Double.doubleToLongBits(this.y) != Double.doubleToLongBits(other.y)) {
            return false;
        }
        return true;
    }
 
}
