#ifndef _CSVREADER_HH
#define _CSVREADER_HH

#include "grid.hh"
#include "track.hh"
#include "scaletypes.hh"

enum SwissType {OTHER=0,PEAK,PIT,SADDLE,HUMAN,ALL};

bool is_sametype (CriticalType ct, SwissType st);
SwissType string2swiss (std::string s);
std::string swiss2string (SwissType t);
SwissType critical2swiss (CriticalType c);

class TrackSpot {
public:
    int idx;
    Point p;
    double life;
    double strength;
    CriticalType type;
    
    TrackSpot (int idx, Point p, double life, double strength, CriticalType type) :
	idx (idx), p (p), life (life), strength (strength), type (type) {}
};

static bool compare_life (const TrackSpot& r, const TrackSpot l)
{
    return (r.life > l.life);
}

static bool compare_strength (const TrackSpot& r, const TrackSpot l)
{
    return (r.strength > l.strength);
}


class SwissSpotHeight {
public:
    Point p;
    int oid;
    SwissType t;
    double life;
    double strength;
    SwissSpotHeight () : p (0.0, 0.0), oid (0) {}
    SwissSpotHeight (Point p) : p (p), oid (0) {}
    SwissSpotHeight (Point p, int oid) : p (p), oid (oid) {}
    SwissSpotHeight (const TrackSpot& ts, int idbase)
    {
	p = ts.p;
	oid = idbase + ts.idx;
	t = critical2swiss (ts.type);
	life = ts.life;
	strength = ts.strength;
    }
};

class CSVReader {
public:
    CSVReader (int width, int height,
	       double cellsize, double xllcorner, double yllcorner);
    
    void load (const char* filename, std::vector<SwissSpotHeight>& points);
    void load (const char* filename, std::vector<SwissSpotHeight>& points,
	       double cut);

    void save (const char* filename, std::vector<SwissSpotHeight>& points);
    void save (const char* filename, std::vector<SwissSpotHeight>& points,
	       bool save_types);
    void save (const char* filename, std::vector<int>& spots,
		     Track* track, ScaleSpace* ssp);

private:
    void asc2img (Point a, Point* i);
    void img2asc (Point i, Point* a);

    int width, height;
    double cellsize;
    double xllcorner, yllcorner;
};


#endif // _CSVREADER_HH
