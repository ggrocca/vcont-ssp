#ifndef _GRID_HH
#define _GRID_HH

#include <vector>
#include <climits>
#include <limits>
#include <iostream>
#include <cstdlib>
#include <stdio.h>

#include "debug.h"

// GG SUBSTITUTE WITH <type_traits> in c++11
template<class T>
struct GenericType {
    enum { IS_NUMERIC = 0 };
    static T lowest() { return (T()); }
};
template<> struct GenericType <char> {
    enum { IS_NUMERIC = 1 };
    static char lowest() { return (std::numeric_limits<char>::min()); }
};
template<> struct GenericType <unsigned char> {
    enum { IS_NUMERIC = 1 };
    static unsigned char lowest() { return (std::numeric_limits<unsigned char>::min());}
};
template<> struct GenericType <short> {
    enum { IS_NUMERIC = 1 };
    static short lowest() { return (std::numeric_limits<short>::min()); }
};
template<> struct GenericType <unsigned short> {
    enum { IS_NUMERIC = 1 };
    static  unsigned short lowest(){return(std::numeric_limits<unsigned short>::min());}
};
template<> struct GenericType <int> {
    enum { IS_NUMERIC = 1 };
    static int lowest() { return (std::numeric_limits<int>::min()); }
};
template<> struct GenericType <unsigned int> {
    enum { IS_NUMERIC = 1 };
    static unsigned int lowest() { return (std::numeric_limits<unsigned int>::min()); }
};
template<> struct GenericType <float> {
    enum { IS_NUMERIC = 1 };
    static float lowest() { return (-std::numeric_limits<float>::max()); }
};
template<> struct GenericType <double> {
    enum { IS_NUMERIC = 1 };
    static double lowest() { return (-std::numeric_limits<double>::max()); }
};
//////////////////


template <class T> class Pair
{
public:

    T x, y;

    Pair () : x (0), y (0) {}
    Pair (const T x, const T y) : x (x), y (y) {}
    Pair (const Pair<T>& p) : x (p.x), y (p.y) {}

    Pair operator=(const Pair& rhs)
    {
	x = rhs.x;
	y = rhs.y;
    }
    
    bool is_inside (const T w, const T h)
    {
	return x >= 0 && x < w && y >= 0 && y < h;
    }

    bool is_outside (const T w, const T h)
    {
	return !is_inside (w, h);
    }
    
    bool operator<(const Pair& rhs)
    {
	return (x < rhs.x && y < rhs.y);
    }

    bool operator>(const Pair& rhs)
    {
	return (x > rhs.x && y > rhs.y);
    }

    bool operator<=(const Pair& rhs)
    {
	return (x <= rhs.x && y <= rhs.y);
    }

    bool operator>=(const Pair& rhs)
    {
	return (x >= rhs.x && y >= rhs.y);
    }

    bool operator==(const Pair& rhs) const
    {
	return (x == rhs.x && y == rhs.y);
    }

    bool operator!=(const Pair& rhs)
    {
	return (! (*this == rhs));
    }

    Pair operator+(const Pair& rhs)
    {
	return Pair (x + rhs.x, y + rhs.y);
    }

};

// template <class T>
// Pair<T>& operator+(const Pair<T>& lhs, const Pair<T>& rhs)
// {
//     return Pair<T> (lhs.x + rhs.x, lhs.y + rhs.y);
// }


typedef Pair<double> Point;

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

    /**
                     <2>           <3>
           [  ,  ] [ 0, 1] [ 1, 1]
       <1> [-1, 0] [ 0, 0] [ 1, 0] <4>
           [-1,-1] [ 0,-1] [  ,  ]
       <0>           <5>
     **/

    int linear_index (Coord c)
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

    // returns next coord around current one in a 6-connected fashion.
    // clockwise trip starting south east (-1,-1).
    void round_trip_6 (Coord* const c)
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
    void round_trip_8 (Coord* const c)
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




template <class T> class Grid
{
public:

    enum AccessType {ABYSS, PLATEU, BOUND};

    T bogus;
    int width;
    int height;
    std::vector<T> data;

    Grid () : bogus (std::numeric_limits<T>::min()),
	      width (0), height (0), data ()
    {
	
    }

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

    T& operator() (Coord c, AccessType a = BOUND)
    {
	if (c.is_inside (width, height))
	    return data[(c.x * height) + c.y];

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
	    c.y = c.y < 0? 0 : height -1;
	    return data[(c.x * height) + c.y];
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
