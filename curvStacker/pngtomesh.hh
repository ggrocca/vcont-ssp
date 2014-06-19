#ifndef PNGTOMESH_H
#define PNGTOMESH_H
#include <string>
#include <iostream>
#include <vector>

#include "vertex.hh"
#include "face.hh"
#include "CImg.h"


class Point3D
{
public:
    double x;
    double y;
    double z;

    Point3D(double x, double y, double z)
    {
        this->x=x;
        this->y=y;
        this->z=z;
    }
};

class PngToMesh
{
private:
    int vertices,faces;
    std::string filename;
    std::vector<Vertex> meshVertices;
    std::vector<Face> meshFaces;
    cimg_library::CImg<unsigned short> img;
    unsigned int width, height;
    int thresh;
    double getPixel(unsigned int, unsigned int);
    void removeIsolatedVertices(std::vector<Vertex> newVertices, std::vector<Face> newFaces, std::string oupath);
public:
    int skipFactor;
    PngToMesh(std::string, int, int thresh);
    bool readFile(const char*);
    bool convert();
    std::string printPointCloud();
    void printFiducialPoints3D();
    std::string execute();
};

#endif // PNGTOMESH_H
