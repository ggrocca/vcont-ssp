#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "../../common/rastertiff.h"
#include "../../common/dem.hh"
#include "../../common/demreader.hh"

#include <iostream> 
#include <vector>



char *imagefile = 0;
char *out_name = "out.off";
char *tiffnames = 0;
bool do_crop = false;
Coord crop_a;
Coord crop_b;

// GG things todo or add back in:

// float multiplier for z values
// background clipping + mantaining only bigger connected component
// presmoothing and / or interaction with scaletracker (??? non si sa che vuol dire)

void print_help (FILE* f)
{
    fprintf (f, "Usage: mesher\n"
	     "[-i imagefile] : supported inputs formats hgt, png, bmp\n"
	     "[-o mesh.off] : write output mesh, in off format.\n"
	     "[-c x0 y0 x1 y1] : crop original image.\n"
	     "[-t tiffname] : write a tiff image for debug.\n"
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

            case 'i':
                imagefile = (*++argv);
                argc--;
                break;

            case 'o':
                out_name = (*++argv);
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

            case 't':
                tiffnames = (*++argv);
                argc--;
                break;

            default:
                goto die;
            }
        }
    }

    if (imagefile == NULL)
    {
	fprintf (stderr, "No image input given.\n");
	goto die;
    }

    return;

    die:
    print_help(stderr);
    exit(1);
}

// GG should use imagewriter
void _mesher_write_tiff (Dem *dem, int mult, char* name);


int main (int argc, char *argv[])
{
    app_init (argc, argv);
    
    DEMReader* dr = NULL;

    if (imagefile != NULL)
    {
	dr = DEMSelector::get (imagefile);
	dr->print_info ();
    }
    else
	exit (1);

    Dem *dem = new Dem (dr);

    if (do_crop)
    {
    	Dem* td = new Dem (*dem, crop_a, crop_b);
	delete (dem);
	dem = td;
    }

    _mesher_write_tiff (dem, 14, tiffnames);

    std::vector< std::vector<int> > vs;
    std::vector< std::vector<int> > fs;

    int k = 0;
    for (int i = 0; i < dem->width; i++)
	for (int j = 0; j < dem->width; j++)
	{
	    // vertices
	    std::vector<int> vertex;
	    vertex.push_back (i * 30);
	    vertex.push_back (j * 30);
	    vertex.push_back ((*dem)(i, j));

	    vs.push_back (vertex);

	    // faces
	    if (i < dem->width-1 && j < dem->height-1)
	    {
		int v   = ( i    * dem->height) +  j;    // current vertex
		int ve  = ((i+1) * dem->height) +  j;    // east vertex
		int vn  = ( i    * dem->height) + (j+1); // north vertex
		int vne = ((i+1) * dem->height) + (j+1); // north-east vertex

		std::vector<int> f1;
		std::vector<int> f2;

		f1.push_back (v);
		f1.push_back (ve);
		f1.push_back (vne);

		f2.push_back (v);
		f2.push_back (vne);
		f2.push_back (vn);
		
		fs.push_back (f1);
		fs.push_back (f2);
	    }
	}

    FILE *f = fopen (out_name, "w");
    
    fprintf (f, "OFF\n%zu %zu 0\n", vs.size(), fs.size());

    for (unsigned i = 0; i < vs.size(); i++)
	fprintf (f, "%d %d %d\n", vs[i][0], vs[i][1], vs[i][2]);

    for (unsigned i = 0; i < fs.size(); i++)
	fprintf (f, "3 %d %d %d\n", fs[i][0], fs[i][1], fs[i][2]);

    delete dr;
    delete dem;

    exit (0);
}




void _mesher_write_tiff (Dem *dem, int mult, char* name)
{
    raster16_t raster_out;
    unsigned width, height;

    raster_out = raster16_new (dem->width, dem->height);
    width = dem->width;
    height = dem->height;
    int max = (int) dem->max;
    int min = mult!= 0? 0 : (int) dem->min;
    int m = mult != 0? mult : (65536 / (max - min));

    printf ("max: %d, min: %d, multiplier: %d\n", max, min, m);

    for (int i = 0; i < dem->width; i++)
    	for (int j = 0; j < dem->height; j++)
	{
	    int v = (int)((((*dem)(Coord (i, j))) * ((double) m)) - ((double) min));
    	    raster16_set (raster_out, i, j, dem->width, dem->height,
			  pix16_mono ((unsigned short)v));
	}

    TIFF* tiff_out = tiff_open_write (name);
    tiff16_tag_write (tiff_out, width, height);
    raster16_write (tiff_out, raster_out, width, height);
    TIFFClose (tiff_out);

}