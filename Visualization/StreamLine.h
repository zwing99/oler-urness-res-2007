/*
 *  StreamLine.h
 *  
 *
 *  Created by Zac Oler on 6/18/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "Point.h"
#include "lattice.h"

#ifndef STREAMLINE_H
#define STREAMLINE_H



class StreamLine
{
	protected:
		point* points;
		point seed;
		lattice* curLat;
		int size;
		int size2;
	
	public:
		StreamLine();
		StreamLine(point newSeed, lattice* lat);
		~StreamLine();
		void calculateStreamInDir(int direction);
		void calculateStream();
		point takeRK4Step(point p0, float stepSize, bool *successful);
		vec getInterpolatedVector(point pos);
		point* getPoints();
		int getSize() {return size;}
		int getSize2() {return size2;}
		
};


#endif