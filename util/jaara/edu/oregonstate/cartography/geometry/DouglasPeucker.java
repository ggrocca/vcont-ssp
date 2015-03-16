package edu.oregonstate.cartography.geometry;

import java.util.ArrayList;

/**
 *
 * @author marstonb
 */
public class DouglasPeucker {

    /**
     * Attribute name for marking points that are retained by this simplification algorithm.
     */
    public static final String CRITICAL_POINT_ATTRIBUTE = "critical";

    private double tolerance = 0;

    public DouglasPeucker(double tolerance) {
        this.tolerance = tolerance;
    }

    /**
     * @param tolerance the tolerance to set
     */
    public void setTolerance(double tolerance) {
        this.tolerance = tolerance;
    }

    public GeometryCollection simplify(GeometryCollection lines) {
        GeometryCollection simplifiedLines = new GeometryCollection();
        int nLines = lines.getNumGeometries();
        for (int lineIdx = 0; lineIdx < nLines; lineIdx++) {
            LineString line = (LineString) lines.getGeometryN(lineIdx);
            simplifiedLines.addGeometry(tolerance > 0 ? simplify(line) : line);
        }
        return simplifiedLines;
    }

    /**
     * @param line the line to simplify
     * @return
     */
    public LineString simplify(LineString line) {

        int numberPoints = line.getNumPoints();
        int lastPoint;

        //Tests if line to be simplified is closed and decides which point is last
        //based on if it is closed or not
        if (line.isClosed()) {
            lastPoint = numberPoints - 2;

        } else {
            lastPoint = numberPoints - 1;
        }

        //Create boolean array
        Boolean[] lineArray = new Boolean[numberPoints];

        //Iterate over array and set first and last values to true, all else are false
        lineArray[0] = lineArray[numberPoints - 1] = true;
        for (int i = 1; i < numberPoints - 1; i++) {
            lineArray[i] = false;
        }

        simplifyLineSection(line.getPoints(), lineArray, 0, lastPoint);

        LineString simplifiedLineString = new LineString();

        //Iterate over array and include points of original line in new LineString
        // also set flag for points in original line
        for (int i = 0; i < numberPoints; i++) {
            if (lineArray[i] == true) {
                simplifiedLineString.addPoint(line.getPointN(i));
            }

            // set flag for points of original line
            line.getPointN(i).setAttribute(CRITICAL_POINT_ATTRIBUTE, lineArray[i] ? 1 : 0);
        }
        return simplifiedLineString;
    }

    //Computes absolute value for unsigned distance
    private static double compDist(Point pointStart, Point pointEnd, Point point) {
        double x0 = pointStart.getX();
        double x1 = pointEnd.getX();
        double y0 = pointStart.getY();
        double y1 = pointEnd.getY();
        double x2 = point.getX();
        double y2 = point.getY();

        double d = Math.abs((((y0 - y1) * x2) + ((x1 - x0) * y2) + ((x0 * y1) - (x1 * y0))) / Math.hypot(x1 - x0, y1 - y0));
        return d;
    }

    private void simplifyLineSection(ArrayList<Point> line, Boolean[] marks, int firstIdx, int lastIdx) {
        double maxDist = 0;
        int maxDistIndex = 0;
        double orthoDistance;
        //Get first index
        Point firstPoint = line.get(firstIdx);
        //Get last index
        Point lastPoint = line.get(lastIdx);

        //Iterate over points between first and last point of line and compute
        //distance to line that connects first and last point
        for (int i = firstIdx; i < lastIdx; i++) {
            Point midPoint = line.get(i);
            orthoDistance = compDist(firstPoint, lastPoint, midPoint);

            //Get largest orthogonal distance and index it
            if (orthoDistance > maxDist) {
                maxDist = orthoDistance;
                maxDistIndex = i;
            }
        }

        //Compare maximum distance to tolerance
        if (maxDist > tolerance) {
            marks[maxDistIndex] = true;
            //Simplify line section from first point to point with max distance
            simplifyLineSection(line, marks, firstIdx, maxDistIndex);
            //Simplify line section from point with max distance to last distance
            simplifyLineSection(line, marks, maxDistIndex, lastIdx);
        }
    }
}
