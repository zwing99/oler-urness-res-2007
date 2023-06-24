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

Point::Point():
  x(0), y(0), z(0)
{}

Point::Point(float px, float py, float pz):
  x(px), y(py), z(pz)
{}


vec Point::PointToVec()
{
  return vec(x, y, z);
}


float Point::DistanceBetween(Point p0, Point p1)
{
  float dx = p0.x - p1.x;
  float dy = p0.y - p1.y;
  float dz = p0.z - p1.z;
  return sqrt(dx*dx + dy*dy + dz*dz);
}

// moves the point 'dist' distance in the given direction
void Point::AddVec(vec vectemp, float dist = 1)
{
  x += (vectemp.getU() * dist);
  y += (vectemp.getV() * dist);
  z += (vectemp.getW() * dist);
}

Point Point::operator+(vec vecToAdd)
{
  Point temp(x + vecToAdd.getU(), 
	         y + vecToAdd.getV(),
	         z + vecToAdd.getW());
  return temp;
}

Point Point::operator*(float factor)
{
  Point temp(x * factor,
	     y * factor,
	     z * factor);
  return temp;
}

bool Point::operator==(const Point & compareTo) const
{
  return ((x == compareTo.x) && (y == compareTo.y) && (z == compareTo.z));
}

vec Point::VectorFromTo(Point p0, Point p1)
{
  vec vectemp(p1.x - p0.x, p1.y - p0.y, p1.z - p0.z);
  return vectemp;
}

const Point & Point::operator=(const Point & toCopy)
{
  if (this != &toCopy) {
    x = toCopy.x;
    y = toCopy.y;
    z = toCopy.z;
  }

  return *this;
}

ostream & operator<<(ostream & out, Point & p)
{
  out << p.GetX() << " " << p.GetY() << " " << p.GetZ() << endl;
  return out;
}
