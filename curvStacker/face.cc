#include "face.hh"

Face::Face(unsigned int x, unsigned int y, unsigned int z)
{
    this->x=x;
    this->y=y;
    this->z=z;
}

std::string Face::toString()
{
    std::string ret_val="";
    ret_val+=x;
    ret_val+=" ";
    ret_val+=y;
    ret_val+=" ";
    ret_val+=z;
    return ret_val;
}
