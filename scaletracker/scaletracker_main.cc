#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "../common/dem.hh"
#include "../common/demreader.hh"
#include "../common/scalespace.hh"
#include "flipper.hh"

#include "imagewriter.hh"
// #include "tracking.hh"

#include <iostream> 
#include <vector>



char *imagefile = 0;
char *out_name = 0;
char *tracking_name = 0;
char *critical_name = 0;
char *tiffnames = 0;
char *statfile = 0;
int numlevel = 0;
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

void print_help (FILE* f)
{
    fprintf (f, "Usage: smoother\n"
	     "-n numlevel : number of levels\n"
	     "-i imagefile : supported inputs formats hgt, png, bmp\n"
	     "[-o outname] : write files outname.trk and outname.crt\n"
	     "[-t tiffnames] : write a tiff for every level\n"
	     "[-p amplitude seed] : randomly perturb data\n"
	     "[-j numjumps] : do not consider first n levels\n"
	     "[-b clip_bottom] : do not consider pixel with value less than bottom\n"
	     "[-c x0 y0 x1 y1] : crop original image\n"
	     "[-g stagenum] : stop at a certain point. 0 disable this.\n"
	     "[-l] : compute integral lines\n"
	     "[-s] : perform presmoothing of data\n"
	     "[-m mult] : multiply tiff output values by mult. Default auto mode\n"
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

            case 'o':
                out_name = (*++argv);
#define _FNLEN 512
		static char _tracking_str[_FNLEN] = {'\0'};
		static char _critical_str[_FNLEN] = {'\0'};
		snprintf (_tracking_str, _FNLEN, "%s%s" ,out_name, ".trk");
		snprintf (_critical_str, _FNLEN, "%s%s" ,out_name, ".crt");
		tracking_name = &(_tracking_str[0]);
		critical_name = &(_critical_str[0]);
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

    return;

    die:
    print_help(stderr);
    exit(1);
}

//void print_final_stats (Tracking* track, bool latex);

int main (int argc, char *argv[])
{
    app_init (argc, argv);

    DEMReader *dr = DEMSelector::get (imagefile);
    dr->print_info ();

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

    
    if (tiffnames)
    {
	Dem* d = new Dem (dr);
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

    if (stage == 1)
	exit (0);

    ScaleSpace ssp (dr, numlevel, opts);

    delete dr;

    if (stage == 2)
	exit (0);

    if (tiffnames)
    {
	for (int i = 0; i < ssp.levels; i++)
	{
	    ImageWriter iw (ssp.dem[i], tiff_mult);
	    iw.write (tiffnames, "", i);
	    iw.draw_points (&(ssp.critical[i]));
	    if (do_ilines)
		iw.draw_lines (&(ssp.ilines[i]));
	    iw.write (tiffnames, "-MARKED-", i);

	    tprintp ("###$$$", "Finished writing tiff %d!\n", i);
	}
    }

    if (stage == 3)
	exit (0);

    for (int i = 0; i < ssp.levels; i++)
    {
	int num_max, num_min, num_sad;
	num_max = num_min = num_sad = 0;
	for (unsigned j = 0; j < ssp.critical[i].size(); j++)
	{
	    if (ssp.critical[i][j].t == MAX)
		num_max++;
	    if (ssp.critical[i][j].t == MIN)
		num_min++;
	    if (ssp.critical[i][j].t == SA2)
		num_sad++;
	    if (ssp.critical[i][j].t == SA3)
		num_sad += 2;
	}
	printf ("\nlevel %d, num_max = %d, num_min = %d, num_sad = %d\n"
		"\tnum_max - num_min = %d, num_max + num_min = %d\n"
		"\t\tnum_max + num_min - num_sad = %d\n",
		i, num_max, num_min, num_sad,
		num_max - num_min, num_max + num_min,
		num_max + num_min - num_sad);
    }

    if (critical_name)
	ssp.write_critical (critical_name);

    if (stage == 4)
	exit (0);

    Flipper* flipper = new Flipper (ssp);
    
    Track* track = flipper->track (ssp);
    
    delete flipper;
	
    track->write (tracking_name);

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

#if 0

void print_final_stats (Tracking* track, bool latex)
{
    if (!latex)
    {
	printf ("\n\n\n==== final query statistics ====\n\n");
	printf ("scale (time)   | max (n) | min (n) | sad (n) "
		"||  av (n)   | avlog (n)\n");
    }
    std::vector<critical_rend_t> v;
    double s = 1.0;
    for (int i = 0; i < 5; i++)
    {
	s *= 3.0;
	if (!latex)
	    track->query (scale2time(s), v);
	else
	    track->query (scale2time(s), v, false);

	int num_sad, num_max, num_min, num_tot;
	int num_sad_new, num_max_new, num_min_new, num_tot_new;
	num_sad = num_max = num_min = num_tot = 0;
	num_sad_new = num_max_new = num_min_new = num_tot_new = 0;
	double av_time, av_scale, av_time_new, av_scale_new;
	av_time = av_scale = av_time_new = av_scale_new = 0.0;

	for (unsigned j = 0; j < v.size(); j++)
	{
	    av_time += v[j].tol;
	    av_scale += time2scale (v[j].tol);
	    num_tot++;

	    if (v[j].is_born)
	    {
		av_time_new += v[j].tol;
		av_scale_new += time2scale (v[j].tol);
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
		    s, scale2time(s),
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

#endif
