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

#include <string>
#include "vec.h"

using namespace std;

#ifndef POINT_H
#define POINT_H



class point
{
 public:
  point();
  point(float px, float py, float pz);
  
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

  static vec VectorFromTo(point p0, point p1);
  static float DistanceBetween(point p0, point p1);

  // overloaded operators
  point operator+(vec vecToAdd);
  point operator*(float factor);
  bool operator==(const point & compareTo) const;
  const point & operator=(const point & toCopy);

 private:
  float x;
  float y;
  float z;
};


// accessors
inline float point::GetX() const {return x;}
inline float point::GetY() const {return y;}
inline float point::GetZ() const {return z;}

// mutators
inline void point::SetX(float px) {x = px;}
inline void point::SetY(float py) {y = py;}
inline void point::SetZ(float pz) {z = pz;}

inline void point::MoveX(float dx) {x += dx;}
inline void point::MoveY(float dy) {y += dy;}
inline void point::MoveZ(float dz) {z += dz;}



ostream & operator<<(ostream & out, point & p);

#endif

