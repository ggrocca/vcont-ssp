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
	{
	    int v = dr->get_pixel (i, j);
	    (*this)(i, j, BOUND) =  (v == INT_MIN)? -DBL_MAX : (double) v;
	}
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

static inline double __random_value (double fmin, double fmax)
{
    double f = ((double) rand()) / RAND_MAX;
    return fmin + f * (fmax - fmin);
}

void __write_random (int i, int j, double o, double r, double p, bool end = false)
{
    static FILE* fp = NULL;

    if (end && fp != NULL)
    {
	fclose (fp);
	return;
    }

    if (fp == NULL)
	fp = fopen ("perturbed.txt", "w");
    
    fprintf (fp, "== [%d][%d] ==\n", i, j);
    fprintf (fp, "ORIG: %4.60lf\n", o);
    fprintf (fp, "RAND: %4.60lf\n", r);
    fprintf (fp, "PERT: %4.60lf\n", p);
    fprintf (fp, "-----------------------\n");
}

// copy & perturb dem
Dem::Dem (Dem& dem, double amp, unsigned int seed) :
    Grid<double> (dem.width, dem.height),
    max (-DBL_MAX),
    min (DBL_MAX)
{
    srand(seed);
    printf ("!!!!!!!!!!!!!!!!!!!!DIO PORCO SEED = %d\n", seed);

    for (int i = 0; i < width; i++)
	for (int j = 0; j < height; j++)
	{
	    double dv = dem (i, j);
	    double p = __random_value (0.0, amp);
	    // double p = __random_value (-amp, amp);
	    // dv = dv < amp * 2.0? amp * 2.0 : dv;
	    double pv = dv + p;

	    (*this)(i, j) = pv;

	    // __write_random (i, j, dv, p, pv);

	    if (pv < min)
		min = pv;
	    if (pv > max)
		max = pv;

	}
    // __write_random (0,0,0,0,0, true);
}

Dem::Dem (Dem& dem, TGaussianBlur<double>& BlurFilter, int window) : // window = 5
    Grid<double> (dem.width, dem.height),
    max (-DBL_MAX),
    min (DBL_MAX)
{
    BlurFilter.Filter(&(dem.data[0]), &((*this).data[0]), width, height, window);

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



double& Dem::operator() (Coord c, AccessType a) // = ABYSS
{
    return Grid<double>::operator()(c, a);
}

double& Dem::operator() (int x, int y, AccessType a) // = ABYSS
{
    return Grid<double>::operator()(Coord (x, y), a);
}


void Dem::clip_background (double v)
{
    clip_background (*this, v);
}



void Dem::clip_background (Dem& d, double v)
{
    if (width != d.width || height != d.height)
	eprint ("Mismatched width or height."
		" t->w: %d, t->h: %d, d->w: %d, d->h: %d",
		width, height, d.width, d.height);

    for (int i = 0; i < width; i++)
	for (int j = 0; j < height; j++)
	    if (d (i, j) < v)
		(*this)(i, j) = -DBL_MAX;
}

bool Dem::is_clip (Coord c)
{
    return is_clip (c.x, c.y);
}

bool Dem::is_clip (int x, int y)
{
    return (*this)(x, y) == -DBL_MAX;
}



bool Dem::is_equal (Coord a, Coord b)
{
    double av = (*this)(a);
    double bv = (*this)(b);

    if (av > bv)
    	return false;
    if (av < bv)
    	return false;

    return true;
}

bool Dem::has_plateus ()
{
    for (int i = 0; i < width; i++)
	for (int j = 0; j < height; j++)
	    for (int k = 0; k < 2; k++)
		for (int w = 0; w < 2; w++)
		    if ((k != 0 || w != 0) && is_equal (Coord(i, j), Coord(i+k, j+w)))
		    {
			tprintsp (SCOPE_PLATEUS, "----> PLATEUS:",
				  "[%d,%d]==[%d,%d]   __ %d,%d\n",
				  i, j, i+k, j+w, k, w);
			return true;
		    }
    return false;
}



RelationType Dem::point_relation (Coord a, Coord b)
{
    if (is_clip(a) && is_clip(b))
	return EQ;

    if (a.is_outside(width, height) || is_clip(a))
	return LT;

    if (b.is_outside(width, height) || is_clip(b))
	return GT;

    double av = (*this)(a);
    double bv = (*this)(b);

    if (av > bv)
    	return GT;
    if (av < bv)
    	return LT;

    // eprint ("Two points with same value: ax:%d, ay:%d, bx:%d, by:%d\n",
    // 	    a.x, a.y, b.x, b.y);

    if (a.x > b.x)
	return GT;
    if (a.x < b.x)
	return LT;

    if (a.y > b.y)
	return GT;
    if (a.y < b.y)
	return LT;

    eprintx (EXIT_RELATION,
	     "Returning impossible value EQ: ax:%d, ay:%d, bx:%d, by:%d\n",
	     a.x, a.y, b.x, b.y);

    return EQ;
}

CriticalType Dem::point_type (Coord c)
{
    if (c.is_outside (width, height))
	return MIN;

    if (is_clip (c))
	return REG;

    RelationType current,previous,first;
    int changes = 0;
    Coord nc;

    c.round_trip_6 (&nc);
    first = previous = point_relation (c, nc);

    for (int i = 0; i < 6; i++)
    {
	c.round_trip_6 (&nc);
	current = point_relation (c, nc);
	point_type_step (current, &previous, &changes);
    }

    CriticalType r = point_type_analyze (first, changes, c);
    point_print (SCOPE_POINTTYPE, c, r);

    return (r);
}

CriticalType Dem::point_type (int x, int y)
{
    return point_type (Coord (x, y));
}



double _pdm (double d)
{
    return (d == -DBL_MAX) ? -4242.0 : d;
}

void Dem::point_print (Coord c)
{
    point_print (SCOPE_ALWAYS, c);
}

void Dem::point_print (int scope, Coord c)
{
    int x = c.x;
    int y = c.y;

    oprints (scope, "--POINT %d %d  -----\n", x,y);
    oprints (scope, "  % 6.8lf % 6.8lf % 6.8lf\n"
	     "  % 6.8lf % 6.8lf % 6.8lf\n"
	     "  % 6.8lf % 6.8lf % 6.8lf\n",
	     _pdm((*this)(x-1, y+1)), _pdm((*this)(x  , y+1)), _pdm((*this)(x+1, y+1)),
	     _pdm((*this)(x-1, y  )), _pdm((*this)(x  , y  )), _pdm((*this)(x+1, y  )),
	     _pdm((*this)(x-1, y-1)), _pdm((*this)(x  , y-1)), _pdm((*this)(x+1, y-1))
	     );
    oprints (scope, "%s", "--------\n");
}

void Dem::point_print (Coord c, CriticalType type)
{
    point_print (SCOPE_ALWAYS, c, type);
}

void Dem::point_print (int scope, Coord c, CriticalType type)
{
    int x = c.x;
    int y = c.y;

    oprints (scope, "--POINT %d %d - %s -----\n", x,y, critical2string (type));
    point_print (scope, c);
}
