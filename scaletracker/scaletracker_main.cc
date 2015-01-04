#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "../common/dem.hh"
#include "../common/demreader.hh"
#include "../common/scalespace.hh"
#include "flipper.hh"
#include "../common/csvreader.hh"
#include "../common/ascheader.hh"

#include "imagewriter.hh"
// #include "tracking.hh"

#include <iostream> 
#include <vector>



char *imagefile = 0;
char *demfile = 0;
char *out_name = 0;
char *tracking_name = 0;
char *critical_name = 0;
char *critical_csv_name = 0;
char *scalespace_write_name = 0;
char *scalespace_read_name = 0;
char *plateau_name = 0;
char *asc_crop_name = 0;
char *tiffnames = 0;
char *statfile = 0;
int numlevel = 0;
bool do_relative_drop = false;
bool do_perturb = false;
double perturb_amp;
int perturb_seed;
bool do_jump = false;
int numjumps = 0;
bool do_clip = false;
double clip_value = -DBL_MAX;
bool do_crop = false;
Coord crop_a;
Coord crop_b;
bool do_ilines = false;
bool do_presmooth = false;
int stage = 0;
int tiff_mult = 0;
bool do_control = false;
int filter_algo = 0;
bool check_plateaus = false;
bool do_final_query = false;
// bool do_border_normalization = false;

void print_help (FILE* f)
{
    fprintf (f, "Usage: scaletracker\n"
	     "[-n numlevel] : number of levels\n"
	     "[-i imagefile] : supported inputs formats hgt, png, bmp\n"
	     "[-d demfile] : load dem in double raw format\n"
	     "[-r scalespace.ssp] : read scalespace in binary format\n"
	     "[-o outname] : write files outname.trk, outname.crt, outname.ssp\n"
	     "[-t tiffnames] : write a tiff for every level\n"
	     "[-p amplitude seed] : randomly perturb data\n"
	     "[-j numjumps] : do not consider first n levels\n"
	     "[-b clip_bottom] : do not consider pixel with value less than bottom\n"
	     "[-B clip -DBL_MAX] : do not consider pixel with value -DBL_MAX\n"
	     "[-c x0 y0 x1 y1] : crop original image\n"
	     "[-g stagenum] : stop at a certain point. 0 disable this.\n"
	     "[-l] : compute integral lines\n"
	     "[-s] : perform presmoothing of data\n"
	     "[-f N] : controlled filtering. 0: slow. 1: normal. 2: fast.\n"
	     "[-m mult] : multiply tiff output values by mult. Default auto mode\n"
	     "[-a] : check for flat areas\n"
	     "[-D] : substitute Relative Drop calculation for strength. Peaks only. \n"
	     // "[-e] : border points normalization\n"
	     "[-q] : do final tracking query.\n"
	     "\n"
	     );
}

//////////////////////////////////////////
//////  Scalespace Options Format  ///////
//////////////////////////////////////////
//
/////////////////////////////////////////////////////////
//
// static const unsigned int ILINES    = 1;
// static const unsigned int PRESMOOTH = 2;
// static const unsigned int PERTURB   = 4;
// static const unsigned int JUMP      = 8;
// static const unsigned int CROP      = 16;
// static const unsigned int CLIP      = 32;
//
// double perturb_amp;
// int perturb_seed;
// int jump_num;
// double clip_value;
// Coord crop_a;
// Coord crop_b;
//
// void set (int o) { os |= o; }
// bool check (int o) { return (bool)(os & o); }
//
/////////////////////////////////////////////////////////

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

	    case 'l':
		do_ilines = true;
		break;

	    case 'g':
                stage = atoi (*++argv);
                argc--;
		break;

	    case 's':
		do_presmooth = true;
		break;

	    case 'q':
		do_final_query = true;
		break;

	    case 'a':
		check_plateaus = true;
		break;

	    // case 'e':
	    // 	do_border_normalization = true;
	    // 	break;

            case 'n':
                numlevel = atoi (*++argv);
                argc--;
                break;

            case 'm':
                tiff_mult = atoi (*++argv);
                argc--;
                break;

            case 'i':
                imagefile = (*++argv);
                argc--;
                break;

            case 'd':
                demfile = (*++argv);
                argc--;
                break;

            case 'r':
                scalespace_read_name = (*++argv);
                argc--;
                break;

            case 'o':
                out_name = (*++argv);
#define _FNLEN 512
		static char _tracking_str[_FNLEN] = {'\0'};
		static char _critical_str[_FNLEN] = {'\0'};
		static char _critical_csv_str[_FNLEN] = {'\0'};
		static char _scalespace_str[_FNLEN] = {'\0'};
		static char _plateau_str[_FNLEN] = {'\0'};
		static char _asc_crop_str[_FNLEN] = {'\0'};
		snprintf (_tracking_str, _FNLEN, "%s%s" ,out_name, ".trk");
		snprintf (_critical_str, _FNLEN, "%s%s" ,out_name, ".crt");
		snprintf (_critical_csv_str, _FNLEN, "%s%s" ,out_name, ".crt.csv");
		snprintf (_scalespace_str, _FNLEN, "%s%s" ,out_name, ".ssp");
		snprintf (_plateau_str, _FNLEN, "%s%s" ,out_name, ".plt");
		snprintf (_asc_crop_str, _FNLEN, "%s%s" ,out_name, "_crop.asc");
		tracking_name = &(_tracking_str[0]);
		critical_name = &(_critical_str[0]);
		critical_csv_name = &(_critical_csv_str[0]);
		scalespace_write_name = &(_scalespace_str[0]);
		plateau_name = &(_plateau_str[0]);
		asc_crop_name = &(_asc_crop_str[0]);
#undef _FNLEN
		argc--;
                break;

            case 't':
                tiffnames = (*++argv);
                argc--;
                break;

            case 'p':
		do_perturb = true;
                perturb_amp = atof (*++argv);
                perturb_seed = atoi (*++argv);
                argc--;
                argc--;
                break;

            case 'j':
		do_jump = true;
                numjumps = atoi (*++argv);
                argc--;
                break;

            case 'b':
		do_clip = true;
                clip_value = atof (*++argv);
                argc--;
                break;

	    case 'B':
		do_clip = true;
                clip_value = -DBL_MAX;
                break;

	    case 'D':
		do_relative_drop = true;
                break;

            case 'f':
                filter_algo = atoi (*++argv);
                argc--;
		do_control = true;
                break;

            case 'c':
		do_crop = true;
                crop_a.x = atoi (*++argv);
                crop_a.y = atoi (*++argv);
                crop_b.x = atoi (*++argv);
                crop_b.y = atoi (*++argv);
                argc--;
                argc--;
                argc--;
                argc--;
                break;
            default:
                goto die;
            }
        }
    }

    if (imagefile == NULL && scalespace_read_name == NULL && demfile == NULL)
    {
	fprintf (stderr, "No image, dem or scalespace input given.\n");
	goto die;
    }

    if ((imagefile != NULL || demfile != NULL) && numlevel < 2)
    {
	fprintf (stderr, "Number of levels `%d' is not valid. "
		 "Positive integer greater than 1 required.\n", numlevel);
	goto die;	
    }
    
    if (filter_algo < 0 || filter_algo > 2)
    {
	fprintf (stderr, "Unrecognized filter algorithm.\n");
	goto die;
    }
    

    return;

    die:
    print_help(stderr);
    exit(1);
}

void print_final_stats (Track* track, bool latex);

int main (int argc, char *argv[])
{
    app_init (argc, argv);
    
    Dem* idem = NULL;
    DEMReader* dr = NULL;
    DEMSelector::Format fmt;
    ASCHeader* asch = NULL;
    CSVReader csvio;

    if (imagefile != NULL)
    {
	dr = DEMSelector::get (imagefile, &fmt);
	dr->print_info ();
	idem = new Dem (dr);
	delete dr;

	if (fmt == DEMSelector::ASC)
	{
	    asch = new ASCHeader (imagefile);
	    csvio = CSVReader (*asch);
	}
	    
    }

    if (demfile != NULL)
    {
	FILE *fp = fopen (demfile, "r");
	if (fp == NULL)
	    eprintx (2, "Could not open file `%s'. %s\n", demfile, strerror (errno));

	idem = new Dem (fp);
	fclose (fp);
    }

    tprintp ("###", "%s", "Start!\n");

    ScaleSpaceOpts opts;
    
    if (do_perturb)
    {
    	opts.set (ScaleSpaceOpts::PERTURB);
    	opts.perturb_amp = perturb_amp;
    	opts.perturb_seed = perturb_seed;
    }

    if (do_jump)
    {
    	opts.set (ScaleSpaceOpts::JUMP);
    	opts.jump_num = numjumps;
    }

    if (do_clip)
    {
    	opts.set (ScaleSpaceOpts::CLIP);
    	opts.clip_value = clip_value;
    }

    if (do_crop)
    {
    	opts.set (ScaleSpaceOpts::CROP);
    	opts.crop_a = crop_a;
    	opts.crop_b = crop_b;
    }

    if (do_ilines)
    	opts.set (ScaleSpaceOpts::ILINES);

    if (do_presmooth)
    	opts.set (ScaleSpaceOpts::PRESMOOTH);

    if (do_control)
    {
    	opts.set (ScaleSpaceOpts::CONTROL);
	opts.filter_algo = filter_algo;
    }
    
    if (tiffnames && idem != NULL)
    {
	Dem* d = new Dem (*idem);
	if (do_crop)
	{
	    Dem* c = new Dem (*d, crop_a, crop_b);
	    ImageWriter iw (c, tiff_mult);
	    iw.write (tiffnames, "-base", -1);
	    delete c;
	}
	else
	{
	    ImageWriter iw (d, tiff_mult);
	    iw.write (tiffnames, "-base", -1);
	}
	delete d;
    }

    if (asch != NULL && idem != NULL && do_crop)
    {
    	Dem* c = new Dem (*idem, crop_a, crop_b);
	ASCHeader asch_crop (*asch, crop_a, crop_b);
    	c->write (asc_crop_name, asch_crop);
    	delete c;
    }
    if (asch != NULL)
    	delete asch;
    
    if (stage == 1)
	exit (0);

    ScaleSpace *ssp;

    if (idem != NULL)
    {
	ssp = new ScaleSpace (*idem, numlevel, opts);
	delete idem;
    }
    else if (scalespace_read_name != NULL)
	ssp = new ScaleSpace (scalespace_read_name, opts);
    else
	eprintx (1, "%s","No image or scalespace input given.\n");

    if (stage == 2)
	exit (0);

    for (int i = 0; i < ssp->levels; i++)
    {
	tprintp ("###>>>", "Dem %d. max: %lf, min: %lf\n",
		 i, ssp->dem[i]->max, ssp->dem[i]->min);

	if (tiffnames)
	{
	    ImageWriter iw (ssp->dem[i], tiff_mult);
	    iw.write (tiffnames, "", i);
	    iw.draw_points (&(ssp->critical[i]));
	    if (do_ilines)
		iw.draw_lines (&(ssp->ilines[i]));
	    iw.write (tiffnames, "-MARKED", i);

	    tprintp ("###$$$", "Finished writing tiff %d!\n", i);
	}
    }

    if (scalespace_write_name)
	ssp->write_scalespace (scalespace_write_name);
    
    if (check_plateaus)
    {
	int level_with_flats = -1;
	for (int i = 0; i < ssp->levels; i++)
	    if (ssp->dem[i]->has_plateaus())
	    {
		level_with_flats = i;
		break;
	    }

	if (level_with_flats != -1)
	{
	    ssp->write_plateaus (plateau_name);	    
	    eprintx (2, "Scalespace has flat areas, starting at level %d."
		     " Critical points extraction and tracking aborted.\n",
		     level_with_flats);
	}
    }
    
    if (stage == 3)
	exit (0);

    for (int i = 0; i < ssp->levels; i++)
    {
	int num_max, num_min, num_sad;
	num_max = num_min = num_sad = 0;
	for (unsigned j = 0; j < ssp->critical[i].size(); j++)
	{
	    if (ssp->critical[i][j].t == MAX)
		num_max++;
	    if (ssp->critical[i][j].t == MIN)
		num_min++;
	    if (ssp->critical[i][j].t == SA2)
		num_sad++;
	    if (ssp->critical[i][j].t == SA3)
		num_sad += 2;
	}
	fprintf (OTHER_STREAM, "\nlevel %d, num_max = %d, num_min = %d, num_sad = %d\n"
		"\tnum_max - num_min = %d, num_max + num_min = %d\n"
		"\t\tnum_max + num_min - num_sad = %d\n",
		i, num_max, num_min, num_sad,
		num_max - num_min, num_max + num_min,
		num_max + num_min - num_sad);
    }

    if (critical_name)
	ssp->write_critical (critical_name);
    
    if (critical_csv_name)
	csvio.save (critical_csv_name, ssp);

    if (stage == 4)
	exit (0);

    Flipper* flipper = new Flipper (*ssp);
    
    Track* track = new Track ();
    TrackOrdering* order = new TrackOrdering ();
    flipper->track (*ssp, track, order);
    
    delete flipper;

    track->drink_elixir ();

    // if (do_border_normalization)
    // 	track->normalize_border_points ();
    
    track->get_strength (ssp->dem[0]);

    if (do_relative_drop)
	track->get_relative_drop (ssp->dem[0]);
    
    //track->write (tracking_name, order);
    track_writer (tracking_name, track, order);

    if (do_final_query)
	print_final_stats (track, false);

    delete track;

	// Tracking *track = new Tracking (demsp);
	
	// printf ("\n=========================\n\n"
	// 	"flips_total = %d\nflips_num: ", track->flips_total);
	// for (unsigned i = 0; i < track->flips_num.size(); i++)
	//     printf ("%d ", track->flips_num[i]);
	// printf ("\n");

	// print track data structure
	// track->write (trackfile);

	// print_final_stats (track, false);
	// print_final_stats (track, true);
	
	// delete track;

    exit (0);
}

void print_final_stats (Track* track, bool latex)
{
    if (!latex)
    {
	printf ("\n\n\n==== final query statistics ====\n\n");
	printf ("scale (time)   | max (n) | min (n) | sad (n) "
		"||  av (n)   | avlog (n)\n");
    }
    std::vector<TrackRenderingEntry> v;
    double s = 1.0;
    for (int i = 0; i < 5; i++)
    {
	s *= 3.0;
	if (!latex)
	    track->query (ScaleSpace::scale2time(s), v);
	else
	    track->query (ScaleSpace::scale2time(s), v, false);

	int num_sad, num_max, num_min, num_tot;
	int num_sad_new, num_max_new, num_min_new, num_tot_new;
	num_sad = num_max = num_min = num_tot = 0;
	num_sad_new = num_max_new = num_min_new = num_tot_new = 0;
	double av_time, av_scale, av_time_new, av_scale_new;
	av_time = av_scale = av_time_new = av_scale_new = 0.0;

	for (unsigned j = 0; j < v.size(); j++)
	{
	    av_time += v[j].tol;
	    av_scale += ScaleSpace::time2scale (v[j].tol);
	    num_tot++;

	    if (v[j].is_born)
	    {
		av_time_new += v[j].tol;
		av_scale_new += ScaleSpace::time2scale (v[j].tol);
		num_tot_new++;
	    }

	    switch (v[j].type)
	    {
	    case MIN:
		num_min++;
		break;
	    case MAX:
		num_max++;
		break;
	    case SA2:
		num_sad++;
		break;
	    case SA3:
		num_sad++;
		num_sad++;
		break;
	    default: ;
	    }
		    
	    if (v[j].is_born)
		switch (v[j].type)
		{
		case MIN:
		    num_min_new++;
		    break;
		case MAX:
		    num_max_new++;
		    break;
		case SA2:
		    num_sad_new++;
		    break;
		case SA3:
		    num_sad_new++;
		    num_sad_new++;
		    break;
		default: ;
		}
	}
		
	av_time /= num_tot;
	av_scale /= num_tot;
	av_time_new /= num_tot_new;
	av_scale_new /= num_tot_new;

	if (!latex)
	    printf ("%.1lf (%.3lf)  | %d (%d) | %d (%d) | %d (%d) "
		    "|| %lf (%lf) | %lf (%lf)\n",
		    s, ScaleSpace::scale2time(s),
		    num_max, num_max_new, num_min, num_min_new, num_sad,
		    num_sad_new, av_time, av_time_new, av_scale, av_scale_new);
	// 3 & 1718 (1163) & ??(??) & ??(??) & ??(??) & ??(??) 
	else
	    printf ("%.1lf & %d(%d) & %d(%d) & %d(%d) "
		    "& %.2lf(%.2lf) & %.2lf(%.2lf) \\\\ \n", s,
		    num_max, num_max_new, num_min, num_min_new, num_sad,
		    num_sad_new, av_time, av_time_new, av_scale, av_scale_new);
	
    }
}
