#ifndef _GRID_HH
#define _GRID_HH

#include "genericlimits.hh"

#include <vector>
#include <list>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <math.h>

#include "debug.h"



template <class T> class Pair
{
public:

    T x, y;

    Pair () : x (0), y (0) {}
    Pair (const T x, const T y) : x (x), y (y) {}
    Pair (const Pair<T>& p) : x (p.x), y (p.y) {}

    Pair& operator=(const Pair& rhs)
    {
	x = rhs.x;
	y = rhs.y;

	return *this;
    }
    
    bool is_border (const T w, const T h) const
    {
	return x == 0 || x == w-1 || y == 0 || y == h-1;
    }

    bool is_inside (const T w, const T h) const
    {
	return x >= 0 && x < w && y >= 0 && y < h;
    }
    
    bool is_inside (const T w, const T h, double cut) const
    {
	double wc = w * (cut / 2.0);
	double hc = h * (cut / 2.0);
	return x >= 0+wc && x < w-wc && y >= 0+hc && y < h-hc;
    }

    bool is_inside (const T w, const T h, int window) const
    {
	return x >= window && x < w-window && y >= window && y < h-window;
    }
    
    bool is_inside (const Pair<T> llc, const Pair<T> hrc) const
    {
	return x >= llc.x && x < hrc.x && y >= llc.y && y < hrc.y;
    }
    

    bool is_outside (const T w, const T h) const
    {
	return !is_inside (w, h);
    }
    
    bool operator<(const Pair& rhs) const
    {
	return (x < rhs.x && y < rhs.y);
    }

    bool operator>(const Pair& rhs) const
    {
	return (x > rhs.x && y > rhs.y);
    }

    bool operator<=(const Pair& rhs) const
    {
	return (x <= rhs.x && y <= rhs.y);
    }

    bool operator>=(const Pair& rhs) const
    {
	return (x >= rhs.x && y >= rhs.y);
    }

    bool operator==(const Pair& rhs) const
    {
	return (x == rhs.x && y == rhs.y);
    }

    bool operator!=(const Pair& rhs) const
    {
	return (! (*this == rhs));
    }

    Pair operator+(const Pair& rhs)
    {
	return Pair (x + rhs.x, y + rhs.y);
    }

    Pair operator-(const Pair& rhs)
    {
	return Pair (x - rhs.x, y - rhs.y);
    }

    static Pair lowest()
    {
	T bogus;

	if (std::numeric_limits<T>::is_integer)
	    bogus = std::numeric_limits<T>::min();
	else 
	    bogus = GenericType<T>::lowest();

	return Pair (bogus, bogus);
    }

    static Pair highest()
    {
	T bogus;
	bogus = std::numeric_limits<T>::max();

	return Pair (bogus, bogus);
    }
};

// template <class T>
// Pair<T>& operator+(const Pair<T>& lhs, const Pair<T>& rhs)
// {
//     return Pair<T> (lhs.x + rhs.x, lhs.y + rhs.y);
// }


typedef Pair<double> Point;

inline double point_distance (Point a, Point b)
{
    return sqrt (((a.x - b.x) * (a.x - b.x)) + ((a.y - b.y) * (a.y - b.y)));
}


// class Point : public Pair<double>
// {
// public:
//     Point () : Pair<double>() {}
//     Point (double x, double y) : Pair<double>(x, y) {}
//     Point (const Point& p) : Pair<double>(p) {}

//     Point operator=(const Point rhs)
//     {
//     	x = rhs.x;
//     	y = rhs.y;
//     }
// };

class Coord : public Pair<int>
{
    
public:
    Coord () : Pair<int>() {}
    Coord (int x, int y) : Pair<int>(x, y) {}
    Coord (const Coord& c) : Pair<int>(c) {}
    Coord (const Pair<int>& pi) : Pair<int>(pi) {}

    /**
                     <2>           <3>
           [  ,  ] [ 0, 1] [ 1, 1]
       <1> [-1, 0] [ 0, 0] [ 1, 0] <4>
           [-1,-1] [ 0,-1] [  ,  ]
       <0>           <5>
     **/
    
    int linear_index (const Coord c) const
    {
	int idx = -1;

	int dx = c.x - x;
	int dy = c.y - y;
	
	if (dx == -1)
	{
	    if (dy == -1) // 0 --> [-1][-1]
		idx = 0;
	    else if (dy ==  0) // 1 --> [-1][ 0]
		idx = 1;
	}
	else if (dx == 0)
	{
	    if (dy ==  1) // 2 --> [ 0][ 1]
		idx = 2;
	    else if (dy == -1) // 5 --> [ 0][-1]
		idx = 5;
	}
	else if (dx == 1)
	{
	    if (dy ==  1) // 3 --> [ 1][ 1]
		idx = 3;	    
	    else if (dy ==  0) // 4 --> [ 1][ 0]
		idx = 4;
	}
	
	if (idx < 0 || idx > 5)
	    eprint ("Coords not adjacent: [%d][%d] - [%d][%d] = [%d][%d] __ idx: %d.\n",
		    c.x, c.y, x, y, dx, dy, idx);
	
	return idx;
    }

    void neigh_6 (std::vector<Coord>& ac) const
    {
    	ac.clear ();

    	ac.push_back (Coord (x, y+1));
    	ac.push_back (Coord (x+1, y+1));
    	ac.push_back (Coord (x+1, y));
	
    	ac.push_back (Coord (x, y-1));
    	ac.push_back (Coord (x-1, y-1));
    	ac.push_back (Coord (x-1, y));
    }
    
    void append_6 (std::vector<Coord>& lc) const
    {
	std::vector<Coord> ac;
	neigh_6 (ac);

	for (int i = 0; i < ac.size(); i++)
	    lc.push_back (ac[i]);
	
    	// lc.push_back (Coord (x, y+1));
    	// lc.push_back (Coord (x+1, y+1));
    	// lc.push_back (Coord (x+1, y));
	
    	// lc.push_back (Coord (x, y-1));
    	// lc.push_back (Coord (x-1, y-1));
    	// lc.push_back (Coord (x-1, y));
    }

    void neigh_8 (std::vector<Coord>& ac) const
    {
    	ac.clear ();

    	ac.push_back (Coord (x, y+1));
    	ac.push_back (Coord (x+1, y+1));
    	ac.push_back (Coord (x+1, y));
	
    	ac.push_back (Coord (x, y-1));
    	ac.push_back (Coord (x-1, y-1));
    	ac.push_back (Coord (x-1, y));

    	ac.push_back (Coord (x-1, y+1));
    	ac.push_back (Coord (x+1, y-1));
    }
    
    void append_8 (std::vector<Coord>& lc) const
    {
	std::vector<Coord> ac;
	neigh_8 (ac);

	for (int i = 0; i < ac.size(); i++)
	    lc.push_back (ac[i]);
    }
    
    // returns next coord around current one in a 6-connected fashion.
    // clockwise trip starting south east (-1,-1).
    void round_trip_6 (Coord* const c) const
    {
	bool do_redo;
	static bool ic = true;
	static int pi = -1;
	static int pj = -1;
	static int pc = 0;
	static int ps = 1;

	static int _x = INT_MIN;
	static int _y = INT_MIN;

	if (_x == INT_MIN && _y == INT_MIN)
	{
	    _x = x;
	    _y = y;
	}

	// reinitialize everything if called on a new instance
	if (_x != x || _y != y)
	{
	    ic = true;
	    pi = -1;
	    pj = -1;
	    pc = 0;
	    ps = 1;

	    _x = x;
	    _y = y;
	}

    redo:
    
	do_redo = false;

	c->x = x+pi;
	c->y = y+pj;

	if (pi == -pj)
	    do_redo = true;

	pc++;
	
	pi = ic? pi : pi + ps;
	pj = ic? pj + ps : pj;
    
	if (! ((pc) % 2))
	    ic = !ic;
    
	if (pc != 0 && ! ((pc) % 4))
	    ps = -ps;

	if (do_redo)
	    goto redo;
    }

    // returns next coord around current one in a 8-connected fashion.
    // clockwise trip starting south east (-1,-1).
    void round_trip_8 (Coord* const c) const
    {
	static bool ic = true;
	static int pi = -1;
	static int pj = -1;
	static int pc = 0;
	static int ps = 1;

	static int _x = INT_MIN;
	static int _y = INT_MIN;

	if (_x == INT_MIN && _y == INT_MIN)
	{
	    _x = x;
	    _y = y;
	}

	// reinitialize everything if called on a new instance
	if (_x != x || _y != y)
	{
	    ic = true;
	    pi = -1;
	    pj = -1;
	    pc = 0;
	    ps = 1;

	    _x = x;
	    _y = y;
	}

	c->x = x+pi;
	c->y = y+pj;
    
	pc++;
	
	pi = ic? pi : pi + ps;
	pj = ic? pj + ps : pj;
    
	if (! ((pc) % 2))
	    ic = !ic;
    
	if (pc != 0 && ! ((pc) % 4))
	    ps = -ps;
    }
};


inline Coord point2coord (Point p)
{
    return Coord (floor (p.x), floor (p.y));
}

inline Point coord2point (Coord c)
{
    return Point ((double) c.x, (double) c.y);
}


template <class T> class Grid
{
public:

    enum AccessType {ABYSS, PLATEU, BOUND};

    T bogus;
    int width;
    int height;
    std::vector<T> data;

    Grid () : bogus (std::numeric_limits<T>::min()),
	      width (0), height (0), data () {}

    Grid (int width, int height) : bogus (std::numeric_limits<T>::min()),
				   width (width), height (height), data ()
    {
	if (width < 0 || height < 0)
	    eprintx (EXIT_GRID, "Negative dimension %d,%d.\n", width, height);

        data.resize(width * height);
    }

    Grid (int width, int height, T t) : bogus (std::numeric_limits<T>::min()),
				   width (width), height (height), data ()
    {
	if (width < 0 || height < 0)
	    eprintx (EXIT_GRID, "Negative dimension %d,%d.\n", width, height);

        data.resize(width * height);

        for(int i = 0; i < width; ++i)
            for(int j = 0; j < height; ++j)
                operator () (i,j) = t;
    }

    static inline T __data (std::vector<T> data, Coord c, int width, int height)
    {
	(void)height;
	
	return data[(c.y * width) + c.x];
	// return data[(c.x * height) + c.y];

    }

#define __data(x, y)				\
    (data[((y) * width) + (x)])
    // (data[((x) * height) + (y)])

    T& operator() (Coord c, AccessType a = BOUND)
    {
	if (c.is_inside (width, height))
	    return __data (c.x, c.y);
	    // return data[(c.y * width) + c.x];
	    // return data[(c.x * height) + c.y];

	switch (a)
	{
	case BOUND:
	    eprint ("Out of mem access with AccessType==BOUND, %d %d\n", c.x, c.y);
	    return bogus;
	    break;
	    
	case ABYSS:
	    if (std::numeric_limits<T>::is_integer)
		return bogus = std::numeric_limits<T>::min();
	    else 
		return bogus = GenericType<T>::lowest();
	    break;
	    
	case PLATEU:
	    c.x = c.x < 0? 0 : width - 1;
	    c.y = c.y < 0? 0 : height - 1;
	    return __data (c.x, c.y);
	    // return data[(c.y * width) + c.x];
	    // return data[(c.x * height) + c.y];
	    break;
	    
	default:
	    ;
	}

	eprintx (EXIT_GRID, "Impossible case, %d %d\n", c.x, c.y);
	return bogus;
    }

    T& operator() (int x, int y, AccessType a = BOUND)
    {
	return (*this)(Coord (x, y), a);
    }

    unsigned size ()
    {
        return data.size();
    }

    void resize (int _width, int _height)
    {
	if (width < 0 || height < 0)
	    eprintx (EXIT_GRID, "Negative dimension %d,%d.\n", _width, _height);

        data.resize(_width * _height);
        height = _height;
        width = _width;
    }
};

#endif // _GRID_HH
