/*
 *  StreamLine.h
 *  
 *
 *  Created by Zac Oler on 6/18/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef STREAMLINE_H
#define STREAMLINE_H

#include "Point.h"
#include "lattice.h"

class StreamLine
{
	protected:
		Point* points;
		Point seed;
		lattice* curLat;
		int size;
		int size2;
	
	public:
		StreamLine();
		StreamLine(Point newSeed, lattice* lat);
		~StreamLine();
		void calculateStreamInDir(int direction);
		void calculateStream();
		Point takeRK4Step(Point p0, float stepSize, bool *successful);
		vec getInterpolatedVector(Point pos);
		Point* getPoints();
		int getSize() {return size;}
		int getSize2() {return size2;}
		
};


#endif