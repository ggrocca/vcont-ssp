#ifndef _CSVREADER_HH
#define _CSVREADER_HH

#include "grid.hh"
#include "track.hh"

class SwissSpotHeight {
public:
    Point p;
    int oid;
    SwissSpotHeight () : p (0.0, 0.0), oid (0) {}
    SwissSpotHeight (Point p) : p (p), oid (0) {}
    SwissSpotHeight (Point p, int oid) : p (p), oid (oid) {}
};

class CSVReader {
public:
    CSVReader (int width, int height,
	       double cellsize, double xllcorner, double yllcorner);
    
    void load (char* filename, std::vector<SwissSpotHeight>& points);
    void save (char* filename, std::vector<SwissSpotHeight>& points);
    void save (char* filename, std::vector<int>& spots,
		     Track* track, ScaleSpace* ssp);

private:
    void asc2img (Point a, Point* i);
    void img2asc (Point i, Point* a);

    int width, height;
    double cellsize;
    double xllcorner, yllcorner;
};


#endif // _CSVREADER_HH
