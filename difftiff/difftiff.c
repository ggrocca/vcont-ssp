#include <stdio.h>
#include <stdlib.h>

#include "../common/rastertiff.h"

/* typedef char bool; */
/* #define true 1 */
/* #define false 0 */

int file_num = 0;
char* file[2] = {NULL, NULL};
int verbose_level = 1;

void print_help (FILE* f)
{
    fprintf (f, "Usage: [-h] [-v LEVEL] file1.tiff [file2.tiff]\n"
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
	    /* printf ("!! 2 : file[0]: %s\n", file[0]); */
	    /* printf ("!! 2 : file[0]: %s\n", file[0]); */
	    /* printf ("!- argc: %d _ argv: %s __ file_num: %d\n", */
	    /* 	    argc, *argv, file_num); */

	    if (file_num < 2)
		file[file_num++] = *argv;
	    else
		file_num++;

	    /* printf ("!! 3 : file[0]: %s\n", file[0]); */
	    /* printf ("!! 3 : file[0]: %s\n", file[0]); */
	}

        if( (*argv)[0] == '-')
        {
	    /* printf ("=- argc: %d _ argv: %s\n", argc, *argv); */
	    /* printf ("!! 4 : file[0]: %s\n", file[0]); */
	    /* printf ("!! 4 : file[0]: %s\n", file[0]); */

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
	    /* printf ("!! 5 : file[0]: %s\n", file[0]); */
	    /* printf ("!! 5 : file[0]: %s\n", file[0]); */
        }
    }

    if (file_num < 1 || file_num > 2)
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
    /* printf ("!! 0 : file[0]: %s\n", file[0]); */
    /* printf ("!! 0 : file[0]: %s\n", file[0]); */

    app_init (argc, argv);

    /* printf ("!! 8 : file[0]: %s\n", file[0]); */
    /* printf ("!! 8 : file[0]: %s\n", file[0]); */

    TIFF* t1 = tiff_open_read (file[0]);
    int w, h;
    tiff_tag_read (t1, &w, &h);
    raster16_t r1 = raster16_read (t1, w, h);

    if (verbose_level > 0)
	printf ("image :[%5d][%5d]\n", w, h);

    if (file_num == 1)
    {
	raster16_print (r1, w, h);
	goto out1;
    }

    TIFF* t2 = tiff_open_read (file[1]);
    int _w, _h;
    tiff_tag_read (t2, &_w, &_h);
    
    if (w != _w || h != _h)
    {
	fprintf (stderr, "Error, second image :[%5d][%5d]\n", _w, _h);
	exit (1);
    }

    raster16_t r2 = raster16_read (t2, w, h);

    int diff_count = raster16_diff (r1, r2, w, h, verbose_level);

    if (diff_count)
    {
	printf ("Tiff files do not match.\n");
	if (verbose_level > 0)
	    printf ("Number of different pixels: %d\n", diff_count);
    }
    else
    {
	if (verbose_level > 0)
	    printf ("Tiff files match.\n");	
    }

    raster16_free (r2);
    
 out1:
    raster16_free (r1);
    exit (0);
}
