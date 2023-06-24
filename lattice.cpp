/*
 *  lattice.cpp
 *  
 *
 *  Created by Zac Oler on 6/4/07.
 *  Copyright 2007 ZWING INC. All rights reserved.
 *
 */

#define PI 3.14159265

#include "lattice.h"

//Basic Constructor Creates a 10by10by10 lattice
lattice::lattice()
{
	srand(time(NULL));
	size[0]=10;
	size[1]=10;
	size[2]=10;
	myLattice = new dipole***[size[0]];
	for(int i = 0; i < size[0]; i++)
	{
		myLattice[i]= new dipole**[size[1]];
		for(int j = 0; j < size[1]; j++)
		{
			myLattice[i][j] = new dipole*[size[2]];
			for(int k=0; k<size[2]; k++)
			{
				//myLattice[i][j][k] = new dipole(2.0*rand()/RAND_MAX-1,(rand()%3-1),cos(rand()*PI/180));
				myLattice[i][j][k] = new dipole(2.0*rand()/RAND_MAX-1,
												2.0*rand()/RAND_MAX-1,
												2.0*rand()/RAND_MAX-1);
				//myLattice[i][j][k] = new dipole(-1,-1,-1);
			}
		}
	}
}

//Constructor creates a Specified Lattice size
lattice::lattice(int x0, int y0, int z0)
{
	srand(20349875);
	size[0]=x0;
	size[1]=y0;
	size[2]=z0;
	myLattice = new dipole***[size[0]];
	for(int i = 0; i < size[0]; i++)
	{
		myLattice[i]= new dipole**[size[1]];
		for(int j = 0; j < size[1]; j++)
		{
			myLattice[i][j] = new dipole*[size[2]];
			for(int k=0; k<size[2]; k++)
			{
				myLattice[i][j][k] = new dipole(2.0*rand()/RAND_MAX-1,
												2.0*rand()/RAND_MAX-1,
												2.0*rand()/RAND_MAX-1);
			}
		}
	}
}

//get the Dipole at given Location
dipole* lattice::getDipole(int x, int y, int z)
{
	return myLattice[x][y][z];
}

//Modifies the Dipole at given Location
void lattice::changeDipole(int x,int y, int z, double u, double v, double w)
{
	dipole* temp = myLattice[x][y][z];
	temp->setDipole(u,v,w);
}

//Destroys then ENTIRE LATTICE
lattice::~lattice()
{
	for(int i=0; i<size[0]; i++)
	{
		for(int j=0; j<size[1]; j++)
		{
			for(int k=0; k<size[2]; k++)
			{
				delete myLattice[i][j][k];
				myLattice[i][j][k]=NULL;
			}
		}
	}
	delete [] myLattice;
	myLattice = NULL;
}

//Sees if a given point would be inbounds
bool lattice::inBounds(Point ptemp)
{
	float xtemp = ptemp.GetX();
	float ytemp = ptemp.GetY();
	float ztemp = ptemp.GetZ();
	
	return (xtemp >= 0 && xtemp <= size[0]
		 && ytemp >= 0 && ytemp <= size[1]
		 && ztemp >= 0 && ztemp <= size[2]);
	
}