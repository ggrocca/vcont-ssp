#include <iostream>
#include <string>
#include "pngtomesh.hh"
#include "curvStacker.hh"

using namespace std;


int main(int argc, char* argv[])
{
  string pngPath(argv[1]);
  
  curvStacker s(16,256,2,true,2);

  s.executeOnPNG(pngPath);

}
