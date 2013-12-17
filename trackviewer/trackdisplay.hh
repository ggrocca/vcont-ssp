#ifndef __TRACKDISPLAY__
#define __TRACKDISPLAY__

#ifdef __APPLE__
#include <glut.h>
#else
#include <GL/glut.h>
#endif

#include "track.hh"
#include "demreader.hh"
#include "grid.hh"

using namespace std;

class TrackDisplay
{
public:
    
    TrackDisplay();

    ~TrackDisplay(){};
    
    void read_dem (char *file);
    void read_track (char *file);
    void query (double t);
    void getbb (double* cx, double* cy, double* diam);
    void getbb (Point* a, Point* b);
    void draw (int dem_idx);
    void draw_line_test (double d);

    // bool draw_time_labels;
    bool draw_terrain;
    double clip_black;
    double clip_white;

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
    
// #define VF_DRAW_MODE_NUM 5
//     typedef enum {SMOOTH, POINTS, WIRE, HIDDEN, FLAT} draw_mode_t;

// #define VF_DRAW_MODE_NUM 3
//     typedef enum {FLAT, POINTS, WIRE} draw_mode_t;

    std::vector<DEMReader*> dems;
    Track* track;
    std::vector<TrackRenderingEntry> vquery;
    TrackOrdering *track_order;
};

#endif
