#ifndef _MESH_HH
#define _MESH_HH

#include "genericlimits.hh"
#include "debug.h"

#define IGL_HEADER_ONLY
#include <igl/read.h>
#include <igl/adjacency_list.h>
#include <igl/edges.h>

#include <string>
#include <stdio.h>
#include <vector>
#include <string>

template <class T> class Triple
{
public:

    T x, y, z;

    Triple () : x (0), y (0), z (0) {}
    Triple (const T x, const T y, const T z) : x (x), y (y), z (z) {}
    Triple (const Triple<T>& p) : x (p.x), y (p.y), z (p.z) {}

    Triple& operator=(const Triple& rhs)
    {
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;

	return *this;
    }
    
    bool is_border (const T w, const T h, const T d) const
    {
	return x == 0 || x == w-1 || y == 0 || y == h-1 || z == 0 || z == d-1;
    }

    bool is_inside (const T w, const T h, const T d) const
    {
	return x >= 0 && x < w && y >= 0 && y < h && z >= 0 && z < d;
    }
    
    bool is_inside (const T w, const T h, const T d, double cut) const
    {
	double wc = w * (cut / 2.0);
	double hc = h * (cut / 2.0);
	double dc = d * (cut / 2.0);
	return x >= 0+wc && x < w-wc && y >= 0+hc && y < h-hc && z >= 0+dc && z < d-dc;
    }

    bool is_inside (const T w, const T h, const T d, int window) const
    {
	return x >= window && x < w-window && y >= window && y < h-window;
    }

    // // gg rewrite this inside a volume
    // bool is_inside (const Triple<T> llc, const Triple<T> hrc) const
    // {
    // 	return x >= llc.x && x < hrc.x && y >= llc.y && y < hrc.y;
    // }
    

    bool is_outside (const T w, const T h, const T d) const
    {
	return !is_inside (w, h, d);
    }
    
    bool operator<(const Triple& rhs) const
    {
	return (x < rhs.x && y < rhs.y && z < rhs.z);
    }

    bool operator>(const Triple& rhs) const
    {
	return (x > rhs.x && y > rhs.y && z > rhs.z);
    }

    bool operator<=(const Triple& rhs) const
    {
	return (x <= rhs.x && y <= rhs.y && z <= rhs.z);
    }

    bool operator>=(const Triple& rhs) const
    {
	return (x >= rhs.x && y >= rhs.y && z >= rhs.z);
    }

    bool operator==(const Triple& rhs) const
    {
	return (x == rhs.x && y == rhs.y);
    }

    bool operator!=(const Triple& rhs) const
    {
	return (! (*this == rhs));
    }

    Triple operator+(const Triple& rhs)
    {
	return Triple (x + rhs.x, y + rhs.y, z + rhs.z);
    }

    Triple operator-(const Triple& rhs)
    {
	return Triple (x - rhs.x, y - rhs.y, z - rhs.z);
    }

    static Triple lowest()
    {
	T bogus;

	if (std::numeric_limits<T>::is_integer)
	    bogus = std::numeric_limits<T>::min();
	else 
	    bogus = GenericType<T>::lowest();

	return Triple (bogus, bogus, bogus);
    }

    static Triple highest()
    {
	T bogus;
	bogus = std::numeric_limits<T>::max();

	return Triple (bogus, bogus, bogus);
    }
};

typedef Triple<double> Point3D;

inline double point_distance (Point3D a, Point3D b)
{
    return sqrt (((a.x - b.x) * (a.x - b.x)) +
		 ((a.y - b.y) * (a.y - b.y)) +
		 ((a.z - b.z) * (a.z - b.z)));
}


// class Mesh;

// class Vertex
// {
// public:

//     Mesh& mesh;
//     int idx;
    
//     Vertex (Mesh& mesh, int idx) mesh (mesh), idx (idx) {}

//     Point3D point3D ()
//     {
// 	return Point3D (V(idx, 0), V(idx, 1), V(idx, 2));
//     }

//     int neighborhood (std::vector<int> vn)
//     {
// 	vn.clear();
	
// 	for (unsigned int i = 0; i < mesh.VV[idx].size(); i++)
// 	    vn.push_back (mesh.VV[k][i]);

// 	return vn.size();
//     }
// };


class Mesh
{
public:

    Eigen::MatrixXd V;
    Eigen::MatrixXi F;
    Eigen::MatrixXi E;
    std::vector<std::vector<int> > VV;

    Mesh (std::string meshFile)
    {
	igl::read (meshFile,V,F);
	igl::adjacency_list (F, VV);
	igl::edges (F, E);

	printf ("created mesh. V(%zu) F(%zu) E (%zu) VV (%zu)\n",
		V.rows(), F.rows(), E.rows(), VV.size());
    }

    int size ()
    {
	return VV.size();
    }

    void print_info (int label)
    {
	printf ("[[%3d]]V(%zu) F(%zu) E (%zu) VV (%zu)\n",
		label, V.rows(), F.rows(), E.rows(), VV.size());
    }
    
    // Vertex vertex (int idx)
    // {
    // 	return Vertex (*this, idx);
    // }

    Point3D point3D (int idx)
    {
	return Point3D (V(idx, 0), V(idx, 1), V(idx, 2));
    }

    int neighborhood (int idx, std::vector<int>& vn)
    {
	vn.clear();
	for (unsigned int i = 0; i < VV[idx].size(); i++)
	    vn.push_back (VV[idx][i]);
	
	assert (vn.size() > (unsigned)0);
	
	return vn.size ();
    }

    int neighborhood_size (int idx)
    {
	return VV[idx].size();
    }
    
    int neighbor_index (int idx, int idx_neigh)
    {
	for (unsigned int i = 0; i < VV[idx].size(); i++)
	    if (VV[idx][i] == idx_neigh)
		return i;
	eprint (-1, "idx (%d) has no neighbour idx_neigh (%d).\n", idx, idx_neigh);
	return -1;
    }

    void edges (std::vector< std::vector<int> >& edges)
    {
	edges.clear ();
	for (int i = 0; i < E.rows(); i++)
	{
	    std::vector<int> edge;
	    edge.push_back (E(i,0));
	    edge.push_back (E(i,1));
	    edges.push_back (edge);
	}
    }
    
};


// GG what happens if the mesh has a boundary?
// should boundary vertices be connected to an arbitrary pit?
// should we support watertight meshes only?
template <class T> class Field
{
public:

    Mesh& mesh;
    std::vector<T> data;

    Field (Mesh& m)
	: mesh (m), data (m.size()) {}

    Field (Mesh& m, T t)
	: mesh (m), data (m.size())
    {
	// if (size < 0)
	//     eprintx (-1, "Negative dimension, %d.\n", size);

        // data.resize(size);

        for(unsigned i = 0; i < size(); ++i)
	    data[i] = t;
	//operator () (i,j) = t;
    }

    T& operator() (int idx)
    {
	if (idx >= (int) data.size())
	    eprintx (-1, "Impossible idx, %d (data length %zu)\n", idx, data.size());

	return data[idx];
    }

    T& operator[] (int idx)
    {
	if (idx >= (int) data.size())
	    eprintx (-1, "Impossible idx, %d (data length %zu)\n", idx, data.size());

	return data[idx];
    }

    unsigned size ()
    {
        return data.size();
    }

    void resize (int sz)
    {
	if (sz < 0)
	    eprintx (-1, "Negative dimension %d.\n", sz);

        data.resize(sz);
    }

};

#endif // _MESH_HH

