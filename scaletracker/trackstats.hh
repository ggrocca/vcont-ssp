#ifndef _TRACKSTATS_HH
#define _TRACKSTATS_HH

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/debug.h"
#include "../common/track.hh"
#include "flipper.hh"

class FlipperDebug {

public:
    static void setSS (ScaleSpace* ss);
    
    static void eprint_invert_map_flip (Coord a, Coord b,
					RelationType r1, RelationType r2, int base);
    
    static void print_flip_interpolated (Flip f, CriticalPair b, CriticalPair a,
					 Map* map, int base);
    
private:

    static ScaleSpace* _ss;

};

#include <list>

#define __MOVELEN 5

class elem {
public:
    int count;
    char move[__MOVELEN];

    elem (CriticalPair a, CriticalPair b);
    elem (const elem& copy);
    
    elem& operator=(const elem& rhs);
    elem& operator++();
    bool operator<(const elem& rhs);
    bool operator==(const elem& rhs);
    void values (int* before, int* after);
};

class TrackString {
public:
    std::list<elem> global;
    std::vector< std::list<elem> > local;

    TrackString (int levels);
    void insert (int level, CriticalPair a, CriticalPair b);
    void print ();
};

void print_stats (std::vector<TrackLine>& cps);

#endif // _TRACKSTATS_HH
