// easy IGL test
// GG: write as mesh.cc and compile as:
// g++ -I../../libs/igl_LOCAL/libigl/include/ -I ../../libs/Eigen/ mesh.cc

#define IGL_HEADER_ONLY
#include <igl/read.h>
#include <igl/adjacency_list.h>

#include <string>
#include <stdio.h>
#include <vector>

int main (int argc, char *argv[])
{
    Eigen::MatrixXd V;
    Eigen::MatrixXi F;

    std::string meshFile (argv[1]);
    igl::read(meshFile,V,F);

    std::vector<std::vector<int> > vertex_to_vertices;
    igl::adjacency_list(F, vertex_to_vertices);

    for (int k = 0; k < V.rows(); k++)
    {
	printf ("(%d)(%lf,%lf,%lf) || ", k, V(k,0), V(k,1), V(k,2));
	for (unsigned int i=0; i<vertex_to_vertices[k].size(); i++)
	{
	    int n = vertex_to_vertices[k][i];
	    printf ("[%d]", n);
	}
	printf ("\n");
    }

}
