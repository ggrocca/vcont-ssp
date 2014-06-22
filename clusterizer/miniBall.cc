/* The following code is taken and adapted from BVH:  https://code.google.com/p/bvh/ */

#include "miniBall.hh"



miniBall MinSphere( Point P[], unsigned int p )
{
  Point **L = new Point*[p];

  for(unsigned int i = 0; i < p; i++)
    L[i] = &P[i];

  miniBall MB = RecurseMin(L, p);

  delete[] L;

  return MB;
}

miniBall RecurseMin( Point *P[], unsigned int p, unsigned int b )
{

  miniBall MB;

  switch(b)
    {
    case 0:
      MB = miniBall();
      break;
    case 1:
      MB = miniBall(*P[-1]);
      break;
    case 2:
      MB = miniBall(*P[-1], *P[-2]);
      break;
    case 3:
      MB = miniBall(*P[-1], *P[-2], *P[-3]);
      break;
    case 4:
      MB = miniBall(*P[-1], *P[-2], *P[-3], *P[-4]);
      return MB;
    }

  for(unsigned int i = 0; i < p; i++)
    if(MB.d2(*P[i]) > 0)   // Signed square distance to sphere
      {
	for(unsigned int j = i; j > 0; j--)
	  {
	    Point *T = P[j];
	    P[j] = P[j - 1];
	    P[j - 1] = T;
	  }

	MB = RecurseMin(P + 1, i, b + 1);
      }

  return MB;
}

float det( float m11, float m12, float m13, float m21, float m22, float m23, float m31, float m32, float m33 )
{

  return m11 * (m22 * m33 - m32 * m23) -
    m21 * (m12 * m33 - m32 * m13) +
    m31 * (m12 * m23 - m22 * m13);
}

miniBall::miniBall( const Point &O )
{
  radius = 0+radiusEpsilon;
  center = O;
}

miniBall::miniBall()
{
  radius = -1;
}

miniBall::miniBall( const Point &O, const Point &A )
{
  vector3 a = A - O;

  vector3 o = 0.5f * a;

  radius = o.length()+radiusEpsilon;
  center = O + o;
}

miniBall::miniBall( const Point &O, const Point &A, const Point &B )
{

  vector3 a = A - O;
  vector3 b = B - O;

  float Denominator = 2.0f * DotProduct(CrossProduct(a,b),CrossProduct(a,b));

  vector3 o= (DotProduct(b,b)*CrossProduct(CrossProduct(a,b),a)+DotProduct(a,a)*CrossProduct(b,CrossProduct(a,b)))/Denominator;

  radius = o.length()+radiusEpsilon;
  center = O + o;
}

miniBall::miniBall( const Point &O, const Point &A, const Point &B, const Point &C )
{

  vector3 a = A - O;
  vector3 b = B - O;
  vector3 c = C - O;

  float Denominator = 2.0f *det(a.x, a.y, a.z,
				b.x, b.y, b.z,
				c.x, c.y, c.z);

  vector3 o=(DotProduct(c,c)*CrossProduct(a,b)+DotProduct(b,b)*CrossProduct(c,a)+DotProduct(a,a)*CrossProduct(b,c))/Denominator;

  radius = o.length()+radiusEpsilon;
  center = O + o;
}

float miniBall::d2( const Point &P ) const
{
  vector3 dif=P-center;
  return DotProduct(dif,dif)-radius*radius;       
}
