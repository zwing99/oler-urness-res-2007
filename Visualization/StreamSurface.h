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
		point*** drawpts;
		int*** drawptsstreamnum;
		vec** normals;
		lattice* myLat;
		point mySeed;
		
		
	public:
		StreamSurface();
		StreamSurface(point newSeed, lattice* lat);
		~StreamSurface();
		void calculateNextStream();
		void updateCurrentStream();
		void calculateNextSurface();
		void calculateDrawPts(int surfaceNum);
		vec calculateNorm(point a, point b, point c);
		void drawSurface(int surfaceNum);
		void drawStream(int streamNum);
		int getNumOfSurfaces(){return numOfSurfaces;};
		int getNumOfStreams(){return numOfStreams;};
		
		
};

#endif