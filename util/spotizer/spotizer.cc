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
char *asc_file = 0;
double target_distance = 2.0;
double filter_curvature = 2.9;

bool do_terrain2swiss;
bool do_curvature2swiss;
bool do_terrain2curvature;
bool do_allthree;

/*

 *-groundtruth-curvature-truepositive.csv == path_gt_c_tp
 *-groundtruth-terrain-truepositive.csv   == path_gt_t_tp
 *-groundtruth-truenegative.csv           == path_gt_tn
 
 *-roc-curvature-strength.dat  == path_roc_c_s
 *-roc-curvature-life.dat      == path_roc_c_l
 *-roc-terrain-strength.dat    == path_roc_t_s
 *-roc-terrain-life.dat        == path_roc_t_l
	
 *-sort-curvature-falsepositive-strength.dat == path_sort_c_fp_s 
 *-sort-curvature-falsepositive-life.dat     == path_sort_c_fp_l 
 *-sort-curvature-truepositive-strength.dat  == path_sort_c_tp_s
 *-sort-curvature-truepositive-life.dat      == path_sort_c_tp_l
 *-sort-terrain-falsepositive-strength.dat   == path_sort_t_fp_s
 *-sort-terrain-falsepositive-life.dat       == path_sort_t_fp_l
 *-sort-terrain-truepositive-strength.dat    == path_sort_t_tp_s
 *-sort-terrain-truepositive-life.dat        == path_sort_t_tp_l

 */

bool do_output = false;
std::string output_name;

std::string path_gt_c_tp;
std::string path_gt_t_tp;
std::string path_gt_tn;

std::string path_roc_c_s;
std::string path_roc_c_l;
std::string path_roc_t_s;
std::string path_roc_t_l;
	
std::string path_sort_c_fp_s;
std::string path_sort_c_fp_l;
std::string path_sort_c_tp_s;
std::string path_sort_c_tp_l;
std::string path_sort_t_fp_s;
std::string path_sort_t_fp_l;
std::string path_sort_t_tp_s;
std::string path_sort_t_tp_l;

void create_output_names ()
{
    path_gt_c_tp = "-groundtruth-curvature-truepositive.csv";
    path_gt_t_tp = "-groundtruth-terrain-truepositive.csv";
    path_gt_tn = "-groundtruth-truenegative.csv";
 
    path_roc_c_s = "-roc-curvature-strength.dat";
    path_roc_c_l = "-roc-curvature-life.dat";
    path_roc_t_s = "-roc-terrain-strength.dat";
    path_roc_t_l = "-roc-terrain-life.dat";
	
    path_sort_c_fp_s  = "-sort-curvature-falsepositive-strength.dat";
    path_sort_c_fp_l  = "-sort-curvature-falsepositive-life.dat";
    path_sort_c_tp_s = "-sort-curvature-truepositive-strength.dat";
    path_sort_c_tp_l = "-sort-curvature-truepositive-life.dat";
    path_sort_t_fp_s = "-sort-terrain-falsepositive-strength.dat";
    path_sort_t_fp_l = "-sort-terrain-falsepositive-life.dat";
    path_sort_t_tp_s = "-sort-terrain-truepositive-strength.dat";
    path_sort_t_tp_l = "-sort-terrain-truepositive-life.dat";
 
    path_gt_c_tp = output_name + path_gt_c_tp; 
    path_gt_t_tp = output_name + path_gt_t_tp;
    path_gt_tn = output_name + path_gt_tn;

    path_roc_c_s = output_name + path_roc_c_s;
    path_roc_c_l = output_name + path_roc_c_l;
    path_roc_t_s = output_name + path_roc_t_s;
    path_roc_t_l = output_name + path_roc_t_l;
	
    path_sort_c_fp_s = output_name + path_sort_c_fp_s;
    path_sort_c_fp_l = output_name + path_sort_c_fp_l;
    path_sort_c_tp_s = output_name + path_sort_c_tp_s;
    path_sort_c_tp_l = output_name + path_sort_c_tp_l;
    path_sort_t_fp_s = output_name + path_sort_t_fp_s;
    path_sort_t_fp_l = output_name + path_sort_t_fp_l;
    path_sort_t_tp_s = output_name + path_sort_t_tp_s;
    path_sort_t_tp_l = output_name + path_sort_t_tp_l;
}

void print_help (FILE* f)
{
    fprintf (f, "Usage: spotfilter\n"
	     // "[-s scalespacefile.ssp]\n"
	     "[-o outputname] : all output files begin with outputname.\n"
	     "[-t terrain trackfile.trk]\n"
	     "[-c curvature trackfile.trk]\n"
	     "[-s swiss input points.csv]\n"
	     "[-a dem.asc]\n"
	     "[-D DISTANCE] : distance window for matching points.\n"
	     "[-F FILTER] : keep only curvature points with life higher than filter.\n"
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
                output_name = (*++argv);
		do_output = true;
		create_output_names ();
                argc--;
                break;

            case 's':
                swiss_file = (*++argv);
                argc--;
                break;

            // case 'o':
            //     csv_out_file = (*++argv);
            //     argc--;
            //     break;

            case 'a':
                asc_file = (*++argv);
                argc--;
                break;

            case 'D':
                target_distance = atof (*++argv);
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

class TrackSpot {
public:
    int idx;
    Point p;
    double life;
    double strength;
    TrackSpot (int idx, Point p, double life, double strength) :
	idx (idx), p (p), life (life), strength (strength) {}
};

// faccio dei vettori di points che corrispondono a trackspot e swisspotheight
// li uso per fare la funzione fill
// il resto puo' essere funzione order/create etc a seconda

std::vector <TrackSpot> terrain;
std::vector <TrackSpot> curvature;
std::vector <SwissSpotHeight> swiss;

std::vector <Point> terrain_points;
std::vector <Point> curvature_points;
std::vector <Point> swiss_points;

    
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

void fill_match (std::vector <Point>& f, std::vector <Point>& s,
		 std::vector <std::vector <Ref> >& f2s)
{
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
}

void load_all ();

int main (int argc, char *argv[])
{
    app_init (argc, argv);
    load_all ();

    /*
     
      GG
      - funzione che prende vettore di ref e conta quanti hanno size > 0
      - funzione che prende vettore di ref e conta quanti hanno size > 0
        e vita e forza maggiori di V e F
      - ritorna un vettore che contiene gli indici di quelli true e gli
        indici di quelli negative

      ----

      da questi dati si puo' fare:
      ROC per terrain2swiss vita - ROC terrain2swiss forza - 
      ROC curvature2swiss vita - ROC curvature2swiss forza
	
      curvatura TP/FP vita (ordinati per vita)
      terreno TP/FP forza (ordinati pe forza)

      csv svizzeri true positive terreno
      csv svizzeri true positive curvatura (no terreno or + forti di terreno)
      csv svizzeri true negative

      --- filenames

      *-groundtruth-curvature-truepositive.csv == path_gt_c_tp
      *-groundtruth-terrain-truepositive.csv   == path_gt_t_tp
      *-groundtruth-truenegative.csv           == path_gt_tn

      *-roc-curvature-strength.dat  == path_roc_c_s
      *-roc-curvature-life.dat      == path_roc_c_l
      *-roc-terrain-strength.dat    == path_roc_t_s
      *-roc-terrain-life.dat        == path_roc_t_l
	
      *-sort-curvature-falsepositive-strength.dat == path_sort_c_fp_s 
      *-sort-curvature-falsepositive-life.dat     == path_sort_c_fp_l 
      *-sort-curvature-truepositive-strength.dat  == path_sort_c_tp_s
      *-sort-curvature-truepositive-life.dat      == path_sort_c_tp_l
      *-sort-terrain-falsepositive-strength.dat   == path_sort_t_fp_s
      *-sort-terrain-falsepositive-life.dat       == path_sort_t_fp_l
      *-sort-terrain-truepositive-strength.dat    == path_sort_t_tp_s
      *-sort-terrain-truepositive-life.dat        == path_sort_t_tp_l
	
    */

    
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
	fill_match (terrain_points, swiss_points, terrain2swiss);
	for (unsigned i = 0; i < terrain2swiss.size (); i++)
	    if (terrain2swiss[i].size () > 0)
		t2s++;
	printf ("Terrain vs Swiss | found: %d, "
		"terrain left out: %d, swiss left out: %d\n", t2s, t-t2s, s-t2s);
    }
    
    if (do_curvature2swiss)
    {
	fill_match (curvature_points, swiss_points, curvature2swiss);
	for (unsigned i = 0; i < curvature2swiss.size (); i++)
	    if (curvature2swiss[i].size () > 0)
		c2s++;
	printf ("Curvature vs Swiss | found: %d, "
		"curvature left out: %d, swiss left out: %d\n", c2s, c-c2s, s-c2s);
    }
    
    if (do_terrain2curvature)
    {
	fill_match (terrain_points, curvature_points, terrain2curvature);
	for (unsigned i = 0; i < terrain2curvature.size (); i++)
	    if (terrain2curvature[i].size () > 0)
		t2c++;
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
	printf ("T+C total: %d\n", tc);
	printf ("T+C vs Swiss | found:%d, "
		"left out in T+C: %d, left out in swiss: %d\n", tc2s, tc-tc2s, s-tc2s);
	printf ("At the same time in Terrain/Curvature/Swiss: %d\n", t2s2c);
    }

    if (do_output)
    {
	printf ("\n"
		"%s\n"
		"%s\n"
		"%s\n"

		"%s\n"
		"%s\n"
		"%s\n"
		"%s\n"
		
		"%s\n"
		"%s\n"
		"%s\n"
		"%s\n"
		"%s\n"
		"%s\n"
		"%s\n"
		"%s\n",

		path_gt_c_tp.c_str(),
		path_gt_t_tp.c_str(),
		path_gt_tn.c_str(),

		path_roc_c_s.c_str(),
		path_roc_c_l.c_str(),
		path_roc_t_s.c_str(),
		path_roc_t_l.c_str(),
	
		path_sort_c_fp_s.c_str(),
		path_sort_c_fp_l.c_str(),
		path_sort_c_tp_s.c_str(),
		path_sort_c_tp_l.c_str(),
		path_sort_t_fp_s.c_str(),
		path_sort_t_fp_l.c_str(),
		path_sort_t_tp_s.c_str(),
		path_sort_t_tp_l.c_str()
		);
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
	csvio.load (swiss_file, swiss);
	swiss_points.clear ();
	for (unsigned i = 0; i < swiss.size (); i++)
	    swiss_points.push_back (swiss[i].p);
    }

    if (terrain_file)
    {
	track_reader (terrain_file, &terrain_track, &terrain_track_order);

	terrain.clear ();
	terrain_points.clear ();
	for (unsigned i = 0; i < terrain_track->lines.size(); i++)
	    if (terrain_track->is_original (i))
	    {
		terrain.push_back (TrackSpot (i, terrain_track->start_point (i),
					      terrain_track->lifetime_elixir (i),
					      terrain_track->lines[i].strength));
		terrain_points.push_back (terrain_track->start_point (i));
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
		)
	    {
		curvature.push_back (TrackSpot (i, curvature_track->start_point (i),
						curvature_track->lifetime_elixir (i),
						curvature_track->lines[i].strength));
		curvature_points.push_back (curvature_track->start_point (i));
	    }
    }
}
