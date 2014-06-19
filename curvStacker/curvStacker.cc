/* Program to build the discrete representation of a scalespace to serve as an input
to Luigi's framewoek. The input is the path to a .png range images, which is then converted to 
a 3D mesh, and three integers representing the base radius, the largest radius, and the step
of sampling */

#define IGL_HEADER_ONLY
#include "curvStacker.hh"
#include "pngtomesh.hh"
#include <cmath>
#include <igl/principal_curvature.h>
#include <igl/read.h>
#include <float.h>


curvStacker::curvStacker(double base_rad, double max_rad, double step, bool expStep, int skipFactor)
{
  isExpStep=expStep;

  base_radius=base_rad;
  max_radius=max_rad;
  this->step=step;
  this->skipFactor=skipFactor;

  if (expStep)
    n_levels=log(max_rad/base_rad)/log(step)+1;
  else
    n_levels=((max_rad-base_rad)/step)+1;

}


void curvStacker::printHeader()
{
  string outfile; /* Here we must obtain the output filename starting from the pngfile (or meshfile*/

  /* Fix temporaneo TODO togliere */
  outfile=pngFile+".ssp";

  fp=fopen(outfile.c_str(), "wb");
  if (!fp)
    {
      std::cerr << "Error opening scalespace output file " << outfile << std::endl;
      return;
    }
  fwrite(&(n_levels),sizeof(int),1,fp);
  return;
}

void curvStacker::executeOnPNG(string pngFile)
{
  this->pngFile=pngFile;
  p= new PngToMesh(pngFile,skipFactor,10);
  executeOnMesh(p->execute());
}




void curvStacker::initializeScaleSpace(Eigen::MatrixXd V)
{
  xmin=xmax=V.row(0)[0];
  ymin=ymax=V.row(0)[1];
  for (int i=0; i<V.rows(); i++)
    {
      if (std::abs(floor(V.row(i)[0])-V.row(i)[0])>1e-8)
	{
	  std::cerr << "Wrong coordinate: " << V.row(i)[0] << std::endl;
	  return;
	}
      if (std::abs(floor(V.row(i)[1])-V.row(i)[1])>1e-8)
	{
	  std::cerr << "Wrong coordinate: " << V.row(i)[1] << std::endl;
	  return;
	}
      if ((int)V.row(i)[0]<xmin)
	xmin=(int)V.row(i)[0];
      if ((int)V.row(i)[0]>xmax)
	xmax=(int)V.row(i)[0];
      if ((int)V.row(i)[1]<ymin)
	ymin=(int)V.row(i)[1];
      if ((int)V.row(i)[1]>ymax)
	ymax=(int)V.row(i)[1];
    }
  width=((xmax-xmin)/skipFactor+1);
  height=((ymax-ymin)/skipFactor+1);
 
}

// void curvStacker::minmaxCurvatures(double& min, double& max, vector<vector<double> > curv)
// {
//   min=max=curv[0][0]*curv[0][1];
//   for (int i=1; i<curv.size(); i++)
//     {
//       double t=curv[i][0]*curv[i][1];
//       if (t<min)
// 	min=t;
//       if (t>max)
// 	max=t;
//     }
// }

void curvStacker::printLevel(Eigen::MatrixXd V, vector<vector<double> > curv)
{
  double min, max;
  minmaxCurvatures(min,max,curv);
  double * data = (double*)malloc(sizeof(double)*width*height);
  for (int i=0; i<width*height; i++)
    data[i]=-DBL_MAX;

  fwrite(&width, sizeof(int), 1, fp);
  fwrite(&height, sizeof(int), 1, fp);

  for (int k=0; k<V.rows(); k++)
    {
      double curvT=curv[k][0]*curv[k][1];
      int x=V.row(k)[0];
      int y=V.row(k)[1];
      int i=(y-ymin)/2;
      int j=(x-xmin)/2;
      int i_gg=(i*width+j)/height;
      int j_gg=(i*width+j)%height;
      data[i*width+j]=curvT;
    }

  fwrite(&(data[0]), sizeof(double), width*height, fp);
  free(data);
 }

void curvStacker::executeOnMesh(string meshFile)
{

  cerr << "Starting execute on mesh " << endl;
  CurvatureCalculator c;
  Eigen::MatrixXd V;
  Eigen::MatrixXi F;
  cerr << "Reading mesh " << meshFile << endl;
  igl::read(meshFile,V,F);
  initializeScaleSpace(V);
  printHeader();
  cerr << "Mesh read with " << V.rows() << " vertices "  << endl;
  c.zeroDetCheck=false;
  c.init(V,F);
  for (double r=base_radius; r<=max_radius; isExpStep?r*=step:r+=step)
    {
      cerr << "Starting computing radius " << r << endl;
      c.sphereRadius=r;
      c.computeCurvature();
      cerr << "Computed radius " << r << endl;
      printLevel(V,c.curv);
  }
  fclose(fp);
}

