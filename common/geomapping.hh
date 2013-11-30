#ifndef _LATLON_HH
#define _LATLON_HH

#include "debug.h"
#include "grid.hh"
#include <math.h>
#include <string>
#include <vector>

class BoundingBox
{

public:

    Point a,b;
    
    BoundingBox () : a (), b () {}

    BoundingBox (Point a, Point b) : a (a), b(b) {}

    BoundingBox (std::vector<BoundingBox> all)
    {
	a = Point::highest ();
	b = Point::lowest ();
	
	for (unsigned i = 0; i < all.size(); i++)
	{
		Point ai, bi;
	
		ai = all[i].a;
		bi = all[i].b;
	
		if (ai < a)
		    a = ai;
	
		if (bi > b)
		    b = bi;
	
		// tprint ("ai (%lf, %lf) __ bi () (%lf, %lf)\n", ai.x, ai.y, bi.x, bi.y);
	}
    }

    static BoundingBox emptiest ()
    {
	return BoundingBox (Point::highest (), Point::lowest ());
    }
};


// class LatLon
// {
// public:

//     double p, l;

//     LatLon () : p (0), l (0) {}
//     LatLon (double p, double l) : p (p), l (l) {}
// };

static inline double __s_tr (double s, double sa, double sb, double ea, double eb)
{
    // printf ("STR a: %lf\n", s*eb  );
    // printf ("STR b: %lf\n", sa*ea );
    // printf ("STR c: %lf\n", s*ea  );
    // printf ("STR d: %lf\n", sa*eb );

    // printf ("STR 1: %lf\n", (s*eb + sa*ea - s*ea - sa*eb) );
    // printf ("STR 2: %lf\n", (s*eb + sa*ea - s*ea - sa*eb) / (sb-sa) );
    // printf ("STR all: %lf\n\n", ((s*eb + sa*ea - s*ea - sa*eb) / (sb-sa)) + ea);

    return ((s*eb + sa*ea - s*ea - sa*eb) / (sb-sa)) + ea;
}

class GeoMapping
{
public:

    BoundingBox local, world;

    GeoMapping () :
	local (), world () {}

    // xl,yl:lat,lon
    // xu,yu:lat,lon
    GeoMapping (std::string filename)
    {
	FILE* fp = fopen (filename.c_str(), "r");

	if (fp == NULL)
	    eprintx (2, "Could not open file %s\n", filename.c_str());

	fscanf (fp,
		"%lf , %lf : %lf , %lf \n "
		"%lf , %lf : %lf , %lf \n ",
		&local.a.x, &local.a.y, &world.a.x, &world.a.y,
		&local.b.x, &local.b.y, &world.b.x, &world.b.y
		);

	fclose (fp);
    }

    GeoMapping (BoundingBox local, BoundingBox world) :
	local (local), world (world) {}

    Point l2w (Coord lp)
    {
	Point wp;
	wp.x = __s_tr (lp.x, local.a.x, local.b.x, world.a.x, world.b.x);
	wp.y = __s_tr (lp.y, local.a.y, local.b.y, world.a.y, world.b.y);
	return wp;
    }

    Coord w2l (Point wp)
    {
	Coord lp;
	lp.x = /*floor*/ (__s_tr (wp.x, world.a.x, world.b.x, local.a.x, local.b.x));
	lp.y = /*floor*/ (__s_tr (wp.y, world.a.y, world.b.y, local.a.y, local.b.y));
	return lp;
    }
};


// cp geomapping.hh prova.cc; g++ prova.cc; ./a.out; rm prova.cc a.out

// int main(int argc, char *argv[])
// {


//     Coord la (0,0);
//     Coord lb (3600,3600);

//     LatLon wa (45,6);
//     LatLon wb (46,7);

//     GeoMapping lm (la, lb, wa, wb);

//     Coord l (1200,2400);
//     LatLon w (45.3, 6.6);

//     Coord tl0, tl1;
//     LatLon tw0, tw1;

//     printf ("\n");

//     // around half
//     tl0 = lm.w2l (w);
//     printf ("w2l: (%lf, %lf) --> (%d, %d)\n", w.p, w.l, tl0.x, tl0.y);

//     tw0 = lm.l2w (l);
//     printf ("l2w: (%d, %d) --> (%lf, %lf)\n", l.x, l.y, tw0.p, tw0.l);

//     printf ("\n");

//     // feeding back

//     tl1 = lm.w2l (tw0);
//     printf ("w2l: (%lf, %lf) --> (%d, %d)\n", tw0.p, tw0.l, tl1.x, tl1.y);

//     tw1 = lm.l2w (tl0);
//     printf ("l2w: (%d, %d) --> (%lf, %lf)\n", tl0.x, tl0.y, tw1.p, tw1.l);

//     printf ("\n");
    
//     // start

//     tl0 = lm.w2l (wa);
//     printf ("w2l: (%lf, %lf) --> (%d, %d)\n", wa.p, wa.l, tl0.x, tl0.y);

//     tw0 = lm.l2w (la);
//     printf ("l2w: (%d, %d) --> (%lf, %lf)\n", la.x, la.y, tw0.p, tw0.l);

//     printf ("\n");

//     // end

//     tl0 = lm.w2l (wb);
//     printf ("w2l: (%lf, %lf) --> (%d, %d)\n", wb.p, wb.l, tl0.x, tl0.y);

//     tw0 = lm.l2w (lb);
//     printf ("l2w: (%d, %d) --> (%lf, %lf)\n", lb.x, lb.y, tw0.p, tw0.l);

//     printf ("\n");

//     return 0;

// }


#endif // _LATLON
