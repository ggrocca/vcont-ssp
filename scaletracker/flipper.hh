#ifndef _FLIPPER_HH
#define _FLIPPER_HH

#include "../common/grid.hh"
#include "../common/dem.hh"
#include "../common/scalespace.hh"

class MapEntry {

public:

    MapEntry ();
    MapEntry (bool is_clip);

    bool is_clip;
    char e[6]; // from -1,-1 then cw, contains relation_t
    int tr[2]; // reference for tracking data struct
    // double v; // intermediate value (maybe useless)

    void set_rel (int i, RelationType r);
    RelationType get_rel (int i);
};

class Map : public Grid<MapEntry> {

public:

    Map ();
    Map (int w, int h);
    Map (Dem& dem);
    Map (Map& map);

    // map_entry get_entry (int x, int y);
    // void set_entry (unsigned int x, unsigned int y, map_entry e);

    MapEntry& operator() (Coord c);        // read & write with coord
    MapEntry& operator() (int x, int y);   // read & write with x,y

    bool is_clip (Coord c);
    bool is_clip (int x, int y);

    RelationType point_relation (Coord c, int idx);
    RelationType point_relation (int x, int y, int idx);

    CriticalType point_type (Coord c);
    CriticalType point_type (int x, int y);

    RelationType invert_relation (Coord a, Coord b);

    void point_print (Coord c);
    void point_print (int scope, Coord c);

    bool check_reference (Map& mc);
    bool check_relation (Map& mc);
};

#include "../common/track.hh"

class CriticalPair {

public:

    CriticalType a,b;
    int sum () { return critical2value (a) + critical2value (b); }
};

class Edge {

public:
    
    Coord l,r;

    Edge () : l (Coord ()), r (Coord ()) {}
    Edge (const Edge& e) : l (e.l), r (e.r) {}
    Edge (Coord l, Coord r) : l (l), r (r) {}

    bool operator == (const Edge& e)
    {
	return l == e.l && r == e.r;	
    }

    bool operator < (const Edge& e) const
    {
	if (l.x < e.l.x)
	    return true;
	if (l.x > e.l.x)
	    return false;

	if (l.y < e.l.y)
	    return true;
	if (l.y > e.l.y)
	    return false;

	if (r.x < e.r.x)
	    return true;
	if (r.x > e.r.x)
	    return false;
	if (r.y < e.r.y)
	    return true;
	if (r.y > e.r.y)
	    return false;

	return false;
    }
};

class Flip {

 public:

    double t;
    Edge e;

    Flip () : t (0.0), e () {}

    Flip (const Flip& f) : t (f.t), e (f.e) {}

    Flip (double t, Edge e) : t (t), e (e) {}
    
    bool operator < (const Flip& f) const
    {
	if (t == f.t)
	    return e < f.e;
	return t < f.t; 
    }
};




class Flipper {

public:
    
    int flips_total;
    std::vector< std::vector<Flip> > flip;

    Flipper (ScaleSpace& ss);

    Track* track (ScaleSpace& ss);
};

#endif // _FLIPPER_HH
