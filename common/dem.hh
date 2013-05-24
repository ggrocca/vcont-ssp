#ifndef _DEM_H
#define _DEM_H

#include "float.h"

#include "grid.hh"
#include "demreader.hh"

class Dem : public Grid<double> {

public:

    double max;
    double min;

    Dem () {}

    Dem (unsigned w, unsigned h) : Grid<double> (w, h) {}

    Dem (DEMReader* dr) :
	Grid<double> (dr->width, dr->height),
	max (dr->max),
	min (dr->min)
    {
	for (unsigned i = 0; i < dr->width; i++)
	    for (unsigned j = 0; j < dr->height; j++)
		(*this)(Coord (i, j), BOUND) = dr->get_pixel (i, j);
    }

    Dem (Dem& dem) :
	Grid<double> (dem.width, dem.height),
	max (dem.max),
	min (dem.min)
    {
	for (unsigned i = 0; i < dem.width; i++)
	    for (unsigned j = 0; j < dem.height; j++)
		(*this)(Coord (i, j), BOUND) = dem (Coord (i, j));
    }

    Dem (Dem& dem, Coord a, Coord b) : 
	Grid<double> (b.x - a.x, b.y - a.y),
	max (-DBL_MAX),
	min (DBL_MAX)
    {
	if (!(a < b) ||
	    !a.is_inside (dem.width, dem.height) ||
	    !b.is_inside (dem.width, dem.height)) 
	    fprintf (stderr, "Dem::Dem(): Cropping constructor parameters problem.\n");
	
	for (unsigned i = 0; i < width; i++)
	    for (unsigned j = 0; j < height; j++)
	    {
		(*this)(Coord (i, j), BOUND) = dem (Coord (i + a.x, j + a.y));

		double v = (*this)(Coord (i, j));
		if (v < min)
		    min = v;
		if (v > max)
		    max = v;

	    }
    }

    double& operator() (Coord c, Access a = ABYSS)
    {
	return Grid<double>::operator()(c, a);
    }

};

#endif // _DEM_HH
