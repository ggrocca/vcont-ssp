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

double target_distance = 2.0;
char *terrain_file = 0;
char *curvature_file = 0;
char *swiss_file = 0;
char *asc_file = 0;

bool do_terrain2swiss;
bool do_curvature2swiss;
bool do_terrain2curvature;
bool do_allthree;


void print_help (FILE* f)
{
    fprintf (f, "Usage: spotfilter\n"
	     // "[-s scalespacefile.ssp]\n"
	     "[-t terrain trackfile.trk]\n"
	     "[-c curvature trackfile.trk]\n"
	     "[-s swiss input points.csv]\n"
	     "[-a dem.asc]\n"
	     "[-d DISTANCE] : distance window for matching points.\n"
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

            case 'd':
                target_distance = atof (*++argv);
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
std::vector <SwissSpotHeight> swiss;
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

std::vector <TrackSpot> terrain;
std::vector <TrackSpot> curvature;


    
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

void fill_terrain2swiss ()
{
    terrain2swiss.clear ();
    
    // per ogni punto originale nel tracking terrain
    for (unsigned i = 0; i < terrain.size(); i++)
    {
	// aggiungilo a terrain2swiss
	terrain2swiss.push_back (std::vector<Ref>());
	    
	// scan ogni punto swiss
	for (unsigned j = 0; j < swiss.size(); j++)
	{
	    // se e' meno distante di target_distance
	    double d = point_distance (swiss[j].p, terrain[i].p);
	    if (d < target_distance)
		// aggiungilo ai Ref del punt di trackin
		terrain2swiss[i].push_back (Ref (j, d));
	}

	// order minivector by distance
	std::sort (terrain2swiss[i].begin (), terrain2swiss[i].end ());
    }
}

void fill_curvature2swiss ()
{
    curvature2swiss.clear ();
    
    // per ogni punto originale nel tracking curvature
    for (unsigned i = 0; i < curvature.size(); i++)
    {
	// aggiungilo a curvature2swiss
	curvature2swiss.push_back (std::vector<Ref>());
	    
	// scan ogni punto swiss
	for (unsigned j = 0; j < swiss.size(); j++)
	{
	    // se e' meno distante di distance
	    double d = point_distance (swiss[j].p, curvature[i].p);
	    if (d < target_distance)
		// aggiungilo ai Ref del punt di trackin
		curvature2swiss[i].push_back (Ref (j, d));
	}

	// order minivector by distance
	std::sort (curvature2swiss[i].begin (), curvature2swiss[i].end ());
    }
}

void fill_terrain2curvature ()
{
    terrain2curvature.clear ();
    
    // per ogni punto originale nel tracking terrain
    for (unsigned i = 0; i < terrain.size(); i++)
    {
	// aggiungilo a terrain2curvature
	terrain2curvature.push_back (std::vector<Ref>());
	    
	// scan ogni punto curvature
	for (unsigned j = 0; j < curvature.size(); j++)
	{
	    // se e' meno distante di distance
	    double d = point_distance (curvature[j].p, terrain[i].p);
	    if (d < target_distance)
		// aggiungilo ai Ref del punt di trackin
		terrain2curvature[i].push_back (Ref (j, d));
	}

	// order minivector by distance
	std::sort (terrain2curvature[i].begin (), terrain2curvature[i].end ());
    }
}

int main (int argc, char *argv[])
{
    app_init (argc, argv);

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
    }

    if (terrain_file)
    {
	track_reader (terrain_file, &terrain_track, &terrain_track_order);

	terrain.clear ();
	for (unsigned i = 0; i < terrain_track->lines.size(); i++)
	    if (terrain_track->is_original (i))
		terrain.push_back (TrackSpot (i, terrain_track->start_point (i),
					      terrain_track->lifetime_elixir (i),
					      terrain_track->lines[i].strength));
    }

    if (curvature_file)
    {
	track_reader (curvature_file, &curvature_track, &curvature_track_order);
	curvature.clear ();
	for (unsigned i = 0; i < curvature_track->lines.size(); i++)
	    if (curvature_track->is_original (i))
		curvature.push_back (TrackSpot (i, curvature_track->start_point (i),
						curvature_track->lifetime_elixir (i),
						curvature_track->lines[i].strength));
    }

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
	fill_terrain2swiss();
	for (int i = 0; i < terrain2swiss.size (); i++)
	    if (terrain2swiss[i].size () > 0)
		t2s++;
	printf ("Terrain vs Swiss | found: %d, "
		"terrain left out: %d, swiss left out: %d\n", t2s, t-t2s, s-t2s);
    }
    
    if (do_curvature2swiss)
    {
	fill_curvature2swiss();
	for (int i = 0; i < curvature2swiss.size (); i++)
	    if (curvature2swiss[i].size () > 0)
		c2s++;
	printf ("Curvature vs Swiss | found: %d, "
		"curvature left out: %d, swiss left out: %d\n", c2s, c-c2s, s-c2s);
    }
    
    if (do_terrain2curvature)
    {
	fill_terrain2curvature();
	for (int i = 0; i < terrain2curvature.size (); i++)
	    if (terrain2curvature[i].size () > 0)
		t2c++;
	printf ("Terrain vs Curvature | found: %d, "
		"terrain left out: %d, curvature left out: %d\n", t2c, t-t2c, c-t2c);
    }
    
    // contare tutti i punti in terrain2swiss che hanno qualcuno anche in terrain2curv 
    if (do_allthree)
    {
	for (int i = 0; i < terrain2swiss.size (); i++)
	    if (terrain2swiss[i].size () > 0 && terrain2curvature[i].size () > 0)
		t2s2c++;
	printf ("At the same time in Terrain/Curvature/Swiss: %d\n", t2s2c);
    }
}
