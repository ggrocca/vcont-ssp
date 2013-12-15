#ifndef _TRACK_HH
#define _TRACK_HH

#include <vector>

#include "debug.h"
#include "grid.hh"
#include "scaletypes.hh"
#include "geomapping.hh"

class TrackEntry 
{

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

class TrackRenderingEntry 
{

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


class TrackLine 
{

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
    // bool is_active ();
    // bool is_active (unsigned j);
    bool is_original ();
    bool is_born ();
    double travel ();
    double lifetime ();

    // GG should be implemented in track
    void final_point (std::vector<TrackLine>& lines,
    		      double* fx, double* fy);
    void start_point (std::vector<TrackLine>& lines, Point* d);
};


class Track 
{

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
    Track (const char *filename);

    void write (char *filename);    
    void query (double t, std::vector<TrackRenderingEntry>& v, bool verbose = true);

    // void print_stats ();

    // posso farmi tornare una copia della struttura a un certo tempo.
    // un elenco dei critici vivi a un certo tempo con posizioni
    // originali e attuali.
};


class TrackEvent
{
 public:

    double t;
    int line;
    int pos;
    bool birth;
    bool death;
};
inline bool operator< (const TrackEvent& lhs, const TrackEvent& rhs){ return lhs.t < rhs.t; }

class TrackOrderEntry
{
 public:

    CriticalType type; 
    Coord c; // current position
    Point d;
    int idx; // index in Track.lines[i].entries
    bool active; // after birth, before death
    bool birth; // getting born now
    bool death; // dying now
};

class TrackOrder
{
 public:

    Track* track;
    // current event index
    int current_event;
    // corrisponding time reached
    double current_time;
    
    // situation of critical points at current time
    std::vector<TrackOrderEntry> current_pos;
    
    // all events in tracking, ordered by time
    std::vector<TrackEvent> events;

    // build the events from track, initialize event and pos at 0
    void assign (Track* track);

    // seek from current position to new_event, updating positions
    // when bounds are reached, stops and return false
    bool seek (int new_event);
    bool seek (bool forward);

    bool seek_bb (bool forward, BoundingBox bb);
    bool seek_life_bb (bool forward, BoundingBox bb);

    // last event handled by seek
    int previous_event;
    // position of last event handled by seek
    Coord last_event_c;

    // GG TBD
    // seek_next_life (); // go to next birth or death
    // seek_prev_life ();
    // int active_num; // number of active criticals.

private:
    bool update ();
};

#endif // _TRACK_HH
