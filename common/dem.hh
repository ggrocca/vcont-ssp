#ifndef _DEM_HH
#define _DEM_HH

#include <float.h>

#include "debug.h"
#include "grid.hh"
#include "demreader.hh"
#include "scaletypes.hh"
#include "gaussian.hh"


class Dem : public Grid<double> {

public:

    double max;
    double min;

    Dem ();                                               // empty dem, no allocation
    Dem (int w, int h);                                  // empty dem, allocate w * h
    Dem (DEMReader* dr);                                 // create dem from demreader
    Dem (Dem& dem);                                                       // copy dem
    Dem (Dem& dem, Coord a, Coord b);                              // crop & copy dem
    Dem (Dem& dem, double amp, unsigned int seed);              // copy & perturb dem
    Dem (Dem& dem, TGaussianBlur<double>& BlurFilter, int window);        // blur dem
    Dem (Dem& dem, TGaussianBlur<double>& BlurFilter, int window,
	 std::vector<CriticalPoint>& crits);          // blur and controlled topology
    Dem (FILE* fp);
    
    void write (FILE* fp);

    double& operator() (Coord c, AccessType a = ABYSS);    // read & write with coord
    double& operator() (int x, int y, AccessType a = ABYSS); // read & write with x,y

    void clip_background (double v);            // values < v are clipped to -DBL_MAX
    void clip_background (Dem& dem, double v);     // clip values that are < v in dem
    bool is_clip (Coord c);                               // is this a clipped value?
    bool is_clip (int x, int y);

    bool is_equal (Coord a, Coord b);                // given points have equal value
    bool has_plateus ();             // this dem has adjacent points with equal value

    RelationType point_relation (Coord a, Coord b);  // relation between adyacent a,b
    CriticalType point_type (Coord c);                       // classify point, coord
    CriticalType point_type (int x, int y);                    // classify point, x,y

    void point_print (Coord c);                       // print point and neighborhood
    void point_print (int scope, Coord c);         // print p. & n.  with debug scope
    void point_print (Coord c, CriticalType type);             // print p., n. & type
    void point_print (int scope, Coord c, CriticalType type);                  // ...
};

#endif // _DEM_HH
