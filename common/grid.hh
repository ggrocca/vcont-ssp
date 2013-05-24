#ifndef _GRID_H
#define _GRID_H

#include <vector>
#include <limits>

class Coord
{
    
public:
    
    int x, y;
    
    Coord () : x (0), y (0) {}
    Coord (int x, int y) : x (x), y (y) {}
    
    bool is_inside (int w, int h)
    {
	return x >= 0 && x < w && y >= 0 && y < h;
    }
    
    bool operator<(Coord& rhs)
    {
	return (x < rhs.x && y < rhs.y);
    }
};

template <class T> class Grid
{
public:

    enum Access {ABYSS, PLATEU, BOUND};

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
	    fprintf (stderr, "Grid::Grid(): Warning, negative dimension.\n");

        data.resize(width * height);
    }

    Grid (int width, int height, T t) : bogus (std::numeric_limits<T>::min()),
				   width (width), height (height), data ()
    {
	if (width < 0 || height < 0)
	    fprintf (stderr, "Grid::Grid(): Warning, negative dimension.\n");

        data.resize(width * height);

        for(int i = 0; i < width; ++i)
            for(int j = 0; j < height; ++j)
                operator () (i,j) = t;
    }

    T& operator() (Coord c, Access a = BOUND)
    {
	if (c.is_inside (width, height))
	    return data[(c.x * height) + c.y];

	switch (a)
	{
	case BOUND:
	    fprintf (stderr, "GridM<T>::operator() out of mem access "
		     " with Access==BOUND, %d %d\n", c.x, c.y);
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

	fprintf (stderr, "GridM<T>::operator() impossible case, %d %d\n", c.x, c.y);
	return bogus;
    }

    unsigned size ()
    {
        return data.size();
    }

    void resize (int _width, int _height)
    {
	if (width < 0 || height < 0)
	    fprintf (stderr, "Grid::resize(): Warning, negative dimension.\n");

        data.resize(_width*_height);
        height = _height;
        width = _width;
    }
};

#endif // _GRID_H
