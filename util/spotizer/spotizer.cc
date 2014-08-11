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
 *-groundtruth-falsenegative.csv           == path_gt_fn
 
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
std::string path_gt_fn;
std::string path_c_fp;
std::string path_t_fp;


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

std::string path_sort_c_s;
std::string path_sort_c_l;
std::string path_sort_t_s;
std::string path_sort_t_l;


void create_output_names ()
{
    path_gt_c_tp = "-groundswiss-curvature-truepositive.csv";
    path_gt_t_tp = "-groundswiss-terrain-truepositive.csv";
    path_gt_fn = "-groundswiss-falsenegative.csv";
    path_c_fp = "-groundcurvature-falsepositive.csv";
    path_t_fp = "-groundterrain-falsepositive.csv";
 
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

    path_sort_c_s = "-sort-curvature-strength.dat";
    path_sort_c_l = "-sort-curvature-life.dat";
    path_sort_t_s = "-sort-terrain-strength.dat";
    path_sort_t_l = "-sort-terrain-life.dat";
    
    path_gt_c_tp = output_name + path_gt_c_tp; 
    path_gt_t_tp = output_name + path_gt_t_tp;
    path_gt_fn = output_name + path_gt_fn;
    path_c_fp = output_name + path_c_fp;
    path_t_fp = output_name + path_t_fp;

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

    path_sort_c_s = output_name + path_sort_c_s;
    path_sort_c_l = output_name + path_sort_c_l;
    path_sort_t_s = output_name + path_sort_t_s;
    path_sort_t_l = output_name + path_sort_t_l;
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

bool compare_life (const TrackSpot& r, const TrackSpot l)
{
    return (r.life < l.life);
}

bool compare_strength (const TrackSpot& r, const TrackSpot l)
{
    return (r.strength < l.strength);
}

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

void confusion (std::vector <std::vector <Ref> >& f2s, std::vector<Point>& s,
	       std::vector<int>& tp, std::vector<int>& fp,
	       std::vector<int>& fn, int* tn_count);

void fill_match (std::vector <Point>& f, std::vector <Point>& s,
		 std::vector <std::vector <Ref> >& f2s);

int count_found (std::vector <std::vector <Ref> >& f2s);

void load_all ();

void count_all ();

void save_sort_points (std::vector <TrackSpot>& spots,
		       bool do_idxs, std::vector<int>& idxs,
		       std::string fn_life, std::string fn_strength);


int main (int argc, char *argv[])
{
    app_init (argc, argv);
    
    load_all ();

    if (do_terrain2swiss)
	fill_match (terrain_points, swiss_points, terrain2swiss);
    
    if (do_curvature2swiss)
	fill_match (curvature_points, swiss_points, curvature2swiss);

    if (do_terrain2curvature)
	fill_match (terrain_points, curvature_points, terrain2curvature);
    

    count_all ();
    
    if (!do_output)
	exit (0);

    if (terrain_file)
    {
	std::vector<int> foo;
	save_sort_points (terrain, false, foo, path_sort_t_l, path_sort_t_s);
    }

    if (curvature_file)
    {
	std::vector<int> foo;
	save_sort_points (terrain, false, foo, path_sort_c_l, path_sort_c_s);
    }
    
    std::vector <int> t2s_tp, t2s_fp, t2s_fn;
    int t2s_tn = 0;
    std::vector <int> c2s_tp, c2s_fp, c2s_fn;
    int c2s_tn = 0;

    if (do_terrain2swiss)
    {
	confusion (terrain2swiss, swiss_points, t2s_tp, t2s_fp, t2s_fn, &t2s_tn);

	// sort tp, fp by stre,life. save dat
	save_sort_points (terrain, true, t2s_tp, path_sort_t_tp_l, path_sort_t_tp_s);
	save_sort_points (terrain, true, t2s_fp, path_sort_t_fp_l, path_sort_t_fp_s);
    }

    if (do_curvature2swiss)
    {
	confusion (curvature2swiss, swiss_points, c2s_tp, c2s_fp, c2s_fn, &c2s_tn);

	// sort tp, fp by stre,life. save dat
	save_sort_points (curvature, true, c2s_tp, path_sort_c_tp_l, path_sort_c_tp_s);
	save_sort_points (curvature, true, c2s_fp, path_sort_c_fp_l, path_sort_c_fp_s);
    }

    if (do_allthree)
    {
	std::vector<bool> curvature_marked (curvature.size (), false);
	std::vector<bool> swiss_marked (swiss.size (),  false);

	std::vector<SwissSpotHeight> st_tp;
	std::vector<SwissSpotHeight> sc_tp;
	std::vector<SwissSpotHeight> t_fp;
	std::vector<SwissSpotHeight> c_fp;

	std::vector<SwissSpotHeight> s_fn;

	// terrain tp, mark curv tp, mark swiss fn, terrain fp.
	for (unsigned i = 0; i < terrain2swiss.size (); i++)
	{
	    if (terrain2swiss[i].size () > 0 &&
		terrain2curvature[i].size () > 0 &&
		terrain[i].life >= curvature[terrain2curvature[i][0].idx].life
		)
		curvature_marked[terrain2curvature[i][0].idx] = true;
		
	    if (terrain2swiss[i].size () > 0)
	    {
		st_tp.push_back (swiss[terrain2swiss[i][0].idx]);
		swiss_marked[terrain2swiss[i][0].idx] = true;
	    }
	    else
		t_fp.push_back (SwissSpotHeight(terrain_points[i], 100000 + i));
	}

	// curvature tp, mark swiss fn, curvature fp.
	for (unsigned i = 0; i < curvature2swiss.size (); i++)
	    if (curvature2swiss[i].size () > 0 && !curvature_marked[i])
	    {
		sc_tp.push_back (swiss[curvature2swiss[i][0].idx]);
		swiss_marked[curvature2swiss[i][0].idx] = true;
	    }
	    else if (!curvature_marked[i])
		c_fp.push_back (SwissSpotHeight(curvature_points[i], 200000 + i));
				
	// swiss fn
	for (unsigned i = 0; i < swiss.size(); i++)
	    if (!swiss_marked[i])
		s_fn.push_back (swiss[i]);

	CSVReader csvio (width, height, cellsize, xllcorner, yllcorner);
	csvio.save (path_gt_t_tp.c_str(), st_tp);
	csvio.save (path_gt_c_tp.c_str(), sc_tp);
	csvio.save (path_gt_fn.c_str(), s_fn);
	csvio.save (path_c_fp.c_str(), c_fp);				
	csvio.save (path_t_fp.c_str(), t_fp);				
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

void save_sort_points (std::vector <TrackSpot>& spots,
		       bool do_idxs, std::vector<int>& idxs,
		       std::string filename_life, std::string filename_strength)
{
    FILE* fp;
    std::vector<TrackSpot> trsp;

    if (do_idxs)
	for (unsigned i = 0; i < idxs.size(); i++)
	    trsp.push_back (spots[idxs[i]]);
    else
	for (unsigned i = 0; i < spots.size(); i++)
	    trsp.push_back (spots[i]);
	
    // life
    std::sort (trsp.begin (), trsp.end (), compare_life);
    fp = fopen (filename_life.c_str(), "w");
    for (unsigned i = 0; i < trsp.size(); i++)
	fprintf (fp, "%lf\n", trsp[i].life);
    fclose (fp);
    
    // strength
    std::sort (trsp.begin (), trsp.end (), compare_strength);
    fp = fopen (filename_strength.c_str(), "w");
    for (unsigned i = 0; i < trsp.size(); i++)
	fprintf (fp, "%lf\n", trsp[i].strength);
    fclose (fp);
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

    // search for duplicates
    // (two points in f have found the same point in s, we keep the nearest)
    for (unsigned i = 0; i < f.size(); i++)
	if (f2s[i].size() > 0)
	{
	    int search_idx = f2s[i][0].idx;
	    for (unsigned j = i+1; j < f.size(); j++)
		if (f2s[j].size() > 0 && f2s[j][0].idx == search_idx)
		{
		    if (f2s[i][0].dist <= f2s[j][0].dist)
			f2s[j].erase (f2s[j].begin());
		    else
		    {
			f2s[i].erase (f2s[i].begin());
			i--;
			break;
		    }	
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

void print_filenames ()
{
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
		path_gt_fn.c_str(),

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
