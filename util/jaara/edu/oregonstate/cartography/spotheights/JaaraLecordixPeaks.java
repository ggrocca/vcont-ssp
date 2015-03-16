package edu.oregonstate.cartography.spotheights;

import edu.oregonstate.cartography.geometry.Point;
import edu.oregonstate.cartography.grid.EsriASCIIGridReader;
import edu.oregonstate.cartography.grid.Grid;
import java.awt.Frame;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JOptionPane;

/**
 * Compute peaks with a method described in: Jaara, K., & Lecordix, F. (2011).
 * Extraction of cartographic contour lines using digital terrain model (DTM).
 * The Cartographic Journal, 48 (2), 131-137.
 *
 * @author Bernhard Jenny, Cartography and Geovisualization Group, Oregon State
 * University
 */
public class JaaraLecordixPeaks {

    /**
     * The dimension of squares. Expressed as number of grid cells.
     */
    static int squareSize = 15;

    /**
     * Finds the maximum elevation in a square that is squareSize x squareSiz large.
     *
     * @param grid The elevation grid.
     * @param squareVertical The vertical position of the square in the grid,
     * counted from top to bottom.
     * @param squareHorizontal The horizontal position of the square in the grid
     * counted from left to right.
     * @return A point with the horizontal xy location of the maximum elevation,
     * which is stored in z.
     */
    private static Point findMaxInSquare(Grid grid, int squareVertical, int squareHorizontal) {

        // top row of square
        int firstRow = squareVertical * squareSize;

        // left column of square
        int firstCol = squareHorizontal * squareSize;

	// last row
	int endRow;
	if (firstRow + squareSize > grid.getRows())
	    endRow = grid.getRows();
	else
	    endRow = firstRow + squareSize;

	// last col
	int endCol;
	if (firstCol + squareSize > grid.getCols())
	    endCol = grid.getCols();
	else
	    endCol = firstCol + squareSize;

        // the maximum elevation found
        float maxElevation = -Float.MAX_VALUE;

        // the vertical location of the maximum elevation relative to the square
        int maxRow = -1;

        // the horizontal location of the maximum elevation relative to the square
        int maxCol = -1;

        for (int r = firstRow; r < endRow; r++) {
            for (int c = firstCol; c < endCol; c++) {
                float v = grid.getValue(c, r);
                if (v > maxElevation) {
                    maxElevation = v;
                    maxRow = r;
                    maxCol = c;
                }
            }
        }

        // convert relative square coordinates to absolute units
        double x = grid.getWest() + grid.getCellSize() * maxCol;
        double y = grid.getNorth() - grid.getCellSize() * maxRow;

        return new Point(x, y, maxElevation);
    }

    /**
     * Extracts all peaks in a grid and prints the found peaks to the console.
     *
     * @param grid Grid with elevation values.
     */
    private static void extractPeaks(Grid grid) {

        // compute the number of  squares in horizontal and vertical direction
        int nbrSquaresH = grid.getCols() / squareSize;
        int nbrSquaresV = grid.getRows() / squareSize;

	// if the square size does not divide exactly our rows and cols add also the reminder
	if (grid.getCols() % squareSize != 0)
	    nbrSquaresH++;
	if (grid.getRows() % squareSize != 0)
	    nbrSquaresV++;
	
        for (int squareVer = 0; squareVer < nbrSquaresV; squareVer++) {
            for (int squareHor = 0; squareHor < nbrSquaresH; squareHor++) {
                // maximum in central square
                Point maxCenter = findMaxInSquare(grid, squareVer, squareHor);

                // compare to left square
                if (squareHor > 0) {
                    Point maxLeft = findMaxInSquare(grid, squareVer, squareHor - 1);
                    if (maxCenter.getZ() <= maxLeft.getZ()) {
                        continue;
                    }
                }

                // compare to right square
                if (squareHor < nbrSquaresH - 1) {
                    Point maxRight = findMaxInSquare(grid, squareVer, squareHor + 1);
                    if (maxCenter.getZ() <= maxRight.getZ()) {
                        continue;
                    }
                }

                // compare to square above
                if (squareVer > 0) {
                    Point maxTop = findMaxInSquare(grid, squareVer - 1, squareHor);
                    if (maxCenter.getZ() <= maxTop.getZ()) {
                        continue;
                    }
                }

                // compare to square below
                if (squareVer < nbrSquaresV - 1) {
                    Point maxBottom = findMaxInSquare(grid, squareVer + 1, squareHor);
                    if (maxCenter.getZ() <= maxBottom.getZ()) {
                        continue;
                    }
                }

                System.out.println(maxCenter);
            }
        }

    }

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        // java.awt.EventQueue.invokeLater(new Runnable() {
        //     @Override
        //     public void run() {
        //         try {
        //             // ask the user for a file
        //             String filePath = askFile("Select an Esri ASCII Grid", true);
        //             System.out.println(filePath);
        //             if (filePath == null) {
        //                 // user canceled
        //                 return;
        //             }
                    
        //             // ask user for square size
        //             String msg = "Size of squares (in number of grid cells, not meters)";
        //             String stringInput = JOptionPane.showInputDialog(msg);
        //             squareSize = Integer.parseInt(stringInput);

        //             Grid grid = EsriASCIIGridReader.read(filePath);
        //             extractPeaks(grid);
        //             System.exit(0);
        //         } catch (IOException ex) {
        //             Logger.getLogger(JaaraLecordixPeaks.class.getName()).log(Level.SEVERE, null, ex);
        //         }
        //     }
	//     });


	try {
	    if (args.length != 2)
	    {
		System.out.println("usage: ./jaara.sh input.asc SquareSize");
		System.exit(-1);
	    }
	    
	    // ask the user for a file
	    String filePath = args[0];
	    // System.out.println(filePath);
	    if (filePath == null) {
		// user canceled
		System.exit(-1);
	    }

	    String stringSquareSize = args[1];
	    // System.out.println(stringSquareSize);
	    squareSize = Integer.parseInt(stringSquareSize);

	    Grid grid = EsriASCIIGridReader.read(filePath);
	    extractPeaks(grid);
	    System.exit(0);
	} catch (IOException ex) {
	    Logger.getLogger(JaaraLecordixPeaks.class.getName()).log(Level.SEVERE, null, ex);
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
