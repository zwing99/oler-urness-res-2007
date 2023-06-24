/*
 *  testLattice.cpp
 *  
 *
 *  Created by Zac Oler on 6/4/07.
 *  Copyright 2007 ZWING INC. All rights reserved.
 *
 */

//Inclue Lattice Stuff
#include "lattice.h"
#include "point.h"

//Define Size
#define WIDTH 600
#define HEIGHT 600
//Define PI
#define PI 3.14159265
//Defince Lattice Size
#define numsurfaces 4
#define sizex 20	
#define sizey 20
#define sizez 20

//Lattice Variables
lattice* myLat1 = new lattice(sizex,sizey,sizez);
int* mySize = myLat1->getSize();
lattice* myLat2 = new lattice(sizex,sizey,sizez);
int latNum = 1;
lattice* currentLat = myLat1;
lattice* writeLat = myLat2;

//Lattice Vars
long counter = 0;
double scaleValue;

//Magnetic Field
float magX = 0;
float magY = 0;
float magZ = 0;
vec* magneticField = new vec(magX,magY,magZ);

//Temp Variables
dipole* myDipole;
vec* tempVec;
dipole* tempDipole;
float totalEnergy;
double maxVecMag = 0;

//Variable for Temperature
float temperature =  .1;
float dtheta = sqrt(temperature);
float realtemperature=temperature;
float realdtheta=dtheta;
vec R1, R2, R3;
float variance;

//fstream variables
ofstream outputData;

//Sets up a rotation using the priciples of a quarternion
   //Note: assumes axis is normalized and angle is in radians
void setRotation(vec axis, double angle)
{	
	double w = cos(angle/2.0);
	double sinangle = sin(angle/2.0);
	double x = axis.getU()*sinangle;
	double y = axis.getV()*sinangle;
	double z = axis.getW()*sinangle;
	
	R1.setVec(1 - 2*y*y - 2*z*z, 2*x*y - 2*w*z, 2*z*x + 2*w*y);
	R2.setVec(2*x*y + 2*w*z, 1 - 2*x*x - 2*z*z, 2*y*z - 2*w*x);
	R3.setVec(2*z*x - 2*w*y, 2*y*z + 2*w*x, 1 - 2*x*x - 2*y*y);
}

//Gives me a Interpolated Vector that is **Normalized**
//Used to display the dipoles for large lattices
	//Code for function taken and modified from Blayne Field's FieldVis

/*
vec getInterpolatedVector(point pos)
{
	float x = pos.GetX();
	float y = pos.GetY();
	float z = pos.GetZ();
	
	int xFloor = (int) x;
	int yFloor = (int) y;
	int zFloor = (int) z;

	int i, j, k;
	int iMax = (xFloor >= (currentLat->getSize())[0] - 1 ? 0 : 1);
	int jMax = (yFloor >= (currentLat->getSize())[1] - 1 ? 0 : 1);
	int kMax = (zFloor >= (currentLat->getSize())[2] - 1 ? 0 : 1);
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
						  (float)(currentLat->getDipole(xFloor+i, yFloor+j, zFloor+k))->getU();
				valueV += weight * 
						  (float)(currentLat->getDipole(xFloor+i, yFloor+j, zFloor+k))->getV();
				valueW += weight * 
						  (float)(currentLat->getDipole(xFloor+i, yFloor+j, zFloor+k))->getW();
			}
	
	vec tempVec(valueU,valueV,valueW);
	tempVec.scale(1/tempVec.magnitude());
	
	return tempVec;
}
*/

//Gives the Energy of the dipole at position x,y,z of the Current Lattice
double getEnergy(int x, int y, int z)
{
	double H = 0;
	dipole* dipxyz = currentLat->getDipole(x,y,z);
	H-=dipxyz->dot(magneticField);
	if(x != 0)
		H-=dipxyz->dot(currentLat->getDipole(x-1,y,z));
	if(x != (mySize[0]-1))
		H-=dipxyz->dot(currentLat->getDipole(x+1,y,z));
	if(y != 0)
		H-=dipxyz->dot(currentLat->getDipole(x,y-1,z));
	if(y != (mySize[1]-1))
		H-=dipxyz->dot(currentLat->getDipole(x,y+1,z));
	if(z != 0)
		H-=dipxyz->dot(currentLat->getDipole(x,y,z-1));
	if(z != (mySize[2]-1))
		H-=dipxyz->dot(currentLat->getDipole(x,y,z+1));
	H+=1/2*temperature;
	return H;
}

//Gives the Maximum Energy for the dipole at position x,y,z
double getMaxEnergy(int x, int y, int z)
{
	double H = 0;
	
	H+=magneticField->magnitude();
	if(x != 0)
		H+=1;
	if(x != (mySize[0]-1))
		H+=1;
	if(y != 0)
		H+=1;
	if(y != (mySize[1]-1))
		H+=1;
	if(z != 0)
		H+=1;
	if(z != (mySize[2]-1))
		H+=1;
	H-=1/2*temperature;
	return H;
}

/*this function iterates the system*/
void iterate()
{
	dtheta = sqrt((double)temperature); //Calculate Angle
	
	//Part 0 of for loop rotates the dipoles radomnly (aka simulation of temperature)
	//Part 1 of for loop moves the vectors to minimize energy
	float mov = 0, finalavg = 0, avgmin = 1000, avgmax = -1000;
	float avgstore[sizex*sizey*sizez];
	for(int part = 0; part<2; part++)
	{
		
		//Loop through entire lattice
		int i,j,k;
		for(i=0; i<sizex; i++)
		{
			for(j=0; j<sizey; j++)
			{
				for(k=0; k<sizez; k++)
				{
					dipole* dipxyz = currentLat->getDipole(i,j,k);  //gets dipole from current lattice
					if(part==0)
					{
						//Sets up rotation axis
						vec rotaxis1(2.0*rand()/RAND_MAX-1,2.0*rand()/RAND_MAX-1,2.0*rand()/RAND_MAX-1);
						vec dipoleaxis(dipxyz->getU(), dipxyz->getV(), dipxyz->getW());
						rotaxis1 = rotaxis1.cross(dipoleaxis);
						rotaxis1.scale(1.0/rotaxis1.magnitude());
						
						//set up radius of Distribution
						float radiusOfDist = variance;
						if(dtheta < radiusOfDist)
							radiusOfDist = dtheta;
						
						//Generate Distribution
						double avg = 0.0;
						for(int r = 0; r < 8; r++)
						{
							avg += (((double)rand()/RAND_MAX)*2)*radiusOfDist;
						}
						avg /= 8.0;
						avg += (dtheta-radiusOfDist);
						avgstore[i*sizez*sizey+j*sizez+k] = avg;
						if(avg<avgmin)
							avgmin = avg;
						if(avg>avgmax)
							avgmax = avg;
						finalavg += avg;
						
						//Setup Rotation and then rotate
						setRotation(rotaxis1,avg);
						dipole heateddipole(R1.dot(dipxyz),R2.dot(dipxyz),R3.dot(dipxyz));
						
						//Write Dipole to the write lattice
						tempDipole = writeLat->getDipole(i,j,k);
						tempDipole->setDipole(heateddipole.getU(),heateddipole.getV(),heateddipole.getW());
					}
					
					if(part==1)
					{
						
						//Vector to Store How to move
						tempVec = new vec();					
						
						//Gets the ideal dirrections to move
						tempVec->add(magneticField);
						if(i != 0)
						{
							tempVec->add(currentLat->getDipole(i-1,j,k));
						}
						if(i != (mySize[0]-1))
						{
							tempVec->add(currentLat->getDipole(i+1,j,k));
						}
						if(j != 0)
						{
							tempVec->add(currentLat->getDipole(i,j-1,k));
						}
						if(j != (mySize[1]-1))
						{
							tempVec->add(currentLat->getDipole(i,j+1,k));
						}
						if(k != 0)
						{
							tempVec->add(currentLat->getDipole(i,j,k-1));
						}
						if(k != (mySize[2]-1))
						{
							tempVec->add(currentLat->getDipole(i,j,k+1));
						}
						
						//Get the vector between where to move and where we are
						tempVec->subtract(dipxyz);
						
						//Scale Down that vector to keep steps small
						tempVec->scale(scaleValue);
						
						//Add it to old dipole to get new vec
						tempVec->add(dipxyz);
						
						//Write Dipole
						tempDipole = writeLat->getDipole(i,j,k);			
						tempDipole->setDipole(tempVec->getU(),
											  tempVec->getV(),
											  tempVec->getW());
						
						//Free Pointer
						delete tempVec;
					}
													
				}
			}
		}
		
		if(part == 0)
		{
		finalavg /= sizex*sizey*sizez;
		//float variance = 0;
		float finalavgsquared = finalavg*finalavg;
		//for(int count = 0; count < (sizex*sizey*sizez); count++)
		//	variance += avgstore[count]*avgstore[count]-finalavgsquared;
		//variance /= sizex*sizey*sizez-1;
		//float stddev = sqrt(variance);
		//cout<<"avgangle: "<<finalavg<<"  minangle: "<<avgmin<<"  maxangle: "<<avgmax<<endl
		//	<<"stddev: "<<stddev<<"  percentdev: "<<stddev/finalavg<<"  temperatureaprox: "<<finalavgsquared
		//	<<"  variance: "<<variance<<endl;
		realdtheta = finalavg;
		realtemperature = finalavgsquared;
		}
		
		
		//SWAP THE READ AND WRITE LATTICES
		if(latNum == 1)
		{
			currentLat = myLat2;
			writeLat = myLat1;
			latNum = 2;
		}
		 else
		 {
			currentLat = myLat1;
			writeLat = myLat2;
			latNum = 1;
		 }
		
	}//End of Part loop
	
	//Increment Counter
	counter++;
	double u_component = 0;
	double v_component = 0;
	double w_component = 0;
	int i,j,k;
	
	//Get Magnetization
	for(i=0; i<sizex; i++)
	{
		for(j=0; j<sizey; j++)
		{
			for(k=0; k<sizez; k++)
			{
				tempDipole = currentLat->getDipole(i,j,k);
				u_component += tempDipole->getU();
				v_component += tempDipole->getV();
				w_component += tempDipole->getW();
			}
		}
	}
	u_component/=sizex*sizey*sizez;
	v_component/=sizex*sizey*sizez;
	w_component/=sizex*sizey*sizez;		
	
	
	
	//Get TotalEnergy and MinEnergy
	//Setup my VolumeRenderer
	totalEnergy = 0;
	float minEnergy = 0;
	for(i=0; i<mySize[0]; i++)
	{
		for(j=0; j<mySize[1]; j++)
		{
			for(k=0; k<mySize[2]; k++)
			{
				tempDipole = currentLat->getDipole(i,j,k);
				minEnergy -= (float)getMaxEnergy(i,j,k);
				double energyat = getEnergy(i,j,k);
				totalEnergy += (float) energyat;
			}
		}
	}
	
	if(counter == 1)
	{
		//outputData<<"# Iteration Energy MinEnergy Delta temp realtemp dtheta realdtheta MagneticFieldX Y Z MagnetizationX Y Z"<<endl;
	}
	if(counter==1000)
		outputData<<temperature<<" "<<u_component<<" "<<magX<<endl;
		/*outputData<<counter<<" "<<totalEnergy<<" "<<minEnergy<<" "<<totalEnergy-minEnergy<<" "
				  <<temperature<<" "<<realtemperature<<" "
				  <<dtheta<<" "<<realdtheta<<" "
				  <<magX<<" "<<magY<<" "<<magZ<<" "
				  <<u_component<<" "<<v_component<<" "<<w_component<<endl;*/
}

//My Setup Function
void init(int argc, char **argv)
{	
	if(argc>1)
		outputData.open(argv[1], ios::app);
	else
		outputData.open("Data.txt", ios::out);
	if(argc>2)
		sscanf(argv[2],"%f",&magX);
	else
		magX = .01;
	if(argc>3)
		sscanf(argv[3],"%f",&temperature);
	else
		temperature = .01;
	if(argc>4)
		sscanf(argv[4],"%f",&variance);
	else
		variance = 0.01;
	magneticField->setVec(magX,0,0);
	srand(time(NULL));
	
	scaleValue = .02;
}

//My MAIN FUNCTION YEAH!!!
int main(int argc, char **argv)
{
	init(argc,argv); //Call init fuction
	
	int timestart = time(NULL);
	for(int i=0;i<1000;i++)
	{
		iterate();
		//if((i+1)%100 == 0)
		//	cout<<i+1<<endl;
	}
	int timeend = time(NULL);
	cout<<timeend-timestart<<endl;
		
	outputData.close();
	
}