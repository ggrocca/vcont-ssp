#include "demreader.hh"
#include "csvreader.hh"
#include "track.hh"

#include <vector>

char *asc_file = 0;
char *output_swiss_file = "swissclassifier_output.csv";
char *track_file = 0;
double strength_threshold = 0.0;
double life_threshold = 0.0;

void print_help (FILE* f)
{
    fprintf (f, "Usage: track2csv\n"
	     "-a asc file : for georegistration\n"
	     "-t trk file : activate optional refining step using tracking\n"
	     "[-o outputname] : csv output file\n"
	     "[-S strength_threshold] : strength threshold for refining step\n"
	     "[-L life_threshold] : life threshold for refining step\n"
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
            case 'h':
		print_help (stdout);
		exit (0);
                break;

            case 'a':
                asc_file = (*++argv);
                argc--;
                break;

            case 't':
                track_file = (*++argv);
                argc--;
                break;

	    case 'o':
                output_swiss_file = (*++argv);
                argc--;
                break;

            case 'L':
                strength_threshold = atof (*++argv);
                argc--;
                break;

            case 'S':
                strength_threshold = atof (*++argv);
                argc--;
                break;

            default:
                goto die;
            }
        }
    }

    if (track_file == 0 || asc_file == 0)
    {
	eprint ("%s\n\n", "Missing input file(s).");
	goto die;
    }
    
    return;
	
 die:
    print_help(stderr);
    exit(1);
}


int main (int argc, char *argv[])
{
    app_init (argc, argv);

    ASCReader ascr (asc_file);    
    std::vector <SwissSpotHeight> swiss;

    Track *track;
    TrackOrdering *track_order;
    track_reader (track_file, &track, &track_order);

    for (unsigned i = 0; i < track->lines.size(); i++)
	if (track->is_original(i) &&
	    track->lifetime_elixir (i) >= life_threshold &&
	    track->strength (i) >= strength_threshold)
	{
	    Coord c = track->lines[i].entries[0].c;
	    Point p = coord2point (c);
	    SwissSpotHeight sh (p);
	    sh.z = ascr.get_pixel (c.x, c.y);
	    sh.ct = critical2classified (track->get_type (i));
	    sh.st = UNDEFINED;
	    db_set (sh.dbt, DB_TRACK);
	    sh.life = track->lifetime_elixir (i);
	    sh.strength = track->strength (i);
	    swiss.push_back (sh);
	}
    
    CSVReader csvio (ascr);
    csvio.save (output_swiss_file, swiss);
}
