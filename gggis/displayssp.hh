#ifndef _DISPLAYSSP_HH
#define _DISPLAYSSP_HH

#include "displaymanager.hh"
#include "scalespace.hh"


// class DisplayPlane
//     TwBar* bar;
//     string bar_name;
//     GeoMapping geo_mapping;
//     int order;
    
//     DisplayPlane (Plane* p);
//     virtual ~DisplayPlane ();
//     virtual void display ();

//     static inline DisplayPlane* selector (Plane* p)


class DisplaySSP : public DisplayPlane
{
    // TwBar* bar;
    // string bar_name;
    // GeoMapping geo_mapping;
    // int order;
public:

    ScaleSpace *ssp;
    
    DisplaySSP (Plane* p, GeoMapping* m, int sidx, int pidx);
    ~DisplaySSP ();

    void display ();

    int level;
    double clip_black;
    double clip_white;
    double multiply;
};

#endif // _DISPLAYSSP.HH
