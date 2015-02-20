#ifndef _SCALETYPES_HH
#define _SCALETYPES_HH

#include "debug.h"
#include "grid.hh"

enum CriticalType {REG=0,MAX,MIN,SA2,SA3,EQU};
enum RelationType {GT,LT,EQ};

static inline RelationType inverted_relation (RelationType rel)
{
    if (rel == GT)
	return LT;

    if (rel == LT)
	return GT;

    eprint ("Impossible relation, %d\n", rel);
    return rel;
}

class CriticalPoint {
 public:
    Coord c;
    CriticalType t;
    CriticalPoint () : c (0,0), t (REG) {}
    CriticalPoint (Coord c, CriticalType t) : c (c), t (t) {}
};

class CriticalVertex {
 public:
    int v;
    CriticalType t;
    CriticalVertex () : v (0), t (REG) {}
    CriticalVertex (int v, CriticalType t) : v (v), t (t) {}
};


static inline void point_type_step (RelationType current,
				    RelationType* previous,
				    int* changes)
{
    if (current == GT && *previous == LT)
    {
	(*changes)++;
	*previous = GT;
    }

    if (current == LT && *previous == GT)
    {
	(*changes)++;
	*previous = LT;
    }
}

static inline CriticalType point_type_analyze (RelationType first,
					       int changes,
					       Coord c)
{
    if (changes == 0 && first == GT)
	return MAX;

    if (changes == 0 && first == LT)
	return MIN;

    if (changes == 0)
    {
	eprint ("Impossible plateu: %d %d\n", c.x, c.y);
	return EQU;
    }

    if ((changes % 2) == 1)
    {
	eprint ("Odd number of changes: %d %d\n", c.x, c.y);
	return REG;
    }

    changes /= 2;

    if (changes == 1)
	return REG;

    if (changes > 2)
	return SA3;

    return SA2;
}

static inline CriticalType point_type_analyze (RelationType first,
					       int changes,
					       int v, int nbsize)
{
    if (changes == 0 && first == GT)
	return MAX;

    if (changes == 0 && first == LT)
	return MIN;

    if (changes == 0)
    {
	eprint ("Impossible plateu: %d\n", v);
	return EQU;
    }

    if ((changes % 2) == 1)
    {
	eprint ("Odd number of changes: %d\n", v);
	return REG;
    }

    changes /= 2;

    if (changes == 1)
	return REG;

    if (changes == 2)
	return SA2;

    if (changes == 3)
	return SA3;

    eprintx (-1, "Unexpected monkey saddle (%d changes) - at vertex %d - nbsz(%d)\n",
	     changes, v, nbsize);

    return SA3;
}


static inline char relation2char (RelationType r)
{
    switch (r)
    {
    case GT:
	return 'G';
    case LT:
	return 'L';
    case EQ:
	return 'E';
    }
    eprint ("Wrong relation %d\n", r);
    return '#';
}

static inline char critical2char (CriticalType t)
{
    switch (t)
    {
    case REG:
	return 'R';
    case MAX:
	return 'M';
    case MIN:
	return 'm';
    case SA2:
	return '1';
    case SA3:
	return '2';
    case EQU:
	return 'E';
    default:
	return '#';
    }
    eprint ("Wrong type %d\n", t);
    return '#';
}

static inline CriticalType char2critical (char c)
{
    switch (c)
    {
    case 'R':
	return REG;
    case 'M':
	return MAX;
    case 'm':
	return MIN;
    case '1':
	return SA2;
    case '2':
	return SA3;
    case 'E':
	// return EQU;
    default:
	;// return -1;
    }
    eprint ("Wrong type %c\n", c);
    return EQU;
}


static inline int critical2value (CriticalType t)
{
    switch (t)
    {
    case REG:
	return 0;
    case MAX:
    case MIN:
	return -1;
    case SA2:
	return 1;
    case SA3:
	return 2;
    case EQU:
    default:
	;
    }
    eprint ("Wrong type %d\n", t);
    return -50;
}

static inline int char2value (char c)
{
    return critical2value (char2critical (c));
}

static inline const char* critical2string (CriticalType type)
{
    static const char* reg_s = "REG";
    static const char* max_s = "MAX";
    static const char* min_s = "MIN";
    static const char* sa2_s = "SA2";
    static const char* sa3_s = "SA3";
    static const char* equ_s = "EQU";

    switch (type)
    {
    case REG:
	return reg_s;
    case MAX:
	return max_s;
    case MIN:
	return min_s;
    case SA2:
	return sa2_s;
    case SA3:
	return sa3_s;
    case EQU:
	return equ_s;
    }
    return reg_s;
}


#endif // _SCALETYPES_HH
