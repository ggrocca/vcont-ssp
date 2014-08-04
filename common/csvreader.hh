#ifndef _CSVREADER_HH
#define _CSVREADER_HH

#include "grid.hh"
#include "track.hh"

class SwissSpotHeight {
public:
    Point p;
};

class CSVReader {
public:
    CSVReader (int width, int height,
	       double cellsize, double xllcorner, double yllcorner);
    
    void load (char* filename, std::vector<SwissSpotHeight>& points);
    void save (char* filename, std::vector<int>& spots,
	       Track* track, ScaleSpace* ssp);

private:
    void asc2img (Point a, Point* i);
    void img2asc (Point i, Point* a);

    int width, height;
    double xllcorner, yllcorner;
    double cellsize;
};


#endif // _CSVREADER_HH
