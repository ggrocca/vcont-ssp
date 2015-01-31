#ifndef _SCALARFIELD_HH
#define _SCALARFIELD_HH

#include <float.h>
#include <math.h>

#include "debug.h"
#include "mesh.hh"
#include "scaletypes.hh"

class ScalarField : public Field<double> {

public:

    double max;
    double min;

    ScalarField (Mesh& mesh);
    ScalarField (Mesh& mesh, const char* curvfile);
    ScalarField (ScalarField& sc);
    ScalarField (Mesh& mesh, FILE* fp);

    void write (FILE* fp);
    double& operator() (int v);
    double& operator[] (int v);

    bool is_equal (int a, int b);                // given points have equal value
    bool has_plateaus ();            // this dem has adjacent points with equal value

    RelationType point_relation (int a, int b);  // relation between adiacent a,b
    CriticalType point_type (int v);                       // classify point, coord
 
};

class SurfaceScaleSpace {

public:
    
    int levels;
  
    SurfaceScaleSpace () {};
    SurfaceScaleSpace (std::vector<ScalarField*> scfs);
    SurfaceScaleSpace (Mesh& mesh, const char* filename);
    ~SurfaceScaleSpace ();

    void write_critical (char* filename);
    void write_scalespace (char* filename);

    std::vector<ScalarField*> fields;
    std::vector< std::vector<CriticalVertex> > criticals;
};

#endif // _SCALARFIELD_HH
