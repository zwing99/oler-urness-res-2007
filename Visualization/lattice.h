/*
 *  lattice.h
 *  
 *
 *  Created by Zac Oler on 6/4/07.
 *  Copyright 2007 ZWING ING. All rights reserved.
 *
 */
 #include "Point.h"
 #include "dipole.h"

#ifndef LATTICE_H
#define LATTICE_H

//Includes a Definition of Dipole and Vector

//Defining the lattice class
class lattice
{
	protected:
		//Place to Store the Lattice
		dipole**** myLattice;
		//Place to Store the Size of the Lattice
		int size[3]; 
		
	public:
		//Constructors
		lattice();
		lattice(int x0, int y0, int z0);
		
		//getFunctions
		int* getSize() { return size; }
		dipole* getDipole(int x,int y,int z);
		bool inBounds(point ptemp);
		
		//otherFuctions
		void changeDipole(int x,int y, int z, double u, double v, double w);
		
		//Destructor
		~lattice();
		
};

#endif
		