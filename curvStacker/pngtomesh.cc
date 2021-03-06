#include "pngtomesh.hh"
#include <fstream>
#include <assert.h>
#include <set>
#include <cfloat>
#include <Eigen/Core>
#include <igl/read.h>
#define IGL_HEADER_ONLY

#include <igl/writeOFF.h>

PngToMesh::PngToMesh(std::string filename, int skipFactor, int thresh)
{
    this->filename=filename;
    this->skipFactor=skipFactor;
    this->thresh=thresh;

   
    if (!readFile(filename.c_str()))
    {
        std::cerr << "Error reading file " << filename << std::endl;
        return;
    }
 
}

bool PngToMesh::readFile(const char * file_name)
{
    img.assign(file_name);
    width = img.width ();
    height = img.height ();

    //  int offsetX=150, offsetY=120;
   // img=img.get_crop(offsetX,offsetY,width-offsetX,height-offsetY);

    //img.save("cropped.png");
    //width = img.width ();
    //height = img.height ();

    return true;
}



std::string PngToMesh::printPointCloud()
{
    std::set<int> zeroVert;
    std::string outpath;

    size_t pos=filename.find_last_of('/');
    size_t pos2=filename.find_last_of('.');
    outpath.assign(filename.begin()+pos+1,filename.begin()+pos2);
    outpath="./"+outpath+".off";

    vertices=meshVertices.size();
    faces=meshFaces.size();
    int totVert=0, totFaces=0;
    std::vector<Vertex> newVertices;
    std::vector<Face> newFaces;
    for (int i=0; i<vertices; i++)
    {
        if (meshVertices[i].z>thresh)
	  {
            zeroVert.insert(i);
	  }
        else
	  {
            totVert++;
	    newVertices.push_back(meshVertices[i]);
	  }
    }
 
    int* realIndex=(int*)calloc(vertices,sizeof(int));
    int k=0;
    for (int i=0; i<vertices; i++)
    {
        if (zeroVert.count(i)!=0)
            continue;     
        realIndex[i]=k++;
    }
    for (int i=0; i<faces; i++)
    {
        if (zeroVert.count(meshFaces[i].x)!=0 || zeroVert.count(meshFaces[i].y)!=0 || zeroVert.count(meshFaces[i].z) !=0 )
            continue;
	newFaces.push_back(Face(realIndex[meshFaces[i].x],realIndex[meshFaces[i].y],realIndex[meshFaces[i].z]));
    }

    removeIsolatedVertices(newVertices, newFaces,outpath);
    return outpath;
}



bool PngToMesh::convert()
{
  unsigned int vWidth=ceil((double)width/(double)skipFactor);
  unsigned int vHeight=ceil((double)height/(double)skipFactor);
  unsigned int totVertices=vWidth*vHeight;

  for (unsigned int x = 0; x < width-1; x+=skipFactor)
    for (unsigned int y= 0; y < height-1; y+=skipFactor)
      {
	/* individuo il pixel come i,i+1, i+Vheight, i1+vHeigt */
	meshVertices.push_back(Vertex(x,y,getPixel(x,y)));
	
      }


  for (unsigned int k=0; k<meshVertices.size()-vHeight-1; k++)
    {

      /* Non devo fare il triangolo quando i%(vWidth-1)==0*/
      //if ((k+1)%(height)!=0)
      if (abs(meshVertices[k].y-meshVertices[k+1].y)>skipFactor || abs(meshVertices[k].y-meshVertices[k+vHeight].y)>skipFactor || abs(meshVertices[k+1].y-meshVertices[k+vHeight].y)>skipFactor || abs(meshVertices[k+1].y-meshVertices[k+vHeight+1].y)>skipFactor)
	continue;
	{
	  /*la faccia è: i,i+1+vWidth,i+1 */
	  meshFaces.push_back(Face(k,k+vHeight,k+1));
	  meshFaces.push_back(Face(k+1,k+vHeight,k+1+vHeight));
        }
    }
  return true;
}


inline int getEuclideanHeight(int i, int height){
    return height-i;
}

inline double PngToMesh::getPixel (unsigned int x, unsigned int y)
{
    y = height - y - 1;
    return img(x,y);
}


std::string PngToMesh::execute()
{
    if (!convert())
    {
        std::cerr << "Error while converting the file!" << std::endl;
        return NULL;
    }

    return printPointCloud();
}


void PngToMesh::removeIsolatedVertices(std::vector<Vertex> vertices, std::vector<Face> faces, std::string outpath)
{
  std::ofstream of;
  of.open(outpath.c_str());

  if (!of)
    {
      fprintf(stderr, "Error: could not open output file %s\n", outpath.c_str());
    }  

  of << "OFF" << std::endl;
  of << vertices.size() << " " << faces.size() << " 0" << std::endl;
  for (int i=0; i<vertices.size(); i++)
    {
      of << (double)vertices[i].x << " " << (double)vertices[i].y << " " << (double)((double)vertices[i].z*(double)1) << std::endl;
      
    }
  for (int i=0; i<faces.size(); i++)
    of << "3 " << faces[i].x << " " <<faces[i].y << " " <<faces[i].z  << std::endl;
  of.close();
  // GG ugly hack to make the call to meshlabserver work for both me and nikolas
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
  
  std::string cmd = meshlabserver_path+" -i "+outpath+" -o "+outpath+" -s "+removescript;
  system(cmd.c_str());

     double z_min=DBL_MAX,z_max=-DBL_MAX;
      double y_max=-DBL_MAX;

      Eigen::MatrixXd Vt;
      Eigen::MatrixXi Ft;
      igl::read(outpath.c_str(),Vt,Ft);

    /* Qui devo scalare la mesh */
  /* TODO: dipendente da parametro */
  /* Devo studiarmi l'opzione grid*/
  /**********************************************************************/
      for (int i=0; i<Vt.rows(); i++)
    {
      if (Vt.row(i)[2]<z_min)
  	z_min=Vt.row(i)[2];
      if (Vt.row(i)[2]>z_max)
  	z_max=Vt.row(i)[2];
      if (Vt.row(i)[1]>y_max)
  	y_max=Vt.row(i)[1];
    }
      std::cout << "****************\nz_min:" << z_min << "\t <_max " << z_max <<  "\t y_max_ " << y_max << std::endl;
      for (int i=0; i<Vt.rows(); i++)
    {
      Vt.row(i)[2]-=z_min;
      Vt.row(i)[2]*=((y_max/(z_max-z_min))/skipFactor)*1.3;
    }
  /**********************************************************************/

     igl::writeOFF(outpath.c_str(),Vt,Ft);

  //   of.open(outpath.c_str());

  // if (!of)
  //   {
  //     fprintf(stderr, "Error: could not open output file %s\n", outpath.c_str());
  //   }  
  
  // of << "OFF" << std::endl;
  // of << Vt.rows() << " " << Ft.rows() << " 0" << std::endl;
  // for (int i=0; i<Vt.rows(); i++)
  //   {
  //     of << (double)Vt.row(i)[0] << " " << (double)Vt.row(i)[1] << " " << (double)Vt.row(i)[2] << std::endl;
      
  //   }
  
  // for (int i=0; i<Ft.rows(); i++)
  //   of << "3 " << Ft.row(i)[0] << " " <<Ft.row(i)[2] << " " <<Ft.row(i)[2]  << std::endl;
  
  // of.close();
}
