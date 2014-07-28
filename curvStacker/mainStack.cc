#include <iostream>
#include <string>
#include "pngtomesh.hh"
#include "curvStacker.hh"

using namespace std;

void print_help ()
{
    fprintf (stdout, "Usage: curvStacker\n"
	     "-m pngFile : path to png image\n"
	     "[-b baseRad] : base radius to compute\n"
	     "[-M maxRad] : max radius to compute\n"
	     "[-s stepRad] : step to increase radius\n"
	     "[-f skipFactor] : skip factor in png2mesh conversion\n"
	     "[-e] : increase radius exponentially\n"
	     "\n"
	     );
}

double base;
double maxV;
double step;
bool expStep;
string * meshFile;
unsigned int skip;






void app_init(int argc, char *argv[])
{
    for (argc--, argv++; argc--; argv++)
    {
        if( (*argv)[0] == '-')
        {
            switch( (*argv)[1] )
            {
	    case 'm':
	      meshFile=new string(*++argv);
	      argc--;
	      break;
	    case 'M':
	      maxV = atof(*++argv);
	      argc--;
	      break;
	    case 'b':
	      base = atof(*++argv);
	      argc--;
	      break;
	    case 's':
	      step = atof(*++argv);
	      argc--;
	      break;
	    case 'f':
	      skip = atoi(*++argv);
	      argc--;
	      break;
	    case 'e':
	      expStep=true;
	      break;
	    default:
	      print_help();
	      exit(1);
	    }
        }	
    }
    
}

int main(int argc, char* argv[])
{
    
  meshFile=NULL;
  base= 16.0;
  maxV = 256.0;
  step = 2.0;
  expStep = false;
  skip = 2;
  app_init(argc,argv);
  cout << "exp : " << expStep << " step: " << step << endl;
  curvStacker s(base,maxV,step,expStep,skip);

  if (!meshFile)
    {
      print_help();
      return -1;
    }
  s.executeOnPNG(*meshFile);

}
