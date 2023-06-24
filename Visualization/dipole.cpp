/*
 *  dipole.cpp
 *  
 *
 *  Created by Zac Oler on 6/4/07.
 *  Copyright 2007 ZWING INC. All rights reserved.
 *
 */

#include "dipole.h"

//Constructor
dipole::dipole(double u0, double v0, double w0)
{
	double un,vn,wn;
	u=u0;
	v=v0;
	w=w0;
	
	//Checks to be certain the magnitude is 1
	//Changes it if neccissary
	double mag = magnitude();
	if ((mag*mag)>1)
	{
		un=u0/mag;
		vn=v0/mag;
		wn=w0/mag;
		
		u=un;
		v=vn;
		w=wn;
	}
}

void dipole::setDipole(double u0, double v0, double w0)
{
	u=u0;
	v=v0;
	w=w0;
	
	//Checks to be certain the magnitude is 1
	//Changes it if neccissary
	double un,vn,wn;
	double mag = magnitude();
	if (mag>1)
	{
		un=u0/mag;
		vn=v0/mag;
		wn=w0/mag;
		
		u=un;
		v=vn;
		w=wn;
	}
	
}