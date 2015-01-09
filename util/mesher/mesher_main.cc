#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "../../common/rastertiff.h"
#include "../../common/dem.hh"
#include "../../common/demreader.hh"
#include "../../common/scalespace.hh"

#include <iostream> 
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <set>

char *imagefile = 0;
char *sspname = 0;
char *out_name = "out.off";
char *tiffnames = 0;
bool do_crop = false;
Coord crop_a;
Coord crop_b;
int cell_size = 25;
int mult_factor = 1;
double threshold=0.0;
bool threshActive=false;

// GG things todo or add back in:

// float multiplier for z values
// background clipping + mantaining only bigger connected component
// presmoothing and / or interaction with scaletracker (??? non si sa che vuol dire)

void print_help (FILE* f)
{
    fprintf (f, "Usage: mesher\n"
	     "[-i imagefile] : supported inputs formats hgt, png, bmp\n"
	     "[-s sspfile] : input file ssp, activation of multiple output format\n"
	     "[-o mesh.off] : write output mesh, in off format.\n"
	     "[-c x0 y0 x1 y1] : crop original image.\n"
	     "[-t tiffname] : write a tiff image for debug.\n"
	     "[-C CELLSIZE] : multiply xy pixel coordinates by this value.\n"
	     "[-M MULTFACTOR] : multiply z intensities by this value.\n"
	     "[-b threshold] : crop values < threshold\n"
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

	    case 's':
		sspname = (*++argv);
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

	    case 'C':
                cell_size = atoi (*++argv);
                argc--;
                break;

	    case 'M':
                mult_factor = atoi (*++argv);
                argc--;
                break;

            case 't':
                tiffnames = (*++argv);
                argc--;
                break;

	    case 'b':
	      threshold = atof(*++argv);
	      argc--;
	      threshActive=true;
	      break;
            default:
                goto die;
            }
        }
    }

    if ((imagefile == NULL && sspname == NULL) ||
	(imagefile != NULL && sspname != NULL))
    {
	fprintf (stderr, "An input image OR an input ssp required.\n");
	goto die;
    }

    return;

    die:
    print_help(stderr);
    exit(1);
}

// GG should use imagewriter
void _mesher_write_tiff (Dem *dem, int mult, char* name);

void mesher (Dem* dem, const char* offname);


int main (int argc, char *argv[])
{
    app_init (argc, argv);

    if (imagefile)
    {
	DEMReader* dr = NULL;

	if (imagefile != NULL)
	{
	    dr = DEMSelector::get (imagefile);
	    dr->print_info ();
	}
	else
	    exit (1);

	Dem* dem = new Dem (dr);

	if (do_crop)
	{
	    Dem* td = new Dem (*dem, crop_a, crop_b);
	    delete (dem);
	    dem = td;
	}

	if (tiffnames)
	    _mesher_write_tiff (dem, 14, tiffnames);

	// call mesher
	mesher (dem, out_name);

	delete dr;
	delete dem;
    }

    if (sspname)
    {
	ScaleSpace *ssp = new ScaleSpace (sspname, ScaleSpaceOpts());

	for (int i = 0; i < ssp->levels; i++)
	{
	    std::string ss (out_name);
	    ss += "-";
	    std::stringstream sstm;
	    sstm << ss << i;
	    std::string offname = sstm.str();
	    offname += ".off";
	    mesher (ssp->dem[i], offname.c_str());
	}
	delete ssp;
    }
    exit (0);
}


void mesher (Dem* dem, const char* offname)
{
    std::vector< std::vector<int> > vs;
    std::vector< std::vector<int> > fs;
    std::set<int> zeroVert;

    std::vector< std::vector<int> > newvs;
    std::vector< std::vector<int> > newfs;
    int skipF=1;
    // int k = 0;
    for (int i = 0; i < dem->width; i+=skipF)
	for (int j = 0; j < dem->height; j+=skipF)
	{
	  // vertices
	    std::vector<int> vertex;
	    vertex.push_back (i * cell_size);
	    vertex.push_back (j * cell_size);
	    vertex.push_back ((((*dem)(i, j)) * mult_factor));
	    vs.push_back (vertex);

	    // faces
	    if (i < dem->width-skipF && j < dem->height-skipF)
	    {
		int v   = ( i    * dem->height) +  j;    // current vertex
		int ve  = ((i+skipF) * dem->height) +  j;    // east vertex
		int vn  = ( i    * dem->height) + (j+skipF); // north vertex
		int vne = ((i+skipF) * dem->height) + (j+skipF); // north-east vertex

		std::vector<int> f1;
		std::vector<int> f2;

		f1.push_back (v/skipF);
		f1.push_back (ve/skipF);
		f1.push_back (vne/skipF);

		f2.push_back (v);
		f2.push_back (vne);
		f2.push_back (vn);
		
		fs.push_back (f1);
		fs.push_back (f2);
	    }
	}



    // if (threshActive)
    //   {
    // 	int totVert=0;
    // 	for (int i=0; i<vs.size(); i++)
    // 	  {
    // 	    //TODO: call depending on parameter, thres as variable
    // 	    if (vs[i][2]<threshold)
    // 	      zeroVert.insert(i);
    // 	    else
    // 	      {
    // 		totVert++;
    // 		newvs.push_back(vs[i]);
    // 	      }
    // 	  }

    // 	int* realIndex=(int*)calloc(vs.size(),sizeof(int));

    // 	int k=0;
    // 	for (int i=0; i<vs.size(); i++)
    // 	  {
    // 	    if (zeroVert.count(i)!=0)
    // 	      continue;
    // 	    realIndex[i]=k++;
    // 	  }

    // 	for (int i=0; i<fs.size(); i++)
    // 	  {
    // 	    if (zeroVert.count(fs[i][0])!=0 || zeroVert.count(fs[i][1])!=0 || zeroVert.count(fs[i][2])!=0 )
    // 	      continue;
    // 	    std::vector<int> vv;
    // 	    vv.push_back(realIndex[fs[i][0]]);
    // 	    vv.push_back(realIndex[fs[i][1]]);
    // 	    vv.push_back(realIndex[fs[i][2]]);
    // 	    newfs.push_back(vv);
    // 	  }
    //   }    
    // else 
    //   {
	newvs=vs;
	newfs=fs;
	//      }
    FILE *f = fopen (offname, "w");
    if (f == NULL)
	eprintx (2, "Could not open file `%s'. %s\n", offname, strerror (errno));
    

    fprintf (f, "OFF\n%zu %zu 0\n", newvs.size(), newfs.size());

    for (unsigned i = 0; i < newvs.size(); i++)
	fprintf (f, "%d %d %d\n", newvs[i][0], newvs[i][1], newvs[i][2]);

    for (unsigned i = 0; i < newfs.size(); i++)
	fprintf (f, "3 %d %d %d\n", newfs[i][0], newfs[i][1], newfs[i][2]);

    fclose(f);

    if (threshActive)
      {
         #include <unistd.h>
	char un[64] = "\0";
	getlogin_r (un, 64);
	std::string username_curr (un);
	std::string username_gg ("gg");
	std::string meshlabserver_path ((username_gg == username_curr)?
					"/Applications/meshlab.app/Contents/MacOS/meshlabserver":
					"meshlabserver");
	// std::cout << username_curr <<":"<< username_gg<<":"<<meshlabserver_path<<std::endl;


	std::string removescript ((username_gg == username_curr)? "removegg.mlx" : "remove.mlx");
  
	std::string cmd = meshlabserver_path+" -i "+offname+" -o "+offname+" -s "+removescript;
	system(cmd.c_str());
      }
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
