#include "vertex.hh"

Vertex::Vertex(double x, double y, double z)
{
    this->x=x;
    this->y=y;
    this->z=z;
}

std::string Vertex::toString()
{
    std::string ret_val="";
    ret_val+=x;
    ret_val+=" ";
    ret_val+=y;
    ret_val+=" ";
    ret_val+=z;
    return ret_val;
}
