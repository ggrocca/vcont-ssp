#ifndef FACE_H
#define FACE_H
#include <string>

class Face
{
public:
    unsigned int x,y,z;

    Face(unsigned int x, unsigned int y, unsigned int z);

    std::string toString();
};

#endif // FACE_H

