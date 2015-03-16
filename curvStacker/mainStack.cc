#include <iostream>
#include <string>
#include "pngtomesh.hh"
#include "curvStacker.hh"


using namespace std;

void print_help ()
{
    fprintf (stdout, "Usage: curvStacker "
	     "{-i infile | -m infile | -D n mesh1 ... meshN} {-d outfile | -s outfile | -o outfile}\n"
	     "-i pngFile : path to input png image\n"
	     "-m meshFile : path to input mesh file\n"
	     "-D n mesh1 ... meshN : use a scalespace of N smoothed meshes as input (the meshes must be named name-0.off ... name-n-1.off\n"
	     "-s sspFile : path to output ssp file\n"
	     "-d demFile : path to output dem file.\n"
	     "-o curvFile : path to output for textual cuvature.\n"
	     "[-U] (Only if launched with -D option): print a single .ssp file instead of multiple .dem files\n"
	     "[-c mapfile] : write map of curvature signs\n"
	     "[-z heightFile] : write vertex heights on a file\n"
	     "[-R radius] : radius to compute (dem output)\n"
	     "[-B baseRad] : base radius to compute (ssp output)\n"
	     "[-M maxRad] : max radius to compute (ssp output)\n"
	     "[-S stepRad] : step to increase radius (ssp output)\n"
	     "[-F skipFactor] : skip factor in png2mesh conversion\n"
	     "[-e] : increase radius exponentially (ssp output)\n"
	     "[-O n] (n=1,2) : speed up the computation (1 = a bit, 2 = more), the algorithm will be less stable for ill-disposed matrices\n"
	     "[-G width height] : assume input mesh is a complete grid of given dim.\n"
	     "[-C cellSize] : grid cells have cellSize size.\n"
	     "[-Z curvMultFactor] : multiply curvature values by this value.\n"
	     "[-L LowestRandomizedValue] : background has this value, randomized\n"
	     "[-I] : compute Shape Index\n"
	     "[-t] : disable topology Index.\n"
	     "\n"
	     );
}

double base = 16.0;
double radius = 8.0;
double maxV = 256.0;
double step = 2.0;
bool expStep = false;
bool shapeIndex = false;
bool do_topoindex = true;
unsigned int skip = 2;
bool grid = false;
int grid_width = 0;
int grid_height = 0;
double curvMultFactor = 0.0;
int cellSize = 25;
double lowestRandomValue = 0.0;
int meshNumb = 0;
bool separateDems = true;
int fastComputation=0;

// input 
string * meshFile = NULL;
string * pngFile = NULL;
// output
string * sspFile = NULL;
string * demFile = NULL;
string * curvFile = NULL;

// not sure, but probably not mandatory
string * mapFile = NULL;
string * heightFile = NULL;
vector<string> meshNames;

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
	    case 'o':
	      curvFile=new string(*++argv);
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
	      do_topoindex=false;
	      break;
	    case 'c':
	      mapFile = new string(*++argv);
	      argc--;
	      break;
	    case 'z':
	      heightFile = new string(*++argv);
	      argc--;
	      break;
	    case 'D':
	      meshNumb=atoi(*++argv);
	      meshNames.reserve(meshNumb);
	      --argc;
	      for (int i=0; i<meshNumb; i++, argc--)
		meshNames.push_back(string(*++argv));
	      break;
	    case 'U':
	      separateDems=false;
	      break;
	    case 'h':
	      print_help();
	      exit(0);
	      break;
	    case'O':
	      fastComputation=atoi(*++argv);
	      argc--;
	      if (fastComputation!=1 && fastComputation!=2)
		{
		  print_help();
		  exit(-1);
		}
	      break;
	    default:
	      print_help();
	      exit(-1);
   	    case 'I':
	      shapeIndex=true;
	      break;
	    }
        }	
    }

    if (curvFile && meshFile)
	return;
    else if (demFile && (meshFile || pngFile || meshNumb>0))
	return;
    else if (sspFile && (meshFile || pngFile))
	return;
    else
	goto die;	
    
    // if ((sspFile == NULL && demFile == NULL) ||
    // 	(sspFile != NULL && demFile != NULL))
    // 	goto die;
     
    // if ((meshFile == NULL && pngFile == NULL && meshNumb == 0) ||
    // 	(meshFile != NULL && pngFile != NULL))
    // 	goto die;

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
  curvStacker s(now_base, now_max, now_step, now_exp ,skip, mapFile, heightFile, fastComputation,shapeIndex);

  if (grid)
      s.setGrid (grid_width, grid_height, cellSize);
  if (curvMultFactor != 0.0)
      s.curvMultFactor = curvMultFactor;
  if (lowestRandomValue != 0.0)
      s.lowestRandomValue = lowestRandomValue;
  s.do_topoindex = do_topoindex;

  if (curvFile)
    {
      	  s.computeTextualCurvature(*meshFile, *curvFile);
    }
   
  if (demFile)
  {
    if (meshNumb>0)
      {
	s.executeOnMultipleMeshes(meshNames, *demFile, separateDems);
      }
    else if (pngFile)
      s.executeOnPNG(*pngFile, *demFile);
    else if (meshFile)
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
