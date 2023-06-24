/*
 *  vec.h
 *  
 *
 *  Created by Zac Oler on 6/4/07.
 *  Copyright 2007 ZWING INC. All rights reserved.
 *
 */

#ifndef VEC_H
#define VEC_H

//Define TRUE AND FALSE
#define TRUE 1
#define FALSE 0

//INCLUDE ALL THE STUFF WE WILL NEED
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

//for cin and cout
using namespace std;

//Defining Vector Class
class vec
{
	//Variables
	protected:
		//Components of Vector
		double u;
		double v;
		double w;
		
	//Functions
	public:
		//Constructors
		vec();
		vec(double u0, double v0, double w0);
		
		//Accessors
		double getU() { return u; }
		double getV() { return v; }
		double getW() { return w; }
		
		//Other Functions
		void setVec(double u0, double v0, double w0);
		double dot(vec* otherVec);
		vec cross(vec otherVec);
		void add(vec* otherVec);
		void subtract(vec* otherVec);
		void scale(double alpha);
		double magnitude();
		vec vec::operator*(float mult);
		vec vec::operator+(const vec toAdd);
		const vec & vec::operator=(const vec & toCopy);

};

#endif