#ifndef _LATLON_HH
#define _LATLON_HH

#include "debug.h"
#include "grid.hh"
#include <math.h>
#include <string>

class LatLon
{
public:

    double p, l;

    LatLon () : p (0), l (0) {}
    LatLon (double p, double l) : p (p), l (l) {}
};

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

    Coord la, lb;
    LatLon wa, wb;

    GeoMapping () :
	la (), lb (), wa (), wb () {}

    // xl,yl:lat,lon
    // xu,yu:lat,lon
    GeoMapping (std::string filename)
    {
	FILE* fp = fopen (filename.c_str(), "r");

	if (fp == NULL)
	    eprintx (2, "Could not open file %s\n", filename.c_str());

	fscanf (fp,
		"%d , %d : %lf , %lf \n "
		"%d , %d : %lf , %lf \n ",
		&la.x, &la.y, &wa.p, &wa.l,
		&lb.x, &lb.y, &wb.p, &wb.l
		);

	fclose (fp);
    }

    GeoMapping (Coord la, Coord lb, LatLon wa, LatLon wb) :
	la (la), lb (lb), wa (wa), wb (wb) {}

    LatLon l2w (Coord lp)
    {
	LatLon wp;
	wp.p = __s_tr (lp.x, la.x, lb.x, wa.p, wb.p);
	wp.l = __s_tr (lp.y, la.y, lb.y, wa.l, wb.l);
	return wp;
    }

    Coord w2l (LatLon wp)
    {
	Coord lp;
	lp.x = /*floor*/ (__s_tr (wp.p, wa.p, wb.p, la.x, lb.x));
	lp.y = /*floor*/ (__s_tr (wp.l, wa.l, wb.l, la.y, lb.y));
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
