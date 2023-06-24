/*
 *  vec.cpp
 *  
 *
 *  Created by Zac Oler on 6/4/07.
 *  Copyright 2007 ZWING INC. All rights reserved.
 *
 */

#include "vec.h"

//Simple Constructor
vec::vec()
{
	u=0;
	v=0;
	w=0;
}

//Constructor for Specifying the Vector
vec::vec(double u0, double v0, double w0)
{
	u=u0;
	v=v0;
	w=w0;
}

//Set the Vec to Anything
void vec::setVec(double u0, double v0, double w0)
{
	u=u0;
	v=v0;
	w=w0;
}

//Dot Product with Another Vector
double vec::dot(vec* otherVec)
{
	double u1=otherVec->getU();
	double v1=otherVec->getV();
	double w1=otherVec->getW();
	
	return (u*u1+v*v1+w*w1);
}

double vec::dot(vec otherVec)
{
	double u1=otherVec.getU();
	double v1=otherVec.getV();
	double w1=otherVec.getW();
	
	return (u*u1+v*v1+w*w1);
}

//Cross Product with Another Vector
vec vec::cross(vec otherVec)
{
	double u1=otherVec.u;
	double v1=otherVec.v;
	double w1=otherVec.w;
	
	double uc = (v*w1-v1*w);
	double vc = (w*u1-w1*u);
	double wc = (u*v1-u1*v);
	
	vec temp(uc,vc,wc);
	
	return temp;
}

//Add and Subtract Functions
void vec::add(vec* otherVec)
{
	double u1=otherVec->getU();
	double v1=otherVec->getV();
	double w1=otherVec->getW();
	
	u=u+u1;
	v=v+v1;
	w=w+w1;
}
void vec::subtract(vec* otherVec)
{
	double u1=otherVec->getU();
	double v1=otherVec->getV();
	double w1=otherVec->getW();
	
	u=u-u1;
	v=v-v1;
	w=w-w1;
}

//Scale a Vector
void vec::scale(double alpha)
{
	u=alpha*u;
	v=alpha*v;
	w=alpha*w;
}

//Magnitude of the Vector
double vec::magnitude()
{
	return sqrt((u*u+v*v+w*w));
	
}

// overloaded * operator
vec vec::operator*(float mult)
{
  vec temp(u * mult, v * mult, w * mult);
  return temp;
}

// overloaded + operator
vec vec::operator+(const vec toAdd)
{
  vec temp(u + toAdd.u, v + toAdd.v, w + toAdd.w);
  return temp;
}

const vec & vec::operator=(const vec & toCopy)
{
	if (this != &toCopy) {
		u = toCopy.u;
		v = toCopy.v;
		w = toCopy.w;
	}

	return *this;
}