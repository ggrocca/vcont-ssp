#include "dem.hh"

// empty dem, no allocation
Dem::Dem () : Grid<double> (),
	 max (-DBL_MAX),
	 min (DBL_MAX)
{}

// empty dem, allocate w * h
Dem::Dem (int w, int h) : Grid<double> (w, h),
		     max (-DBL_MAX),
		     min (DBL_MAX)
{}

// create dem from demreader
Dem::Dem (DEMReader* dr) : Grid<double> (dr->width, dr->height),
		      max (dr->max),
		      min (dr->min)
{
    for (unsigned i = 0; i < dr->width; i++)
	for (unsigned j = 0; j < dr->height; j++)
	    (*this)(i, j, BOUND) = dr->get_pixel (i, j);
}

// copy dem
Dem::Dem (Dem& dem) : Grid<double> (dem.width, dem.height),
		      max (dem.max),
		      min (dem.min)
{
    for (int i = 0; i < dem.width; i++)
	for (int j = 0; j < dem.height; j++)
	    (*this)(i, j, BOUND) = dem (i, j);
}


// crop & copy dem
Dem::Dem (Dem& dem, Coord a, Coord b) : Grid<double> (b.x - a.x, b.y - a.y),
					max (-DBL_MAX),
					min (DBL_MAX)
{
    if (!(a < b) ||
	!a.is_inside (dem.width, dem.height) ||
	!b.is_inside (dem.width, dem.height)) 
	eprint ("%s","Cropping constructor parameters problem.\n");
	
    for (int i = 0; i < width; i++)
	for (int j = 0; j < height; j++)
	{
	    (*this)(i, j, BOUND) = dem (Coord (i + a.x, j + a.y));

	    double v = (*this)(i, j);
	    if (v < min)
		min = v;
	    if (v > max)
		max = v;

	}
}

double __random_value (double fmin, double fmax)
{
    double f = ((double) rand()) / RAND_MAX;
    return fmin + f * (fmax - fmin);
}

// copy & perturb dem
Dem::Dem (Dem& dem, double amp, unsigned int seed) :
    Grid<double> (dem.width, dem.height),
    max (-DBL_MAX),
    min (DBL_MAX)
{
    srand(seed);

    for (int i = 0; i < width; i++)
	for (int j = 0; j < height; j++)
	{
	    double dv = dem (i, j);
	    double p = __random_value (-amp, amp);
	    dv = dv < amp * 2.0? amp * 2.0 : dv;
	    double pv = dv + p;

	    (*this)(i, j) = pv;

	    if (pv < min)
		min = pv;
	    if (pv > max)
		max = pv;

	}
	
}

Dem::Dem (Dem& dem, TGaussianBlur<double>& BlurFilter) :
    Grid<double> (dem.width, dem.height),
    max (-DBL_MAX),
    min (DBL_MAX)
{
    BlurFilter.Filter(&(dem.data[0]), &((*this).data[0]), width, height, 5);

    for (int i = 0; i < width; i++)
	for (int j = 0; j < height; j++)
	{
	    double v = (*this)(i, j);
	    if (v < min)
		min = v;
	    if (v > max)
		max = v;

	}

}


// read & write element with coord
double& Dem::operator() (Coord c, Access a) // = ABYSS
{
    return Grid<double>::operator()(c, a);
}

// read & write element with x,y
double& Dem::operator() (int x, int y, Access a) // = ABYSS
{
    return Grid<double>::operator()(Coord (x, y), a);
}
