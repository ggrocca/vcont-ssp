#include "dem.hh"
#include "../scaletracker/flipper.hh"



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
	    // int v = dr->get_pixel (i, j);
	    // (*this)(i, j, BOUND) = (v == INT_MIN)? -DBL_MAX : (double) v;
	    (*this)(i, j, BOUND) = dr->get_pixel (i, j);
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
	!b.is_inside (dem.width+1, dem.height+1)) 
	eprint ("Cropping constructor parameters problem.\n"
		"\tw: %d, h: %d.  a(%d,%d) - b(%d,%d).\n"
		"\t!(a<b):%d, !a.is_inside():%d, !b.si_inside():%d\n",
		dem.width, dem.height, a.x, a.y, b.x, b.y,
		!(a < b),
		!a.is_inside (dem.width, dem.height),
		!b.is_inside (dem.width, dem.height));
	
    tprint ("CROP: width %d, height %d\n", b.x - a.x, b.y - a.y);

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

// void __write_random (int i, int j, double o, double r, double p, bool end = false)
// {
//     static FILE* fp = NULL;

//     if (end && fp != NULL)
//     {
// 	fclose (fp);
// 	return;
//     }

//     if (fp == NULL)
// 	fp = fopen ("perturbed.txt", "w");
    
//     fprintf (fp, "== [%d][%d] ==\n", i, j);
//     fprintf (fp, "ORIG: %4.60lf\n", o);
//     fprintf (fp, "RAND: %4.60lf\n", r);
//     fprintf (fp, "PERT: %4.60lf\n", p);
//     fprintf (fp, "-----------------------\n");
// }

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

Dem::Dem (Dem& dem, TGaussianBlur<double>& BlurFilter, int window) :
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

Dem::Dem (Dem& dem, TGaussianBlur<double>& BlurFilter, int window,
	  int filter_algo, std::vector<CriticalPoint>& crits) :
    Grid<double> (dem.width, dem.height),
    max (-DBL_MAX),
    min (DBL_MAX)
{
    BlurFilter.Filter(&(dem.data[0]), &((*this).data[0]), width, height, window);

    Flipper::filter (&dem, this, filter_algo, crits);

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

Dem::Dem (FILE* fp) : Grid<double> (), max (-DBL_MAX), min (DBL_MAX)
{
    int width, height;

    fread (&width, sizeof (int), 1, fp);
    fread (&height, sizeof (int), 1, fp);

    // int length = width * height;
    tprints (SCOPE_IO, "w: %d, h:%d\n", width, height);

    resize (width, height);
    fread (&(data[0]), sizeof (double), data.size(), fp);

    for (int i = 0; i < width; i++)
	for (int j = 0; j < height; j++)
	{
	    double v = (*this)(i, j);
	    if (v < min)
		min = v;
	    if (v > max)
		max = v;

	}
    tprints (SCOPE_IO, "min: %le, max:%le\n", min, max);
}
    
void Dem::write (FILE* fp)
{
    fwrite (&width, sizeof (int), 1, fp);
    fwrite (&height, sizeof (int), 1, fp);
    fwrite (&(data[0]), sizeof (double), data.size(), fp);
}


double& Dem::operator() (Coord c, AccessType a) // = ABYSS
{
    return Grid<double>::operator()(c, a);
}

double& Dem::operator() (int x, int y, AccessType a) // = ABYSS
{
    return Grid<double>::operator()(x, y, a);
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

bool Dem::_iterative_label (Grid<int>* idplats, Coord c, unsigned label,
			    std::vector < std::vector <Coord> >& plateaus_list)
{
    if ((*idplats)(c) != -1)
	return false;

    bool found_plats = false;
    std::vector <Coord>* vnc = new std::vector <Coord>(0);
    c.append_6 (*vnc);

    for (unsigned i = 0; i < vnc->size(); i++)
    {
	Coord nc = (*vnc)[i];

	if (is_equal (c, nc) && (*idplats)(nc) == -1)
	{
	    if (plateaus_list.size() == label)
	    {
		found_plats = true;
		plateaus_list.push_back (std::vector <Coord> (0));
		plateaus_list[label].push_back (c);
		(*idplats)(c) = label;
	    }
	    // else if (plateaus_list.size() != label+1)
	    // 	eprintx (-24, "label(%d) and size(%zu) mismatch.\n",
	    // 		 label, plateaus_list.size());
	    
	    plateaus_list[label].push_back (nc);
	    (*idplats)(nc) = label;
	    nc.append_6 (*vnc);
	}
	// else
	// {
	//    vnc->erase(vnc->begin() + i);
	//    i--;
	// }
    }

    delete vnc;
    return found_plats;
}

bool Dem::_recursive_label (Grid<int>* idplats, Coord c, unsigned label,
			    std::vector < std::vector <Coord> >& plateaus_list)
{
    if ((*idplats)(c) != -1)
	return false;

    bool found_plats = false;
    std::vector <Coord> ac;
    c.neigh_6 (ac);
    for (int k = 0; k < 6; k++)
    {
	//c.round_trip_6 (&nc);
	Coord nc = ac[k];

	if (is_equal (c, nc))
	{
	    if (plateaus_list.size() == label)
		plateaus_list.push_back (std::vector <Coord> (0));
	    else if (plateaus_list.size() != label+1)
		eprintx (-24, "label(%d) and size(%zu) mismatch.\n",
			 label, plateaus_list.size());
		
	    if (found_plats == false)
	    {
		found_plats = true;
		plateaus_list[label].push_back (c);
		(*idplats)(c) = label;
	    }
	    
	    _recursive_label (idplats, nc, label, plateaus_list);
	}
    }

    return found_plats;
}

void Dem::identify_plateaus (std::vector < std::vector <Coord> >& plateaus_list)
{
    unsigned label = 0;
    plateaus_list.clear ();
    Grid<int>* idplats = new Grid<int> (width, height, -1);

    for (int i = 0; i < width; i++)
	for (int j = 0; j < height; j++)
	{
	    Coord c = Coord (i,j);
	    
	    if (is_clip (c))
	    	continue;
	    
	    if (_iterative_label (idplats, c, label, plateaus_list))
	    {
		label++;

		if (plateaus_list[label-1].size () <= 1)
		    eprintx (-1, "at c(%d,%d), label %d, size %zu\n",
			     i, j, label-1, plateaus_list[label-1].size ());
	    }
	}
    
    delete idplats;
}


bool Dem::has_plateaus ()
{
    for (int i = 0; i < width; i++)
	for (int j = 0; j < height; j++)
	{
	    Coord c,nc;
	    c = Coord (i,j);
	    
	    if (is_clip (c))
	    	continue;

	    for (int k = 0; k < 6; k++)
	    {
		c.round_trip_6 (&nc);
		if (is_equal (c, nc))
		{
		    printf ( "----> PLATEAUS: [%d,%d]==[%d,%d]. Values: %le,%le --- -DBL_MAX=%le\n",
			      c.x, c.y, nc.x, nc.y, (*this)(c), (*this)(nc), -DBL_MAX);
		    return true;
		}
	    }
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
    point_print (scope, c.x, c.y);
    // int x = c.x;
    // int y = c.y;

    // oprints (scope, "--POINT %d %d  -----\n", x,y);
    // oprints (scope, "  % 6.8lf % 6.8lf % 6.8lf\n"
    // 	     "  % 6.8lf % 6.8lf % 6.8lf\n"
    // 	     "  % 6.8lf % 6.8lf % 6.8lf\n",
    // 	     _pdm((*this)(x-1, y+1)), _pdm((*this)(x  , y+1)), _pdm((*this)(x+1, y+1)),
    // 	     _pdm((*this)(x-1, y  )), _pdm((*this)(x  , y  )), _pdm((*this)(x+1, y  )),
    // 	     _pdm((*this)(x-1, y-1)), _pdm((*this)(x  , y-1)), _pdm((*this)(x+1, y-1))
    // 	     );
    // oprints (scope, "%s", "--------\n");
}

void Dem::point_print (int scope, int x, int y)
{
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

Averhood Dem::averhood (CriticalPoint cp, int kernel, Averhood prev)
{
    Averhood next;
    next = Averhood ();
    next = kernel == 1? Averhood (cp.t, (*this)(cp.c)) : Averhood (prev);

    // printf ("K|%d|\n", kernel);
    for (int i = cp.c.x - kernel; i <= cp.c.x + kernel; i++)
    { // go on (i, cp.c.y - kernel) and (i, cp.c.y + kernel)
	Coord cn;
	cn = Coord (i, cp.c.y - kernel);
	// printf ("|XY-| (%d,%d) -- (%d,%d) -- (%d,%d)\n",
	// 	cn.x-cp.c.x,cn.y-cp.c.y,cn.x,cn.y,cp.c.x,cp.c.y);
	next.add (cn, (*this)(cn), width, height);
	cn = Coord (i, cp.c.y + kernel);
	// printf ("|XY+| (%d,%d) -- (%d,%d) -- (%d,%d)\n",
	// 	cn.x-cp.c.x,cn.y-cp.c.y,cn.x,cn.y,cp.c.x,cp.c.y);
	next.add (cn, (*this)(cn), width, height);
    }
    for (int j = cp.c.y - kernel + 1; j <= cp.c.y + kernel - 1; j++)
    { // go on (cp.c.x - kernel, j) and (cp.c.x + kernel, j)
	Coord cn;
	cn = Coord (cp.c.x - kernel, j);
	// printf ("|YX-| (%d,%d) -- (%d,%d) -- (%d,%d)\n",
	// 	cn.x-cp.c.x,cn.y-cp.c.y,cn.x,cn.y,cp.c.x,cp.c.y);
	next.add (cn, (*this)(cn), width, height);
	cn = Coord (cp.c.x + kernel, j);
	// printf ("|YX+| (%d,%d) -- (%d,%d) -- (%d,%d)\n",
	// 	cn.x-cp.c.x,cn.y-cp.c.y,cn.x,cn.y,cp.c.x,cp.c.y);
	next.add (cn, (*this)(cn), width, height);
    }

    return next;
}

void Dem::averhood_max (CriticalPoint cp, int kernel, double* strength, int* kernel_max)
{
    int k = (kernel - 1) / 2;
    Averhood prev, next, max;

    prev = max = averhood (cp, 1, next);
    *kernel_max = 1;

    // if (std::isnan (max.get()))
    // {
	// printf ("IN. cp (%d, %d) . ct:%c cv:%lf as:%lf bs:%lf an:%d bn:%d\n",
	// 	cp.c.x, cp.c.y, critical2char (max.ct), max.cv, max.above_sum,
	// 	max.below_sum, max.above_n, max.below_n);
	// int x = cp.c.x;
	// int y = cp.c.y;
	// printf ("  % 6.8lf % 6.8lf % 6.8lf\n"
	//      "  % 6.8lf % 6.8lf % 6.8lf\n"
	//      "  % 6.8lf % 6.8lf % 6.8lf\n",
	//      _pdm((*this)(x-1, y+1)), _pdm((*this)(x  , y+1)), _pdm((*this)(x+1, y+1)),
	//      _pdm((*this)(x-1, y  )), _pdm((*this)(x  , y  )), _pdm((*this)(x+1, y  )),
	//      _pdm((*this)(x-1, y-1)), _pdm((*this)(x  , y-1)), _pdm((*this)(x+1, y-1))
	//      );
    // }
    for (int i = 2; i <= k; i++)
    {
	next = averhood (cp, i, prev);
	// if (std::isnan (next.get()))
	//     printf ("NEXT %d. cp (%d, %d) . ct:%c cv:%lf as:%lf bs:%lf an:%d bn:%d\n",
	// 	    i, cp.c.x, cp.c.y, critical2char (max.ct), max.cv, max.above_sum,
	// 	    max.below_sum, max.above_n, max.below_n);

	if (max < next)
	{
	    max = next;
	    *kernel_max = i;
	}

	prev = next;
    }

    *kernel_max = (*kernel_max * 2) + 1;
    *strength = max.get ();

    // if (std::isnan (*strength))
    // 	printf ("OUT. cp (%d, %d) . ct:%c cv:%lf as:%lf bs:%lf an:%d bn:%d\n\n",
    // 		cp.c.x, cp.c.y, critical2char (max.ct), max.cv, max.above_sum,
    // 		max.below_sum, max.above_n, max.below_n);


    // if (kernel > 80)
    // 	printf ("kernel: %d, kmax: %d\n", kernel, *kernel_max);
}
