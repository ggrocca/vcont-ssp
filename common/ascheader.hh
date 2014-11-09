#ifndef _ASCHEADER_HH
#define _ASCHEADER_HH

#include "demreader.hh"
#include "grid.hh"

class ASCHeader {

public:
    ASCHeader ();
    ASCHeader (ASCReader& ascr);
    ASCHeader (ASCHeader& asch);
    ASCHeader (ASCHeader& asch, Coord crop_llc, Coord crop_hrc);
    ASCHeader (char* name);
    ASCHeader (FILE* fp);
    // ASCHeader (int width, int height, double cellsize, double xllcorner, double yllcorner);
 
    void asc2img (Point a, Point* i);
    void img2asc (Point i, Point* a);

    Point asc2img (Point a);
    Point img2asc (Point i);

    int width, height;
    double xllcorner, yllcorner;
    double cellsize;
    double nodata_value;
};

#endif // _ASCHEADER_HH
