/* The following code is taken and adapted from BVH:  https://code.google.com/p/bvh/ */

#ifndef MINSPHERE_H
#define MINSPHERE_H

#include "mtxlib.hh"
#include <math.h>
#include <vector>

#define radiusEpsilon 1e-4f

typedef vector3 Point3;

class miniBall
{
public:
  miniBall();
  miniBall(const Point3 &O);
  miniBall(const Point3 &O, const Point3 &A);  
  miniBall(const Point3 &O, const Point3 &A, const Point3 &B);  
  miniBall(const Point3 &O, const Point3 &A, const Point3 &B, const Point3 &C); 
  float d2(const Point3 &P) const;


  Point3 center;
  float radius;
};

miniBall MinSphere(Point3 P[], unsigned int p);
miniBall RecurseMin(Point3 *P[], unsigned int p, unsigned int b=0);
float det(float m11, float m12, float m13, 
	  float m21, float m22, float m23, 
	  float m31, float m32, float m33);


#endif
