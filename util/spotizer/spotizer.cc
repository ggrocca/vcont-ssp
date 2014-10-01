/*
load csv and tracking

vector <vector<int>> csv_points; // tanti quanti i csv.
 vettore contenuto contiene dei trackref -> indice su track_points per ogni csv

class csvref { int csvidx; double dist } // csv_ref -> indice sui csv_points
vector <vector <csvcomp>> track2csvs_relations; // tanti quanti  i track points

for every point in tracking see how distant it is from every other
point in csv in a radius R or window W. If it is less than R,
memorize: csv point; distance. (push back in a vector memorized at the
tracking point index)

at the end scan tracking and order by distance all secondary vectors.

------------

a questo punto conosciamo, per ogni punto di tracking: il candidato
tra i csv piu' vicino, se esiste, e la vita/forza del tracking point.

numero di punti senza candidato

nuvola di punti sulle distanze; // ordinata per distanze
nuvola di punti sulla vita; // ordinata per vita
nuvola di punti sulla forza // ordinata per forza
nuvola di punti sulla vita ordinata per distanza

-------

caricare curvatura e tracking e confrontare anche loro?

*/

#include "track.hh"
#include "demreader.hh"
#include "csvreader.hh"

#include <vector>
#include <algorithm>

char *terrain_file = 0;
char *curvature_file = 0;
char *swiss_file = 0;
char *mod_swiss_file = 0;
char *asc_file = 0;
double target_distance = 2.0;
double filter_curvature = 2.9;

bool do_terrain2swiss;
bool do_curvature2swiss;
bool do_terrain2curvature;
bool do_allthree;
bool query_mode = false;
bool prune_swiss_points = false;
int life_steps = 0;
int strength_steps = 0;
double cut_borders = 0.0;

bool do_output = false;
std::string oname;

// std::string path_gt_c_tp;
// std::string path_gt_t_tp;
// std::string path_gt_fn;
// std::string path_c_fp;
// std::string path_t_fp;

// // std::string path_roc_c_s;
// // std::string path_roc_c_l;
// // std::string path_roc_t_s;
// // std::string path_roc_t_l;
// std::string path_roc_c;
// std::string path_roc_t;
// std::string path_roc_c_max;
// std::string path_roc_t_max;
// std::string path_roc_c_min;
// std::string path_roc_t_min;
// std::string path_roc_c_sad;
// std::string path_roc_t_sad;
	
// std::string path_sort_c_fp_s;
// std::string path_sort_c_fp_l;
// std::string path_sort_c_tp_s;
// std::string path_sort_c_tp_l;
// std::string path_sort_t_fp_s;
// std::string path_sort_t_fp_l;
// std::string path_sort_t_tp_s;
// std::string path_sort_t_tp_l;

// std::string path_sort_c_s;
// std::string path_sort_c_l;
// std::string path_sort_t_s;
// std::string path_sort_t_l;

// std::string path_sort_t_ls;
// std::string path_sort_c_ls;

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

// void create_output_names ()
// {
//     // path_gt_c_tp = "-groundswiss-curvature-truepositive.csv";
//     // path_gt_t_tp = "-groundswiss-terrain-truepositive.csv";
//     // path_gt_fn = "-groundswiss-falsenegative.csv";
//     // path_c_fp = "-groundcurvature-falsepositive.csv";
//     // path_t_fp = "-groundterrain-falsepositive.csv";

/////////////////////////////////////////////////
    
//     // path_roc_c_all = "-roc-curvature-all.dat";
//     // path_roc_t_all = "-roc-terrain-all.dat";
//     // path_roc_c_max = "-roc-curvature-max.dat";
//     // path_roc_t_max = "-roc-terrain-max.dat";
//     // path_roc_c_min = "-roc-curvature-min.dat";
//     // path_roc_t_min = "-roc-terrain-min.dat";
//     // path_roc_c_sad = "-roc-curvature-sad.dat";
//     // path_roc_t_sad = "-roc-terrain-sad.dat";

/////////////////////////////////////////////////

//     // path_sort_c_fp_s  = "-sort-curvature-falsepositive-strength.dat";
//     // path_sort_c_fp_l  = "-sort-curvature-falsepositive-life.dat";
//     // path_sort_c_tp_s = "-sort-curvature-truepositive-strength.dat";
//     // path_sort_c_tp_l = "-sort-curvature-truepositive-life.dat";
//     // path_sort_t_fp_s = "-sort-terrain-falsepositive-strength.dat";
//     // path_sort_t_fp_l = "-sort-terrain-falsepositive-life.dat";
//     // path_sort_t_tp_s = "-sort-terrain-truepositive-strength.dat";
//     // path_sort_t_tp_l = "-sort-terrain-truepositive-life.dat";

//     // path_sort_c_s = "-sort-curvature-strength.dat";
//     // path_sort_c_l = "-sort-curvature-life.dat";
//     // path_sort_t_s = "-sort-terrain-strength.dat";
//     // path_sort_t_l = "-sort-terrain-life.dat";
//     // // path_sort_c_l = "-sort-curvature.dat";
//     // // path_sort_t_s = "-sort-terrain.dat";

//     // path_sort_t_ls = "-sort-terrain-lifestrength.dat";
//     // path_sort_c_ls = "-sort-curvature-lifestrength.dat";
    
/////////////////////////////////////////////////
    
//     // path_gt_c_tp = output_name + path_gt_c_tp; 
//     // path_gt_t_tp = output_name + path_gt_t_tp;
//     // path_gt_fn = output_name + path_gt_fn;
//     // path_c_fp = output_name + path_c_fp;
//     // path_t_fp = output_name + path_t_fp;

//     // // path_roc_c_s = output_name + path_roc_c_s;
//     // // path_roc_c_l = output_name + path_roc_c_l;
//     // // path_roc_t_s = output_name + path_roc_t_s;
//     // // path_roc_t_l = output_name + path_roc_t_l;

    
//     // path_roc_c = output_name + path_roc_c;
//     // path_roc_t = output_name + path_roc_t;
//     // path_roc_c_max = output_name + path_roc_c_max;
//     // path_roc_t_max = output_name + path_roc_t_max;
//     // path_roc_c_min = output_name + path_roc_c_min;
//     // path_roc_t_min = output_name + path_roc_t_min;
//     // path_roc_c_sad = output_name + path_roc_c_sad;
//     // path_roc_t_sad = output_name + path_roc_t_sad;
	
//     // path_sort_c_fp_s = output_name + path_sort_c_fp_s;
//     // path_sort_c_fp_l = output_name + path_sort_c_fp_l;
//     // path_sort_c_tp_s = output_name + path_sort_c_tp_s;
//     // path_sort_c_tp_l = output_name + path_sort_c_tp_l;
//     // path_sort_t_fp_s = output_name + path_sort_t_fp_s;
//     // path_sort_t_fp_l = output_name + path_sort_t_fp_l;
//     // path_sort_t_tp_s = output_name + path_sort_t_tp_s;
//     // path_sort_t_tp_l = output_name + path_sort_t_tp_l;

//     // path_sort_c_s = output_name + path_sort_c_s;
//     // path_sort_c_l = output_name + path_sort_c_l;
//     // path_sort_t_s = output_name + path_sort_t_s;
//     // path_sort_t_l = output_name + path_sort_t_l;

//     // path_sort_t_ls = output_name + path_sort_t_ls;
//     // path_sort_c_ls = output_name + path_sort_c_ls;
// }

void print_help (FILE* f)
{
    fprintf (f, "Usage: spotfilter\n"
	     // "[-s scalespacefile.ssp]\n"
	     "[-o outputname] : all output files begin with outputname.\n"
	     "[-t terrain trackfile.trk]\n"
	     "[-c curvature trackfile.trk]\n"
	     "[-s swiss input points.csv]\n"
	     "[-m modified swiss output points.csv]: dumb classifier\n"
	     "[-a dem.asc]\n"
	     "[-Q LIFE_STEPS STRENGTH_STEPS] query mode - ROC analysis.\n"
	     "[-D DISTANCE] : distance window for matching points.\n"
	     "[-F FILTER] : keep only curvature points with life higher than filter.\n"
	     "[-C CUT] : cut border areas by percentage in [0,1].\n"
	     "[-P] : prune swiss input points using classification.\n"
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
	    // case 'u':
	    // 	do_normalize_border_points = false;
            //     break;


            // case 's':
            //     ssp_file = (*++argv);
            //     argc--;
            //     break;

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
		// create_output_names ();
                argc--;
                break;

            case 's':
                swiss_file = (*++argv);
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

	    case 'Q':
		query_mode = true;
                life_steps = atoi (*++argv);
                argc--;
                strength_steps = atoi (*++argv);
                argc--;
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
    
    if (!do_terrain2curvature && !do_curvature2swiss && !do_terrain2swiss)
    {
	fprintf (stderr, "Two datasets are needed to make a comparison.\n\n");
	goto die;
    }
    
    if (swiss_file != NULL && asc_file == NULL)
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
double cellsize, xllcorner, yllcorner;
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
double terrain_life_max;
double terrain_strength_max;
double curvature_life_max;
double curvature_strength_max;
std::vector <SwissSpotHeight> swiss;
std::vector <SwissSpotHeight> swiss_peaks;
std::vector <SwissSpotHeight> swiss_pits;
std::vector <SwissSpotHeight> swiss_saddles;

std::vector <Point> terrain_points;
std::vector <Point> curvature_points;
std::vector <Point> swiss_points;
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

void main_query ();
void main_old ();

int main (int argc, char *argv[])
{
    app_init (argc, argv);

    load_all ();

    if (query_mode)
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

void query_and_save (std::string prepend, double life, double strength,
		     std::vector <SwissSpotHeight>& sw, std::vector<Point>& swps,
		     ClassifiedType st = ALL);

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

	std::string kt_output;
	std::vector<Point> kt_swps;
	std::vector <SwissSpotHeight> kt_sw;
	ClassifiedType kt_st;
	std::string kt_name;

	switch (kt)
	{
	case 0: // max
	    kt_output = oname+"-"+peaks+"-"+path_performance; // path_roc_t_max;
	    kt_swps = swiss_peaks_points;
	    kt_sw = swiss_peaks;
	    kt_st = PEAK;
	    kt_name = peaks;
	    printf ("\n -- PEAKS - swiss[%zu] - track[%d] --\n",
		    swiss_peaks.size(), terrain_peak_num++);
	    break;
	case 1: // min
	    kt_output = oname+"-"+pits+"-"+path_performance; //path_roc_t_min;
	    kt_swps = swiss_pits_points;
	    kt_sw = swiss_pits;
	    kt_name = pits;
	    printf ("\n -- PITS - swiss[%zu] - track[%d] --\n",
		    swiss_pits.size(), terrain_pit_num++);
	    kt_st = PIT;
	    break;
	case 2: // sad
	    kt_output = oname+"-"+saddles+"-"+path_performance; //path_roc_t_sad;
	    kt_swps = swiss_saddles_points;
	    kt_sw = swiss_saddles;
	    kt_st = SADDLE;
	    kt_name = saddles;
	    printf ("\n -- SADDLES - swiss[%zu] - track[%d] --\n",
		    swiss_saddles.size(), terrain_saddle_num++);
	    break;
	case 3: // all
	    kt_output = oname+"-"+all+"-"+path_performance; //path_roc_t;
	    kt_swps = swiss_points;
	    kt_sw = swiss;
	    kt_st = ALL;
	    kt_name = all;
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
    
	for (int i = 0; i < life_steps; i++)
	    for (int j = 0; j < strength_steps; j++)
	    {		
		double life_s = (terrain_life_max / (double) life_steps) * i;
		double strength_s = (terrain_strength_max / (double) strength_steps) * j;

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

	query_and_save (oname+"-"+kt_name, life_best_pr, strength_best_pr, kt_sw, kt_swps, kt_st);
    }
    
    CSVReader csvio (width, height, cellsize, xllcorner, yllcorner);
    std::vector<SwissSpotHeight> other;
    for (int i = 0; i < swiss.size(); i++)
	if (swiss[i].t == OTHER)
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
    CSVReader csvio (width, height, cellsize, xllcorner, yllcorner);
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
	    track_tp.push_back (SwissSpotHeight (query_t[i], 100000));
	}
	else
	    track_fp.push_back (SwissSpotHeight(query_t[i], 100000));
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
		track_fn.push_back (SwissSpotHeight (qzero_t[i], 500000));
		
    // track_out: points that are fp in 0,0 and are not in track_fp
    for (unsigned i = 0; i < qzero_tz2s.size(); i++)
    {
	bool found = false;
	for (unsigned j = 0; j < track_fp.size(); j++)
	    if (query_t2s[i].size () == 0 && qzero_tp[i] == track_fp[j].p)
		found = true;
	if (!found)
	    track_out.push_back (SwissSpotHeight (qzero_t[i], 600000));	    
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
		    t_fp.push_back (SwissSpotHeight(terrain_points[i], 100000 + i));
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
		t_fp.push_back (SwissSpotHeight(terrain_points[i], 100000 + i));
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
		c_fp.push_back (SwissSpotHeight(curvature_points[i], 200000 + i));
    }

    if (do_terrain2swiss || do_allthree)
    {	// swiss fn
	for (unsigned i = 0; i < swiss.size(); i++)
	    if (!swiss_marked[i])
		s_fn.push_back (swiss[i]);
    }
    
    CSVReader csvio (width, height, cellsize, xllcorner, yllcorner);
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

bool ref_comparison (const std::vector <Ref>& v1, const std::vector <Ref>& v2)
{
    return v1.size () < v2.size ();
}

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

    std::sort (f2s.begin (), f2s.end (), ref_comparison);

    if (f2s[0].size() > f2s[f2s.size()-1].size())
	eprintx (-2,"begin: %zu, end: %zu\n", f2s[0].size(), f2s[f2s.size()-1].size());

    if (maxcandidates)
	*maxcandidates = f2s[f2s.size()-1].size();
    // search for duplicates
    // (two points in f have found the same point in s, we keep the nearest)

    unsigned i, j;
    int search_idx;
    int dup = -1;

 duplicates_restart:
    
    i = j = 0;
    dup++;
    
    for (i = 0; i < f.size(); i++)
	if (f2s[i].size() > 0)
	{
	    search_idx = f2s[i][0].idx;
	    for (j = i+1; j < f.size(); j++)
		if (f2s[j].size() > 0 && f2s[j][0].idx == search_idx)
		{
		    if (f2s[j].size() == f2s[i].size())
		    {   // same size, we keep the nerest one 
			if (f2s[i][0].dist <= f2s[j][0].dist)
			{
			    f2s[j].erase (f2s[j].begin());
			    goto duplicates_restart;
			}
			else
			{
			    f2s[i].erase (f2s[i].begin());
			    goto duplicates_restart;
			}
		    }
		    else if (f2s[j].size() > f2s[i].size())
		    { // we delete j, because it has more matches
			f2s[j].erase (f2s[j].begin());
			goto duplicates_restart;
		    }
		    else
			eprintx (-1, "%s", "Wrong vector order in dups removal.\n");
		}
	}
    
    if (duplicates != NULL)
	*duplicates = dup;
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
	sc[i].t = OTHER;
    }
    
    for (unsigned i = 0; i < t.size(); i++)
	if (t2s[i].size() > 0)
	    sc[t2s[i][0].idx].t = critical2swiss (t[i].type);

    CSVReader csvio (width, height, cellsize, xllcorner, yllcorner);
    csvio.save (fn.c_str(), sc, true);
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


void load_all ()
{
    if (asc_file)
    {
	ASCReader ascr (asc_file);

	width = ascr.width;
	height = ascr.height;
	xllcorner = ascr.xllcorner;
	yllcorner = ascr.yllcorner;
	cellsize = ascr.cellsize;
    }

    if (swiss_file)
    {
	CSVReader csvio (width, height, cellsize, xllcorner, yllcorner);
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
		if (swiss_copy[i].t == PEAK ||
		    swiss_copy[i].t == PIT ||
		    swiss_copy[i].t == SADDLE)
		    swiss.push_back (swiss_copy[i]);
		
		if (swiss_copy[i].t == PEAK)
		    swiss_peaks.push_back (swiss_copy[i]);
		if (swiss_copy[i].t == PIT)
		    swiss_pits.push_back (swiss_copy[i]);
		if (swiss_copy[i].t == SADDLE)
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
	terrain_life_max = 0.0;
	terrain_strength_max = 0.0;
	for (unsigned i = 0; i < terrain_track->lines.size(); i++)
	    if (terrain_track->is_original (i)
		&& terrain_track->start_point (i).is_inside ((double) width,
							     (double) height,
							     cut_borders))
	    {
		CriticalType cti = terrain_track->get_type (i);
		double life_i = terrain_track->lifetime_elixir (i);
		double strength_i = terrain_track->lines[i].strength;
		if (terrain_life_max < life_i)
		    terrain_life_max = life_i;
		if (terrain_strength_max < strength_i)
		    terrain_strength_max = strength_i;
		terrain.push_back (TrackSpot (i, terrain_track->start_point (i),
					      life_i, strength_i, cti));
		terrain_points.push_back (terrain_track->start_point (i));
		switch (critical2swiss (cti))
		{
		case PEAK:
		    terrain_peak_num++;
		    break;
		case PIT:
		    terrain_pit_num++;
		    break;
		case SADDLE:
		    terrain_saddle_num++;
		    break;
		default:
		    break;
		}
	    }
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
