/*
 *  StreamSurface.h
 *  
 *
 *  Created by Zac Oler on 6/19/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "StreamLine.h"
#ifndef STREAMSURFACE_H
#define STREAMSURFACE_H

class StreamSurface
{
	protected:
		StreamLine** streams;
		int numOfStreams;
		const int maxNumOfStreams;
		int numOfSurfaces;
		int* numOfPolygons;
		Point*** drawpts;
		vec** normals;
		lattice* myLat;
		Point mySeed;
		
		
	public:
		StreamSurface();
		StreamSurface(Point newSeed, lattice* lat);
		~StreamSurface();
		void calculateNextStream();
		void updateCurrentStream();
		void calculateNextSurface();
		void calculateDrawPts(int surfaceNum);
		vec calculateNorm(Point a, Point b, Point c);
		void drawSurface(int surfaceNum);
		void drawStream(int streamNum);
		int getNumOfSurfaces(){return numOfSurfaces;};
		int getNumOfStreams(){return numOfStreams;};
		
		
};

#endif