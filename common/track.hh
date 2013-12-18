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
    bool is_getting_born (unsigned j);
    double travel ();

    // GG should be implemented in track
    // Point final_point (std::vector<TrackLine>& lines);
    // Point start_point (std::vector<TrackLine>& lines);
    // double lifetime ();
};


class Track 
{

public:

    double time_of_life;
    // int critical_original_num;
    // int critical_born_num;
    // used in check critical lines, which should be member function really
    // int dem_width;
    // int dem_height;

    std::vector<TrackLine> lines;

    // GG should probably be somewhere else?
    // std::vector<int> flips_num;
    // int flips_total;

    Track () {}
    ~Track () {}

    Track (FILE *f);
    void write (FILE *f);
    
    void query (double t, std::vector<TrackRenderingEntry>& v, bool verbose = true);

    Track* afterlife ();

    // void print_stats ();

    // posso farmi tornare una copia della struttura a un certo tempo.
    // un elenco dei critici vivi a un certo tempo con posizioni
    // originali e attuali.

    // helper accessors
    int get_entry (int i, double t);
    CriticalType get_type (int i);
    CriticalType get_type (int i, unsigned j);
    bool is_dead (int i);
    bool is_dead (int i, unsigned j);
    bool is_alive (int i);
    bool is_alive (int i, unsigned j);
    // bool is_active (int i);
    // bool is_active (int i, unsigned j);
    bool is_original (int i);
    bool is_born (int i);
    bool is_getting_born (int i, unsigned j);
    double travel (int i);

    Point final_point (int i);
    Point start_point (int i);
    double lifetime (int i);
};

class TrackEntryPointer
{
public:
    int line;
    int idx;

    TrackEntryPointer () {}
    TrackEntryPointer (int line, int idx) : line (line), idx (idx) {}
};

typedef enum {
    DEATH_1, DEATH_2, BIRTH_1, BIRTH_2,
    MOVE_1, MOVE_3, MOVE_12, MOVE_21
} FlippingEventType;

class FlippingEvent
{
 public:

    FlippingEventType type;
    double time;
    std::vector<TrackEntryPointer> track_ptrs;
};

// GG WARN FIX strange things:

// births that seems deaths (points disappear after birth) and maybe versavice
// last surviving points

class TrackOrdering
{
 public:

    std::vector<int> track_positions;
    std::vector<FlippingEvent> events;
    int current_event;
    Track *track;
    bool previous_forward;

    TrackOrdering () {}
    ~TrackOrdering () {}

    TrackOrdering (FILE *f);
    void write (FILE *f);

    // init positions from events and track pointer
    void init (Track* t);
    
    // seek functions
    bool seek (int new_event);
    bool seek (bool forward);

    bool seek_bb (bool forward, BoundingBox bb);
    bool seek_life_bb (bool forward, BoundingBox bb);


    // rendering: draws all tracking points at track_positions if they
    // are not dead at those positions and already born at the time of
    // current event; then draws birth point if current event is a
    // birth or all the points involved and death point if it's a
    // death.

    // seek: ciclo da current event fino a dove si deve arrivare; per
    // ogni evento aggiorna track_position[line] al nuovo idx per
    // tutti i punti dell'evento.

    // helper functions
    double current_time ();
    FlippingEventType current_type ();
    BoundingBox current_bb ();
    bool is_current_death ();
    bool is_current_birth ();
    Point current_final ();
    Point current_start ();
    std::vector<Point> current_coords ();
    std::vector<CriticalType> current_types ();

    bool is_point_active (int line);
    Point point_coord (int line);
    CriticalType point_type (int line);

private:    
    void update_positions (bool forward);
};


void track_writer (const char *filename, Track* track, TrackOrdering* order);
void track_reader (const char *filename, Track** track, TrackOrdering** order);

static inline const char* flip_event2string (FlippingEventType type)
{
    static const char* d1 = "D1";
    static const char* d2 = "D2";
    static const char* b1 = "B1";
    static const char* b2 = "B2";
    static const char* m1 = "M1";
    static const char* m3 = "M3";
    static const char* m12 = "12";
    static const char* m21 = "21";

    switch (type)
    {
    case DEATH_1:
	return d1;
    case DEATH_2:
	return d2;
    case BIRTH_1:
	return b1;
    case BIRTH_2:
	return b2;
    case MOVE_1:
	return m1;
    case MOVE_3:
	return m3;
    case MOVE_12:
	return m12;
    case MOVE_21:
	return m21;
    }

    assert (0);
    return m1;
}

static inline FlippingEventType string2flip_event (const char* s)
{
    std::string ss (s);

    if (ss == "D1")
	return DEATH_1;
    if (ss == "D2")
	return DEATH_2;
    if (ss == "B1")
	return BIRTH_1;
    if (ss == "B2")
	return BIRTH_2;
    if (ss == "M1")
	return MOVE_1;
    if (ss == "M3")
	return MOVE_3;
    if (ss == "12")
	return MOVE_12;
    if (ss == "21")
	return MOVE_21;

    assert (0);
    return MOVE_1;
}


#endif // _TRACK_HH
