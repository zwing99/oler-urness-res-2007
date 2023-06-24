/* Project: FieldVis
 * File:    Point.h
 * Author:  Blayne Field
 * Purpose: Definition header file for the Point class
 *
 * This class just holds a 3D point
 */
/*
 * Implementation/Modification by: Zac Oler
 * Purpose: To use for calculating Streamline points
 */

#ifndef POINT_H
#define POINT_H

#include <string>
#include "vec.h"

using namespace std;

class Point
{
 public:
  Point();
  Point(float px, float py, float pz);
  
  // accessors
  float GetX() const;
  float GetY() const;
  float GetZ() const;
  
  // mutators
  void SetX(float px);
  void SetY(float py);
  void SetZ(float pz);  

  void MoveX(float dx);
  void MoveY(float dy);
  void MoveZ(float dz);

  vec PointToVec();
 
  void AddVec(vec vectemp, float dist);

  static vec VectorFromTo(Point p0, Point p1);
  static float DistanceBetween(Point p0, Point p1);

  // overloaded operators
  Point operator+(vec vecToAdd);
  Point operator*(float factor);
  bool operator==(const Point & compareTo) const;
  const Point & operator=(const Point & toCopy);

 private:
  float x;
  float y;
  float z;
};


// accessors
inline float Point::GetX() const {return x;}
inline float Point::GetY() const {return y;}
inline float Point::GetZ() const {return z;}

// mutators
inline void Point::SetX(float px) {x = px;}
inline void Point::SetY(float py) {y = py;}
inline void Point::SetZ(float pz) {z = pz;}

inline void Point::MoveX(float dx) {x += dx;}
inline void Point::MoveY(float dy) {y += dy;}
inline void Point::MoveZ(float dz) {z += dz;}



ostream & operator<<(ostream & out, Point & p);

#endif

