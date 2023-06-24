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

//Define Size
#define WIDTH 600
#define HEIGHT 600
//Define TRUE AND FALSE
#define TRUE 1
#define FALSE 0
//Define PI
#define PI 3.14159265

//START ASSUMING LEFT BUTTON IS DOWN
int left_button_down = FALSE;

//VARIALBES FOR ROTATION
int previous_mouseX;
int previous_mouseY;
double view_rotX;
double view_rotY;
double view_rotZ;

//Zoom Variable
float zoom;

//Lattice Variables
lattice* myLat = new lattice(10,10,10);
int* mySize = myLat->getSize();

//Temporary Dipole Storage
dipole* myDipole;

//Magnetic Field
vec* magneticField = new vec(0,0,1);

//Matrix Vectors
vec* A1 = new vec();
vec* A2 = new vec();
vec* A3 = new vec();
double sinphi, cosphi, sintheta, costheta, sinpsi, cospsi;

//Temp Variables
double ut,vt,wt;
int xt,yt,zt;
int phiTemp, thetaTemp, psiTemp;
dipole* tempDipole;
double energyBefore, energyAfter;

time_t myTime = time(NULL);
int frameCount =0;

void createRot(double phi, double theta, double psi)
{
	phi = phi*PI/180.0;
	theta = theta*PI/180.0;
	psi = psi*PI/180.0;
	sinphi = sin(phi);
	cosphi = cos(phi);
	sintheta = sin(theta);
	costheta = cos(theta);
	sinpsi = sin(psi);
	cospsi = cos(psi);
	A1->setVec(costheta*cosphi,
			  costheta*sinphi,
			  -sintheta);
	A2->setVec(sinpsi*sintheta*cosphi-cospsi*sinphi,
			  sinpsi*sintheta*sinphi+cospsi*cosphi,
			  costheta*sinpsi);
	A3->setVec(cospsi*sintheta*cosphi+sinpsi*sinphi,
			  cospsi*sintheta*sinphi-sinpsi*cosphi,
			  costheta*cospsi);
}

double getEnergy(int x, int y, int z)
{
	double H = 0;
	dipole* dipxyz = myLat->getDipole(x,y,z);
	H-=6*dipxyz->dot(magneticField);
	if(x != 0)
		H-=dipxyz->dot(myLat->getDipole(x-1,y,z));
	if(x != (mySize[0]-1))
		H-=dipxyz->dot(myLat->getDipole(x+1,y,z));
	if(y != 0)
		H-=dipxyz->dot(myLat->getDipole(x,y-1,z));
	if(y != (mySize[1]-1))
		H-=dipxyz->dot(myLat->getDipole(x,y+1,z));
	if(z != 0)
		H-=dipxyz->dot(myLat->getDipole(x,y,z-1));
	if(z != (mySize[2]-1))
		H-=dipxyz->dot(myLat->getDipole(x,y,z+1));
	
	return H;
}

double getEnergy(int x, int y, int z, double u, double v, double w)
{
	double H = 0;
	dipole* dipxyz = new dipole(u,v,w);
	H-=6*dipxyz->dot(magneticField);
	if(x != 0)
		H-=dipxyz->dot(myLat->getDipole(x-1,y,z));
	if(x != (mySize[0]-1))
		H-=dipxyz->dot(myLat->getDipole(x+1,y,z));
	if(y != 0)
		H-=dipxyz->dot(myLat->getDipole(x,y-1,z));
	if(y != (mySize[1]-1))
		H-=dipxyz->dot(myLat->getDipole(x,y+1,z));
	if(z != 0)
		H-=dipxyz->dot(myLat->getDipole(x,y,z-1));
	if(z != (mySize[2]-1))
		H-=dipxyz->dot(myLat->getDipole(x,y,z+1));
	
	delete dipxyz;
	
	return H;
}

//My Display Function
void display(void)
{
	/*clear all pixels*/
	glClear(GL_COLOR_BUFFER_BIT);

	/*set pen color to white and make width 1*/
	glLineWidth(1.0);
	glColor3f(1.0,1.0,1.0);
	
	//Push on Matrix
	glPushMatrix();
		//Set Eye and Zoom
		gluLookAt(.5,.5,2.8+zoom,.5,.5,.5,0,1,0);
		//Set Up Rotating with Mouse
		glTranslatef(.5,.5,.5);
		glRotatef(view_rotX,1,0,0);
		glRotatef(view_rotY,0,1,0);
		glRotatef(view_rotZ,0,0,1);
		glTranslatef(-.5,-.5,-.5);
		
		//DRAW THE OUTER CUBE
		glBegin(GL_LINE_LOOP);
			glVertex3f(1.0, 1.0, 1.0);
			glVertex3f(1.0, 0.0, 1.0);
			glVertex3f(0.0, 0.0, 1.0);
			glVertex3f(0.0, 1.0, 1.0); 
		glEnd();
		glBegin(GL_LINE_LOOP);
			glVertex3f(1.0, 1.0, 0.0);
			glVertex3f(1.0, 0.0, 0.0);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(0.0, 1.0, 0.0); 
		glEnd();
		glBegin(GL_LINE_LOOP);
			glVertex3f(0.0, 1.0, 1.0);
			glVertex3f(0.0, 0.0, 1.0);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(0.0, 1.0, 0.0); 
		glEnd();
		glBegin(GL_LINE_LOOP);
			glVertex3f(1.0, 1.0, 1.0);
			glVertex3f(1.0, 0.0, 1.0);
			glVertex3f(1.0, 0.0, 0.0);
			glVertex3f(1.0, 1.0, 0.0); 
		glEnd();
		
		//Set Width to 1.5 for Dipoles
		glLineWidth(1.5);
		
		//Draw THE DIPOLES
		int i,j,k;
		for(i=0; i<mySize[0]; i++)
		{
			for(j=0; j<mySize[1]; j++)
			{
				for(k=0; k<mySize[2]; k++)
				{
					myDipole = myLat->getDipole(i,j,k);
					glColor3d((myDipole->getU()+1)/2,(myDipole->getV()+1)/2,(myDipole->getW()+1)/2);
					//glColor3f(i/10.0,j/10.0,k/10.0);
					//glColor3f(0,1,0);
					glBegin(GL_LINES);
						glVertex3d((double)i/mySize[0]+ .5/mySize[0],
								   (double)j/mySize[1]+ .5/mySize[1],
								   (double)k/mySize[2]+ .5/mySize[2]);
						glVertex3d((double)i/mySize[0]+ .5/mySize[0]+(myDipole->getU())*.5/mySize[0],
								   (double)j/mySize[1]+ .5/mySize[0]+(myDipole->getV())*.5/mySize[1],
								   (double)k/mySize[2]+ .5/mySize[0]+(myDipole->getW())*.5/mySize[2]);
					glEnd();
				}
			}
		}

	//Pop the Matrix OFF
	glPopMatrix();
	
	glutSwapBuffers();  //SWAP BUFFERS
}

//Function for SpecialKeys (aka the ZOOM)
void my_specialkey(int key, int x, int y)
{
	if(key==GLUT_KEY_UP)   //Pushing up key changes the zoom  (zoom in)
	{
		zoom-=.1;
	}
	if(key==GLUT_KEY_DOWN) //Pushing down key changes the zoom (zoom out)
	{
		zoom+=.1;
	}
	
	glutPostRedisplay();  //REDISPLAY
}

//Functions for Rotating Mouse
void mouse(int button, int state, int x, int y) {
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
	{
		left_button_down = TRUE;
		previous_mouseX = x;
		previous_mouseY = y;
	}
	else if (state == GLUT_UP)
	{
		left_button_down = FALSE;
	}
	glutPostRedisplay();
}
void mouse_dragged(int x, int y)
{
	double thetaY, thetaX;
	if (left_button_down)
	{
		thetaY = 360.0 * (x-previous_mouseX)/WIDTH;
		thetaX = 360.0 * (y-previous_mouseY)/HEIGHT; 
		previous_mouseX = x;
		previous_mouseY = y;
		view_rotX += thetaX;
		view_rotY += thetaY;    
	}
	glutPostRedisplay();
}

//Fuction for Keyboard (basically exiting)
void Key(unsigned char key, int x, int y)
{
	/* 
	press escape or q to quit
	press 'c' to clear screen
	*/
	switch(key)
	{
		case 'q':
			exit(0);
			break;
		default:
			return;            
	}     
}

void my_idle()
{
	for(int m=0; m < 1;m++)
	{
		xt=rand()%mySize[0];
		yt=rand()%mySize[1];
		zt=rand()%mySize[2];
		
		energyBefore = getEnergy(xt,yt,zt);
		
		phiTemp = rand()%61 - 30;
		thetaTemp = rand()%61 - 30;
		psiTemp = rand()%61 - 30;
		createRot(phiTemp,thetaTemp,psiTemp);
		tempDipole = myLat->getDipole(xt,yt,zt);
		ut = A1->dot(tempDipole);
		vt = A2->dot(tempDipole);
		wt = A3->dot(tempDipole);
		
		energyAfter = getEnergy(xt,yt,zt,ut,vt,wt);
		
		if(energyAfter < energyBefore)
			tempDipole->setDipole(ut,vt,wt);
		
		/*if(( time(NULL) - myTime)< 1)
			frameCount++;
		  else
		  {
			cout<<frameCount<<endl;
			frameCount = 0;
			myTime=time(NULL);
		  }*/
	}
		
	glutPostRedisplay();  //REDISPLAY
}


/*This is my timer function used for animation*/
void my_timer(int v)
{
	xt=rand()%mySize[0];
	yt=rand()%mySize[1];
	zt=rand()%mySize[2];
	
	energyBefore = getEnergy(xt,yt,zt);
	
	phiTemp = rand()%61 - 30;
	thetaTemp = rand()%61 - 30;
	psiTemp = rand()%61 - 30;
	createRot(phiTemp,thetaTemp,psiTemp);
	tempDipole = myLat->getDipole(xt,yt,zt);
	ut = A1->dot(tempDipole);
	vt = A2->dot(tempDipole);
	wt = A3->dot(tempDipole);
	
	energyAfter = getEnergy(xt,yt,zt,ut,vt,wt);
	
	if(energyAfter < energyBefore)
		tempDipole->setDipole(ut,vt,wt);
	
	glutPostRedisplay();  //REDISPLAY
	glutTimerFunc(1000/v,my_timer,v);   //RECURSIVE CALL TO THE TIMER  (called 30 times per second)
}

//My Setup Function
void init(void)
{
	/*set clearing (background) color*/
	glClearColor(0.0, 0.0, 0.0, 0.0);

	/*initialize viewing values*/
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-.1,.1,-.1,.1,.1,10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//My MAIN FUNCTION YEAH!!!
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); //Double Buffered
	glutInitWindowPosition(100,100);			 //Set Initial Postion
	glutInitWindowSize(WIDTH,HEIGHT);			 //Set Width and Height
	glutCreateWindow("Pretty Picture");			 //Name at top of Window
	init();										 //Call init fuction
	glutDisplayFunc(display);					 //Set Display Function
	glutMouseFunc(mouse);						 //Set Mouse Functions
	glutMotionFunc(mouse_dragged);
	glutKeyboardFunc(Key);						 //Set KeyBoard Functions
	glutSpecialFunc(my_specialkey);
	
	//glutTimerFunc(500, my_timer, 60);   //starts my timer for animation!!!!!!!
	glutIdleFunc(my_idle); 
	
	//Enter OpenGL loop
	glutMainLoop();
}