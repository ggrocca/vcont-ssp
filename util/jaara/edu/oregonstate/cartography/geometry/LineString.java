package edu.oregonstate.cartography.geometry;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.geom.GeneralPath;
import java.awt.geom.Rectangle2D;
import java.util.ArrayList;

public class LineString extends Geometry {

    //Stores Point objects in an ArrayList called points
    ArrayList<Point> points = new ArrayList<>();

    //Returns the number of points
    public int getNumPoints() {
        return points.size();
    }

    //Returns point at specified index
    public Point getPointN(int index) {
        return points.get(index);
    }
    
    public Point getFirstPoint() {
        return points.isEmpty() ? null : points.get(0);
    }
    
    public Point getLastPoint() {
        return points.isEmpty() ? null : points.get(points.size() - 1);
    }

    //Points of line field
    public ArrayList<Point> getPoints() {
        return points;
    }

    //Tests if first and last point are identical and returns true if they are
    //If true, line is closed
    public boolean isClosed() {

        Point point1 = points.get(0);
        Point point2 = points.get(points.size() - 1);

        return (point1.getX() == point2.getX() && point1.getY() == point2.getY());
        /*
         if (point1 == point2){
         return true;
         }
         return false;*/
    }

    //Appends a passed point to list of existing points
    public void addPoint(Point addedPoint) {
        points.add(addedPoint);
    }

    public void addPointIfDifferentFromLast(Point p) {
        int nbrPoints = getNumPoints();
        if (nbrPoints == 0) {
            this.addPoint(p);
        } else {
            Point lastPoint = getPointN(nbrPoints - 1);
            if (!lastPoint.equals(p)) {
                points.add(p);
            }
        }
    }

    //Calls the toString method of each point and concatenates returned strings 
    //to a single string and returns the concatenated string
    @Override
    public String toString() {
        String lineDesc = "Attributes:\n" + super.toString() + "\nPoints:" + "\n";

        for (Point point : points) {
// the variable point is a reference to a point in the variable arrayList
            String desc = point.toString();
// append desc to a String object that is created outside of this for loop
            lineDesc += desc + "\n";
        }
        return "";
    }

    @Override
    public void paint(Graphics2D g2d
    ) {
        //Sets color of lines
        g2d.setColor(Color.green);
        //Creates a GeneralPath shape
        GeneralPath.Double path = new GeneralPath.Double();

        //Retrieves the first point
        Point firstPoint = getPointN(0);
        //Pass x and y coordinates of the first point in path.moveTo()
        path.moveTo(firstPoint.getX(), firstPoint.getY());

        //Starts at index 1 and iterates over all remaining points, then 
        //calls path.lineTo
        for (int i = 1; i < points.size(); i++) {
            Point nextPoint = getPointN(i);
            path.lineTo(nextPoint.getX(), nextPoint.getY());

        }

        //Paints line
        g2d.draw(path);
    }

    @Override
    public Rectangle2D getBoundingBox() {

        //Returns null if points ArrayList does not have any points
        if (points.size() < 1) {
            return null;
        }

        //Retrieves bounding box of first point of line
        Point firstPoint = getPointN(0);
        //Assign bounding box to variable
        Rectangle2D bb = firstPoint.getBoundingBox();
        //Iterate over remaining points and retrieve bounding boxes
        for (Point point : points) {
            //creates new bounding box that includes previous points and current point
            bb = bb.createUnion(point.getBoundingBox());
        }
        return bb;
    }

    /**
     * Returns the length of this line.
     *
     * @return
     */
    public double getLength() {
        int nPoints = getNumPoints();
        if (nPoints < 2) {
            return Double.NaN;
        }
        double length = 0;
        Point p1 = getPointN(0);
        for (int i = 1; i < nPoints; i++) {
            Point p2 = getPointN(i);
            double dx = p1.getX() - p2.getX();
            double dy = p1.getY() - p2.getY();
            length += Math.hypot(dx, dy);
        }
        return length;
    }

    /**
     * Invert order of points.
     */
    public void invert() {
        int nPoints = getNumPoints();
        for (int i = 0; i < nPoints / 2; i++) {
            Point p1 = getPointN(i);
            Point p2 = getPointN(nPoints - i - 1);
            points.set(nPoints - i - 1, p1);
            points.set(i, p2);
        }
    }

    /**
     * Append line
     *
     * @param l2
     */
    public void append(LineString l2) {
        int nPoints = l2.getNumPoints();
        for (int i = 0; i < nPoints; i++) {
            Point p = l2.getPointN(i);
            addPointIfDifferentFromLast(p);
        }
    }

}
