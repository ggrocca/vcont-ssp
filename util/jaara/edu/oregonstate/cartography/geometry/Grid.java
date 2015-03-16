/*
 * Grid.java
 * Created on January 14, 2014
 * Original author: Bernhard Jenny, Institute of Cartography, ETH Zurich.
 */
package edu.oregonstate.cartography.geometry;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.StringTokenizer;

/**
 * The Grid class models regularly spaced values, for example a digital
 * elevation model.
 *
 * @author Brooke Marston, Oregon State University
 */
public class Grid {

    /**
     * The size between two neighboring columns or rows.
     */
    private double cellSize;
    /**
     * The grid values.
     */
    private float[][] grid;
    /**
     * horizontal coordinate of west border
     */
    private double west = 0;
    /**
     * vertical coordinate of south border
     */
    private double south = 0;

    /**
     * Creates a new instance of Grid.
     *
     * @param cols The number of vertical columns in the new grid.
     * @param rows The number of horizontal rows in the new grid.
     */
    public Grid(int cols, int rows) {
        // the grid must contain at least 2 x 2 cells.
        if (cols < 3 || rows < 3) {
            throw new IllegalArgumentException("Not enough data points.");
        }

        this.cellSize = 1;
        this.grid = new float[rows][cols];
    }

    /**
     * Creates a new instance of Grid.
     *
     * @param cols The number of vertical columns in the new grid.
     * @param rows The number of horizontal rows in the new grid.
     * @param cellSize The size between two rows or columns.
     */
    public Grid(int cols, int rows, double cellSize) {
        // the grid must contain at least 2 x 2 cells.
        if (cols < 3 || rows < 3) {
            throw new IllegalArgumentException("Not enough data points.");
        }

        this.cellSize = cellSize;
        this.grid = new float[rows][cols];
    }

    /**
     * Read a Grid from a file in ESRI ASCII format.
     *
     * @param filePath A path to the file to be read.
     * @throws java.io.IOException Throws IOException if the grid cannot be
     * read.
     */
    public Grid(String filePath) throws IOException {
        float noDataValue = 0.f;

        BufferedReader reader = null;
        int cols = 0;
        int rows = 0;
        try {
            File file = new File(filePath);
            reader = new BufferedReader(
                    new InputStreamReader(new FileInputStream(file)));

            while (true) {
                String line = reader.readLine();
                if (line == null) {
                    break;
                }
                StringTokenizer tokenizer = new StringTokenizer(line, " \t");
                String str = tokenizer.nextToken().trim().toLowerCase();

                if (str.equals("ncols")) {
                    cols = Integer.parseInt(tokenizer.nextToken());
                } else if (str.equals("nrows")) {
                    rows = Integer.parseInt(tokenizer.nextToken());
                } else if (str.equals("xllcenter")) {
                    this.west = Double.parseDouble(tokenizer.nextToken());
                } else if (str.equals("xllcorner")) {
                    this.west = Double.parseDouble(tokenizer.nextToken());
                } else if (str.equals("yllcenter")) {
                    this.south = Double.parseDouble(tokenizer.nextToken());
                } else if (str.equals("yllcorner")) {
                    this.south = Double.parseDouble(tokenizer.nextToken());
                } else if (str.equals("cellsize")) {
                    this.cellSize = Float.parseFloat(tokenizer.nextToken());
                } else if (str.equals("nodata_value")) {
                    noDataValue = Float.parseFloat(tokenizer.nextToken());
                    break;
                }
            }

            // test if valid values have been found
            if (cols <= 0 || rows <= 0 || cellSize <= 0.f) {
                throw new java.io.IOException();
            }

            // create a grid of float values
            grid = new float[rows][cols];

            // Read values of the grid. 
            // Rows are stored from top to bottom in the file.
            for (int row = 0; row < rows; row++) {

                String line = reader.readLine();
                StringTokenizer tokenizer = new StringTokenizer(line, " ");

                for (int col = 0; col < cols; col++) {

                    float v = Float.parseFloat(tokenizer.nextToken());
                    if (v == noDataValue) {
                        setValue(Float.NaN, col, row);
                    } else {
                        setValue(v, col, row);
                    }
                }
            }

        } finally {
            if (reader != null) {
                try {
                    reader.close();
                } catch (Exception e) {
                }
            }
        }
    }

    /**
     * Returns the value at a specific position in the grid.
     *
     * @param col The vertical column for which a value is returned.
     * @param row The horizontal row for which a value is returned.
     * @return The value at the specified position.
     */
    public final float getValue(int col, int row) {
        return grid[row][col];
    }

    public float getValue(double x, double y) {
        //Returns column of point
        int col = (int) Math.round((x - getWest()) / cellSize);

        //Returns row of point
        int row = (int) Math.round((getNorth() - y) / cellSize);
        return getValue(col, row);
    }

    /**
     * Sets a value in the grid.
     *
     * @param value The value to store in the grid.
     * @param col The vertical column for which a value is set.
     * @param row The horizontal row for which a value is set.
     */
    public final void setValue(float value, int col, int row) {
        grid[row][col] = value;
    }

    /**
     * Sets a value in the grid.
     *
     * @param value The value to store in the grid. The value is cast to a
     * float.
     * @param col The vertical column for which a value must be set.
     * @param row The horizontal row for which a value must be set.
     */
    public void setValue(double value, int col, int row) {
        grid[row][col] = (float) value;
    }

    /**
     * Returns the minimum and the maximum value in the grid.
     *
     * @return Returns an array with two elements. The first element is the
     * minimum value in the grid, the second value is the maximum value in the
     * grid.
     */
    public float[] getMinMax() {
        int cols = this.getCols();
        int rows = this.getRows();
        float min = Float.MAX_VALUE;
        float max = -Float.MAX_VALUE;
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                if (grid[r][c] < min) {
                    min = grid[r][c];
                }
                if (grid[r][c] > max) {
                    max = grid[r][c];
                }
            }
        }
        return new float[]{min, max};
    }

    /**
     * Returns the number of columns in the grid.
     *
     * @return The number of columns in the grid.
     */
    public int getCols() {
        return grid[0].length;
    }

    /**
     * Returns the number of rows in the grid.
     *
     * @return The number of rows in the grid.
     */
    public int getRows() {
        return grid.length;
    }

    /**
     * Returns the distance between two neighboring rows or columns-
     *
     * @return The distance between two rows or columns.
     */
    public double getCellSize() {
        return cellSize;
    }

    protected float[][] getGrid() {
        return grid;
    }

    public boolean hasSameExtensionAndResolution(Grid grid) {
        if (grid == null) {
            return false;
        }
        return getCols() == grid.getCols()
                && getRows() == grid.getRows()
                // FIXME
                //&& getWest() == grid.getWest()
                //&& getNorth() == grid.getNorth()
                && getCellSize() == grid.getCellSize();

    }

    @Override
    public Grid clone() {
        final int nRows = getRows();
        final int nCols = getCols();
        Grid clone = new Grid(nCols, nRows, cellSize);

        // deep clone of grid
        for (int row = 0; row < nRows; row++) {
            System.arraycopy(grid[row], 0, clone.grid[row], 0, nCols);
        }
        return clone;
    }

    @Override
    public String toString() {
        float[] minMax = getMinMax();
        return "Grid: rows:" + getRows() + " cols:" + getCols() + " range:" + minMax[0] + " to " + minMax[1];
    }

    /**
     * @return the west
     */
    protected double getWest() {
        return west;
    }

    /**
     * @param west the west to set
     */
    protected void setWest(double west) {
        this.west = west;
    }

    /**
     * @return the south
     */
    protected double getSouth() {
        return south;
    }

    /**
     * @param south the south to set
     */
    protected void setSouth(double south) {
        this.south = south;
    }

    protected double getWidth() {
        return getCols() * cellSize;
    }

    protected double getHeight() {
        return getRows() * cellSize;
    }

    protected double getNorth() {
        return getHeight() + south;
    }

    /**
     * compute a bilinear interpolation.
     *
     * @param h1 value bottom left
     * @param h2 value bottom right
     * @param h3 value top left
     * @param h4 value top right
     * @param relX relative horizontal coordinate (0 .. 1) counted from left to
     * right
     * @param relY relative vertical coordinate (0 .. 1) counted from bottom to
     * top
     * @return The interpolated value
     */
    private static float bilinearInterpolation(float h1, float h2, float h3, float h4, double relX, double relY) {
        return (float) (h1 + (h2 - h1) * relX + (h3 - h1) * relY + (h1 - h2 - h3 + h4) * relX * relY);
    }

    /**
     * Bilinear interpolation. See
     * http://www.geovista.psu.edu/sites/geocomp99/Gc99/082/gc_082.htm "What's
     * the point? Interpolation and extrapolation with a regular grid DEM"
     */
    public final float getBilinearInterpol(double x, double y) {

        float h1, h2, h3, h4;
        // column and row of the top left corner
        int col = (int) ((x - this.west) / this.cellSize);
        int row = (int) ((this.getNorth() - y) / this.cellSize);

        if (col < 0 || col >= getCols() || row < 0 || row >= this.getRows()) {
            return Float.NaN;
        }
        double relX = (x - this.west) / this.cellSize - col;
        double relY = (y - this.south) / this.cellSize - this.getRows() + row + 2;

        if (row + 1 < this.getRows()) {
            // value at bottom left corner
            h1 = this.getValue(col, row + 1);
            // value at bottom right corner
            h2 = col + 1 < this.getCols() ? this.getValue(col + 1, row + 1) : Float.NaN;
        } else {
            h1 = Float.NaN;
            h2 = Float.NaN;
        }

        // value at top left corner
        h3 = this.getValue(col, row);

        // value at top right corner
        h4 = col + 1 < this.getCols() ? this.getValue(col + 1, row) : Float.NaN;

        // start with the optimistic case: all values are valid
        return Grid.bilinearInterpolation(h1, h2, h3, h4, relX, relY);
    }

    /**
     * Returns aspect angle in radians for a provided position.
     *
     * @param x
     * @param y
     * @return Angle in radians in clockwise direction. East is 0.
     */
    public double getAspect(double x, double y) {
        // an arbitrary offset to sample neighboring values
        final double d = this.cellSize / 5;

        final float w = this.getBilinearInterpol(x - d, y);
        final float e = this.getBilinearInterpol(x + d, y);
        final float s = this.getBilinearInterpol(x, y - d);
        final float n = this.getBilinearInterpol(x, y + d);
        return Math.atan2(n - s, e - w);
    }

}
