/*
 *  dipole.h
 *  
 *
 *  Created by Zac Oler on 6/4/07.
 *  Copyright 2007 ZWING INC. All rights reserved.
 *
 */
#include "vec.h"

#ifndef DIPOLE_H
#define DIPOLE_H

//Include Vec Class


//Class dipole is a vector so it inherits its data and functions
class dipole : public vec
{
	public:
		//constructor
		dipole(double u0, double v0, double w0);
		//set the dipole
		void setDipole(double u0, double v0, double w0);
};

#endif
		
