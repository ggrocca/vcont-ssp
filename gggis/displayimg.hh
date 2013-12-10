#ifndef _DISPLAYIMG_HH
#define _DISPLAYIMG_HH

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


class DisplayIMG : public DisplayPlane
{
    // TwBar* bar;
    // string bar_name;
    // GeoMapping geo_mapping;
    // int order;
public:

    cimg_library::CImg<unsigned char>* img;
    
    DisplayIMG (Plane* p, GeoMapping* m, int sidx, int pidx);
    ~DisplayIMG ();

    void display ();

    double clip_black;
    double clip_white;
    double multiply;
};

#endif // _DISPLAYIMG.HH
