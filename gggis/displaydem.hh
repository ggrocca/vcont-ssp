#ifndef _DISPLAYDEM_HH
#define _DISPLAYDEM_HH

#include "displaymanager.hh"



// class DisplayPlane
//     TwBar* bar;
//     string bar_name;
//     GeoMapping geo_mapping;
//     int order;
    
//     DisplayPlane (Plane* p);
//     virtual ~DisplayPlane ();
//     virtual void display ();

//     static inline DisplayPlane* selector (Plane* p)


class DisplayDem : public DisplayPlane
{
    // TwBar* bar;
    // string bar_name;
    // GeoMapping geo_mapping;
    // int order;
public:

    DEMReader* dr;
    
    DisplayDem (Plane* p, GeoMapping* m, int pidx, int sidx);
    ~DisplayDem ();

    void display ();

    double clip_black;
    double clip_white;
    double multiply;
};

#endif // _DISPLAYDEM.HH
