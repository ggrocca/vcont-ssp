#include <iostream>
#include <string>
#include "pngtomesh.hh"
#include "curvStacker.hh"

using namespace std;

void print_help ()
{
    fprintf (stdout, "Usage: curvStacker "
	     "{-i infile | -m infile} {-d outfile | -s outfile}\n"
	     "-i pngFile : path to input png image\n"
	     "-m meshFile : path to input mesh file\n"
	     "-s sspFile : path to output ssp file\n"
	     "-d demFile : path to output dem file.\n"
	     "[-c mapfile] : write map of curvature signs\n"
	     "[-R radius] : radius to compute (dem output)"
	     "[-B baseRad] : base radius to compute (ssp output)\n"
	     "[-M maxRad] : max radius to compute (ssp output)\n"
	     "[-S stepRad] : step to increase radius (ssp output)\n"
	     "[-F skipFactor] : skip factor in png2mesh conversion\n"
	     "[-e] : increase radius exponentially (ssp output)\n"
	     "[-G width height] : assume input mesh is a complete grid of given dim.\n"
	     "[-C cellSize] : grid cells have cellSize size.\n"
	     "[-Z curvMultFactor] : multiply curvature values by this value.\n"
	     "[-L LowestRandomizedValue] : background has this value, randomized"
	     "[-t topological index search]"
	     "[-L LowestRandomizedValue] : background has this value, randomized."
	     "[-t] : disable topology Index.\n"
	     "\n"
	     );
}

double base = 16.0;
double radius = 8.0;
double maxV = 256.0;
double step = 2.0;
bool expStep = false;
bool do_topoindex = false;
unsigned int skip = 2;
string * meshFile = NULL;
string * pngFile = NULL;
string * sspFile = NULL;
string * demFile = NULL;
bool grid = false;
int grid_width = 0;
int grid_height = 0;
double curvMultFactor = 0.0;
string * mapFile = NULL;
int cellSize = 25;
double lowestRandomValue = 0.0;

void app_init(int argc, char *argv[])
{
    for (argc--, argv++; argc--; argv++)
    {
        if( (*argv)[0] == '-')
        {
            switch( (*argv)[1] )
            {
	    case 'i':
	      pngFile=new string(*++argv);
	      argc--;
	      break;
	    case 'm':
	      meshFile=new string(*++argv);
	      argc--;
	      break;
	    case 's':
	      sspFile=new string(*++argv);
	      argc--;
	      break;
	    case 'd':
	      demFile=new string(*++argv);
	      argc--;
	      break;
	    case 'M':
	      maxV = atof(*++argv);
	      argc--;
	      break;
	    case 'R':
	      radius = atof(*++argv);
	      argc--;
	      break;
	    case 'B':
	      base = atof(*++argv);
	      argc--;
	      break;
	    case 'S':
	      step = atof(*++argv);
	      argc--;
	      break;
	    case 'Z':
	      curvMultFactor = atof(*++argv);
	      argc--;
	      break;
	    case 'L':
	      lowestRandomValue = atof(*++argv);
	      argc--;
	      break;
	    case 'C':
	      cellSize = atoi(*++argv);
	      argc--;
	      break;
	    case 'F':
	      skip = atoi(*++argv);
	      argc--;
	      break;
	    case 'G':
	      grid_width = atoi(*++argv);
	      argc--;
	      grid_height = atoi(*++argv);
	      argc--;
	      grid = true;
	      break;
	    case 'e':
	      expStep=true;
	      break;
	    case 't':
	      do_topoindex=true;
	      break;
	    case 'c':
	      mapFile = new string(*++argv);
	      argc--;
	      break;
	    case 'h':
	      print_help();
	      exit(0);
	      break;
	    default:
	      print_help();
	      exit(-1);
	    }
        }	
    }

    if ((sspFile == NULL && demFile == NULL) ||
	(sspFile != NULL && demFile != NULL))
	goto die;
     
    if ((meshFile == NULL && pngFile == NULL) ||
	(meshFile != NULL && pngFile != NULL))
	goto die;

    return;

 die:
    print_help ();
    exit (-1);
}

int main(int argc, char* argv[])
{
  app_init(argc,argv);
  cout << "exp : " << expStep << " step: " << step << endl;

  int now_base; int now_max; int now_step; int now_exp;
  now_base = demFile? radius : base;
  now_max = demFile? radius : maxV;
  now_step = demFile? 1 : step;
  now_exp = demFile? 0 : expStep;
  curvStacker s(now_base, now_max, now_step, now_exp ,skip, mapFile);

  if (grid)
      s.setGrid (grid_width, grid_height, cellSize);
  if (curvMultFactor != 0.0)
      s.curvMultFactor = curvMultFactor;
  if (lowestRandomValue != 0.0)
      s.lowestRandomValue = lowestRandomValue;
  s.do_topoindex = do_topoindex;
  
  if (demFile)
  {
      if (pngFile)
	  s.executeOnPNG(*pngFile, *demFile);
      if (meshFile)
	  s.executeOnMesh(*meshFile, *demFile);
  }

  if (sspFile)
  {
      if (pngFile)
	  s.executeOnPNG(*pngFile, *sspFile);
      if (meshFile)
	  s.executeOnMesh(*meshFile, *sspFile);
  }
  
}
