#include "track.hh"
#include "demreader.hh"
#include "csvreader.hh"
#include "ascheader.hh"
#include "distribution_sampler.hh"

#include <vector>
#include <algorithm>

char *terrain_file = NULL;
char *curvature_file = NULL;
char *swiss_file = NULL;
char *mod_swiss_file = NULL;
char *asc_file = NULL;
char *generic_file = NULL;
double target_distance = 2.0;
double filter_curvature = 2.9;

bool do_generic2swiss;
bool do_terrain2swiss;
bool do_curvature2swiss;
bool do_terrain2curvature;
bool do_allthree;
bool query_mode = false;
bool prune_swiss_points = false;
int life_steps = 0;
int strength_steps = 0;
double life_exp = 0.0;
double strength_exp = 0.0;
double cut_borders = 0.0;

bool do_output = false;
bool do_peaks_only = false;

bool save_f05_query = true;
bool save_pr_query = false;
char* save_best_query = NULL;

std::string oname;

bool strength_user_limits = false;
bool life_user_limits = false;
double user_strength_min;
double user_strength_max;
double user_life_min;
double user_life_max;

    //     csv saves:
    // TP: mycoords + swcoords (points that match swiss and are important)
    // FP: mycoords            (points that do not match swiss and are important)
    // FN: mycoords + swcoords (points that may have matched swiss and are unimportant)
    // TRACKOUT: mycoords           (points that do not match swiss and are unimportant)
    // SWISSOUT: swiss points marked other

    // 6 databases, each divided in three classes + 1 db marked other
std::string path_swiss_tp = "points_swiss_tp.csv";
std::string path_track_tp = "points-track-tp.csv";
std::string path_track_fp = "points-track-fp.csv";
std::string path_swiss_fn = "points-swiss-fn.csv";
std::string path_track_fn = "points-track-fn.csv";
std::string path_track_out = "points-track-out.csv";
std::string path_swiss_out = "points-swiss-out.csv";

std::string path_performance = "performance.dat";

std::string path_query = "query.dat";


std::string curvature_s = "curv";
std::string terrain_s = "terr";
std::string peaks = "peaks";
std::string pits = "pits";
std::string saddles = "saddles";
std::string all = "all";


void print_help (FILE* f)
{
    fprintf (f, "Usage: spotfilter\n"
	     // "[-s scalespacefile.ssp]\n"
	     "[-o outputname] : all output files begin with outputname.\n"
	     "[-t terrain trackfile.trk]\n"
	     "[-c curvature trackfile.trk]\n"
	     "[-g generic csv input"
	     "[-s swiss input points.csv]\n"
	     "[-m modified swiss output points.csv]: dumb classifier\n"
	     "[-a dem.asc]\n"
	     "[-q 'pr'|'f05'] save best pr query or best f05 query.\n"
	     "[-Q LIFE_STEPS STRENGTH_STEPS] query mode - ROC and PR analysis.\n"
	     "[-S STRENGTH_MAX STRENGTH_MIN] keep strength values in those boundaries.\n"
	     "[-L LIFE_MAX LIFE_MIN] keep life values in those boundaries.\n"
	     "[-E LIFE_EXP STRENGTH_EXP] use an exponential distribution with given base. Zero defaults to linear.\n"
	     "[-D DISTANCE] : distance window for matching points.\n"
	     "[-F FILTER] : keep only curvature points with life higher than filter.\n"
	     "[-C CUT] : cut border areas by percentage in [0,1].\n"
	     "[-P] : prune swiss input points using classification.\n"
	     "[-M] : experiment on maxima (peaks) only.\n"
	     "\n"
	     );
}

void app_init(int argc, char *argv[])
{
    for (argc--, argv++; argc--; argv++)
    {
        if( (*argv)[0] == '-')
        {
            switch( (*argv)[1] )
            {
            case 't':
                terrain_file = (*++argv);
                argc--;
                break;

            case 'c':
                curvature_file = (*++argv);
                argc--;
                break;

            case 'o':
                oname = (*++argv);
		do_output = true;
                argc--;
                break;

	    case 'q':
                save_best_query = (*++argv);
                argc--;
		if (save_best_query != NULL && strlen(save_best_query) == 2 &&
		    save_best_query[0] == 'p' && save_best_query[1] == 'r')
		{
		    save_f05_query = false;
		    save_pr_query = true;
		}
                break;

            case 's':
                swiss_file = (*++argv);
                argc--;
                break;

            case 'g':
                generic_file = (*++argv);
                argc--;
                break;

            case 'm':
                mod_swiss_file = (*++argv);
                argc--;
                break;

            case 'a':
                asc_file = (*++argv);
                argc--;
                break;

            case 'D':
                target_distance = atof (*++argv);
                argc--;
                break;

	    case 'C':
                cut_borders = atof (*++argv);
                argc--;
                break;

	    case 'P':
		prune_swiss_points = true;
                break;

	    case 'M':
		do_peaks_only = true;
                break;

	    case 'Q':
		query_mode = true;
                life_steps = atoi (*++argv);
                argc--;
                strength_steps = atoi (*++argv);
                argc--;
                break;

	    case 'E':
                life_exp = atof (*++argv);
                argc--;
                strength_exp = atof (*++argv);
                argc--;
                break;

	    case 'S':
                user_strength_max = atof (*++argv);
                argc--;
                user_strength_min = atof (*++argv);
                argc--;
		strength_user_limits = true;
                break;

	    case 'L':
                user_life_max = atof (*++argv);
                argc--;
                user_life_min = atof (*++argv);
                argc--;
		life_user_limits = true;
                break;

            case 'F':
                filter_curvature = atof (*++argv);
                argc--;
                break;

            default:
                goto die;
            }
        }
    }

    do_generic2swiss = generic_file && swiss_file;
    do_terrain2curvature = terrain_file && curvature_file;
    do_curvature2swiss = curvature_file && swiss_file;
    do_terrain2swiss = terrain_file && swiss_file;
    do_allthree = terrain_file && curvature_file && swiss_file;

    if (cut_borders < 0.0 || cut_borders > 1.0)
    {
	fprintf (stderr, "Cut border value %lf not valid. Range should be [0,1].\n",
		 cut_borders);
	goto die;	
    }
    
    if (!do_terrain2curvature && !do_curvature2swiss
	&& !do_terrain2swiss && !do_generic2swiss)
    {
	fprintf (stderr, "Two datasets are needed to make a comparison.\n\n");
	goto die;
    }
    
    if ((swiss_file != NULL || generic_file != NULL) && asc_file == NULL)
    {
	fprintf (stderr, "Error: asc file needed for csv"
		 " points input/output.\n\n");
	goto die;
    }

    return;

 die:
    print_help(stderr);
    exit(1);
}



Track *curvature_track;
Track *terrain_track;
int width, height;
// double cellsize, xllcorner, yllcorner;
ASCHeader asch;
TrackOrdering *terrain_track_order;
TrackOrdering *curvature_track_order;

// faccio dei vettori di points che corrispondono a trackspot e swisspotheight
// li uso per fare la funzione fill
// il resto puo' essere funzione order/create etc a seconda

std::vector <TrackSpot> terrain;
std::vector <TrackSpot> curvature;
int terrain_peak_num = 0;
int terrain_pit_num = 0;
int terrain_saddle_num = 0;
double generic_life_max;
double generic_strength_max;
double terrain_all_life_max;
double terrain_all_strength_max;
double terrain_peaks_life_max;
double terrain_peaks_strength_max;
double terrain_pits_life_max;
double terrain_pits_strength_max;
double terrain_saddles_life_max;
double terrain_saddles_strength_max;
double curvature_life_max;
double curvature_strength_max;
std::vector <SwissSpotHeight> swiss;
std::vector <SwissSpotHeight> generic;
std::vector <SwissSpotHeight> swiss_peaks;
std::vector <SwissSpotHeight> swiss_pits;
std::vector <SwissSpotHeight> swiss_saddles;

std::vector <Point> terrain_points;
std::vector <Point> curvature_points;
std::vector <Point> swiss_points;
std::vector <Point> generic_points;
std::vector <Point> swiss_peaks_points;
std::vector <Point> swiss_pits_points;
std::vector <Point> swiss_saddles_points;


class Ref {
public:
    int idx;
    double dist;
    Ref (int idx, double dist) : idx (idx), dist (dist) {}
    bool operator<(const Ref& rhs) const { return (dist < rhs.dist); }
};

std::vector <std::vector <Ref> > terrain2swiss;
std::vector <std::vector <Ref> > curvature2swiss;
std::vector <std::vector <Ref> > terrain2curvature;

// count and compute tp, fp, fn, tn from f2s and s
void confusion (std::vector <std::vector <Ref> >& f2s, std::vector<Point>& s,
	       std::vector<int>& tp, std::vector<int>& fp,
	       std::vector<int>& fn, int* tn_count);

// fill f2s vector of refs of points in s that are within target_dist of points in f
void fill_match (std::vector <Point>& f, std::vector <Point>& s,
		 std::vector <std::vector <Ref> >& f2s,
		 int* duplicates = NULL, int* maxcandidates = NULL);

int count_found (std::vector <std::vector <Ref> >& f2s);

void load_all ();

void count_all ();

// plot life and strength of spots, optionally using only points in idxs
void save_sort_points (std::vector <TrackSpot>& spots,
		       bool do_idxs, std::vector<int>& idxs,
		       std::string fn_life, std::string fn_strength);

// plot life and strength of spots points, along with tp and fp membership
void save_lifestrength (std::vector <TrackSpot>& spots,
			     std::vector<int>& idxs_tp, std::vector<int>& idxs_fp,
			     std::string fn);

// save set of swiss points marked with type classification. dumb automatic version
void classify_swiss (std::vector <SwissSpotHeight> s,
		     std::vector <TrackSpot> t,
		     std::vector <std::vector <Ref> > t2s,
		     std::string fn);

void main_generic ();
void main_query ();
void main_old ();

int main (int argc, char *argv[])
{
    app_init (argc, argv);

    load_all ();

    if (query_mode && generic_file)
	main_generic ();
    else if (query_mode)
	main_query ();
    else
	main_old ();
}

void query_track (Track* track, double life, double strength,
		  std::vector <TrackSpot>& spots,
		  std::vector <Point>& points,
		  ClassifiedType st = ALL);

char* roc_header = "x y life strength tpr fpr tnr fnr ppv npv "
		  "f1 f2 f05 informedness markedness mcc tp fp fn tn "
		  "duplicates maxcandidates";
enum roc_types_t {
    TPR_NUM=0, FPR_NUM, TNR_NUM, FNR_NUM,
    PPV_NUM, NPV_NUM,
    F1_NUM, F2_NUM, F05_NUM,
    INFORMEDNESS_NUM, MARKEDNESS_NUM, MCC_NUM,
} roc_types;
#define ROC_TYPES_NUMS 12
void write_roc_stats (FILE* f, int i, int j, bool last, double life, double strength,
		      double tp, double fp, double fn, double tn,
		      int duplicates, int maxcandidates,
		      std::vector<double>& results)
{
    double tpr = tp / (tp + fn);
    double fpr = fp / (fp + tn);
    double tnr = tn / (fp + tn);
    double fnr = fn / (tp + fn);

    double ppv = tp / (tp + fp);
    double npv = tn / (tn + fn);

    double f1 = 2.0 * ppv * tpr / (ppv + tpr);
    double f2 = 5.0 * ppv * tpr / ((5 * ppv) + tpr);
    double f05 = 1.25 * ppv * tpr / ((0.25 * ppv) + tpr);

    double informedness = tpr + tnr - 1.0;
    double markedness = ppv + npv - 1.0;
    double mcc =
	((tp * tn) - (fp * fn)) /
	sqrt ((tp + fn) * (fp + tn) * (tp + fp) * (fn + tn));
    
    // salva dati per ROC
    fprintf (f, "%d %d %lf %lf "
	     "%lf %lf %lf %lf %lf %lf "
	     "%lf %lf %lf %lf %lf %lf "
	     "%.0f %.0f %.0f %.0f %d %d\n ",
	     i, j, life, strength,
	     tpr, fpr, tnr, fnr, ppv, npv,
	     f1, f2, f05, informedness, markedness, mcc,
	     tp, fp, fn, tn, duplicates, maxcandidates);

    if (last)
	fprintf (f, "\n");

    results.clear ();
    results.resize (ROC_TYPES_NUMS);

    results[TPR_NUM] = tpr;
    results[FPR_NUM] = fpr;
    results[TNR_NUM] = tnr;
    results[FNR_NUM] = fnr;
    results[PPV_NUM] = ppv;
    results[NPV_NUM] = npv;
    results[F1_NUM] = f1;
    results[F2_NUM] = f2;
    results[F05_NUM] = f05;
    results[INFORMEDNESS_NUM] = informedness;
    results[MARKEDNESS_NUM] = markedness;
    results[MCC_NUM] = mcc;
    
    //return mcc;
    // return f05;
}

void query_generic (std::vector <SwissSpotHeight>& spots_in,
		    double life, double strength,
		    std::vector <SwissSpotHeight>& spots,
		    std::vector <Point>& points,
		    ClassifiedType st);

void query_and_save (std::string prepend, double life, double strength,
		     std::vector <SwissSpotHeight>& sw, std::vector<Point>& swps,
		     ClassifiedType st = ALL);

void main_generic ()
{
    double best_query_f05 = -DBL_MAX;
    double best_query_mcc = -DBL_MAX;
    double best_query_roc_dist = DBL_MAX;
    double best_query_pr_dist = DBL_MAX;
    Point best_query_roc_point;
    Point best_query_pr_point;
    double life_best_f05, strength_best_f05;
    double life_best_mcc, strength_best_mcc;
    double life_best_roc, strength_best_roc;
    double life_best_pr, strength_best_pr;

    DistributionSampler life_sampler (generic_life_max, 0.0, life_steps, life_exp);
    DistributionSampler strength_sampler (generic_strength_max, 0.0, strength_steps, strength_exp);

    std::string output = oname+"-"+peaks+"-"+path_performance; // path_roc_t_max;
    // kt_swps = swiss_peaks_points;
    // kt_sw = swiss_peaks;
    // kt_st = PEAK;
    // kt_name = peaks;
    printf ("\n -- PEAKS - swiss[%zu] - [%zu] --\n",
    	    swiss_peaks.size(), generic.size());
    
    best_query_f05 = -DBL_MAX;
    best_query_mcc = -DBL_MAX;
    best_query_roc_dist = DBL_MAX;
    best_query_pr_dist = DBL_MAX;
	
    FILE* fp = fopen (output.c_str(), "w");
    fprintf (fp, "%s\n", roc_header);
    
    for (int i = 0; i < life_steps; i++)
	for (int j = 0; j < strength_steps; j++)
	{
	    // double life_s = (generic_life_max / (double) life_steps) * i;
	    // double strength_s = (generic_strength_max / (double) strength_steps) * j;
	    double life_s = life_exp == 0.0?
		life_sampler.lin (i) :
		life_sampler.exp_ts (i);
	    double strength_s = strength_exp == 0.0?
		strength_sampler.lin (j) :
		strength_sampler.exp_ts (j);

	    // printf ("i %d j %d -- ls %lf ss %lf -- lmax %lf smax %lf\n",
	    // 	    i, j, life_s, strength_s, generic_life_max, generic_strength_max);

	    // query estrai punti del terreno: terrain + terrain2swiss
	    std::vector <SwissSpotHeight> query_t;
	    std::vector <Point> query_tp;
	    std::vector <std::vector <Ref> > query_t2s;

	    // query_t.clear ();
	    // query_tp.clear ();
	    // query_t2s.clear ();
	    query_generic (generic, life_s, strength_s,
			   query_t, query_tp, PEAK); // here choose type
	
	    int duplicates;
	    int maxcandidates;
	    fill_match (query_tp, swiss_peaks_points, query_t2s,
			&duplicates, &maxcandidates);
	    
	    std::vector <int> t2s_tp, t2s_fp, t2s_fn;
	    int t2s_tn = 0;
	    // t2s_tp.clear ();
	    // t2s_fp.clear ();
	    // t2s_fn.clear ();
	    // t2s_tn = 0;
	    confusion (query_t2s, swiss_peaks_points, t2s_tp, t2s_fp, t2s_fn, &t2s_tn);

	    std::vector<double> query_results;
	    query_results.clear ();
	    write_roc_stats (fp, i, j, j == strength_steps-1,
			     life_s, strength_s,
			     t2s_tp.size (), t2s_fp.size (),
			     t2s_fn.size (), t2s_tn, duplicates, maxcandidates,
			     query_results);

	    double query_mcc;
	    query_mcc = query_results[MCC_NUM];
	    double query_f05;
	    query_f05 = query_results[F05_NUM];
	    Point query_roc_point;
	    query_roc_point = Point(query_results[FPR_NUM], query_results[TPR_NUM]);
	    double query_roc_dist;
	    query_roc_dist = point_distance (Point (0.0, 1.0), query_roc_point);
	    Point query_pr_point;
	    query_pr_point = Point (query_results[TPR_NUM], query_results[PPV_NUM]);
	    double query_pr_dist;
	    query_pr_dist = point_distance (Point (1.0, 1.0), query_pr_point);
	    
	    // di quale parametro devo memorizzare la query?
	    // memorizza parametri query piu' vicina a 1,1
	    // sostituire con quello ritornato 
	    if (best_query_mcc <= query_mcc)
	    {
		life_best_mcc = life_s;
		strength_best_mcc = strength_s;
		best_query_mcc = query_mcc;
	    }
	    
	    if (best_query_f05 <= query_f05)
	    {
		life_best_f05 = life_s;
		strength_best_f05 = strength_s;
		best_query_f05 = query_f05;
	    }
	    
	    if (best_query_roc_dist >= query_roc_dist)
	    {
		life_best_roc = life_s;
		strength_best_roc = strength_s;
		best_query_roc_dist = query_roc_dist;
		best_query_roc_point = query_roc_point;
	    }
	    if (best_query_pr_dist >= query_pr_dist)
	    {
		life_best_pr = life_s;
		strength_best_pr = strength_s;
		best_query_pr_dist = query_pr_dist;
		best_query_pr_point = query_pr_point;
	    }
	}
    fclose (fp);
    
    fprintf (stdout, "best query mcc: %lf, life: %lf, strength: %lf\n",
	     best_query_mcc, life_best_mcc, strength_best_mcc);
    fprintf (stdout, "best query f05: %lf, life: %lf, strength: %lf\n",
	     best_query_f05, life_best_f05, strength_best_f05);
    fprintf (stdout, "best query roc: %lf (%lf,%lf), life: %lf, strength: %lf\n",
	     best_query_roc_dist,
	     best_query_roc_point.x, best_query_roc_point.y,
	     life_best_roc, strength_best_roc);
    fprintf (stdout, "best query pr: %lf (%lf,%lf), life: %lf, strength: %lf\n",
	     best_query_pr_dist,
	     best_query_pr_point.x, best_query_pr_point.y, 
	     life_best_pr, strength_best_pr);

    // -- GG should have query_save_generic that calls query_generic, not query_track
    // fprintf (stdout, "saving f05 query\n");
    // query_and_save (oname+"-"+peaks, life_best_f05, strength_best_f05,
    // 		    swiss_peaks, swiss_peaks_points, PEAK);

    // -- GG what's the point here?
    // CSVReader csvio (asch);
    // std::vector<SwissSpotHeight> other;
    // for (unsigned i = 0; i < swiss.size(); i++)
    // 	if (swiss[i].type () == OTHER)
    // 	    other.push_back (swiss[i]);
    // csvio.save ((oname+"-"+path_swiss_out).c_str(), other);
}


void main_query ()
{
    // per sequenza di vita
    // e sequenza di forza
    double best_query_f05 = -DBL_MAX;
    double best_query_mcc = -DBL_MAX;
    double best_query_roc_dist = DBL_MAX;
    double best_query_pr_dist = DBL_MAX;
    Point best_query_roc_point;
    Point best_query_pr_point;
    double life_best_f05, strength_best_f05;
    double life_best_mcc, strength_best_mcc;
    double life_best_roc, strength_best_roc;
    double life_best_pr, strength_best_pr;

    for (int kt = 0; kt < 4; kt++)
    {
	if (!prune_swiss_points && kt != 3)
	    continue;

	if (do_peaks_only && kt != 0)
	    continue;
	
	std::string kt_output;
	std::vector<Point> kt_swps;
	std::vector <SwissSpotHeight> kt_sw;
	ClassifiedType kt_st;
	std::string kt_name;
	double kt_life_max, kt_strength_max;

	switch (kt)
	{
	case 0: // max
	    kt_output = oname+"-"+peaks+"-"+path_performance; // path_roc_t_max;
	    kt_swps = swiss_peaks_points;
	    kt_sw = swiss_peaks;
	    kt_st = PEAK;
	    kt_name = peaks;
	    kt_life_max = terrain_peaks_life_max;
	    kt_strength_max = terrain_peaks_strength_max;
	    printf ("\n -- PEAKS - swiss[%zu] - track[%d] --\n",
		    swiss_peaks.size(), terrain_peak_num++);
	    break;
	case 1: // min
	    kt_output = oname+"-"+pits+"-"+path_performance; //path_roc_t_min;
	    kt_swps = swiss_pits_points;
	    kt_sw = swiss_pits;
	    kt_st = PIT;
	    kt_name = pits;
	    kt_life_max = terrain_pits_life_max;
	    kt_strength_max = terrain_pits_strength_max;
	    printf ("\n -- PITS - swiss[%zu] - track[%d] --\n",
		    swiss_pits.size(), terrain_pit_num++);
	    break;
	case 2: // sad
	    kt_output = oname+"-"+saddles+"-"+path_performance; //path_roc_t_sad;
	    kt_swps = swiss_saddles_points;
	    kt_sw = swiss_saddles;
	    kt_st = SADDLE;
	    kt_name = saddles;
	    kt_life_max = terrain_saddles_life_max;
	    kt_strength_max = terrain_saddles_strength_max;
	    printf ("\n -- SADDLES - swiss[%zu] - track[%d] --\n",
		    swiss_saddles.size(), terrain_saddle_num++);
	    break;
	case 3: // all
	    kt_output = oname+"-"+all+"-"+path_performance; //path_roc_t;
	    kt_swps = swiss_points;
	    kt_sw = swiss;
	    kt_st = ALL;
	    kt_name = all;
	    kt_life_max = terrain_all_life_max;
	    kt_strength_max = terrain_all_strength_max;
	    printf ("\n -- ALL - swiss[%zu] - track[%zu] --\n",
		    swiss.size(), terrain.size());
	    break;
	default:
	    break;
	}

	best_query_f05 = -DBL_MAX;
	best_query_mcc = -DBL_MAX;
	best_query_roc_dist = DBL_MAX;
	best_query_pr_dist = DBL_MAX;
	
	FILE* fp = fopen (kt_output.c_str(), "w");
	fprintf (fp, "%s\n", roc_header);

	float smax, smin;
	if (kt == 0 && strength_user_limits)
	{
	    smax = user_strength_max;
	    smin = user_strength_min;
	}
	else
	{
	    smax = kt_strength_max;
	    smin = 0.0;
	}
	float lmax, lmin;
	if (kt == 0 && life_user_limits)
	{
	    lmax = user_life_max;
	    lmin = user_life_min;
	}
	else
	{
	    lmax = kt_life_max;
	    lmin = 0.0;
	}
	DistributionSampler life_sampler (lmax, lmin, life_steps, life_exp);
	DistributionSampler strength_sampler (smax, smin, strength_steps, strength_exp);

	for (int i = 0; i < life_steps; i++)
	    for (int j = 0; j < strength_steps; j++)
	    {
		// double life_s = (terrain_life_max / (double) life_steps) * i;
		// double strength_s = (terrain_strength_max / (double) strength_steps) * j;
		double life_s = life_exp == 0.0?
		    life_sampler.lin (i) :
		    life_sampler.exp_ts (i);
		double strength_s = strength_exp == 0.0?
		    strength_sampler.lin (j) :
		    strength_sampler.exp_ts (j);

		// printf ("!!! [%d] - %lf - max %lf min %lf exp %lf\n",
		// 	j, strength_s, smax, smin, strength_exp);
		
		// query estrai punti del terreno: terrain + terrain2swiss
		std::vector <TrackSpot> query_t;
		std::vector <Point> query_tp;
		std::vector <std::vector <Ref> > query_t2s;

		// query_t.clear ();
		// query_tp.clear ();
		// query_t2s.clear ();
		query_track (terrain_track, life_s, strength_s,
			     query_t, query_tp, kt_st); // here choose type
		int duplicates;
		int maxcandidates;
		fill_match (query_tp, kt_swps, query_t2s, &duplicates, &maxcandidates);
	    
		std::vector <int> t2s_tp, t2s_fp, t2s_fn;
		int t2s_tn = 0;
		// t2s_tp.clear ();
		// t2s_fp.clear ();
		// t2s_fn.clear ();
		// t2s_tn = 0;
		confusion (query_t2s, kt_swps, t2s_tp, t2s_fp, t2s_fn, &t2s_tn);

		std::vector<double> query_results;
		query_results.clear ();
		write_roc_stats (fp, i, j, j == strength_steps-1,
				 life_s, strength_s,
				 t2s_tp.size (), t2s_fp.size (),
				 t2s_fn.size (), t2s_tn, duplicates, maxcandidates,
				 query_results);

		double query_mcc;
		query_mcc = query_results[MCC_NUM];
		double query_f05;
		query_f05 = query_results[F05_NUM];
		Point query_roc_point;
		query_roc_point = Point(query_results[FPR_NUM], query_results[TPR_NUM]);
		double query_roc_dist;
		query_roc_dist = point_distance (Point (0.0, 1.0), query_roc_point);
		Point query_pr_point;
		query_pr_point = Point (query_results[TPR_NUM], query_results[PPV_NUM]);
		double query_pr_dist;
		query_pr_dist = point_distance (Point (1.0, 1.0), query_pr_point);
	    
		// di quale parametro devo memorizzare la query?
		// memorizza parametri query piu' vicina a 1,1
		// sostituire con quello ritornato 
		if (best_query_mcc <= query_mcc)
		{
		    life_best_mcc = life_s;
		    strength_best_mcc = strength_s;
		    best_query_mcc = query_mcc;
		}

		if (best_query_f05 <= query_f05)
		{
		    life_best_f05 = life_s;
		    strength_best_f05 = strength_s;
		    best_query_f05 = query_f05;
		}

		if (best_query_roc_dist >= query_roc_dist)
		{
		    life_best_roc = life_s;
		    strength_best_roc = strength_s;
		    best_query_roc_dist = query_roc_dist;
		    best_query_roc_point = query_roc_point;
		}
		if (best_query_pr_dist >= query_pr_dist)
		{
		    life_best_pr = life_s;
		    strength_best_pr = strength_s;
		    best_query_pr_dist = query_pr_dist;
		    best_query_pr_point = query_pr_point;
		}
	    }
	fclose (fp);

	fprintf (stdout, "best query mcc: %lf, life: %lf, strength: %lf\n",
		 best_query_mcc, life_best_mcc, strength_best_mcc);
	fprintf (stdout, "best query f05: %lf, life: %lf, strength: %lf\n",
		 best_query_f05, life_best_f05, strength_best_f05);
	fprintf (stdout, "best query roc: %lf (%lf,%lf), life: %lf, strength: %lf\n",
		 best_query_roc_dist,
		 best_query_roc_point.x, best_query_roc_point.y,
		 life_best_roc, strength_best_roc);
	fprintf (stdout, "best query pr: %lf (%lf,%lf), life: %lf, strength: %lf\n",
		 best_query_pr_dist,
		 best_query_pr_point.x, best_query_pr_point.y, 
		 life_best_pr, strength_best_pr);

	if (save_f05_query)
	{
	    fprintf (stdout, "saving f05 query\n");
	    query_and_save (oname+"-"+kt_name, life_best_f05, strength_best_f05, kt_sw, kt_swps, kt_st);
	}
	if (save_pr_query)
	{
	    fprintf (stdout, "saving pr query\n");
	    query_and_save (oname+"-"+kt_name, life_best_pr, strength_best_pr, kt_sw, kt_swps, kt_st);
	}
    }
    
    CSVReader csvio (asch);
    std::vector<SwissSpotHeight> other;
    for (unsigned i = 0; i < swiss.size(); i++)
	if (swiss[i].type () == OTHER)
	    other.push_back (swiss[i]);
    csvio.save ((oname+"-"+path_swiss_out).c_str(), other);
}

void query_and_save (std::string prepend, double life, double strength,
		     std::vector<SwissSpotHeight>& sw, std::vector<Point>& swps,
		     ClassifiedType st)
{
    
    std::vector <TrackSpot> query_t;
    std::vector <Point> query_tp;
    std::vector <std::vector <Ref> > query_t2s;

    query_track (terrain_track, life, strength,
		 query_t, query_tp, st);
    fill_match (query_tp, swps, query_t2s);

    // salva roba terreno
    std::vector <int> t2s_tp, t2s_fp, t2s_fn;
    int t2s_tn = 0;
    confusion (query_t2s, swps, t2s_tp, t2s_fp, t2s_fn, &t2s_tn);

    save_lifestrength (query_t, t2s_tp, t2s_fp, prepend+"-"+path_query);

    // save csv points sets
    CSVReader csvio (asch);
    std::vector<bool> swiss_marked (sw.size (),  false);
    std::vector<SwissSpotHeight> swiss_tp;
    std::vector<SwissSpotHeight> track_tp;
    std::vector<SwissSpotHeight> track_fp;
    std::vector<SwissSpotHeight> swiss_fn;
    std::vector<SwissSpotHeight> track_fn;
    std::vector<SwissSpotHeight> track_out;
    
    for (unsigned i = 0; i < query_t2s.size (); i++)
    {
	
	if (query_t2s[i].size () > 0)
	{
	    swiss_tp.push_back (sw[query_t2s[i][0].idx]);
	    swiss_marked[query_t2s[i][0].idx] = true;
	    track_tp.push_back (SwissSpotHeight (query_t[i]));
	}
	else
	    track_fp.push_back (SwissSpotHeight(query_t[i]));
    }

    for (unsigned i = 0; i < sw.size(); i++)
	if (!swiss_marked[i])
	    swiss_fn.push_back (sw[i]);

    // make query 0,0
    std::vector <TrackSpot> qzero_t;
    std::vector <Point> qzero_tp;
    std::vector <std::vector <Ref> > qzero_tz2s;
    query_track (terrain_track, 0.0, 0.0, qzero_t, qzero_tp, st);
    fill_match (qzero_tp, swps, qzero_tz2s);
    std::vector <int> tz2s_tp, tz2s_fp, tz2s_fn;
    int tz2s_tn = 0;
    confusion (qzero_tz2s, swps, tz2s_tp, tz2s_fp, tz2s_fn, &tz2s_tn);

    // track_fn: track points that are tp in 0,0 and match with swiss points in fn.
    for (unsigned i = 0; i < qzero_tz2s.size(); i++)
	for (unsigned j = 0; j < swiss_fn.size(); j++)
	    if (qzero_tz2s[i].size () > 0 && sw[qzero_tz2s[i][0].idx].p == swiss_fn[j].p)
		track_fn.push_back (SwissSpotHeight (qzero_t[i]));
		
    // track_out: points that are fp in 0,0 and are not in track_fp
    for (unsigned i = 0; i < qzero_tz2s.size(); i++)
    {
	bool found = false;
	for (unsigned j = 0; j < track_fp.size(); j++)
	    if (query_t2s[i].size () == 0 && qzero_tp[i] == track_fp[j].p)
		found = true;
	if (!found)
	    track_out.push_back (SwissSpotHeight (qzero_t[i]));	    
    }
    
    // write

    csvio.save ((prepend+"-"+path_swiss_tp).c_str(), swiss_tp);
    csvio.save ((prepend+"-"+path_track_tp).c_str(), track_tp);
    csvio.save ((prepend+"-"+path_track_fp).c_str(), track_fp);
    csvio.save ((prepend+"-"+path_swiss_fn).c_str(), swiss_fn);
    csvio.save ((prepend+"-"+path_track_fn).c_str(), track_fn);
    csvio.save ((prepend+"-"+path_track_out).c_str(), track_out);
  
    fprintf (stdout, "in best %s query: tp: %zu, fp: %zu, fn: %zu\n",
	     prepend.c_str(), swiss_tp.size (), track_fp.size (), swiss_fn.size ());
}

void main_old ()
{
    if (do_terrain2swiss)
	fill_match (terrain_points, swiss_points, terrain2swiss);
    
    if (do_curvature2swiss)
	fill_match (curvature_points, swiss_points, curvature2swiss);

    if (do_terrain2curvature)
	fill_match (terrain_points, curvature_points, terrain2curvature);
    
    count_all ();

    if (mod_swiss_file)
	classify_swiss (swiss, terrain, terrain2swiss, mod_swiss_file);

    if (!do_output)
	exit (0);

    // if (terrain_file)
    // {
    // 	std::vector<int> foo;
    // 	save_sort_points (terrain, false, foo, path_sort_t_l, path_sort_t_s);
    // }

    // if (curvature_file)
    // {
    // 	std::vector<int> foo;
    // 	save_sort_points (curvature, false, foo, path_sort_c_l, path_sort_c_s);
    // }



    // make function of this part with query in mind.
    // make better counts and reporting with query in mind.
    // this part works on both curvature and terrain
    std::vector <int> t2s_tp, t2s_fp, t2s_fn;
    int t2s_tn = 0;
    std::vector <int> c2s_tp, c2s_fp, c2s_fn;
    int c2s_tn = 0;

    if (do_terrain2swiss)
    {
	confusion (terrain2swiss, swiss_points, t2s_tp, t2s_fp, t2s_fn, &t2s_tn);

	// sort tp, fp by stre,life. save dat
	// save_sort_points (terrain, true, t2s_tp, path_sort_t_tp_l, path_sort_t_tp_s);
	// save_sort_points (terrain, true, t2s_fp, path_sort_t_fp_l, path_sort_t_fp_s);

	// save_sort_lifestrength (terrain, t2s_tp, t2s_fp, path_sort_t_ls);
	save_lifestrength (terrain, t2s_tp, t2s_fp, oname+"-"+terrain_s+"-"+path_query);
    }

    if (do_curvature2swiss)
    {
	confusion (curvature2swiss, swiss_points, c2s_tp, c2s_fp, c2s_fn, &c2s_tn);

	// sort tp, fp by stre,life. save dat
	// save_sort_points (curvature, true, c2s_tp, path_sort_c_tp_l, path_sort_c_tp_s);
	// save_sort_points (curvature, true, c2s_fp, path_sort_c_fp_l, path_sort_c_fp_s);

	// save_sort_lifestrength (curvature, c2s_tp, c2s_fp, path_sort_c_ls);
	save_lifestrength (curvature, c2s_tp, c2s_fp, oname+"-"+curvature_s+"-"+path_query);
    }

    std::vector<bool> curvature_marked (curvature.size (), false);
    std::vector<bool> swiss_marked (swiss.size (),  false);

    std::vector<SwissSpotHeight> st_tp;
    std::vector<SwissSpotHeight> sc_tp;
    std::vector<SwissSpotHeight> t_fp;
    std::vector<SwissSpotHeight> c_fp;

    std::vector<SwissSpotHeight> s_fn;

    if (do_terrain2swiss)
    {
	// terrain tp, mark curv tp, mark swiss fn, terrain fp.
	for (unsigned i = 0; i < terrain2swiss.size (); i++)
	{
	    if (!do_allthree)
	    {
		if (terrain2swiss[i].size () > 0)
		{
		    st_tp.push_back (swiss[terrain2swiss[i][0].idx]);
		    swiss_marked[terrain2swiss[i][0].idx] = true;
		}
		else
		    t_fp.push_back (SwissSpotHeight(terrain_points[i]));
		continue;
	    }

	    // there is also curvature
	    if (terrain2swiss[i].size () > 0 &&
		terrain2curvature[i].size () > 0 &&
		terrain[i].life >= curvature[terrain2curvature[i][0].idx].life
		) // also in curvature but terrain wins
	    {
		curvature_marked[terrain2curvature[i][0].idx] = true;
		st_tp.push_back (swiss[terrain2swiss[i][0].idx]);
		swiss_marked[terrain2swiss[i][0].idx] = true;
	    }
	    else if (terrain2swiss[i].size () > 0 &&
		     terrain2curvature[i].size () > 0 &&
		     terrain[i].life < curvature[terrain2curvature[i][0].idx].life
		) // also in curvature and curvature wins
		;
	    else if (terrain2swiss[i].size () > 0 &&
		     terrain2curvature[i].size () == 0
		     ) // in terrain but not in curvature
	    {
		st_tp.push_back (swiss[terrain2swiss[i][0].idx]);
		swiss_marked[terrain2swiss[i][0].idx] = true;
	    }
	    else
		t_fp.push_back (SwissSpotHeight(terrain_points[i]));
	}
    }
    
    if (do_allthree)
    {
	// curvature tp, mark swiss fn, curvature fp.
	for (unsigned i = 0; i < curvature2swiss.size (); i++)
	    if (curvature2swiss[i].size () > 0 && !curvature_marked[i])
	    {
		sc_tp.push_back (swiss[curvature2swiss[i][0].idx]);
		swiss_marked[curvature2swiss[i][0].idx] = true;
	    }
	    else if (!curvature_marked[i])
		c_fp.push_back (SwissSpotHeight(curvature_points[i]));
    }

    if (do_terrain2swiss || do_allthree)
    {	// swiss fn
	for (unsigned i = 0; i < swiss.size(); i++)
	    if (!swiss_marked[i])
		s_fn.push_back (swiss[i]);
    }
    
    CSVReader csvio (asch);
    if (do_terrain2swiss || do_allthree)
    {
	// csvio.save (path_gt_fn.c_str(), s_fn);
	// csvio.save (path_t_fp.c_str(), t_fp);				
	// csvio.save (path_gt_t_tp.c_str(), st_tp);
	csvio.save ((oname+"-"+terrain_s+"-"+path_swiss_fn).c_str(), s_fn);
	csvio.save ((oname+"-"+terrain_s+"-"+path_track_fp).c_str(), t_fp);				
	csvio.save ((oname+"-"+terrain_s+"-"+path_swiss_tp).c_str(), st_tp);
    }
    if (do_allthree)
    {
	// csvio.save (path_c_fp.c_str(), c_fp);				
	// csvio.save (path_gt_c_tp.c_str(), sc_tp);
	csvio.save ((oname+"-"+curvature_s+"-"+path_track_fp).c_str(), c_fp);				
	csvio.save ((oname+"-"+curvature_s+"-"+path_swiss_tp).c_str(), sc_tp);
    }
    // GG count all better prints - or use data from confusion
}

void count_all ()
{
    int t = terrain.size ();
    int c = curvature.size ();
    int s = swiss.size ();
    int t2s, c2s, t2c, t2s2c;
    t2s = c2s = t2c = t2s2c = 0;

    printf ("TOTALS :: terrain: %d, curvature: %d, swiss: %d.\n"
	    "distance test: %lf. pixels in dataset: %d\n",
	    t, c, s, target_distance, width * height);
    
    if (do_terrain2swiss)
    {
	t2s = count_found (terrain2swiss);
	printf ("Terrain vs Swiss | found: %d, "
		"terrain left out: %d, swiss left out: %d\n", t2s, t-t2s, s-t2s);
    }
    
    if (do_curvature2swiss)
    {
	c2s = count_found (curvature2swiss);
	printf ("Curvature vs Swiss | found: %d, "
		"curvature left out: %d, swiss left out: %d\n", c2s, c-c2s, s-c2s);
    }
    
    if (do_terrain2curvature)
    {
	t2c = count_found (terrain2curvature);
	printf ("Terrain vs Curvature | found: %d, "
		"terrain left out: %d, curvature left out: %d\n\n", t2c, t-t2c, c-t2c);
    }
    
    // contare tutti i punti in terrain2swiss che hanno qualcuno anche in terrain2curv 
    if (do_allthree)
    {
	for (unsigned i = 0; i < terrain2swiss.size (); i++)
	{
	    if (terrain2swiss[i].size () > 0 && terrain2curvature[i].size () > 0)
		t2s2c++;
	}

	int tc = t + c;
	int tc2s = t2s + c2s - t2s2c;
	printf ("T+C total: %d. \n", tc);
	printf ("T+C vs Swiss | found:%d, "
		"left out in T+C: %d, left out in swiss: %d\n", tc2s, tc-tc2s, s-tc2s);
	printf ("At the same time in Terrain/Curvature/Swiss: %d\n", t2s2c);
    }
}

// void save_sort_points (std::vector <TrackSpot>& spots,
// 		       bool do_idxs, std::vector<int>& idxs,
// 		       std::string filename_life, std::string filename_strength)
// {
//     FILE* fp;
//     std::vector<TrackSpot> trsp;

//     if (do_idxs)
// 	for (unsigned i = 0; i < idxs.size(); i++)
// 	    trsp.push_back (spots[idxs[i]]);
//     else
// 	for (unsigned i = 0; i < spots.size(); i++)
// 	    trsp.push_back (spots[i]);
	
//     // life
//     std::sort (trsp.begin (), trsp.end (), compare_life);
//     fp = fopen (filename_life.c_str(), "w");
//     for (unsigned i = 0; i < trsp.size(); i++)
// 	fprintf (fp, "%lf\n", trsp[i].life);
//     fclose (fp);
    
//     // strength
//     std::sort (trsp.begin (), trsp.end (), compare_strength);
//     fp = fopen (filename_strength.c_str(), "w");
//     for (unsigned i = 0; i < trsp.size(); i++)
// 	fprintf (fp, "%lf\n", trsp[i].strength);
//     fclose (fp);
// }

void save_lifestrength (std::vector <TrackSpot>& spots,
			std::vector<int>& idxs_tp, std::vector<int>& idxs_fp,
			std::string fn)
{
    FILE* filep;
    std::vector<TrackSpot> trsp;
    std::vector<bool> tp (spots.size(), false);
    std::vector<bool> fp (spots.size(), false);
    std::vector<bool> out (spots.size(), false);

    for (unsigned i = 0; i < spots.size(); i++)
	trsp.push_back (spots[i]);

    for (unsigned i = 0; i < idxs_tp.size(); i++)
	tp[idxs_tp[i]] = true;

    // GG WARN ADDED THIS FOR WHEN MAKE DEBUG COMPLAINED ABOUT idxs_fp not used
    // you don't really understand what's going on here
    for (unsigned i = 0; i < idxs_fp.size(); i++)
	fp[idxs_fp[i]] = true;

    for (unsigned i = 0; i < spots.size(); i++)
	if (!tp[i] && !fp[i])
	    out[i] = true;

    // life
    filep = fopen (fn.c_str(), "w");
    fprintf (filep, "life strength tp fp out\n");
    for (unsigned i = 0; i < trsp.size(); i++)
    {
	fprintf (filep, "%lf %lf %d %d %d\n",
		 trsp[i].life, trsp[i].strength, (int)tp[i], (int)fp[i], (int)out[i]);
    }
    fclose (filep);
}


int count_found (std::vector <std::vector <Ref> >& f2s)
{
    int f = 0;
    for (unsigned i = 0; i < f2s.size (); i++)
	if (f2s[i].size () > 0)
	    f++;
    return f;
}

void confusion (std::vector <std::vector <Ref> >& f2s, std::vector<Point>& s,
	       std::vector<int>& tp, std::vector<int>& fp,
	       std::vector<int>& fn, int* tn_count)
{
    std::vector<bool> mark (s.size (), false);
    tp.clear ();
    fp.clear ();
    fn.clear ();
    
    for (unsigned i = 0; i < f2s.size (); i++)
    {
	if (f2s[i].size () > 0) // tp
	{
	    tp.push_back (i);
	    mark[f2s[i][0].idx] = true;
	}
	else // fp
	    fp.push_back (i);
    }

    for (unsigned i = 0; i < mark.size (); i++)
	if (!mark[i])
	    fn.push_back (i);

    *tn_count = (width * height) - tp.size() - fp.size() - fn.size();
}

// bool ref_comparison (const std::vector <Ref>& v1, const std::vector <Ref>& v2)
// {
//     return v1.size () < v2.size ();
// }

class IdxRef {
public:
    int idx;
    std::vector <Ref>* refs;
    IdxRef (int idx, std::vector <Ref>* refs) : idx (idx), refs (refs) {}
    bool operator< (const IdxRef rhs) const { return refs->size () < rhs.refs->size (); }
};
    

void fill_match (std::vector <Point>& f, std::vector <Point>& s,
		 std::vector <std::vector <Ref> >& f2s,
		 int* duplicates, int *maxcandidates)
{
    if (f.size () == 0 || s.size () == 0)
    {
	if (duplicates)
	    *duplicates = 0;
	if (maxcandidates)
	    *maxcandidates = 0;
	return;
    }
    
    f2s.clear ();
    
    // per ogni punto in first
    for (unsigned i = 0; i < f.size(); i++)
    {
	// aggiungilo a first2second
	f2s.push_back (std::vector<Ref>());
	    
	// scan ogni punto second
	for (unsigned j = 0; j < s.size(); j++)
	{
	    // se e' meno distante di target_distance
	    double d = point_distance (s[j], f[i]);
	    if (d < target_distance)
		// aggiungilo ai Ref di first
		f2s[i].push_back (Ref (j, d));
	}

	// order minivector by distance
	std::sort (f2s[i].begin (), f2s[i].end ());
    }

    //std::sort (f2s.begin (), f2s.end (), ref_comparison);

    std::vector <IdxRef> f2s_idx;
    for (unsigned i = 0; i < f.size(); i++)
	f2s_idx.push_back (IdxRef (i, &f2s[i]));
	
    if (maxcandidates)
	*maxcandidates = f2s_idx[f2s_idx.size()-1].refs->size();
    // search for duplicates
    // (two points in f have found the same point in s, we keep the nearest)

    unsigned i, j;
    int search_idx;
    int dup = -1;

 duplicates_restart:

    std::sort (f2s_idx.begin (), f2s_idx.end ());
    if (f2s_idx[0].refs->size() > f2s_idx[f2s_idx.size()-1].refs->size())
	eprintx (-2,"%s", "size ordering mismatch\n");
    
    i = j = 0;
    dup++;
    
    for (i = 0; i < f.size(); i++)
	if (f2s_idx[i].refs->size() > 0)
	{
	    search_idx = (*(f2s_idx[i].refs))[0].idx;
	    for (j = i+1; j < f.size(); j++)
		if (f2s_idx[j].refs->size() > 0 && (*(f2s_idx[j].refs))[0].idx == search_idx)
		{
		    if (f2s_idx[j].refs->size() == f2s_idx[i].refs->size())
		    {   // same size, we keep the nearest one 
			if ((*(f2s_idx[i].refs))[0].dist <= (*(f2s_idx[j].refs))[0].dist)
			{
			    f2s_idx[j].refs->erase (f2s_idx[j].refs->begin());
			    goto duplicates_restart;
			}
			else
			{
			    f2s_idx[i].refs->erase (f2s_idx[i].refs->begin());
			    goto duplicates_restart;
			}
		    }
		    else if (f2s_idx[j].refs->size() > f2s_idx[i].refs->size())
		    { // we delete j, because it has more matches
			f2s_idx[j].refs->erase (f2s_idx[j].refs->begin());
			goto duplicates_restart;
		    }
		    else
			eprintx (-1, "%s", "Wrong vector order in dups removal.\n");
		}
	}
    
    if (duplicates != NULL)
	*duplicates = dup;

    // double check
    for (unsigned i = 0; i < f.size(); i++)
	if (f2s_idx[i].refs->size() != f2s[f2s_idx[i].idx].size())
	    eprintx (-1, "%s", "much check very wrong.\n");
}

// void query_roc ()
// {

// }

// void write_roc (double sl, double el, double ss, double es, )
// {

// }

void classify_swiss (std::vector <SwissSpotHeight> s,
		     std::vector <TrackSpot> t,
		     std::vector <std::vector <Ref> > t2s,
		     std::string fn)
{
    std::vector<SwissSpotHeight> sc;

    for (unsigned i = 0; i < s.size(); i++)
    {
	sc.push_back (s[i]);
	sc[i].ct = OTHER;
    }
    
    for (unsigned i = 0; i < t.size(); i++)
	if (t2s[i].size() > 0)
	    sc[t2s[i][0].idx].ct = critical2classified (t[i].type);

    CSVReader csvio (asch);
    csvio.save (fn.c_str(), sc);
}

void query_track (Track* track, double life, double strength,
		  std::vector <TrackSpot>& spots,
		  std::vector <Point>& points,
		  ClassifiedType st)
{
    spots.clear ();
    points.clear ();
    for (unsigned i = 0; i < track->lines.size(); i++)
	if (track->is_original (i)
	    && track->start_point (i).is_inside ((double) width,
						 (double) height,
						 cut_borders))
	{
	    double life_i = track->lifetime_elixir (i);
	    double strength_i = track->lines[i].strength;
	    CriticalType ct = track->get_type (i);
	    if (life_i >= life && strength_i >= strength && is_sametype (ct, st))
	    {
		spots.push_back (TrackSpot (i, track->start_point (i),
					    life_i, strength_i,
					    track->get_type (i)));
		points.push_back (track->start_point (i));
	    }
	}

}

void query_generic (std::vector <SwissSpotHeight>& spots_in,
		    double life, double strength,
		    std::vector <SwissSpotHeight>& spots,
		    std::vector <Point>& points,
		    ClassifiedType st)
{
    spots.clear ();
    points.clear ();
    for (unsigned i = 0; i < spots_in.size(); i++)
    {
	double life_i = spots_in[i].life;
	double strength_i = spots_in[i].strength;
	ClassifiedType ct = spots_in[i].ct;
	if (life_i >= life && strength_i >= strength && st == ct)
	{
	    spots.push_back (spots_in[i]);
	    points.push_back (spots_in[i].p);
	}
    }
}


void load_all ()
{
    if (asc_file)
    {
	ASCReader ascr (asc_file);
	asch = ASCHeader (ascr);
	width = asch.width;
	height = asch.height;
	// xllcorner = ascr.xllcorner;
	// yllcorner = ascr.yllcorner;
	// cellsize = ascr.cellsize;
    }

    if (generic_file)
    {
	CSVReader csvio (asch);
	csvio.load (generic_file, generic, cut_borders);
	generic_points.clear ();

	generic_life_max = 0.0;
	generic_strength_max = 0.0;
	
	for (unsigned i = 0; i < generic.size (); i++)
	{
 	    generic_points.push_back (generic[i].p);

	    if (generic[i].life > generic_life_max)
		generic_life_max = generic[i].life;

	    if (generic[i].strength > generic_strength_max)
		generic_strength_max = generic[i].strength;
	}
	
	printf ("generic_life_max %lf\n", generic_life_max);
	printf ("generic_strength_max %lf\n", generic_strength_max);
    }
    
    if (swiss_file)
    {
	CSVReader csvio (asch);
	csvio.load (swiss_file, swiss, cut_borders);
	swiss_points.clear ();

	if (prune_swiss_points)
	{
	    std::vector<SwissSpotHeight> swiss_copy;
	
	    for (unsigned i = 0; i < swiss.size (); i++)
		swiss_copy.push_back (swiss[i]);

	    swiss.clear ();
	    for (unsigned i = 0; i < swiss_copy.size (); i++)
		// GG se anche curvatura e punti HUMAN check controllo
	    {
		if (swiss_copy[i].type () == PEAK ||
		    swiss_copy[i].type () == PIT ||
		    swiss_copy[i].type () == SADDLE)
		    swiss.push_back (swiss_copy[i]);
		
		if (swiss_copy[i].type () == PEAK)
		    swiss_peaks.push_back (swiss_copy[i]);
		if (swiss_copy[i].type () == PIT)
		    swiss_pits.push_back (swiss_copy[i]);
		if (swiss_copy[i].type () == SADDLE)
		    swiss_saddles.push_back (swiss_copy[i]);
	    }
	}
	
	for (unsigned i = 0; i < swiss.size (); i++)
	    swiss_points.push_back (swiss[i].p);

	if (prune_swiss_points)
	{
	    for (unsigned i = 0; i < swiss_peaks.size (); i++)
		swiss_peaks_points.push_back (swiss_peaks[i].p);
	    for (unsigned i = 0; i < swiss_pits.size (); i++)
		swiss_pits_points.push_back (swiss_pits[i].p);
	    for (unsigned i = 0; i < swiss_saddles.size (); i++)
		swiss_saddles_points.push_back (swiss_saddles[i].p);

	    printf ("loaded %zu swiss points. peaks: %zu, pits: %zu, saddles: %zu\n",
		    swiss_points.size (), swiss_peaks_points.size (),
		    swiss_pits_points.size (), swiss_saddles.size ());
	}
    }

    
    if (terrain_file)
    {
	track_reader (terrain_file, &terrain_track, &terrain_track_order);

	terrain.clear ();
	terrain_points.clear ();
	terrain_all_life_max = 0.0;
	terrain_all_strength_max = 0.0;
	terrain_peaks_life_max = 0.0;
	terrain_peaks_strength_max = 0.0;
	terrain_pits_life_max = 0.0;
	terrain_pits_strength_max = 0.0;
	terrain_saddles_life_max = 0.0;
	terrain_saddles_strength_max = 0.0;
	
	for (unsigned i = 0; i < terrain_track->lines.size(); i++)
	    if (terrain_track->is_original (i)
		&& terrain_track->start_point (i).is_inside ((double) width,
							     (double) height,
							     cut_borders))
	    {
		CriticalType cti = terrain_track->get_type (i);
		double life_i = terrain_track->lifetime_elixir (i);
		double strength_i = terrain_track->lines[i].strength;
		
		if (terrain_all_life_max < life_i)
		    terrain_all_life_max = life_i;
		if (terrain_all_strength_max < strength_i)
		    terrain_all_strength_max = strength_i;
		
		terrain.push_back (TrackSpot (i, terrain_track->start_point (i),
					      life_i, strength_i, cti));
		terrain_points.push_back (terrain_track->start_point (i));
		switch (critical2classified (cti))
		{
		case PEAK:
		    if (terrain_peaks_life_max < life_i)
			terrain_peaks_life_max = life_i;
		    if (terrain_peaks_strength_max < strength_i)
			terrain_peaks_strength_max = strength_i;
		    terrain_peak_num++;
		    break;
		case PIT:
		    if (terrain_pits_life_max < life_i)
			terrain_pits_life_max = life_i;
		    if (terrain_pits_strength_max < strength_i)
			terrain_pits_strength_max = strength_i;
		    terrain_pit_num++;
		    break;
		case SADDLE:
		    if (terrain_saddles_life_max < life_i)
			terrain_saddles_life_max = life_i;
		    if (terrain_saddles_strength_max < strength_i)
			terrain_saddles_strength_max = strength_i;
		    terrain_saddle_num++;
		    break;
		default:
		    break;
		}
	    }
	printf ("terrain_all_life_max %lf\n", terrain_all_life_max);
	printf ("terrain_all_strength_max %lf\n", terrain_all_strength_max);
	printf ("terrain_peaks_life_max %lf\n", terrain_peaks_life_max);
	printf ("terrain_peaks_strength_max %lf\n", terrain_peaks_strength_max);
	printf ("terrain_pits_life_max %lf\n", terrain_pits_life_max);
	printf ("terrain_pits_strength_max %lf\n", terrain_pits_strength_max);
	printf ("terrain_saddles_life_max %lf\n", terrain_saddles_life_max);
	printf ("terrain_saddles_strength_max %lf\n", terrain_saddles_strength_max);
    }

    if (curvature_file)
    {
	track_reader (curvature_file, &curvature_track, &curvature_track_order);
	curvature.clear ();
	curvature_points.clear ();
	for (unsigned i = 0; i < curvature_track->lines.size(); i++)
	    if (curvature_track->is_original (i)
		&& (curvature_track->get_type (i) == MAX ||
		    curvature_track->get_type (i) == MIN)
		&& curvature_track->lifetime_elixir (i) >= filter_curvature
		&& curvature_track->start_point (i).is_inside ((double) width,
							       (double) height,
							       cut_borders)
		)
	    {
		double life_i = curvature_track->lifetime_elixir (i);
		double strength_i = curvature_track->lines[i].strength;
		if (curvature_life_max < life_i)
		    curvature_life_max = life_i;
		if (curvature_strength_max < strength_i)
		    curvature_strength_max = strength_i;
		curvature.push_back (TrackSpot (i, curvature_track->start_point (i),
						life_i, strength_i,
						curvature_track->get_type (i)));
		curvature_points.push_back (curvature_track->start_point (i));
	    }
    }
}
