#ifndef _CSVREADER_HH
#define _CSVREADER_HH

#include "grid.hh"
#include "track.hh"
#include "scaletypes.hh"
#include "demreader.hh"
#include "ascheader.hh"

enum ClassifiedType {OTHER=0, PEAK, PIT, SADDLE, HUMAN, ALL};
enum SwisstopoType {
    NAMED_ALPINE_SUMMIT_100, NAMED_MAIN_SUMMIT_200, NAMED_SUMMIT_300,
    NAMED_MAIN_HILL_400, NAMED_HILL_500, NAMED_ROCK_HEAD_600,
    NAMED_PASS_700, NAMED_ROAD_PASS_800,
    HEIGHT_LFP1_100, HEIGHT_LV95_200, HEIGHT_AGNES_300,
    HEIGHT_NATURAL_400, HEIGHT_HUMAN_500, HEIGHT_LAKE_600,
    MORPH_DOLINE_100, MORPH_ERRATIC_200, MORPH_BLOCK_300, MORPH_SINK_400,
    UNDEFINED
};

#define DB_CLASSIFICATION (1 << 0)
#define DB_TRACK          (1 << 1)
#define DB_TOPO_SHOP      (1 << 2)
#define DB_TOPO_NAMED     (1 << 3)
#define DB_TOPO_HEIGHT    (1 << 4)
#define DB_TOPO_MORPH     (1 << 5)
#define DB_SCALESPACE     (DB_CLASSIFICATION | DB_TRACK)
#define DB_TOPO_KARTE     (DB_TOPO_NAMED | DB_TOPO_MORPH | DB_TOPO_HEIGHT)
#define db_new(d)     ((d) = 0)
#define db_set(d, s)  ((d) = (s))
#define db_add(d, s)  ((d) |= (s))
#define db_del(d, s)  ((d) &= ~(s))
#define db_test(d, s) (((d) & (s)) == (s))
typedef unsigned int DatabaseType;

inline bool is_peak (SwisstopoType st)
{
    return
	st == NAMED_ALPINE_SUMMIT_100 || st == NAMED_MAIN_SUMMIT_200 ||
	st == NAMED_SUMMIT_300 || st == NAMED_MAIN_HILL_400 || st == NAMED_HILL_500;
}

inline bool is_saddle (SwisstopoType st)
{
    return st == NAMED_PASS_700;
}

inline bool is_pit (SwisstopoType st)
{
    return st == MORPH_DOLINE_100 || st == MORPH_SINK_400;
}


/*
  ClassifiedType -> ClassifiedType
  SwisstopoType { all types in the three databases - named/unnamed/morph}
  
  headers: classified,track,toposhop,named, height,morph
  / si leggono tutti i tipi, si salva quale header e' stato letto
  / si scrivono classified e track (come faccio adesso la lettura in spotizer?)
  /// si scrive un solo tipo che possiede tutti i campi swissspotheight
  //// DBFIELD compreso!!

  experimento - come mappa la  mia classificazione sui tipi svizzeri
      singoli e aggregazioni?

  ///

  scegliere su quali classi di aggregati svizzeri lanciare il riconoscimento peak-saddle-pit

  potrebbe essere: named_tops + unnamed 400 peaks / named_passes + unnamed 400 passes / sinkhole+ sink

  meglio cosi' o tenere solo quelli della mia classificazione?
  meglio cosi' o TUTTI QUELLI DELLA MIA CLASSIFICAZIONE FRA TUTTI I PUNTI?
  
*/
SwisstopoType encode_swisstopo (DatabaseType dbt, int code);
SwisstopoType string2swisstopo (std::string s);
std::string swisstopo2string (SwisstopoType t);

bool is_sametype (CriticalType ct, ClassifiedType st);
ClassifiedType string2classified (std::string s);
std::string classified2string (ClassifiedType t);
ClassifiedType critical2classified (CriticalType c);

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

inline bool compare_life (const TrackSpot& r, const TrackSpot l)
{
    return (r.life > l.life);
}

inline bool compare_strength (const TrackSpot& r, const TrackSpot l)
{
    return (r.strength > l.strength);
}


class SwissSpotHeight {
public:
    Point p;
    double z;
    //int oid;
    ClassifiedType ct;
    SwisstopoType st;
    DatabaseType dbt;
    double life;
    double strength;
    // no oid, add database types
    SwissSpotHeight () : p (0.0, 0.0), z (0.0), ct (OTHER), st (UNDEFINED),
			 dbt (0), life (0.0), strength (0.0) {}
    SwissSpotHeight (Point p) : p (p), z (0.0), ct (OTHER), st (UNDEFINED),
			 dbt (0), life (0.0), strength (0.0) {}
    SwissSpotHeight (const TrackSpot& ts)
    {
	p = ts.p;
	z = 0.0;
	ct = critical2classified (ts.type);
	st = UNDEFINED;
	dbt = DB_TRACK;
	life = ts.life;
	strength = ts.strength;
    }

    ClassifiedType type ()
    {
	if (is_peak (st))
	    return PEAK;
	else if (is_saddle (st))
	    return SADDLE;
	else if (is_pit (st))
	    return PIT;
	else
	    return ct;
	// GG-WARN should return HUMAN for types that we know we don't want to use.
	// but is spotizer up to the task?
    }
    
    void print ()
    {
	printf ("Point(%lf,%lf,%lf). CLASS=%s - SWISS=%s\n", p.x, p.y, z,
		(classified2string (ct)).c_str (), (swisstopo2string (st)).c_str ());
    }
};

class CSVReader {
    // gg constructors and conv methods
public:
    CSVReader ();
    CSVReader (ASCReader& ascr);
    CSVReader (ASCHeader& asch);
    // CSVReader (int width, int height,
    // 	       double cellsize, double xllcorner, double yllcorner);
    
    void load (const char* filename, std::vector<SwissSpotHeight>& points);
    void load (const char* filename, std::vector<SwissSpotHeight>& points,
	       double cut);

    void save (const char* filename, std::vector<SwissSpotHeight>& points);
    // void save (const char* filename, std::vector<SwissSpotHeight>& points,
    // 	       bool save_types);
    void save (const char* filename, std::vector<int>& spots,
		     Track* track, ScaleSpace* ssp);
    void save (const char* filename, ScaleSpace* ssp);
    
    void asc2img (Point a, Point* i);
    void img2asc (Point i, Point* a);

    Point asc2img (Point a);
    Point img2asc (Point i);

private:
    ASCHeader asch;
    // int width, height;
    // double cellsize;
    // double xllcorner, yllcorner;
};


#endif // _CSVREADER_HH
