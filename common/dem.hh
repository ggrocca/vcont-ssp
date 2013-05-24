#ifndef _DEM_H
#define _DEM_H

#include "float.h"

#include "grid.hh"
#include "demreader.hh"

class Dem : public Grid<double> {

public:

    double max;
    double min;

    Dem ();                                            // empty dem, no allocation
    Dem (int w, int h);                                // empty dem, allocate w * h
    Dem (DEMReader* dr);                               // create dem from demreader
    Dem (Dem& dem);                                    // copy dem
    Dem (Dem& dem, Coord a, Coord b);                  // crop & copy dem
    Dem (Dem& dem, double amp, unsigned int seed);     // copy & perturb dem

    double& operator() (Coord c, Access a = ABYSS);       // read & write with coord
    double& operator() (int x, int y, Access a = ABYSS);  // read & write with x,y

};

#endif // _DEM_HH
