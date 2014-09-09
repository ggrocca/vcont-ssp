#include "csvreader.hh"
#include "debug.h"

#include <stdio.h>
#include <stdlib.h>

SwissType string2swiss (std::string s)
{
    static const char* other_s = "OTHER";
    static const char* peak_s = "PEAK";
    static const char* pit_s = "PIT";
    static const char* saddle_s = "SADDLE";
    static const char* human_s = "HUMAN";

    if (s == other_s)
	return OTHER;
    else if (s == peak_s)
	return PEAK;
    else if (s == pit_s)
	return PIT;
    else if (s == saddle_s)
	return SADDLE;
    else if (s == human_s)
	return HUMAN;

    eprint ("wrong type: %s\n", s.c_str());
    return OTHER;
}

std::string swiss2string (SwissType t)
{
    static const char* other_s = "OTHER";
    static const char* peak_s = "PEAK";
    static const char* pit_s = "PIT";
    static const char* saddle_s = "SADDLE";
    static const char* human_s = "HUMAN";

    if (t == OTHER)
	return other_s;
    else if (t == PEAK)
	return peak_s;
    else if (t == PIT)
	return pit_s;
    else if (t == SADDLE)
	return saddle_s;
    else if (t == HUMAN)
	return human_s;

    eprint ("wrong type: %d\n", t);
    return "";
}

SwissType critical2swiss (CriticalType c)
{
    switch (c)
    {
    case REG:
	return OTHER;
    case MAX:
	return PEAK;
    case MIN:
	return PIT;
    case SA2:
    case SA3:
	return SADDLE;
    case EQU:
    default:
	;
    }
    eprint ("Wrong type %d\n", c);
    return OTHER;
}

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
    load (filename, points, 0.0);
}
void CSVReader::load (const char* filename, std::vector<SwissSpotHeight>& points, double cut)
{
    points.clear ();
    
    FILE *fp = fopen (filename, "r");
    if (fp == NULL)
	eprintx (2, "Could not open file `%s'. %s\n", filename, strerror (errno));

    // header
    char header[2048] = "\0";
    fscanf (fp, "%2048s", header);

    bool swiss_vanilla = false;
    bool swiss_typed = false;
    
    if ((std::string(header)) == "X,Y,OBJECTID,OBJECTVAL,OBJECTORIG,YEAROFCHAN")
	swiss_vanilla = true;
    else if ((std::string(header)) == "X,Y,OBJECTID,OBJECTVAL,OBJECTORIG,YEAROFCHAN,TYPE")
	swiss_typed = true;
    else
	eprintx (-1, "%s","unrecognized swiss csv format.\n");
	
    // for every line
    while (true)
    {
	Point pa;
	int id;
	SwissSpotHeight sh;

	if (swiss_vanilla)
	{
	    int r = fscanf (fp, "%lf,%lf,%d,%*s", &pa.x, &pa.y, &id);
	    // printf ("%s: %d\n", filename, r);

	    if (r != 3)
		break;
	}

	if (swiss_typed)
	{
	    char stype[16] = "\0";
	    int r = fscanf (fp, "%lf,%lf,%d,Hoehenkote,LK25,1986,%16s",
			    &pa.x, &pa.y, &id, stype);
	    // printf ("%s: %d\n", filename, r);
	    if (r != 4)
		break;

	    sh.t = string2swiss (stype);
	}

	asc2img (pa, &sh.p);
	sh.oid = id;
	if (sh.p.is_inside ((double) width, (double) height, cut))
	    points.push_back (sh);
    }

    // printf ("read %d points from file %s\n", points.size (), filename);
    fclose (fp);
}

void CSVReader::save (const char* filename, std::vector<SwissSpotHeight>& points)
{
    save (filename, points, false);
}
void CSVReader::save (const char* filename, std::vector<SwissSpotHeight>& points,
		      bool save_types)
{
    FILE *fp = fopen (filename, "w");
    if (fp == NULL)
	eprintx (2, "Could not open file `%s'. %s\n", filename, strerror (errno));

    fprintf (fp, "X,Y,OBJECTID,OBJECTVAL,OBJECTORIG,YEAROFCHAN%s%s\n",
	     save_types? "," : "",
	     save_types? "TYPE" : "");
    
    for (unsigned i = 0; i < points.size(); i++)
    {
	Point pa;
	img2asc (points[i].p, &pa);

	fprintf (fp, "%lf,%lf,%d,Hoehenkote,LK25,1986%s%s\n",
		 pa.x, pa.y, points[i].oid, save_types? "," : "",
		 save_types? (swiss2string (points[i].t)).c_str() : "");
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
