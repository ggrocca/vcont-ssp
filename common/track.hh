#ifndef _TRACK_HH
#define _TRACK_HH

#include <vector>

#include "debug.h"
#include "grid.hh"
#include "scaletypes.hh"


class TrackEntry {

public:

    Coord c;
    double t;
    int mate; // other saddle if 2-saddle
    int life; // birth or death companion

    TrackEntry () : c (0,0), t (0.0), mate (-1), life (-1) {}
    TrackEntry (Coord c) : c (c), t (0.0), mate (-1), life (-1) {}
    TrackEntry (double t) : c (0,0), t (t), mate (-1), life (-1) {}
    TrackEntry (Coord c, double t) : c (c), t (t), mate (-1), life (-1) {}
    // TrackEntry (const TrackEntry& te) :
    // 	c (te.c), t (te.t), mate (te.mate), life (te.life) {}

    // Coord& position() { return  c; }
    // double& time() { return t; }
    // void reset_mate { mate = -1; }
    // void reset_life { life = -1; }
};

class TrackRenderingEntry {

public:
    
    CriticalType type; 
    Coord tc; // current position
    Coord oc; // original position
    double fx, fy; // death position
    int idx; // index in Track.lines[i].entries
    double tol; // time of life
    double tob; // time of birth
    bool is_born;
    bool is_alive;
};


class TrackLine {

public:

    bool mark;
    CriticalType type;
    std::vector<TrackEntry> entries;

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
    double travel ();
    double lifetime ();

    // GG should be implemented in track
    void final_point (std::vector<TrackLine>& lines,
    		      double* fx, double* fy);
};


class Track {

public:

    static double time_of_life;
    // int critical_original_num;
    // int critical_born_num;
    // used in check critical lines, which should be member function really
    // int dem_width;
    // int dem_height;

    std::vector<TrackLine> lines;

    // GG should probably be somewhere else?
    // std::vector<int> flips_num;
    // int flips_total;

    Track ();
    ~Track ();
    // Track (ScaleSpace& ss); // should be implemented as flipper algorithm
    Track (char *filename);

    void write (char *filename);    
    void query (double t, std::vector<TrackRenderingEntry>& v, bool verbose = true);

    // void print_stats ();

    // posso farmi tornare una copia della struttura a un certo tempo.
    // un elenco dei critici vivi a un certo tempo con posizioni
    // originali e attuali.
};


#endif // _TRACK_HH
