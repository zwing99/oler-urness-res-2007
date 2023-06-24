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

StreamSurface::StreamSurface(Point newSeed, lattice* lat):
maxNumOfStreams(250)
{
	mySeed = newSeed;
	myLat = lat;
	streams = new StreamLine*[maxNumOfStreams];
	numOfStreams = 0;
	numOfSurfaces = 0;
	numOfPolygons = new int[maxNumOfStreams-1];
	drawpts = new Point**[maxNumOfStreams-1];
	normals = new vec*[maxNumOfStreams-1];
	for(int i = 0; i< (maxNumOfStreams-1); i++)
	{
		drawpts[i] = new Point*[3200];
		for(int j = 0; j < 3200; j++)
			drawpts[i][j] = new Point[3];
		normals[i] = new vec[1600];
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

vec StreamSurface::calculateNorm(Point a, Point b, Point c)
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
	StreamLine* stream1 = streams[surfaceNum];
	StreamLine* stream2 = streams[surfaceNum+1];
	Point* pts1 = stream1->getPoints();
	Point* pts2 = stream2->getPoints();
	int size11 = stream1->getSize();
	int size21 = stream2->getSize();
	int size12 = stream1->getSize2();
	int size22 = stream2->getSize2();
	numOfPolygons[surfaceNum] = 0;
	
	drawpts[surfaceNum][0][0] = pts1[0];
	drawpts[surfaceNum][0][1] = pts2[1];
	drawpts[surfaceNum][0][2] = pts1[1];
	normals[surfaceNum][0] = calculateNorm(drawpts[surfaceNum][0][0],
			drawpts[surfaceNum][0][1],drawpts[surfaceNum][0][2]);
	numOfPolygons[surfaceNum]++;
			
	for(int i = 1; (((i+1) < size11) && ((i+1) < size21)); i++)
	{
		drawpts[surfaceNum][numOfPolygons[surfaceNum]][0] = pts1[i];
		drawpts[surfaceNum][numOfPolygons[surfaceNum]][1] = pts2[i];
		drawpts[surfaceNum][numOfPolygons[surfaceNum]][2] = pts1[i+1];
		normals[surfaceNum][numOfPolygons[surfaceNum]] = 
		        calculateNorm(pts1[i],pts2[i],pts1[i+1]);
		numOfPolygons[surfaceNum]++;
		
		drawpts[surfaceNum][numOfPolygons[surfaceNum]][0] = pts2[i];
		drawpts[surfaceNum][numOfPolygons[surfaceNum]][1] = pts2[i+1];
		drawpts[surfaceNum][numOfPolygons[surfaceNum]][2] = pts1[i+1];
		normals[surfaceNum][numOfPolygons[surfaceNum]] = 
		        calculateNorm(pts2[i],pts2[i+1],pts1[i+1]);
		numOfPolygons[surfaceNum]++;
	}
	/*
	if(size11 > size21)
	{
		for(int i = (size21-1); (i+1) < size11;  i++)
		{
			drawpts[surfaceNum][numOfPolygons[surfaceNum]][0] = pts2[size21 - 1];
			drawpts[surfaceNum][numOfPolygons[surfaceNum]][1] = pts1[i+1];
			drawpts[surfaceNum][numOfPolygons[surfaceNum]][2] = pts1[i];
			normals[surfaceNum][numOfPolygons[surfaceNum]] = 
					calculateNorm(pts2[size21-1],pts1[i+1],pts1[i]);
			numOfPolygons[surfaceNum]++;
		}
	}
	else if(size21 > size11)
	{
		for(int i = (size11-1); (i+1) < size21;  i++)
		{
			drawpts[surfaceNum][numOfPolygons[surfaceNum]][0] = pts1[size11 - 1];
			drawpts[surfaceNum][numOfPolygons[surfaceNum]][1] = pts2[i];
			drawpts[surfaceNum][numOfPolygons[surfaceNum]][2] = pts2[i+1];
			normals[surfaceNum][numOfPolygons[surfaceNum]] = 
					calculateNorm(pts1[size11-1],pts2[i],pts2[i+1]);
			numOfPolygons[surfaceNum]++;
		}
	
	}
	*/
	drawpts[surfaceNum][800][0] = pts1[0];
	drawpts[surfaceNum][800][1] = pts2[1];
	drawpts[surfaceNum][800][2] = pts1[1];
	normals[surfaceNum][800] = calculateNorm(drawpts[surfaceNum][800][0],
			drawpts[surfaceNum][800][1],drawpts[surfaceNum][800][2]);
	numOfPolygons[surfaceNum]++;
			
	for(int i = 801; (((i+1) < size12) && ((i+1) < size22)); i++)
	{
		drawpts[surfaceNum][numOfPolygons[surfaceNum]][0] = pts1[i];
		drawpts[surfaceNum][numOfPolygons[surfaceNum]][1] = pts2[i];
		drawpts[surfaceNum][numOfPolygons[surfaceNum]][2] = pts1[i+1];
		normals[surfaceNum][numOfPolygons[surfaceNum]] = 
		        calculateNorm(pts1[i],pts2[i],pts1[i+1]);
		numOfPolygons[surfaceNum]++;
		
		drawpts[surfaceNum][numOfPolygons[surfaceNum]][0] = pts2[i];
		drawpts[surfaceNum][numOfPolygons[surfaceNum]][1] = pts2[i+1];
		drawpts[surfaceNum][numOfPolygons[surfaceNum]][2] = pts1[i+1];
		normals[surfaceNum][numOfPolygons[surfaceNum]] = 
		        calculateNorm(pts2[i],pts2[i+1],pts1[i+1]);
		numOfPolygons[surfaceNum]++;
	}
	
	/*
	if(size12 > size22)
	{
		for(int i = (size22-1); (i+1) < size12;  i++)
		{
			drawpts[surfaceNum][numOfPolygons[surfaceNum]][0] = pts2[size22 - 1];
			drawpts[surfaceNum][numOfPolygons[surfaceNum]][1] = pts1[i+1];
			drawpts[surfaceNum][numOfPolygons[surfaceNum]][2] = pts1[i];
			normals[surfaceNum][numOfPolygons[surfaceNum]] = 
					calculateNorm(pts2[size22-1],pts1[i+1],pts1[i]);
			numOfPolygons[surfaceNum]++;
		}
	}
	else if(size22 > size12)
	{
		for(int i = (size12-1); (i+1) < size22;  i++)
		{
			drawpts[surfaceNum][numOfPolygons[surfaceNum]][0] = pts1[size12 - 1];
			drawpts[surfaceNum][numOfPolygons[surfaceNum]][1] = pts2[i];
			drawpts[surfaceNum][numOfPolygons[surfaceNum]][2] = pts2[i+1];
			normals[surfaceNum][numOfPolygons[surfaceNum]] = 
					calculateNorm(pts1[size12-1],pts2[i],pts2[i+1]);
			numOfPolygons[surfaceNum]++;
		}
	
	}
	*/
}

void StreamSurface::drawSurface(int surfaceNum)
{
	int* mySize = myLat->getSize();
	for(int i = 0; i < numOfPolygons[surfaceNum]; i++)
	{
		glBegin(GL_POLYGON);
			//glNormal3f(1,0,0);
			glNormal3d(normals[surfaceNum][i].getU(),
					   normals[surfaceNum][i].getV(),
					   normals[surfaceNum][i].getW());
			//if(i == 5)
			//cout<<normals[surfaceNum][i].getU()<<" "
			//		   <<normals[surfaceNum][i].getV()<<" "
			//		   <<normals[surfaceNum][i].getW()<<endl;
			glVertex3f(drawpts[surfaceNum][i][0].GetX(),
					   drawpts[surfaceNum][i][0].GetY(),
					   drawpts[surfaceNum][i][0].GetZ());
			glVertex3f(drawpts[surfaceNum][i][1].GetX(),
					   drawpts[surfaceNum][i][1].GetY(),
					   drawpts[surfaceNum][i][1].GetZ());
			glVertex3f(drawpts[surfaceNum][i][2].GetX(),
					   drawpts[surfaceNum][i][2].GetY(),
					   drawpts[surfaceNum][i][2].GetZ());
			//glVertex3f(drawpts[surfaceNum][i][0].GetX(),
			//		   drawpts[surfaceNum][i][0].GetY(),
			//		   drawpts[surfaceNum][i][0].GetZ());
		glEnd();
	}
}

void StreamSurface::drawStream(int streamNum)
{
	int* mySize = myLat->getSize();
	StreamLine* myStream = streams[streamNum];
	Point* myPts = myStream->getPoints();
	int numOfPoints = myStream->getSize();
	int numOfPoints2 = myStream->getSize2();
	glBegin(GL_LINE_STRIP);
	for(int i = 0; i< (numOfPoints); i++)
	{
		glVertex3f(myPts[i].GetX(), 
				   myPts[i].GetY(),
				   myPts[i].GetZ());
	}
	glEnd();
	glBegin(GL_LINE_STRIP);
	for(int i = 800; i< (numOfPoints2); i++)
	{
		glVertex3f(myPts[i].GetX(), 
				   myPts[i].GetY(),
				   myPts[i].GetZ());
	}
	glEnd();
}
