#ifndef _FLIPPER_HH
#define _FLIPPER_HH

#include "../common/grid.hh"
#include "../common/dem.hh"
#include "../common/scalespace.hh"
#include "../common/track.hh"


class CriticalPair {

public:

    CriticalType a,b;

    int sum () { return critical2value (a) + critical2value (b); }

    bool operator == (const CriticalPair& cp)
    {
	return a == cp.a && b == cp.b;	
    }

    bool operator != (const CriticalPair& cp)
    {
	return !(*this == cp);
    }

};

class Edge {

public:
    
    Coord l,r;

    Edge () : l (Coord ()), r (Coord ()) {}
    Edge (const Edge& e) : l (e.l), r (e.r) {}
    Edge (Coord l, Coord r) : l (l), r (r) {}

    bool operator == (const Edge& e) const
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



class MapEntry {

public:

    MapEntry () : is_clip (false) { tr[0] = tr[1] = -1; }
    MapEntry (bool is_clip) : is_clip (is_clip) { tr[0] = tr[1] = -1; }

    bool is_clip;
    char e[6]; // from -1,-1 then cw, contains relation_t
    int tr[2]; // reference for tracking data struct
    // double v; // intermediate value (maybe useless)

    void set_rel (int i, RelationType r) { e[i] = (char) r; }
    RelationType get_rel (int i) { return (RelationType) e[i]; }

    void reset_ref_all () { tr[0] = tr[1] = -1; }
    void reset_ref_first () { tr[0] = -1; }
    void reset_ref_second () { tr[1] = -1; }

    void set_ref_all (int r0, int r1) { tr[0] = r0; tr[1] = r1; }
    void set_ref_first (int r) { tr[0] = r; }
    void set_ref_second (int r) { tr[1] = r; }
    
    int get_ref_first () { return tr[0]; }
    int get_ref_second () { return tr[1]; } 
};

class Map : public Grid<MapEntry> {

public:

    Map ();
    Map (int w, int h);
    Map (Dem& dem, std::vector<TrackLine>& lines);
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
    CriticalPair pair(Edge e);

    RelationType invert_relation (Edge e);
    void update_relations (Dem* d, Coord c);

    void point_print (Coord c);
    void point_print (int scope, Coord c);

    bool check_relation (Dem& dem);
    bool check_relation (Map& mc);
    bool check_reference (Map& mc);
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

    bool operator == (const Flip& f) const
    {
	return t == f.t && e == f.e;
    }

    static Flip highest ();
    static Flip lowest ();
};




class Flipper {

public:
    
    int flips_total;
    std::vector< std::vector<Flip> > flip;

    Flipper (ScaleSpace& ss);

    Track* track (ScaleSpace& ss);


    static void filter (Dem* b, Dem* n, int filter_algo,
		       std::vector<CriticalPoint>& crits);
    static void filter_slow (Dem* b, Dem* n, std::vector<CriticalPoint>& crits);
    static void filter_norm (Dem* b, Dem* n, std::vector<CriticalPoint>& crits);
    static void filter_fast (Dem* b, Dem* n, std::vector<CriticalPoint>& crits);
    // static void filter_check (Dem* b, Dem* n, std::vector<CriticalPoint>& crits);
};

#endif // _FLIPPER_HH
