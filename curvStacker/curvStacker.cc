/* Program to build the discrete representation of a scalespace to serve as an input
to Luigi's framework. The input is the path to a .png range images, which is then converted to 
a 3D mesh, and three integers representing the base radius, the largest radius, and the step
of sampling */

#define IGL_HEADER_ONLY
#include "curvStacker.hh"
#include "pngtomesh.hh"
#include <cmath>
#include <igl/principal_curvature.h>
#include <igl/read.h>
#include <float.h>
#include "../common/scalespace.hh"
#include "../common/grid.hh"
#include <sstream>
#include <cfloat>
#include <algorithm>

curvStacker::curvStacker(double base_rad, double max_rad, double step, bool expStep, int skipFactor, string* mapFile, string* heightFile, int fast)
{
  srand((unsigned)time(NULL));
  isExpStep=expStep;

  base_radius=base_rad;
  max_radius=max_rad;
  this->step=step;
  this->skipFactor=skipFactor;
  this->mapFile=mapFile;
  this->heightFile=heightFile;
  if (expStep)
    n_levels=log(max_rad/base_rad)/log(step)+1;
  else
    n_levels=((max_rad-base_rad)/step)+1;

  curvMultFactor = 10e3;
  grid = false;
  do_topoindex = false;
  fast_computation=fast;
  validFile=new string("valid");
}


void curvStacker::printHeader(string outfile)
{


  fp=fopen(outfile.c_str(), "wb");
  if (!fp)
    {
      std::cerr << "Error opening scalespace output file " << outfile << std::endl;
      return;
    }
  if (n_levels>1)
   fwrite(&(n_levels),sizeof(int),1,fp);
  
  if (mapFile!=NULL)
    {
      map=fopen(mapFile->c_str(),"wb");      
      if (!map)
	{
	  std::cerr << "Error opening curvature sign output file " << *mapFile << std::endl;
	  return;
	}
    }
  
    if (validFile!=NULL)
    {
      vp=fopen(validFile->c_str(),"wb");      
      if (!vp)
	{
	  std::cerr << "Error opening validity  output file " << *validFile << std::endl;
	  return;
	}
    }


    if (heightFile!=NULL)
    {
      hp=fopen(heightFile->c_str(),"wb");      
      if (!hp)
	{
	  std::cerr << "Error opening height output file " << *heightFile << std::endl;
	  return;
	}
    }


}

void curvStacker::executeOnPNG(string pngFile, string outFile)
{
  this->pngFile=pngFile;
  p= new PngToMesh(pngFile,skipFactor,40000);
  executeOnMesh(p->execute(), outFile);
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


void curvStacker::printLevel(Eigen::MatrixXd V, vector<vector<double> > curv)
{
  double min=DBL_MAX, max=-DBL_MAX;
  //  minmaxCurvatures(min,max,curv);
  double * data = (double*)malloc(sizeof(double)*width*height);
  Grid<char> signs(width,height,'n');
  Grid<double> heights(width,height,-DBL_MAX);
  Grid<unsigned int> val(width,height,DBL_MAX);
  //  bool * signs = (bool*)malloc(sizeof(bool)*width*height);
  for (int i=0; i<width*height; i++)
    {
      if (lowestRandomValue != 0.0)
        {
	  double X=((double)rand()/(double)RAND_MAX);
	  data[i]=lowestRandomValue+X;
	  	  //	  cout << "Aggiunto X: " << X << endl;
	}
	else
	  data[i] = -DBL_MAX;
    }
  fwrite(&width, sizeof(int), 1, fp);
  fwrite(&height, sizeof(int), 1, fp);
  
  for (int k=0; k<V.rows(); k++)
    {
      double curvT=curv[k][0]*curv[k][1]*curvMultFactor;

      if (curvT<min)
	min=curvT;
      if (curvT>max)
	max=curvT;
      int x=V.row(k)[0];
      int y=V.row(k)[1];
      int i=(y-ymin)/skipFactor;
      int j=(x-xmin)/skipFactor;
      int i_gg=(i*width+j)/height;
      int j_gg=(i*width+j)%height;
      data[i*width+j]=curvT;
      if(curv[k][0]>0 && curv[k][1]>0)
	signs(j,i)='p';
      else if (curv[k][0]<0 && curv[k][1]<0)
	signs(j,i)='c';
      else
	signs(j,i)='u';
      heights(j,i)=V.row(k)[2];
      /* Inizializzo Val=0 i valori di background più bassi */
      if (V.row(k)[2]<10e-2)
	val(j,i) = 0;
    }
  cout << "Range dei livelli di curvatura: ["<<min<<","<<max<<"]"<<endl;
  fwrite(&(data[0]), sizeof(double), width*height, fp);
  
  free(data);


  /* Qui il ciclo per val */
  
  bool keepGoing=false;
  int v=0;
  int m,n;
  do 
    {
      /* Ciclo sui val */
      for (int k=0; k<V.rows(); k++)
	{
	  int x=V.row(k)[0];
	  int y=V.row(k)[1];
	  int i=(y-ymin)/skipFactor;
	  int j=(x-xmin)/skipFactor;
	  if (val(j,i)==v)
	    {
	      /* Qui guardo i vicini (8-vicini?)*/
	      for (m=(int)std::max(i-1,0);m++;m<std::min(i+1,width))
		for (n=std::max(j-1,0);n++;m<std::min(n+1,height))
		  {
		    if (val(m,n)>v)
		      {
			val(m,n)=v+1;
			keepGoing=true;
		      }
		  }
	    }
	}
      /* Se val (j,i) == i allora ciclo sui vicini, quelli che hanno val>i metto a i+1 e keepGoing=true*/
      /* QUesto vuol dire che se a fine ciclo non ho keep=true ho marcato tutto. */
      
      v++;
    }
  while (keepGoing);

  /* Now I print the curvature signs */

  if (mapFile!=NULL)
    {
      fwrite(&width, sizeof(int), 1, map);
      fwrite(&height, sizeof(int), 1, map);
      fwrite(&(signs.data[0]), sizeof(char), width*height, map);
    } 
  if (validFile!=NULL)
    {
      fwrite(&width, sizeof(int), 1, vp);
      fwrite(&height, sizeof(int), 1, vp);
      fwrite(&(val.data[0]), sizeof(unsigned int), width*height, vp);
    }
  if (heightFile!=NULL)
    {
      fwrite(&width, sizeof(int), 1, hp);
      fwrite(&height, sizeof(int), 1, hp);
      fwrite(&(heights.data[0]), sizeof(double), width*height, hp);
    } 
//  free(signs);
 }


void curvStacker::printLevelGrid(Eigen::MatrixXd V, vector<vector<double> > curv)
{
    double min=DBL_MAX;
    double max=-DBL_MAX;
    //  minmaxCurvatures(min,max,curv);
    double * data = (double*) malloc (sizeof(double) * grid_width * grid_height);
    Grid<char> signs(grid_width,grid_height,'n');
    Grid<double> heights(grid_width,grid_height,-DBL_MAX);
    Grid<unsigned int> val(width,height,UINT_MAX);
    for (int i = 0; i < grid_width * grid_height; i++)
	data[i]=-DBL_MAX;

    fwrite(&grid_width, sizeof(int), 1, fp);
    fwrite(&grid_height, sizeof(int), 1, fp);

    // for (int i = 0; i < grid_width; i++)
    // 	for (int j = 0; j < grid_height; j++)
    // 	{
    // 	    int k = (i * grid_height) + j; //(j * grid_width) + i;
    // 	    data[k] = curv[k][0] * curv[k][1] * curvMultFactor;
    // 	    if (data[k] < min)
    // 		min = data[k];
    // 	    if (data[k] > max)
    // 		max = data[k];

    // 	}

    for (int kk=0; kk<V.rows(); kk++)
    {
      int x = V.row(kk)[0];
      int y=  V.row(kk)[1];
      int i = x / grid_cell_size;
      int j = y / grid_cell_size;
      int k = (j * grid_width) + i;

      if(curv[k][0]>0 && curv[k][1]>0)
	signs(i,j)='p';
      else if (curv[k][0]<0 && curv[k][1]<0)
	signs(i,j)='c';
      else
	signs(i,j)='u';
      heights(i,j)=V.row(kk)[2];
      
      data[k] = curv[kk][0] * curv[kk][1] * curvMultFactor;

      if (data[k] < min)
	  min = data[k];
      if (data[k] > max)
	  max = data[k];
      if (V.row(k)[2]<1)
	val(j,i) = 0;
    }
    
 /* Qui il ciclo per val */
  
  bool keepGoing=false;
  int v=0;
  int m,n;
  // do 
  //   {
  //     keepGoing=false;
  //     /* Ciclo sui val */
  //     for (int k=0; k<V.rows(); k++)
  // 	{
  // 	  int x=V.row(k)[0];
  // 	  int y=V.row(k)[1];
  // 	  int i=(y-ymin)/skipFactor;
  // 	  int j=(x-xmin)/skipFactor;
  // 	  cout << "i,j" << " " << i << " " << j << endl;
  // 	  if (val(j,i)==v)
  // 	    {
  // 	      /* Qui guardo i vicini (8-vicini?)*/
  // 	      for (m=(int)std::max(i-1,0);m++;m<std::min(i+1,width))
  // 		for (n=std::max(j-1,0);n++;m<std::min(j+1,height))
  // 		  {	 
		    
  // 		    if (val(n,m)>v)
  // 		      {
  // 			val(n,m)=v+1;
  // 			keepGoing=true;
  // 		      }
  // 		  }
  // 	    }
  // 	}
  //     /* Se val (j,i) == i allora ciclo sui vicini, quelli che hanno val>i metto a i+1 e keepGoing=true*/
  //     /* QUesto vuol dire che se a fine ciclo non ho keep=true ho marcato tutto. */
      
  //     v++;
  //   }
  // while (keepGoing);

 

    cerr << "printLevelGrid(): Range dei livelli di curvatura. "
	"[min:"<<min<<", max:"<<max<<"]. Mult factor: "<<curvMultFactor<<
	". -- grid["<<grid_width<<"],["<<grid_height<<"]. cellsize="<<grid_cell_size
	 <<endl;

    fwrite(&(data[0]), sizeof(double), grid_width * grid_height, fp);
    //free(data);


  if (mapFile!=NULL)
    {
      fwrite(&grid_width, sizeof(int), 1, map);
      fwrite(&grid_height, sizeof(int), 1, map);
      fwrite(&(signs.data[0]), sizeof(char), grid_width*grid_height, map);
    } 
  if (validFile!=NULL)
    {
      fwrite(&width, sizeof(int), 1, vp);
      fwrite(&height, sizeof(int), 1, vp);
      fwrite(&(val.data[0]), sizeof(unsigned int), width*height, vp);
    }
  if (heightFile!=NULL)
    {
      fwrite(&grid_width, sizeof(int), 1, hp);
      fwrite(&grid_height, sizeof(int), 1, hp);
      fwrite(&(heights.data[0]), sizeof(double), grid_width*grid_height, hp);
    } 
//  free(signs);


 }



void curvStacker::setGrid (int w, int h, int c)
{
    grid = true;
    grid_width = w;
    grid_height = h;
    grid_cell_size = c;

    cout << "setGrid(): grid["<<grid_width<<","<<grid_height<<"]"<<endl;    
}

void curvStacker::executeOnMultipleMeshes(vector<string> meshNames, string outFile, bool separateDems)
{
  
  
  if (!separateDems)
    {
      printHeader(outFile);
      n_levels=meshNames.size();
    }
  else n_levels=1;
  for (int i=0; i<meshNames.size(); i++)
    {
      std::stringstream ss;
      
      unsigned first = meshNames[i].find('-');
      unsigned last = meshNames[i].find('.');
      string strNew = meshNames[i].substr (first+1,last-first-1);
      int time= atoi(strNew.c_str());
      int radius=ceil((double)(ScaleSpace::time2window(time))/(double)2);
      
      ss << time;
      string trueOut = outFile;
      trueOut.append(ss.str());
      CurvatureCalculator c;
      Eigen::MatrixXd V;
      Eigen::MatrixXi F;
      cerr << "Reading mesh " << meshNames[i] << endl;
      igl::read(meshNames[i],V,F);
      if (i==0)
	initializeScaleSpace(V);
      if (separateDems)
	printHeader(trueOut);
      cerr << "Mesh read with " << V.rows() << " vertices "  << endl;
      c.zeroDetCheck=false;
      c.init(V,F);

           if (grid && (V.rows() != grid_width * grid_height))
	{
	  fprintf (stderr, "Number of vertices [%zu] and given grid [%d,%d]->[%d]"
		   " do not coincide. Abort.\n",
		   V.rows(), grid_width, grid_height, grid_width * grid_height);
	  exit (-5);
	}


      cerr << "Starting computing radius " << radius << endl;
      c.sphereRadius=radius;
      if (radius<32)
	c.computeCurvature(false,fast_computation);
      else
	c.computeCurvature(do_topoindex,fast_computation);
      cerr << "Computed radius " << radius << endl;
      grid? printLevelGrid (V,c.curv) : printLevel(V,c.curv);
      if (separateDems)
	fclose(fp);
      mapFile=NULL;
      heightFile=NULL;
      validFile=NULL;
    }
  if (!separateDems)
    fclose(fp);
}



void curvStacker::computeTextualCurvature(string meshFile,string outFile)
{

  cerr << "Starting execute on mesh " << endl;
  CurvatureCalculator c;
  Eigen::MatrixXd V;
  Eigen::MatrixXi F;
  cerr << "Reading mesh " << meshFile << endl;
  igl::read(meshFile,V,F);
  c.zeroDetCheck=false;
  cerr << "Mesh read with " << V.rows() << " vertices "  << endl << endl;
  c.init(V,F);
  for (double r=base_radius; r<=max_radius; isExpStep?r*=step:r+=step)
    {
      std::ostringstream strs;
      strs << std::setw(3) << std::setfill('0') << r;
      std::string str = strs.str();
      cerr << "Starting computing radius " << r << endl;
      c.sphereRadius=r;
      if (r<20)
	c.computeCurvature(false,fast_computation);
      else
	c.computeCurvature(do_topoindex,fast_computation);
      cerr << "Computed radius " << r << endl;
      c.printCurvature(outFile+"_"+str+".curv");
    }
 
}



void curvStacker::executeOnMesh(string meshFile,string outFile)
{



  cerr << "Starting execute on mesh " << endl;
  CurvatureCalculator c;
  Eigen::MatrixXd V;
  Eigen::MatrixXi F;
  cerr << "Reading mesh " << meshFile << endl;
  igl::read(meshFile,V,F);

  

  initializeScaleSpace(V);

  if (grid && (V.rows() != grid_width * grid_height))
    {
	fprintf (stderr, "Number of vertices [%zu] and given grid [%d,%d]->[%d]"
		 " do not coincide. Abort.\n",
		 V.rows(), grid_width, grid_height, grid_width * grid_height);
	exit (-5);
    }
  
  printHeader(outFile);
  c.zeroDetCheck=false;
  cerr << "Mesh read with " << V.rows() << " vertices "  << endl << endl;
  c.init(V,F);
  cout << "expStep: " << isExpStep << " step: " << step << endl;
  for (double r=base_radius; r<=max_radius; isExpStep?r*=step:r+=step)
    {
      cerr << "Starting computing radius " << r << endl;
      c.sphereRadius=r;
      if (r<20)
	c.computeCurvature(false,fast_computation);
      else
	c.computeCurvature(do_topoindex,fast_computation);
      cerr << "Computed radius " << r << endl;
      grid? printLevelGrid (V,c.curv) : printLevel(V,c.curv);
    }

 
  fclose(fp);
}

