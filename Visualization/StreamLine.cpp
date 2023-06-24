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

StreamLine::StreamLine(point newSeed, lattice* lat)
{
	seed = newSeed;
	curLat = lat;
	int sizeoflat = curLat->getSize()[0];
	size = sizeoflat*100;
	size2 = 1600;
	points = new point[size];
	calculateStream();
}

StreamLine::~StreamLine()
{
	delete [] points;
}

void StreamLine::calculateStreamInDir(int direction)
{
	int sizeoflat = curLat->getSize()[0];
	int pointsCalculated;
	int maxSteps;
	if(direction == 1)
	{
		pointsCalculated = 1;
		maxSteps = sizeoflat*50;
	}
	else
	{
		pointsCalculated = sizeoflat*50+1;
		maxSteps = sizeoflat*100;
	}
		
	
	// figure out how much to calculate
	float threshold = 0.00001;
	float minDist = 0.001*sizeoflat;
	float stepSize = 0.01;
	float tol = minDist;
	float maxError = .0001;
	float minError = maxError/100;
	point p0 = seed;
	
	// set up the initial parts of the streamline
	points[pointsCalculated-1] = p0;

	// advance through the vector field until it exits the grid, an error occurs,
	// or the max number of points is reached
	bool successful = true;
	point fullStep;
	point halfStep1;
	point halfStep2;
	float tempdis;
	point currentPoint = p0;
	point previousPoint = p0;
	float distanceFromLast = 0;
	int iters = 0;
	while (pointsCalculated < maxSteps && iters < 100000)
	{
		// if the field is too small, stop integrating
		if (getInterpolatedVector(currentPoint).magnitude() <= threshold)
		{
			cout<<"yikes"<<endl;
			break;
		}

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
				{
					cout<<iters<<" "<<stepSize<<endl;
					break;
				}
			}
			else
			{
				cout<<iters<<" "<<stepSize<<endl;
				break;
			}
		}
		else
		{
			cout<<iters<<" "<<stepSize<<endl;
			break;
		}
		
		
		float error = point::DistanceBetween(fullStep, halfStep2);
		if(error<minError)
			error = minError;
		float relativeError = error/maxError;
		//cout<<relativeError<<endl;
		if (relativeError <= 1.0)
		{
			if (relativeError < 0.9)
			{
				stepSize /= pow( (double) relativeError,(double) 0.2 );
				stepSize /= 0.9;
				//cout<<stepSize<<endl;
			}
			// use the two results to estimate the next point
			currentPoint.SetX((16 * fullStep.GetX() - halfStep2.GetX()) / 15);
			currentPoint.SetY((16 * fullStep.GetY() - halfStep2.GetY()) / 15);
			currentPoint.SetZ((16 * fullStep.GetZ() - halfStep2.GetZ()) / 15);
		
			// find out how far the step went  
			tempdis = point::DistanceBetween(currentPoint, previousPoint);  
			distanceFromLast += tempdis;
			previousPoint = currentPoint;
			
			if (distanceFromLast > minDist)
			{	  
				points[pointsCalculated] = currentPoint;
				distanceFromLast = 0;
				pointsCalculated++;
			}
			
		}
		else
		{
			stepSize /= pow((double) relativeError, (double) .2 );
			stepSize *= 0.5;
			//cout<<stepSize<<endl;
			//cout<<"bad"<<endl;
		}
    
		iters++;     
	}
	if(direction == 1)
	{
		size = pointsCalculated;
		cout<<iters<<endl;
	}
	else
	{
		size2 = pointsCalculated;
		cout<<iters<<endl;
	}
	
	
}

void StreamLine::calculateStream()
{
	calculateStreamInDir(1);
	calculateStreamInDir(-1);
	
}

point StreamLine::takeRK4Step(point p0, float stepSize, bool *successful)
{
  // variables used in the Runge-Kutta method
  vec k[4];
  point curPosition = p0;

  // the amount of k[i] to add to p0
  float weights[4] = {0.5, 0.5, 1, 0};
  
  for (int i = 0; i < 4; i++) {    
    if (!(curLat->inBounds(curPosition)))
	{
	  cout<<"OUT"<<" "<<curPosition.GetX()<<" "<<curPosition.GetY()<< " "<<curPosition.GetZ()<<endl;
      *successful = false;
      return p0;
    }
 
    k[i] = getInterpolatedVector(curPosition) * stepSize;
    curPosition = p0 + k[i] * weights[i];
  }    
  
  *successful = true;
  return p0 + (k[0] * (1.0/6) + k[1] * (1.0/3) + k[2] * (1.0/3) + k[3] * (1.0/6));

}

vec StreamLine::getInterpolatedVector(point pos)
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
	float valueUtotal = 0;
	float valueVtotal = 0;
	float valueWtotal = 0;
	float weight;

	for (i = 0; i <= iMax; i++)
		for (j = 0; j <= jMax; j++)
			for (k = 0; k <= kMax; k++)
			{
				//weight = fabs((1-(x-xFloor+i)) *
				//			  (1-(y-yFloor+j)) *
				//			  (1-(z-zFloor+k)));
				valueU =  (float)(curLat->getDipole(xFloor+i, yFloor+j, zFloor+k))->getU();
				valueV =  (float)(curLat->getDipole(xFloor+i, yFloor+j, zFloor+k))->getV();
				valueW =  (float)(curLat->getDipole(xFloor+i, yFloor+j, zFloor+k))->getW();
				
				vec m(valueU,valueV,valueW);
				
				vec r(x-(xFloor+i), y-(yFloor+j), z-(zFloor+k));
				vec rhat = r;
				float rhatmag = rhat.magnitude();
				rhat = rhat*(1/rhatmag);
				
				vec b = (  (rhat*((float)m.dot(rhat)))*3 + m*(-1)  )*(1/(rhatmag*rhatmag*rhatmag));
				
				valueUtotal += (float)b.getU();
				valueVtotal += (float)b.getV();
				valueWtotal += (float)b.getW();						  
						  
			}
	
	vec tempVec(valueUtotal,valueVtotal,valueWtotal);
	
	return tempVec;
}

point* StreamLine::getPoints()
{
	return points;
}