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
	     "[-R radius] : radius to compute (dem output)"
	     "[-B baseRad] : base radius to compute (ssp output)\n"
	     "[-M maxRad] : max radius to compute (ssp output)\n"
	     "[-S stepRad] : step to increase radius (ssp output)\n"
	     "[-F skipFactor] : skip factor in png2mesh conversion (ssp output)\n"
	     "[-e] : increase radius exponentially (ssp output)\n"
	     "\n"
	     );
}

double base = 16.0;
double radius = 8.0;
double maxV = 256.0;
double step = 2.0;
bool expStep = false;
unsigned int skip = 2;
string * meshFile = NULL;
string * pngFile = NULL;
string * sspFile = NULL;
string * demFile = NULL;



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
	    case 'F':
	      skip = atoi(*++argv);
	      argc--;
	      break;
	    case 'e':
	      expStep=true;
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

  if (demFile)
  {
      curvStacker s(radius,radius,step,expStep,skip);

      if (pngFile)
	  s.executeOnPNG(*pngFile, *demFile);
      if (meshFile)
	  s.executeOnMesh(*meshFile, *demFile);
  }

  if (sspFile)
  {
      curvStacker s(base,maxV,step,expStep,skip);

      if (pngFile)
	  s.executeOnPNG(*pngFile, *sspFile);
      if (meshFile)
	  s.executeOnMesh(*meshFile, *sspFile);
  }
  
}
