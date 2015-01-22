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
//#include "csvreader.hh"

#include <vector>
#include <algorithm>

using namespace std;

class CritElix {
public:
    int crit;
    double elix;

    CritElix (int i, double d) : crit (i), elix (d) {}
    
    bool operator <(const CritElix& rhs) const
    {
	if (this->elix == rhs.elix)
	    return this->crit < rhs.crit;
	else
	    return this->elix < rhs.elix;
    }
};

class CritNikolas
{
public:
  int crit;
  double life;
  double strength;
  double scale;

  CritNikolas(int i, double l, double s, double s2) : crit (i), life (l), strength (s), scale(s2) {}

  bool operator <(const CritNikolas& rhs) const
  {
    return ((this->scale*this->life*this->strength) > (rhs.scale*rhs.life*rhs.strength));
  }
};
  
class TrackDisplay
{
public:
    
    TrackDisplay();

    ~TrackDisplay(){};
    
    // void read_dem (char *file);
    void read_ssp (char *file);
    void read_track (char *file);
  void read_signs(char*file);
  void read_height(char*file);
  void read_val(char*file);
    void query (double t);
  void search_points(int t);
    void getbb (double* cx, double* cy, double* diam);
    void getbb (Point* a, Point* b);
    void draw (int dem_idx);
    void draw_line_test (double d);

    // additional csv points


  

  bool draw_maxima;
  bool draw_minima;
  bool draw_sellae;
  int maxima_draw_num;  
  int minima_draw_num;
  int sellae_draw_num;
  bool scale_by_life;
  bool scale_by_size;
  double life_imp;
  double strength_imp;

    // bool draw_time_labels;
    bool draw_terrain;
    double clip_black;
    double clip_white;
    double multiply;

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

    bool draw_elixir;
    bool normal_lives;
    double elixir_mult;
    double elixir_scale;
    double elixir_cut;

    bool draw_importance;
    double importance_mult;
    double importance_scale;
    double importance_cut;

    bool draw_spots;
    bool multiply_elix_spots;
    bool draw_final;
    bool draw_always_selected;
    bool draw_density_selected;
    bool draw_density_pool;
  bool draw_nikolas;
  
  bool show_axis;
  bool showSigns;
  bool show_nose;
  bool show_noseRoot;
  bool show_noseLimits;
  bool show_underNose;
  bool show_eyesOuterAngles;
  int fiducialShow;
    double spot_scale;
    void init_spots ();
    double spots_maxima_imp_cut;
  double spots_maxima_life_cut;
    double spots_minima_imp_cut;
  double spots_minima_life_cut;
    double spots_sellae_imp_cut;
  double spots_sellae_life_cut;
  double simmetry_imp_cut_min;
  double simmetry_life_cut_min;
  double simmetry_imp_cut_max;
  double simmetry_life_cut_max;

    int maxima_always_selected_num;
    int maxima_density_pool_num;
    int minima_always_selected_num;
    int minima_density_pool_num;
    int sellae_always_selected_num;
    int sellae_density_pool_num;

    std::vector<CritElix> spots_maxima;
    std::vector<CritElix> spots_minima;
    std::vector<CritElix> spots_sellae;
    double density_maxima_val;
    bool is_density (double val, int idx, vector<int>& spots_current);

    //statistics
    int total_num;
    int always_selected_num;
    int density_pool_num;
    int density_selected_num;

    int maxima_total_num;
    int maxima_density_selected_num;
    int maxima_always_discarded_num;
    int maxima_importance_discarded_num;
    double maxima_always_life;
    double maxima_excluded_life;

    int minima_total_num;
    int minima_density_selected_num;
    int minima_always_discarded_num;
    int minima_importance_discarded_num;
    double minima_always_life;
    double minima_excluded_life;
    
    int sellae_total_num;
    int sellae_density_selected_num;
    int sellae_always_discarded_num;
    int sellae_importance_discarded_num;
    double sellae_always_life;
    double sellae_excluded_life;


    // std::vector<DEMReader*> dems;
    ScaleSpace* ssp;
    Track* track;
    std::vector<TrackRenderingEntry> vquery;
    TrackOrdering *track_order;
    vector<int> spots_current;


    // swisstopo additional points load/save/show
  //  bool draw_csv;
    bool swpts_active;
    bool swpts_display;
    double swpts_xllcorner, swpts_yllcorner;
    double swpts_cellsize;
  //std::vector<SwissSpotHeight> swpts_ground_truth;
    
    // void swpts_load_asc (char* filename);
    // void swpts_draw ();
    // void swpts_load_csv (char* filename);
    // void swpts_save_csv (char* filename);
  
  int searchNoseTip();
  int searchNoseRoot(double, double);
  int searchUnderNose(double, double);
  void searchEyesOuterAngles(double, double,int&,int&);
  void searchEyesOuterAngles(int&,int&);
  void searchNoseLimits(double,double,int&,int&);
  void computeAxis(double&,double&,double&,double&,vector<CritNikolas>);
};


#endif
