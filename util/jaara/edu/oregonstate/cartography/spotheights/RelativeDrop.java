package edu.oregonstate.cartography.spotheights;

import edu.oregonstate.cartography.grid.EsriASCIIGridReader;
import edu.oregonstate.cartography.grid.Grid;
import java.awt.Frame;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashSet;
import java.util.Set;
import java.util.TreeSet;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JOptionPane;

/**
 * Compute peaks with a method described in: Wood, J. (2004). A new method for
 * the identification of peaks and summits in surface models. In Proceedings of
 * GISCience 2004---The Third International Conference on Geographic Information
 * Science, Adelphi, MD, pp. 227-230.
 *
 * @author Bernhard Jenny, Cartography and Geovisualization Group, Oregon State
 * University
 */
public class RelativeDrop {

    private static double minRelativeDrop = 10;
    private final Grid grid;

    /**
     * A cell encapsulates a cell position in a grid
     */
    public class Cell implements Comparable<Cell> {

        protected final double x;
        protected final double y;
        protected float z;
        private final int cellID;
        protected final int col;
        protected final int row;
        protected float relativeDrop;

        protected Cell(int col, int row, Grid grid) {
            this.col = col;
            this.row = row;
            this.cellID = col + row * grid.getCols();
            this.x = grid.getWest() + grid.getCellSize() * col;
            this.y = grid.getNorth() - grid.getCellSize() * row;
            this.z = grid.getValue(col, row);
            this.relativeDrop = 0f;
        }

        @Override
        public int compareTo(Cell o) {
            if (z < o.z) {
                return -1;
            } else if (z > o.z) {
                return 1;
            } else {
                return o.cellID - cellID;
            }
        }

        @Override
        public String toString() {
            return x + "\t" + y + "\t" + relativeDrop;
        }

        @Override
        public int hashCode() {
            return cellID;
        }

        @Override
        public boolean equals(Object o) {
            if (o instanceof Cell) {
                Cell other = (Cell) o;
                return cellID == other.cellID;
            }
            return false;
        }
    }

    /**
     * Compares cells based on their relativeDrop value
     */
    public static class CellRelativeDropComparator implements Comparator<Cell> {

        @Override
        public int compare(Cell o1, Cell o2) {
            if (o1.relativeDrop > o2.relativeDrop) {
                return -1;
            } else if (o1.relativeDrop < o2.relativeDrop) {
                return 1;
            } else {
                return 0;
            }
        }
    }

    public RelativeDrop(Grid grid) {
        this.grid = grid;
    }

    /**
     * Returns true if the passed cell is on one of the border rows or columns
     * of the grid.
     *
     * @param c
     * @return
     */
    private boolean isOnEdge(Cell c) {
        return c.col == 0
                || c.row == 0
                || c.col == grid.getCols() - 1
                || c.row == grid.getRows() - 1;
    }

    /**
     * Adds a cell to the passed list if the cell is not contained in
     * visitedCells
     *
     * @param list The list to add the cell to.
     * @param visitedCells A list with visited cells.
     * @param col The column of the new cell that may be added to list
     * @param row The row of the new cell that may be added to list
     */
    private void addUnvisitedCellToList(TreeSet<Cell> list, Set<Cell> visitedCells, int col, int row) {
        Cell cell = new Cell(col, row, grid);
        if (!visitedCells.contains(cell)
                && col >= 0 && row >= 0
                && col < grid.getCols()
                && row < grid.getRows()) {
            list.add(cell);
        }
    }

    /**
     * Add the 8 cells around a central cell at col/row. Only adds cells that
     * are not contained in visitedCells
     *
     * @param list List to add cells to.
     * @param visitedCells Only add cells to list if they are not contained in
     * visitedCells
     * @param cell The central cell
     */
    private void addNeighborsToList(TreeSet<Cell> list, Set<Cell> visitedCells, Cell cell) {
        int col = cell.col;
        int row = cell.row;
        addUnvisitedCellToList(list, visitedCells, col - 1, row - 1);
        addUnvisitedCellToList(list, visitedCells, col, row - 1);
        addUnvisitedCellToList(list, visitedCells, col + 1, row - 1);
        addUnvisitedCellToList(list, visitedCells, col - 1, row);
        addUnvisitedCellToList(list, visitedCells, col + 1, row);
        addUnvisitedCellToList(list, visitedCells, col - 1, row + 1);
        addUnvisitedCellToList(list, visitedCells, col, row + 1);
        addUnvisitedCellToList(list, visitedCells, col + 1, row + 1);
    }

    /**
     * Returns true if all 8 neighbors of a cell have the same elevation as the
     * cell itself.
     *
     * @param cell
     * @return
     */
    private boolean isPlane(Cell cell) {
        if (isOnEdge(cell)) {
            return false;
        }
        float z = grid.getValue(cell.col, cell.row);
        return (z == grid.getValue(cell.col - 1, cell.row + 1)
                && z == grid.getValue(cell.col, cell.row + 1)
                && z == grid.getValue(cell.col + 1, cell.row + 1)
                && z == grid.getValue(cell.col - 1, cell.row)
                && z == grid.getValue(cell.col + 1, cell.row)
                && z == grid.getValue(cell.col - 1, cell.row - 1)
                && z == grid.getValue(cell.col, cell.row - 1)
                && z == grid.getValue(cell.col + 1, cell.row - 1));
    }

    /**
     * Returns true if the passed cell is a peak or has one or more neighbors
     * with the same elevation.
     *
     * @param cell
     * @return
     */
    private boolean isLocalPeak(Cell cell) {
        if (isOnEdge(cell)) {
            return false;
        }
        float z = grid.getValue(cell.col, cell.row);
        return (z >= grid.getValue(cell.col - 1, cell.row + 1)
                && z >= grid.getValue(cell.col, cell.row + 1)
                && z >= grid.getValue(cell.col + 1, cell.row + 1)
                && z >= grid.getValue(cell.col - 1, cell.row)
                && z >= grid.getValue(cell.col + 1, cell.row)
                && z >= grid.getValue(cell.col - 1, cell.row - 1)
                && z >= grid.getValue(cell.col, cell.row - 1)
                && z >= grid.getValue(cell.col + 1, cell.row - 1));
    }

    /**
     * Extracts all peaks in a grid and prints the found peaks to the console.
     *
     * @param grid Grid with elevation values.
     */
    private ArrayList<Cell> extractPeaks() {

        // list with found peaks
        ArrayList<Cell> peaks = new ArrayList<>();

        // a temporary list with cells ordered by elevation
        // this is the peak area around the current peak
        // this list is cleared for each local peak
        TreeSet<Cell> orderedList = new TreeSet<>();

        // a temporary set with previously found highest neighbors in the peak area
        // this set is cleared for each local peak
        HashSet<Cell> visitedCells = new HashSet<>();

        int nCols = grid.getCols();
        int nRows = grid.getRows();
        for (int r = 0; r < nRows; r++) {
            for (int c = 0; c < nCols; c++) {

                // search for peaks
                Cell cell = new Cell(c, r, grid);
                if (!isLocalPeak(cell) || isPlane(cell)) {
                    continue;
                }
                peaks.add(cell);

                // the rest computes the relative drop value for the found peak.
                visitedCells.clear();
                orderedList.clear();
                orderedList.add(cell);
                float peakElevation = grid.getValue(c, r);
                do {
                    // remove the cell with the highest elevation from orderedList
                    Cell highestBorderCell = orderedList.pollLast();

                    // store this cell
                    visitedCells.add(highestBorderCell);

                    // compute the maximum drop (between the start cell and the 
                    // highest cell)
                    float dz = cell.z - highestBorderCell.z;
                    cell.relativeDrop = Math.max(cell.relativeDrop, dz);

                    // extend the peak area around the highest cell
                    addNeighborsToList(orderedList, visitedCells, highestBorderCell);
                } while (!orderedList.isEmpty()
                        // stop when the highest border point of the peak area
                        // is on the edge of the grid
                        && !isOnEdge(orderedList.last())
                        // stop when the found cell has a higher elevation than the start cell
                        && orderedList.last().z <= peakElevation);
            }
        }

        return peaks;
    }

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        // java.awt.EventQueue.invokeLater(new Runnable() {

        //     @Override
        //     public void run() {
        //         try {
        //             System.out.println("Attention: When neighboring cells have "
        //                     + "the same elevation, this implementation creates a "
        //                     + "peak for each cell (except when all 8 neighbors "
        //                     + "have the sam elevation).");
                    
        //             // ask the user for a file
        //             String filePath = askFile("Select an Esri ASCII Grid", true);
        //             //System.out.println(filePath);
        //             if (filePath == null) {
        //                 // user canceled
        //                 return;
        //             }
        //             //String filePath = System.getProperty("user.dir") + File.separator + "data" + File.separator + "Wood5x5.asc";

        //             //ask user for minimum relative drop
        //             String msg = "Minimum relative drop (in elevation units, usually meters)";
        //             String stringInput = JOptionPane.showInputDialog(msg);
        //             minRelativeDrop = Double.parseDouble(stringInput);

        //             Grid grid = EsriASCIIGridReader.read(filePath);
        //             ArrayList<Cell> peaks = new RelativeDrop(grid).extractPeaks();
        //             System.out.println("# peaks: " + peaks.size());
        //             Collections.sort(peaks, new CellRelativeDropComparator());

        //             // print results to console
        //             // header line
        //             System.out.println("ID\tx\ty\trelative_drop");
        //             int counter = 0;
        //             for (Cell cell : peaks) {
        //                 if (cell.relativeDrop >= minRelativeDrop) {
        //                     counter++;
        //                     System.out.println(counter + "\t" + cell.toString());
        //                 }
        //             }

        //             System.exit(0);
        //         } catch (IOException ex) {
        //             Logger.getLogger(RelativeDrop.class.getName()).log(Level.SEVERE, null, ex);
        //         }
        //     }
        // });


	try {
	    if (args.length != 2)
	    {
		System.out.println("usage: ./drop.sh input.asc RelativeDrop");
		System.exit(-1);
	    }

	    // System.out.println("Attention: When neighboring cells have "
	    // 		       + "the same elevation, this implementation creates a "
	    // 		       + "peak for each cell (except when all 8 neighbors "
	    // 		       + "have the sam elevation).");
                    
	    // ask the user for a file
	    String filePath = args[0]; //askFile("Select an Esri ASCII Grid", true);
	    //System.out.println(filePath);
	    if (filePath == null) {
		// user canceled
		System.exit(-1);
	    }
	    //String filePath = System.getProperty("user.dir") + File.separator + "data" + File.separator + "Wood5x5.asc";

	    //ask user for minimum relative drop
	    //String msg = "Minimum relative drop (in elevation units, usually meters)";
	    String stringInput = args[1]; //JOptionPane.showInputDialog(msg);
	    minRelativeDrop = Double.parseDouble(stringInput);

	    Grid grid = EsriASCIIGridReader.read(filePath);
	    ArrayList<Cell> peaks = new RelativeDrop(grid).extractPeaks();
	    // System.out.println("# peaks: " + peaks.size());
	    Collections.sort(peaks, new CellRelativeDropComparator());

	    // print results to console
	    // header line
	    System.out.println("ID\tx\ty\trelative_drop");
	    int counter = 0;
	    for (Cell cell : peaks) {
		if (cell.relativeDrop >= minRelativeDrop) {
		    counter++;
		    System.out.println(counter + "\t" + cell.toString());
		}
	    }

	    System.exit(0);
	} catch (IOException ex) {
	    Logger.getLogger(RelativeDrop.class.getName()).log(Level.SEVERE, null, ex);
	    System.exit(-1);
	}
	System.exit(0);
    }

    /**
     * Ask the user for a file to read or write.
     *
     * @param frame A Frame for which to display the dialog.
     * @param message A message that will be displayed in the dialog.
     * @param load Pass true if an existing file for reading should be selected.
     * Pass false if a new file for writing should be specified.
     * @return A path to the file, including the file name.
     */
    private static String askFile(String message, boolean load) {
        int flag = load ? java.awt.FileDialog.LOAD : java.awt.FileDialog.SAVE;

        // display the dialog
        java.awt.FileDialog fd = new java.awt.FileDialog((Frame) null, message, flag);
        fd.setVisible(true);

        // construct the path to the file that the user selected.
        String fileName = fd.getFile();
        String directory = fd.getDirectory();
        if (fileName == null || directory == null) {
            return null;
        }
        return directory + fileName;
    }
}
