#include <iostream>
#include <string>
#include <vector>
#include "pngtomesh.hh"
#include <Eigen/Core>

using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;

class curvStacker
{
  int n_levels; /* Number of levels in the stack */
  double base_radius, max_radius, step;
  bool isExpStep;
  FILE * fp;
  FILE * map;
  PngToMesh * p;
  double xmin,xmax,ymin,ymax;
  int width,height;
  string pngFile;
  int skipFactor;
  int grid_width, grid_height, grid_cell_size;  
  bool grid;
  string * mapFile;
  int fast_computation;
 public:
  curvStacker(double,double,double,bool,int,string*,int);
  void executeOnMesh(string,string);
  void executeOnPNG(string,string);
  void executeOnMultipleMeshes(vector<string>,string, bool);
  void printHeader(string);
  void printLevel(Eigen::MatrixXd V, vector<vector<double> > curv);
  void printLevelGrid(Eigen::MatrixXd V, vector<vector<double> > curv);
  void initializeScaleSpace(Eigen::MatrixXd V);
  void minmaxCurvatures(double& min, double& max, vector<vector<double> > curv);
  void setGrid (int, int, int);


  double curvMultFactor;  
  double lowestRandomValue;
  bool do_topoindex;  
};
