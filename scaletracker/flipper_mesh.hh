#ifndef _FLIPPER_MESH_HH
#define _FLIPPER_MESH_HH

#include "flipper.hh"
#include "track_mesh.hh"
#include "mesh.hh"
#include "scalarfield.hh"

class Edge_M {

public:
    
    int l,r;

    Edge_M () : l (0), r (0) {}
    Edge_M (const Edge_M& e) : l (e.l), r (e.r) {}
    Edge_M (int l, int r) : l (l), r (r) {}

    bool operator == (const Edge_M& e) const
    {
	return l == e.l && r == e.r;	
    }

    bool operator < (const Edge_M& e) const
    {
	if (l < e.l)
	    return true;
	if (l > e.l)
	    return false;

	if (r < e.r)
	    return true;
	if (r > e.r)
	    return false;

	return false;
    }
};

class MapEntry_M {

public:

    MapEntry_M () { tr[0] = tr[1] = -1; }

    //char e[6]; // from -1,-1 then cw, contains relation_t
    std::vector<RelationType> e;
    int tr[2]; // reference for tracking data struct
    // double v; // intermediate value (maybe useless)

    void set_rel_num (int size) { e.resize(size); }
    int get_rel_num () { return e.size (); }
    void set_rel (int i, RelationType r) { e[i] = r; }
    RelationType get_rel (int i) { return e[i]; }

    void reset_ref_all () { tr[0] = tr[1] = -1; }
    void reset_ref_first () { tr[0] = -1; }
    void reset_ref_second () { tr[1] = -1; }

    void set_ref_all (int r0, int r1) { tr[0] = r0; tr[1] = r1; }
    void set_ref_first (int r) { tr[0] = r; }
    void set_ref_second (int r) { tr[1] = r; }
    
    int get_ref_first () { return tr[0]; }
    int get_ref_second () { return tr[1]; } 
};

class Map_M : public Field<MapEntry_M> {

public:

    //Map ();
    Map_M (Mesh& mesh);
    Map_M (ScalarField& sc, std::vector<TrackLine_M>& lines);
    Map_M (Map_M& map);

    // map_entry get_entry (int x, int y);
    // void set_entry (unsigned int x, unsigned int y, map_entry e);

    MapEntry_M& operator() (int v);        // read & write with coord
    MapEntry_M& operator[] (int v);        // read & write with coord

    // bool is_clip (int v);

    int point_relation_num (int v);
    RelationType point_relation (int v, int idx);

    CriticalType point_type (int v);
    CriticalPair pair(Edge_M e);

    RelationType invert_relation (Edge_M e);
    void update_relations (ScalarField* sc, int v);

    // void point_print (int v);
    // void point_print (int scope, int v);

    // bool check_relation (Dem& dem);
    // bool check_relation (Map& mc);
    // bool check_reference (Map& mc);
};

class Flip_M {

 public:

    double t;
    Edge_M e;

    Flip_M () : t (0.0), e () {}

    Flip_M (const Flip_M& f) : t (f.t), e (f.e) {}

    Flip_M (double t, Edge_M e) : t (t), e (e) {}
    
    bool operator < (const Flip_M& f) const
    {
	if (t == f.t)
	    return e < f.e;
	return t < f.t; 
    }

    bool operator == (const Flip_M& f) const
    {
	return t == f.t && e == f.e;
    }

    static Flip_M highest ();
    static Flip_M lowest ();
};



class Flipper_M {

public:
    
    int flips_total;
    std::vector< std::vector<Flip_M> > flip;

    Flipper_M (SurfaceScaleSpace& ss);

    void track (SurfaceScaleSpace& ss, Track_M* track);
};


#endif // _FLIPPER_MESH_HH
