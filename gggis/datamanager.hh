#ifndef _DATAMANAGER_HH
#define _DATAMANAGER_HH

#include "../common/utils.hh"
#include "../common/grid.hh"
#include "../common/geomapping.hh"
#include "../common/demreader.hh"


using std::vector;
using std::string;

/*
  nome di directory in input

  la directory contiene tante altre directory, con un nome che e' il
  nome del dataset ogni dataset ha una particolare registrazione di
  coordinate in un file di nome __GEOREF.txt

  __GEOREF.txt contiene un mapping lineare:
  xl,yl:lat,lon
  xu,yu:lat,lon

  che da' la trasformazione lineare da applicare a un rettangolo che
  contiene le possibili coordinate contenute nel dataset.

  Il dataset contiene un numero arbitrario di file che vengono riconosciuti

  il file puo' essere di tipo: raster, raster lines, points, tracking,
  vector field, tensor field, scalespace of the former

*/

#define _DM_STR_LEN 10 // max length of strings in type and extensions
#define _DM_EXT_NUM 10  // max num of etensions associated to a type, terminator included
#define _DM_TERM "___"
#define _DM_TYPEFILE_EXT ".type" // extension to be appended at files containing type assignment
// 0 unknown/unsupported type
// 1 digital elevation models,
// 2 raster images,
// 3 critical points,
// 4 tracking data sttructure,
// 5 scalespace
// 6 curvature
// 7 extremalities
// 8 integral lines
// E end
typedef enum            {UKW=0, DEM=1, IMG=2, CRT=3, TRK=4, SSP=5, CRV=6, EXM=7, INL=8} DataSetType;
#define __DST_STRINGS  {"UKW", "dem", "img", "crt", "trk", "ssp", "crv", "exm", "inl", _DM_TERM};
#define __DST_EXTENSIONS					\
    {								\
	/* 0 */    {"UKW",_DM_TERM},				\
	    /* 1 */    {"hgt","tif","png",_DM_TERM},		\
	    /* 2 */    {"tiff","jpg",_DM_TERM},			\
	    /* 3 */    {"crt",_DM_TERM},			\
	    /* 4 */    {"trk",_DM_TERM},			\
	    /* 5 */    {"ssp",_DM_TERM},			\
	    /* 6 */    {"crv",_DM_TERM},			\
	    /* 7 */    {"exm",_DM_TERM},			\
	    /* 8 */    {"inl",_DM_TERM},			\
	    /* E */    {_DM_TERM}				\
    };

extern char* DST_strings[];
extern char* DST_extensions[][_DM_EXT_NUM];

static inline DataSetType string2ds_type (string s)
{
    string e;

    for (int i = 0; (e = string(DST_strings[i])) != _DM_TERM;  i++)
    	if (e == s)
    	    return (DataSetType) i;
    return UKW;
}

static inline string ds_type2string (DataSetType dst)
{
    int i = 0;
    while (string(DST_strings[i]) != _DM_TERM)
	i++;

    return ((int) dst < i-1)? DST_strings[(int) dst] : DST_strings[0];
}

static inline string ext2ds_string (string ext)
{
    for (int i = 0; string (DST_extensions[i][0]) != _DM_TERM; i++)
	for (int j = 0; string (DST_extensions[i][j]) != _DM_TERM; j++)
	    if (ext == DST_extensions[i][j])
		return string (DST_strings[i]);
    return DST_strings[0];
}


typedef enum {MULTIPLE, SINGLE, PLANE} DataOrganization;


class Plane
{
public:
    
    string pathname;
    string filename;
    DataSetType type;
    void* data;

    Plane (string pathname);
    ~Plane ();
    BoundingBox getbb ();
};

#define DM_GEOREF_FILE "__GEOREF.txt"

class DataSet
{
public:
    GeoMapping map;
    vector <Plane*> planes;
    string dir;
    string name;

    // read files inside dir, all registered to _GEOREF.txt
    DataSet (string dir, DataOrganization org);
    ~DataSet ();

    // GG should implement bb class
    BoundingBox getbb_local ();
    BoundingBox getbb_world ();
};



class DataManager
{
public:
    vector<DataSet*> datasets;

    // read a dir and load all datasets inside it.
    DataManager (char *dir, DataOrganization org);
    ~DataManager ();

    // GG should implement bb class
    BoundingBox getbb ();
};



#endif // _DATAMANAGER.HH
