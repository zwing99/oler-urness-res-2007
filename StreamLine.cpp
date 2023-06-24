/*
 *  StreamLine.cpp
 *  
 *
 *  Created by Zac Oler on 6/18/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "StreamLine.h"

using namespace std;

StreamLine::StreamLine()
{
}

StreamLine::StreamLine(Point newSeed, lattice* lat)
{
	seed = newSeed;
	curLat = lat;
	size = 1600;
	size2 = 1600;
	points = new Point[size];
	calculateStream();
}

StreamLine::~StreamLine()
{
	delete [] points;
}

void StreamLine::calculateStreamInDir(int direction)
{
	int pointsCalculated;
	int maxSteps;
	if(direction == 1)
	{
		pointsCalculated = 1;
		maxSteps = 800;
	}
	else
	{
		pointsCalculated = 800+1;
		maxSteps = 1600;
	}
		
	
	// figure out how much to calculate
	float threshold = 0.000001;
	float minDist = 0.001;  
	float tol = 0.01;
	float minError = tol / 10;
	float stepSize = 0.1;
	Point p0 = seed;
	
	// set up the initial parts of the streamline
	points[pointsCalculated-1] = p0;

	// advance through the vector field until it exits the grid, an error occurs,
	// or the max number of points is reached
	bool successful = true;
	Point fullStep;
	Point halfStep1;
	Point halfStep2;
	Point currentPoint = p0;
	Point previousPoint = p0;
	float distanceFromLast = 0;
	int iters = 0;
	while (pointsCalculated < maxSteps && iters < 4000)
	{
		// if the field is too small, stop integrating
		if (getInterpolatedVector(currentPoint).magnitude() <= threshold)
			break;

		// calculate the next point by using a full step,
		// and by using 2 half steps
		// stop integrating if the grid is exited
		fullStep = takeRK4Step(currentPoint, direction * stepSize, &successful);
		if (successful)
		{
			halfStep1 = takeRK4Step(currentPoint, direction * stepSize / 2, &successful);
			if (successful)
			{
				halfStep2 = takeRK4Step(halfStep1, direction * stepSize / 2, &successful);
				if(!successful)
					break;
			}
			else
				break;
		}
		else
			break;
    
		// estimate the error, and adjust the step size
		float error = Point::DistanceBetween(fullStep, halfStep2);
		if (error < minError)
			error = minError;
		//stepSize *= (float) pow((double) tol / error, (double) 0.2);
    
		// use the two results to estimate the next point
		currentPoint.SetX((16 * fullStep.GetX() - halfStep2.GetX()) / 15);
		currentPoint.SetY((16 * fullStep.GetY() - halfStep2.GetY()) / 15);
		currentPoint.SetZ((16 * fullStep.GetZ() - halfStep2.GetZ()) / 15);
    
		// find out how far the step went       
		distanceFromLast += Point::DistanceBetween(currentPoint, previousPoint);
		previousPoint = currentPoint;
  
		// update the streamline coordinates      
		if (distanceFromLast > minDist)
		{	  
		  points[pointsCalculated] = currentPoint;
		  distanceFromLast = 0;
		  pointsCalculated++;
		}
		iters++;     
	}
	if(direction == 1)
		size = pointsCalculated;
	else
		size2 = pointsCalculated;
	
}

void StreamLine::calculateStream()
{
	calculateStreamInDir(1);
	calculateStreamInDir(-1);
	
}

Point StreamLine::takeRK4Step(Point p0, float stepSize, bool *successful)
{
  // variables used in the Runge-Kutta method
  vec k[4];
  Point curPosition = p0;

  // the amount of k[i] to add to p0
  float weights[4] = {0.5, 0.5, 1, 0};
  
  for (int i = 0; i < 4; i++) {    
    if (!curLat->inBounds(curPosition)) {
      *successful = false;
      return p0;
    }
 
    k[i] = getInterpolatedVector(curPosition) * stepSize;
    curPosition = p0 + k[i] * weights[i];
  }    
  
  *successful = true;
  return p0 + (k[0] * (1.0/6) + k[1] * (1.0/3) + k[2] * (1.0/3) + k[3] * (1.0/6));

}

vec StreamLine::getInterpolatedVector(Point pos)
{
	float x = pos.GetX();
	float y = pos.GetY();
	float z = pos.GetZ();
	
	int xFloor = (int) x;
	int yFloor = (int) y;
	int zFloor = (int) z;

	int i, j, k;
	int iMax = (xFloor >= (curLat->getSize())[0] - 1 ? 0 : 1);
	int jMax = (yFloor >= (curLat->getSize())[1] - 1 ? 0 : 1);
	int kMax = (zFloor >= (curLat->getSize())[2] - 1 ? 0 : 1);
	float valueU = 0;
	float valueV = 0;
	float valueW = 0;
	float weight;

	for (i = 0; i <= iMax; i++)
		for (j = 0; j <= jMax; j++)
			for (k = 0; k <= kMax; k++)
			{
				weight = fabs((1-(x-xFloor+i)) *
							  (1-(y-yFloor+j)) *
							  (1-(z-zFloor+k)));
				valueU += weight * 
						  (float)(curLat->getDipole(xFloor+i, yFloor+j, zFloor+k))->getU();
				valueV += weight * 
						  (float)(curLat->getDipole(xFloor+i, yFloor+j, zFloor+k))->getV();
				valueW += weight * 
						  (float)(curLat->getDipole(xFloor+i, yFloor+j, zFloor+k))->getW();
	}
	
	vec tempVec(valueU,valueV,valueW);
	
	return tempVec;
}

Point* StreamLine::getPoints()
{
	return points;
}