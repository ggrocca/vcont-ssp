/* The following code is taken and adapted from BVH:  https://code.google.com/p/bvh/ */

#ifndef MINSPHERE_H
#define MINSPHERE_H

#include "mtxlib.hh"
#include <math.h>

#define radiusEpsilon 1e-4f

typedef vector3 Point;

class miniBall
{
public:
  miniBall();
  miniBall(const Point &O);
  miniBall(const Point &O, const Point &A);  
  miniBall(const Point &O, const Point &A, const Point &B);  
  miniBall(const Point &O, const Point &A, const Point &B, const Point &C); 
  float d2(const Point &P) const;


  Point center;
  float radius;
};

miniBall MinSphere(Point P[], unsigned int p);
miniBall RecurseMin(Point *P[], unsigned int p, unsigned int b=0);
float det(float m11, float m12, float m13, 
	  float m21, float m22, float m23, 
	  float m31, float m32, float m33);


#endif
