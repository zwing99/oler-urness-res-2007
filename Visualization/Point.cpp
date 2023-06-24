/* Project: FieldVis
 * File:    Point.cpp
 * Author:  Blayne Field
 * Purpose: Implementation file for the Point class
 *
 * This class just holds a 3D point
 */
/*
 * Implementation/Modification by: Zac Oler
 * Purpose: To use for calculating Streamline points
 */

#include "Point.h"

using namespace std;

point::point():
  x(0), y(0), z(0)
{}

point::point(float px, float py, float pz):
  x(px), y(py), z(pz)
{}


vec point::PointToVec()
{
  return vec(x, y, z);
}


float point::DistanceBetween(point p0, point p1)
{
  float dx = p0.x - p1.x;
  float dy = p0.y - p1.y;
  float dz = p0.z - p1.z;
  return sqrt(dx*dx + dy*dy + dz*dz);
}

// moves the point 'dist' distance in the given direction
void point::AddVec(vec vectemp, float dist = 1)
{
  x += (vectemp.getU() * dist);
  y += (vectemp.getV() * dist);
  z += (vectemp.getW() * dist);
}

point point::operator+(vec vecToAdd)
{
  point temp(x + vecToAdd.getU(), 
	         y + vecToAdd.getV(),
	         z + vecToAdd.getW());
  return temp;
}

point point::operator*(float factor)
{
  point temp(x * factor,
	     y * factor,
	     z * factor);
  return temp;
}

bool point::operator==(const point & compareTo) const
{
  return ((x == compareTo.x) && (y == compareTo.y) && (z == compareTo.z));
}

vec point::VectorFromTo(point p0, point p1)
{
  vec vectemp(p1.x - p0.x, p1.y - p0.y, p1.z - p0.z);
  return vectemp;
}

const point & point::operator=(const point & toCopy)
{
  if (this != &toCopy) {
    x = toCopy.x;
    y = toCopy.y;
    z = toCopy.z;
  }

  return *this;
}

ostream & operator<<(ostream & out, point & p)
{
  out << p.GetX() << " " << p.GetY() << " " << p.GetZ() << endl;
  return out;
}
