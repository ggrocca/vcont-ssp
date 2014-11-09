#ifndef _DEM_HH
#define _DEM_HH

#include <float.h>
#include <math.h>

#include "debug.h"
#include "grid.hh"
#include "demreader.hh"
#include "scaletypes.hh"
#include "gaussian.hh"
#include "ascheader.hh"

class Averhood {

public:
    
    CriticalType ct;
    double cv;
    double above_sum;
    double below_sum;
    int above_n;
    int below_n;

    Averhood ()
    {
	ct = REG;
	cv = above_sum = below_sum = 0.0;
	above_n = below_n = 0;

    }
    
    Averhood (CriticalType ct, double cv) : ct (ct), cv (cv)
    {
	above_sum = below_sum = 0.0;
	above_n = below_n = 0;
    }

    Averhood (const Averhood& a) :
	ct (a.ct), cv (a.cv),
	above_sum (a.above_sum), below_sum (a.below_sum),
	above_n (a.above_n), below_n (a.below_n) {}
    
    double above_av () { return above_sum / (double) above_n; }
    double below_av () { return below_sum / (double) below_n; }
    double both_av () {
	return (below_sum + above_sum) / (double) (below_n + above_n);
    }

    double above_diff () { return above_av() - cv; }
    double below_diff () { return cv - below_av(); }
    double max_diff () { return cv - both_av(); }
    double min_diff () { return both_av() - cv; }
    double aggregate_diff() {
	return ((above_diff () * above_n) + (below_diff () * below_n)) /
	    (double) (below_n + above_n);
    }
    
    double get ()
    {
	if (ct == MAX)
	    return max_diff() > 0.0? max_diff() : 0.0;
	if (ct == MIN)
	    return min_diff() > 0.0? min_diff() : 0.0;
	if (ct == SA2 || ct == SA3)
	{
	    // return above_diff() < below_diff()? above_diff() : below_diff();
	    //double ret = above_diff() < below_diff()? above_diff() : below_diff();
	    //double ret = above_diff() < below_diff()? below_diff() : above_diff();
	    double ret = aggregate_diff ();
	    
	    return ret;
	}
	return -DBL_MAX;
    }

    void add (Coord c, double v, int w, int h)
    {
	if (c.is_inside (w, h))
	{
	    if (v > cv)
	    {
		above_sum += v;
		above_n++;
	    }
	    else if (v < cv)
	    {
		below_sum += v;
		below_n++;
		// printf ("GNE c(%d,%d) v: %lf cv:%lf\n", c.x, c.y, v, cv);
	    }
	    // else
	    // 	printf ("MECO c(%d,%d) v: %lf cv:%lf\n", c.x, c.y, v, cv);
	}
	// else
	//     printf ("STICA c(%d,%d) v: %lf cv:%lf\n", c.x, c.y, v, cv);
    }
    
    bool operator<(Averhood& rhs)
    {
	return (get() < rhs.get());
    }
};

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
    Dem (Dem& dem, TGaussianBlur<double>& BlurFilter, int window, int filter_algo,
	 std::vector<CriticalPoint>& crits);          // blur and controlled topology
    Dem (FILE* fp);
    
    void write (FILE* fp);
    void write (char* asc_name, ASCHeader asch);

    double& operator() (Coord c, AccessType a = ABYSS);    // read & write with coord
    double& operator() (int x, int y, AccessType a = ABYSS); // read & write with x,y

    void clip_background (double v);            // values < v are clipped to -DBL_MAX
    void clip_background (Dem& dem, double v);     // clip values that are < v in dem
    bool is_clip (Coord c);                               // is this a clipped value?
    bool is_clip (int x, int y);

    bool is_equal (Coord a, Coord b);                // given points have equal value
    bool has_plateaus ();            // this dem has adjacent points with equal value
    void identify_plateaus (std::vector < std::vector <Coord> >& plateaus_list);

    RelationType point_relation (Coord a, Coord b);  // relation between adyacent a,b
    CriticalType point_type (Coord c);                       // classify point, coord
    CriticalType point_type (int x, int y);                    // classify point, x,y

    void point_print (Coord c);                       // print point and neighborhood
    void point_print (int scope, Coord c);         // print p. & n.  with debug scope
    void point_print (Coord c, CriticalType type);             // print p., n. & type
    void point_print (int scope, Coord c, CriticalType type);                  // ...
    void point_print (int scope, int x, int y);
    
    Averhood averhood (CriticalPoint cp, int kernel, Averhood prev);
    void averhood_max (CriticalPoint cp, int kernel,
		       double* strength, int* kernel_max);

private:
    // bool _recursive_label (Grid<int>* idplats, Coord c, unsigned label,
    // 			    std::vector < std::vector <Coord> >& plateaus_list);
    bool _iterative_label (Grid<int>* idplats, Coord c, unsigned label,
			   std::vector < std::vector <Coord> >& plateaus_list);
};

#endif // _DEM_HH
