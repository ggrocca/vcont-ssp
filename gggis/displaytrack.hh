#ifndef _DISPLAYTRACK_HH
#define _DISPLAYTRACK_HH

#include "displaymanager.hh"
#include "track.hh"



// class DisplayPlane
//     TwBar* bar;
//     string bar_name;
//     GeoMapping geo_mapping;
//     int order;
    
//     DisplayPlane (Plane* p);
//     virtual ~DisplayPlane ();
//     virtual void display ();

//     static inline DisplayPlane* selector (Plane* p)


class DisplayTrack : public DisplayPlane
{
    // TwBar* bar;
    // string bar_name;
    // GeoMapping geo_mapping;
    // int order;
public:

    Track* track;
    TrackOrdering* track_order;
    std::vector<TrackRenderingEntry> vquery;
    BoundingBox bb;
    
    DisplayTrack (Plane* p, GeoMapping* m, int sidx, int pidx);
    ~DisplayTrack ();

    void display ();
    void query (double t);

    bool draw_track;
    double track_mult;
    double track_scale;

    // double scale_criticals;
    // double life_multiplier;
    bool draw_query;
    double query_mult;
    double query_scale;
    bool query_cur_pos;
    bool query_death;

    bool draw_lines;
    double lines_width;
    double lines_life_clip;
    unsigned int lines_size_clip;
    bool lines_query;

    double scale_value;
    double time_value;

    // ordered tracking
    bool draw_order;
    double order_mult;
    int order_step;
    double order_time;
    int order_event;
};

#endif // _DISPLAYTRACK.HH
