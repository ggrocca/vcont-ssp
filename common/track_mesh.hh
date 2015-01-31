#ifndef _TRACKMESH_HH
#define _TRACKMESH_HH

#include <vector>

#include "debug.h"
#include "mesh.hh"
#include "scaletypes.hh"
#include "scalarfield.hh"

class TrackEntry_M
{

public:

    int v;
    double t;
    int mate; // other saddle if 2-saddle
    int life; // birth or death companion

    TrackEntry_M () : v (0), t (0.0), mate (-1), life (-1) {}
    TrackEntry_M (int v) : v (v), t (0.0), mate (-1), life (-1) {}
    TrackEntry_M (double t) : v (0), t (t), mate (-1), life (-1) {}
    TrackEntry_M (int v, double t) : v (v), t (t), mate (-1), life (-1) {}
    // TrackEntry (const TrackEntry& te) :
    // 	c (te.c), t (te.t), mate (te.mate), life (te.life) {}

    // Coord& position() { return  c; }
    // double& time() { return t; }
    // void reset_mate { mate = -1; }
    // void reset_life { life = -1; }
};


class TrackLine_M
{

public:

    bool mark;
    CriticalType type;
    double elixir;
    double strength;
    double scale;
    std::vector<TrackEntry_M> entries;

    // returns index to first alive entry occurring before t.
    // -1 if not found
    int get_entry (double t);
    CriticalType get_type ();
    CriticalType get_type (unsigned j);
    bool is_dead ();
    bool is_dead (unsigned j);
    bool is_alive ();
    bool is_alive (unsigned j);
    bool is_original ();
    bool is_born ();
    bool is_getting_born (unsigned j);
    // double travel ();
};

class Track_M
{

public:

    int intoxicated;
    double time_of_life;
    int size;

    std::vector<TrackLine_M> lines;

    Track_M () {}
    ~Track_M () {}

    Track_M (FILE *f);
    void write (FILE *f);
    
    //void query (double t, std::vector<TrackRenderingEntry>& v, bool verbose = true);

    void drink_elixir ();
    // void normalize_border_points ();
    void get_strength (ScalarField* d);
    void get_relative_drop (ScalarField* d);

    int get_entry (int i, double t);
    CriticalType get_type (int i);
    CriticalType get_type (int i, unsigned j);
    bool is_dead (int i);
    bool is_dead (int i, unsigned j);
    bool is_alive (int i);
    bool is_alive (int i, unsigned j);
    bool is_original (int i);
    bool is_born (int i);
    bool is_getting_born (int i, unsigned j);
    // double travel (int i);

    // Coord coord (int i);
    // Point point (int i);
    // Point final_point (int i);
    // Point start_point (int i);
    int vertex (int i);
    double lifetime (int i);

    int get_other_death (int i);
    int get_other_birth (int i);
    double death_time (int i);
    double birth_time (int i);

    CriticalType original_type (int i);
    
    double lifetime_elixir (int i);
    // double strength (int i);

// private:
//     int __get_min_border_distance (int i);
};

class TrackMap_M {
    //Track& track;
    Field<int> field;
    
public:
    
    TrackMap_M (Track_M& track, Mesh& mesh) : field (mesh, -1)
    {
	for (unsigned i = 0; i < track.lines.size(); i++)
	    if (track.is_original (i))
		field (track.vertex (i)) = i;
    }

    int operator() (int v)
    {
	return field (v); // -1 if nothing's here
    }
    int operator[] (int v)
    {
	return field (v); // -1 if nothing's here
    }
};

// void track_writer (const char *filename, Track* track, TrackOrdering* order);
// void track_reader (const char *filename, Track** track, TrackOrdering** order);
void track_writer_m (const char *filename, Track_M* track);
void track_reader_m (const char *filename, Track_M** track);


#endif // _TRACKMESH_HH
