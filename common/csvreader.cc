#include "csvreader.hh"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>

CSVReader::CSVReader (int width, int height,
		      double cellsize, double xllcorner, double yllcorner) :
    width (width), height (height), 
    cellsize (cellsize), xllcorner (xllcorner), yllcorner (yllcorner) {}

void CSVReader::asc2img (Point a, Point* i)
{
    i->x = (a.x - xllcorner) / cellsize;
    i->y = (a.y - yllcorner) / cellsize;
}

void CSVReader::img2asc (Point i, Point* a)
{
    a->x = (i.x * cellsize) + xllcorner;
    a->y = (i.y * cellsize) + yllcorner;
}

void CSVReader::load (const char* filename, std::vector<SwissSpotHeight>& points)
{
    points.clear ();
    
    FILE *fp = fopen (filename, "r");
    if (fp == NULL)
	eprintx (2, "Could not open file `%s'. %s\n", filename, strerror (errno));

    // header
    fscanf (fp, "%*s");
    // for every line
    while (true)
    {
	Point pa;
	int id;
	int r = fscanf (fp, "%lf,%lf,%d,%*s", &pa.x, &pa.y, &id);
	// printf ("%s: %d\n", filename, r);

	if (r != 3)
	    break;
	
	SwissSpotHeight sh;
	asc2img (pa, &sh.p);
	sh.oid = id;
	if (sh.p.is_inside ((double) width, (double) height))
	    points.push_back (sh);
    }

    // printf ("read %d points from file %s\n", points.size (), filename);
    fclose (fp);
}

void CSVReader::save (const char* filename, std::vector<SwissSpotHeight>& points)
{
    FILE *fp = fopen (filename, "w");
    if (fp == NULL)
	eprintx (2, "Could not open file `%s'. %s\n", filename, strerror (errno));
    
    fprintf (fp, "X,Y,OBJECTID\n");
    
    for (unsigned i = 0; i < points.size(); i++)
    {
	Point pa;
	img2asc (points[i].p, &pa);

	fprintf (fp, "%lf,%lf,%d\n", pa.x, pa.y, points[i].oid);
    }

    fclose (fp);
}

void CSVReader::save (const char* filename, std::vector<int>& spots,
		      Track* track, ScaleSpace* ssp)
{
    FILE *fp = fopen (filename, "w");
    if (fp == NULL)
	eprintx (2, "Could not open file `%s'. %s\n", filename, strerror (errno));
    
    fprintf (fp, "X,Y,Z,TYPE,STRENGTH\n");
    
    for (unsigned i = 0; i < spots.size(); i++)
    {
	int idx = spots[i];
	// coords:      track->lines[idx].entries[0].c,
	// type:        track->original_type (idx)
	// importance:  track->lifetime_elixir (idx)

	Point pi (track->lines[idx].entries[0].c.x + 0.5,
		  track->lines[idx].entries[0].c.y + 0.5);
	double height = (*(ssp->dem[0]))(track->lines[idx].entries[0].c.x,
					 track->lines[idx].entries[0].c.y);
	Point pa;
	img2asc (pi, &pa);
	    
	fprintf (fp, "%lf,%lf,%lf,%s,%lf,\n", pa.x, pa.y, height,
		 critical2string (track->original_type (idx)),
		 track->lifetime_elixir (idx));
    }

    fclose (fp);
}
