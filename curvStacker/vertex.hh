#ifndef VERTEX_H
#define VERTEX_H
#include <string>

class Vertex
{
public:
    double x,y,z;

    Vertex(double x, double y, double z);

    std::string toString();
};

#endif // VERTEX_H
