/*
 *  StreamSurface.cpp
 *  
 *
 *  Created by Zac Oler on 6/19/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "StreamSurface.h"

StreamSurface::StreamSurface():
maxNumOfStreams(250)
{
}

StreamSurface::StreamSurface(point newSeed, lattice* lat):
maxNumOfStreams(250)
{
	mySeed = newSeed;
	myLat = lat;
	int sizeoflat = myLat->getSize()[0];
	streams = new StreamLine*[maxNumOfStreams];
	numOfStreams = 0;
	numOfSurfaces = 0;
	numOfPolygons = new int[maxNumOfStreams-1];
	drawpts = new point**[maxNumOfStreams-1];
	drawptsstreamnum = new int**[maxNumOfStreams-1];	
	normals = new vec*[maxNumOfStreams-1];
	for(int i = 0; i< (maxNumOfStreams-1); i++)
	{
		drawpts[i] = new point*[sizeoflat*200];
		drawptsstreamnum[i] = new int*[sizeoflat*200];
		for(int j = 0; j < 3200; j++)
		{
			drawpts[i][j] = new point[3];
			drawptsstreamnum[i][j] = new int[3];
		}
		normals[i] = new vec[sizeoflat*100];
	}
	
}

void StreamSurface::calculateNextStream()
{
	streams[numOfStreams++] = new StreamLine(mySeed,myLat);
}

void StreamSurface::updateCurrentStream()
{
	streams[numOfStreams-1]->calculateStream();
}

void StreamSurface::calculateNextSurface()
{
	calculateDrawPts(numOfSurfaces++);
}
//Assumption is that points are given in counter clockwise order

vec StreamSurface::calculateNorm(point a, point b, point c)
{
	vec av(c.GetX()-a.GetX(),c.GetY()-a.GetY(),c.GetZ()-a.GetZ());
	vec bv(c.GetX()-b.GetX(),c.GetY()-b.GetY(),c.GetZ()-b.GetZ());
	vec cross = av.cross(bv);
	
	double mag = cross.magnitude();
	
	cross.setVec(cross.getU()/mag,cross.getV()/mag,cross.getW()/mag);
	
	return cross;
}

void StreamSurface::calculateDrawPts(int surfaceNum)
{
	int sizeoflat = myLat->getSize()[0];
	StreamLine* stream1 = streams[surfaceNum];
	StreamLine* stream2 = streams[surfaceNum+1];
	point* pts1 = stream1->getPoints();
	point* pts2 = stream2->getPoints();
	int size11 = stream1->getSize();
	int size21 = stream2->getSize();
	int size12 = stream1->getSize2();
	int size22 = stream2->getSize2();
	numOfPolygons[surfaceNum] = 0;
	float length1 = 0.0;
	float length2 = 0.0;
	int* connect1 = new int[sizeoflat*50];
	int* connect2 = new int[sizeoflat*50];
	int connectCount = 0;
	int jprev = 0;
	
	length1 += point::DistanceBetween(pts1[0],pts1[1]);
	for(int i = 0; (i+1)<size11 ; i+=1)
	{
		length1 += point::DistanceBetween(pts1[i],pts1[i+1]);
		int j=0;
		while( length2 < length1 && (j+1)<size21)
		{
			length2 += point::DistanceBetween(pts2[j],pts2[j+1]);
			j++;
		}
		if((j+1)==size21)
		{
			break;
		}
		j--;
		
		if(j > jprev)
		{
			connect1[connectCount]=i;
			connect2[connectCount]=j;
			connectCount++;
			jprev = j;
		}
		length2 = 0;
	}
	
	
	drawpts[surfaceNum][0][0] = pts1[0];
	drawpts[surfaceNum][0][1] = pts2[1];
	drawpts[surfaceNum][0][2] = pts1[1];
	drawptsstreamnum[surfaceNum][0][0] = surfaceNum;
	drawptsstreamnum[surfaceNum][0][1] = surfaceNum+1;
	drawptsstreamnum[surfaceNum][0][2] = surfaceNum;
	//normals[surfaceNum][0] = calculateNorm(drawpts[surfaceNum][0][0],
	//		drawpts[surfaceNum][0][1],drawpts[surfaceNum][0][2]);
	numOfPolygons[surfaceNum]++;
	
	for(int i = 0; (i+1)<connectCount ; i++)
	{
		
		drawpts[surfaceNum][numOfPolygons[surfaceNum]][0] = pts1[connect1[i]];
		drawpts[surfaceNum][numOfPolygons[surfaceNum]][1] = pts2[connect2[i]];
		drawpts[surfaceNum][numOfPolygons[surfaceNum]][2] = pts1[connect1[i+1]];
		drawptsstreamnum[surfaceNum][numOfPolygons[surfaceNum]][0] = surfaceNum;
		drawptsstreamnum[surfaceNum][numOfPolygons[surfaceNum]][1] = surfaceNum+1;
		drawptsstreamnum[surfaceNum][numOfPolygons[surfaceNum]][2] = surfaceNum;
		//normals[surfaceNum][numOfPolygons[surfaceNum]] = 
		//        calculateNorm(pts1[connect1[i]],pts2[connect2[i]],pts1[connect1[i+1]]);
		numOfPolygons[surfaceNum]++;
		
		drawpts[surfaceNum][numOfPolygons[surfaceNum]][0] = pts2[connect2[i]];
		drawpts[surfaceNum][numOfPolygons[surfaceNum]][1] = pts2[connect2[i+1]];
		drawpts[surfaceNum][numOfPolygons[surfaceNum]][2] = pts1[connect1[i+1]];
		drawptsstreamnum[surfaceNum][numOfPolygons[surfaceNum]][0] = surfaceNum+1;
		drawptsstreamnum[surfaceNum][numOfPolygons[surfaceNum]][1] = surfaceNum+1;
		drawptsstreamnum[surfaceNum][numOfPolygons[surfaceNum]][2] = surfaceNum;
		//normals[surfaceNum][numOfPolygons[surfaceNum]] = 
		//        calculateNorm(pts2[connect2[i]],pts2[connect2[i+1]],pts1[connect1[i+1]]);
		numOfPolygons[surfaceNum]++;
	}
	
	length1 = 0.0;
	length2 = 0.0;
	delete [] connect1;
	delete [] connect2;
	connect1 = new int[sizeoflat*50];
	connect2 = new int[sizeoflat*50];
	connectCount = 0;
	jprev = 0;
	
	length1 += point::DistanceBetween(pts1[0],pts1[1]);
	for(int i = sizeoflat*50+1; (i+1)<size12 ; i+=1)
	{
		length1 += point::DistanceBetween(pts1[i],pts1[i+1]);
		int j=801;
		while( length2 < length1 && (j+1)<size22)
		{
			length2 += point::DistanceBetween(pts2[j],pts2[j+1]);
			j++;
		}
		if((j+1)==size22)
		{
			break;
		}
		j--;
		
		if(j > jprev)
		{
			connect1[connectCount]=i;
			connect2[connectCount]=j;
			connectCount++;
			jprev = j;
		}
		length2 = 0;
	}
	
	drawpts[surfaceNum][numOfPolygons[surfaceNum]][0] = pts1[sizeoflat*50];
	drawpts[surfaceNum][numOfPolygons[surfaceNum]][1] = pts2[sizeoflat*50+1];
	drawpts[surfaceNum][numOfPolygons[surfaceNum]][2] = pts1[sizeoflat*50+1];
	drawptsstreamnum[surfaceNum][numOfPolygons[surfaceNum]][0] = surfaceNum;
	drawptsstreamnum[surfaceNum][numOfPolygons[surfaceNum]][1] = surfaceNum+1;
	drawptsstreamnum[surfaceNum][numOfPolygons[surfaceNum]][2] = surfaceNum;
	//normals[surfaceNum][numOfPolygons[surfaceNum]] = calculateNorm(drawpts[surfaceNum][numOfPolygons[surfaceNum]][0],
	//		drawpts[surfaceNum][numOfPolygons[surfaceNum]][1],drawpts[surfaceNum][numOfPolygons[surfaceNum]][2]);
	numOfPolygons[surfaceNum]++;
	
	for(int i = 0; (i+1)<connectCount ; i++)
	{
		
		drawpts[surfaceNum][numOfPolygons[surfaceNum]][0] = pts1[connect1[i]];
		drawpts[surfaceNum][numOfPolygons[surfaceNum]][1] = pts2[connect2[i]];
		drawpts[surfaceNum][numOfPolygons[surfaceNum]][2] = pts1[connect1[i+1]];
		drawptsstreamnum[surfaceNum][numOfPolygons[surfaceNum]][0] = surfaceNum;
		drawptsstreamnum[surfaceNum][numOfPolygons[surfaceNum]][1] = surfaceNum+1;
		drawptsstreamnum[surfaceNum][numOfPolygons[surfaceNum]][2] = surfaceNum;
		//normals[surfaceNum][numOfPolygons[surfaceNum]] = 
		//        calculateNorm(pts1[connect1[i]],pts2[connect2[i]],pts1[connect1[i+1]]);
		numOfPolygons[surfaceNum]++;
		
		drawpts[surfaceNum][numOfPolygons[surfaceNum]][0] = pts2[connect2[i]];
		drawpts[surfaceNum][numOfPolygons[surfaceNum]][1] = pts2[connect2[i+1]];
		drawpts[surfaceNum][numOfPolygons[surfaceNum]][2] = pts1[connect1[i+1]];
		drawptsstreamnum[surfaceNum][numOfPolygons[surfaceNum]][0] = surfaceNum+1;
		drawptsstreamnum[surfaceNum][numOfPolygons[surfaceNum]][1] = surfaceNum+1;
		drawptsstreamnum[surfaceNum][numOfPolygons[surfaceNum]][2] = surfaceNum;
		//normals[surfaceNum][numOfPolygons[surfaceNum]] = 
		//        calculateNorm(pts2[connect2[i]],pts2[connect2[i+1]],pts1[connect1[i+1]]);
		numOfPolygons[surfaceNum]++;
	}
	delete [] connect1;
	delete [] connect2;
}

void StreamSurface::drawSurface(int surfaceNum)
{
	float colordivider = numOfSurfaces;
	float color;
	for(int i = 0; i < numOfPolygons[surfaceNum]; i++)
	{
		glBegin(GL_POLYGON);
			//glNormal3f(1,0,0);
			//glNormal3d(normals[surfaceNum][i].getU(),
			//		   normals[surfaceNum][i].getV(),
			//		   normals[surfaceNum][i].getW());
			color = drawptsstreamnum[surfaceNum][i][0]/colordivider;
			glColor4f(1.0,1.0-color,1.0-color,color);
			glVertex3f(drawpts[surfaceNum][i][0].GetX(),
					   drawpts[surfaceNum][i][0].GetY(),
					   drawpts[surfaceNum][i][0].GetZ());
			color = drawptsstreamnum[surfaceNum][i][1]/colordivider;
			glColor4f(1.0,1.0-color,1.0-color,color);
			glVertex3f(drawpts[surfaceNum][i][1].GetX(),
					   drawpts[surfaceNum][i][1].GetY(),
					   drawpts[surfaceNum][i][1].GetZ());
			color = drawptsstreamnum[surfaceNum][i][2]/colordivider;
			glColor4f(1.0,1.0-color,1.0-color,color);
			glVertex3f(drawpts[surfaceNum][i][2].GetX(),
					   drawpts[surfaceNum][i][2].GetY(),
					   drawpts[surfaceNum][i][2].GetZ());
		glEnd();
	}
}

void StreamSurface::drawStream(int streamNum)
{
	StreamLine* myStream = streams[streamNum];
	point* myPts = myStream->getPoints();
	int numOfPoints = myStream->getSize();
	int numOfPoints2 = myStream->getSize2();
	int sizeoflat = myLat->getSize()[0];
	glBegin(GL_LINE_STRIP);
	for(int i = 0; i< (numOfPoints); i++)
	{
		glVertex3f(myPts[i].GetX(), 
				   myPts[i].GetY(),
				   myPts[i].GetZ());
	}
	glEnd();
	glBegin(GL_LINE_STRIP);
	for(int i = sizeoflat*50; i< (numOfPoints2); i++)
	{
		glVertex3f(myPts[i].GetX(), 
				   myPts[i].GetY(),
				   myPts[i].GetZ());
	}
	glEnd();
}
