#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "../common/dem.hh"
#include "../common/demreader.hh"
// #include "../common/scalespace.hh"
// #include "../common/rastertiff.h"

#include "imagewriter.hh"
// #include "tracking.hh"

#include <iostream> 
#include <vector>



char *imagefile = 0;
char *trackfile = 0;
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

void print_help (FILE* f)
{
    fprintf (f, "Usage: smoother\n"
	     "-n numlevel : number of levels\n"
	     "-i imagefile : supported inputs formats hgt, png, bmp\n"
	     "[-o trackfile] : compute and write to file traking data structure\n"
	     "[-o averagefile] : compute and write to file averages\n"
	     "[-t tiffnames] : write a tiff for every level\n"
	     "[-p amplitude seed] : randomly perturb data\n"
	     "[-j numjumps] : do not consider first n levels\n"
	     "[-b clip_bottom] : do not consider pixel with value less than bottom\n"
	     "[-c x0 y0 x1 y1] : crop original image\n"
	     "[-g stagenum] : stop at a certain point. 0 disable this.\n"
	     "[-l] : compute integral lines\n"
	     "[-s] : perform presmoothing of data\n"
	     "\n"
	     );
}

//////////////////////////////////////////
//////  Scalespace Options Format  ///////
//////////////////////////////////////////
//
/////////////////////////////////////////////////////////
//
// #define SS_ILINES 1
// #define SS_PRESMOOTH 2
// #define SS_PERTURB 4
// #define SS_JUMP 8
// #define SS_CROP 16
// #define SS_CLIP 32
//
// typedef unsigned int SS_Opts;
//
// inline void set_opt (SS_Opts* os, int o);
// inline bool check_opt (SS_Opts os, int o);
//
// struct SS_OptValues {
//     double perturb_amp;
//     int perturb_seed;
//     int jump_num;
//     double clip_value;
//     int crop_x0;
//     int crop_y0;
//     int crop_x1;
//     int crop_y1;
// };
//
// DEMScaleSpace (DEMReader* base,
// 	       int num_levels,
// 	       SS_Opts op,
// 	       SS_OptValues opvs);
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

            case 'i':
                imagefile = (*++argv);
                argc--;
                break;

            case 'o':
                trackfile = (*++argv);
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

    printf ("### Start!\n");

    // SS_Opts op = 0;
    // SS_OptValues ov;
    
    // if (do_perturb)
    // {
    // 	set_opt (&op, SS_PERTURB);
    // 	ov.perturb_amp = perturb_amp;
    // 	ov.perturb_seed = ov.perturb_seed;
    // }

    // if (do_jump)
    // {
    // 	set_opt (&op, SS_JUMP);
    // 	ov.jump_num = numjumps;
    // }

    // if (do_clip)
    // {
    // 	set_opt (&op, SS_CLIP);
    // 	ov.clip_value = clip_value;
    // }

    // if (do_crop)
    // {
    // 	set_opt (&op, SS_CROP);
    // 	ov.crop_x0 = crop_x0;
    // 	ov.crop_y0 = crop_y0;
    // 	ov.crop_x1 = crop_x1;
    // 	ov.crop_y1 = crop_y1;
    // }

    // if (do_ilines)
    // 	set_opt (&op, SS_ILINES);

    // if (do_presmooth)
    // 	set_opt (&op, SS_PRESMOOTH);

    
    if (tiffnames)
    {
	Dem* d = new Dem (dr);
	if (do_crop)
	{
	    Dem* c = new Dem (*d, crop_a, crop_b);
	    ImageWriter iw (c);
	    iw.write (tiffnames, "-base", -1);
	    delete c;
	}
	else
	{
	    ImageWriter iw (d);
	    iw.write (tiffnames, "-base", -1);
	}
	delete d;
    }


    ///////////////////////////////////////////////////
    ///////// DEBUG END
    ///////////////////////////////////////////////////
    exit(0);
}
///////////////////////////////////////////////////
///////// DEBUG END
///////////////////////////////////////////////////


#if 0

    if (stage == 1)
	exit (0);

    DEMScaleSpace demsp (image, numlevel, op, ov);

    if (stage == 2)
	exit (0);

    if (tiffnames)
    {
#define __FNLEN 64
	char str[__FNLEN] = {'\0'};
	// ==== tiff'o'rama =========
	for (int i = 0; i < demsp.num_levels; i++)
	{
	    write_tiff (demsp.dems[i], 0, 0,
			tiffnames, i, multiplier);
	    snprintf (str, __FNLEN, "%s%s" ,tiffnames, "-MARKED-");
	    write_tiff (demsp.dems[i], &(demsp.c_points[i]),
			do_ilines? &(demsp.ilines[i]) : 0,
			str, i, multiplier);
	    printf ("###$$$ Finished writing tiff %d!\n", i);
	}
#undef __FNLEN
    }

    if (stage == 3)
	exit (0);

    for (int i = 0; i < demsp.num_levels; i++)
    {
	int num_max, num_min, num_sad;
	num_max = num_min = num_sad = 0;
	for (unsigned j = 0; j < demsp.c_points[i].size(); j++)
	{
	    if (demsp.c_points[i][j].c_type == MAX)
		num_max++;
	    if (demsp.c_points[i][j].c_type == MIN)
		num_min++;
	    if (demsp.c_points[i][j].c_type == SA2)
		num_sad++;
	    if (demsp.c_points[i][j].c_type == SA3)
		num_sad += 2;
	}
	printf ("\nlevel %d, num_max = %d, num_min = %d, num_sad = %d\n"
		"\tnum_max - num_min = %d, num_max + num_min = %d\n"
		"\t\tnum_max + num_min - num_sad = %d\n",
		i, num_max, num_min, num_sad,
		num_max - num_min, num_max + num_min,
		num_max + num_min - num_sad);
    }

    if (stage == 4)
	exit (0);

    if (trackfile)
    {
	Tracking *track = new Tracking (demsp);
	
	printf ("\n=========================\n\n"
		"flips_total = %d\nflips_num: ", track->flips_total);
	for (unsigned i = 0; i < track->flips_num.size(); i++)
	    printf ("%d ", track->flips_num[i]);
	printf ("\n");

	// print track data structure
	track->write (trackfile);

	print_final_stats (track, false);
	print_final_stats (track, true);
	
	delete track;
    }

    delete image;

    exit (0);

}



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
