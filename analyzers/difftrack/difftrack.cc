#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../../common/track.hh"

int file_num = 0;
char* file[2] = {NULL, NULL};
int verbose_level = 1;

void print_help (FILE* f)
{
    fprintf (f, "Usage: [-h] [-v LEVEL] file1.track file2.track\n"
	     "[-h] : print help and exit\n"
	     "[-v level] : verbose level: 0, 1 (default) or 2\n"
	     "\n"
	     );
}

void app_init(int argc, char* argv[])
{
    for (argc--, argv++; argc--; argv++)
    {
	if( (*argv)[0] != '-')
	{
	    if (file_num < 2)
		file[file_num++] = *argv;
	    else
		file_num++;
	}

        if( (*argv)[0] == '-')
        {
            switch( (*argv)[1] )
            {
	    case 'h':
		print_help (stdout);
		exit (0);

	    case 'v':
                verbose_level = atoi (*++argv);
                argc--;
		break;

            default:
                goto die;
            }
        }
    }

    if (file_num != 2)
    	goto die;

    if (verbose_level < 0 || verbose_level > 2)
    	goto die;

    return;

    die:
    print_help(stderr);
    exit(1);
}


int main (int argc, char* argv[])
{
    app_init (argc, argv);

    Track t0 (file[0]);
    Track t1 (file[1]);

    int time_diff_total = 0;
    int time_diff_num = 0;
    double time_diff_avg = 0.0;
    double time_diff_max = 0.0;

    if (t0.time_of_life != t1.time_of_life)
	printf ("!! time_of_life diff: %.80lf\n",
		fabs (t0.time_of_life - t1.time_of_life));

    if (t0.lines.size() != t1.lines.size())
    {
	printf (">> tracks of different sizes: %zu %zu\n",
		t0.lines.size(), t1.lines.size());
	exit (1);
    }

    // cicla su t0.lines.size()
    for (int i = 0; i < t0.lines.size(); i++)
    {
	// controlla type
	if (t0.lines[i].type != t1.lines[i].type)
	    printf (">>> line %d. different types: %c %c\n", i,
		    critical2char (t0.lines[i].type),
		    critical2char (t1.lines[i].type));
	// controlla entries.size
	if (t0.lines[i].entries.size() != t1.lines[i].entries.size())
	{
	    printf (">>> line %d. different entries.sizes: %zu %zu\n", i,
		    t0.lines[i].entries.size(), t1.lines[i].entries.size());
	    continue;
	}

	time_diff_total += t0.lines[i].entries.size();

	// cicla entries.size
	for (int j = 0; j < t0.lines[i].entries.size(); j++)
	{
	    // Coord c;
	    if (t0.lines[i].entries[j].c != t1.lines[i].entries[j].c)
		printf (">>>> line %d, entry %d. different c: [%d,%d][%d,%d]\n", i, j,
			t0.lines[i].entries[j].c.x, t0.lines[i].entries[j].c.y,
			t1.lines[i].entries[j].c.x, t1.lines[i].entries[j].c.y);
	    // int mate; // other saddle if 2-saddle
	    if (t0.lines[i].entries[j].mate != t1.lines[i].entries[j].mate)
		printf (">>>> line %d, entry %d. different mate: %d %d\n", i, j,
			t0.lines[i].entries[j].mate, t1.lines[i].entries[j].mate);
	    // int life; // birth or death companion
	    if (t0.lines[i].entries[j].life != t1.lines[i].entries[j].life)
		printf (">>>> line %d, entry %d. different life: %d %d\n", i, j,
			t0.lines[i].entries[j].life, t1.lines[i].entries[j].life);

	    // double t;
	    if (t0.lines[i].entries[j].t != t1.lines[i].entries[j].t)
	    {
		time_diff_num++;
		double time_diff = t0.lines[i].entries[j].t - t1.lines[i].entries[j].t;
		time_diff = fabs (time_diff);

		time_diff_avg += time_diff;

		if (time_diff > time_diff_max)
		    time_diff_max = time_diff;

	    }
	}
    }

    if (time_diff_num > 0)
    {
	time_diff_avg /= ((double) time_diff_num);

	printf ("> time_diff_num: %d / %d.\n"
		"> time_diff_avg: %.80lf.\n"
		"> time_diff_max: %.80lf.\n",
		time_diff_num, time_diff_total,
		time_diff_avg, time_diff_max);
    }

    exit (0);
}
