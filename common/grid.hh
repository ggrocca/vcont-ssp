#ifndef _GRID_HH
#define _GRID_HH

#include <vector>
#include <limits>

#include "debug.h"

class Coord
{
    
public:
    
    int x, y;
    
    Coord () : x (0), y (0) {}
    Coord (int x, int y) : x (x), y (y) {}
    
    bool is_inside (const int w, const int h)
    {
	return x >= 0 && x < w && y >= 0 && y < h;
    }

    bool is_outside (const int w, const int h)
    {
	return !is_inside (w, h);
    }
    
    bool operator<(const Coord& rhs)
    {
	return (x < rhs.x && y < rhs.y);
    }

    bool operator>(const Coord& rhs)
    {
	return (x > rhs.x && y > rhs.y);
    }

    bool operator==(const Coord& rhs)
    {
	return (x == rhs.x && y == rhs.y);
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
		return bogus = -std::numeric_limits<T>::max();
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

        data.resize(_width*_height);
        height = _height;
        width = _width;
    }
};

#endif // _GRID_HH
