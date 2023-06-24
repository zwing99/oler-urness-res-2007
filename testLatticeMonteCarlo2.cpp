/*
 *  testLattice.cpp
 *  
 *
 *  Created by Zac Oler on 6/4/07.
 *  Copyright 2007 ZWING INC. All rights reserved.
 *
 */

//MONTE CARLO 2

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
lattice* myLat1 = new lattice();
int* mySize = myLat1->getSize();
lattice* myLat2 = new lattice();
int latNum = 1;
lattice* currentLat = myLat1;
lattice* writeLat = myLat2;

float** myColor = new float*[mySize[0]*mySize[1]*mySize[2]];

//Temporary Dipole Storage
dipole* myDipole;

//Magnetic Field
vec* magneticField = new vec(0,0,0);

//Temp Variables
double ut,vt,wt,ubest,vbest,wbest;
int xt,yt,zt;
vec* tempVec;
dipole* tempDipole;
double totalEnergy;
double maxVecMag = 0;

//Matrix Vectors
vec* A1 = new vec();
vec* A2 = new vec();
vec* A3 = new vec();
double sinphi, cosphi, sintheta, costheta, sinpsi, cospsi;
double phiTemp, thetaTemp, psiTemp;
double energyBefore, energyAfter, energyBest;


//fstream variables
ofstream outputEnergy("TotalEnergy.txt", ios::out);
ofstream angleOut("CheckAngles.txt", ios::out);
long counter = 0;

void createRot(double phi, double theta, double psi)
{
	//phi = phi*PI/180.0;
	//theta = theta*PI/180.0;
	//psi = psi*PI/180.0;
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
	dipole* dipxyz = currentLat->getDipole(x,y,z);
	H-=6*dipxyz->dot(magneticField);
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
	
	return H;
}

double getEnergy(int x, int y, int z, double u, double v, double w)
{
	double H = 0;
	dipole* dipxyz = new dipole(u,v,w);
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
	
	delete dipxyz;
	
	return H;
}

void colorFunc(double change, float* array)
{
	change = change/ 0.022;
	change = 1.0 - change;
	delete [] array;
	array = new float[3];
	array[0] = 1.0;
	array[1] = (float) change;
	array[2] = (float) change;
	
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
		glBegin(GL_LINES);
		for(i=0; i<mySize[0]; i++)
		{
			for(j=0; j<mySize[1]; j++)
			{
				for(k=0; k<mySize[2]; k++)
				{
					myDipole = currentLat->getDipole(i,j,k);
					//glColor3fv(myColor[i*mySize[1]*mySize[2]+j*mySize[2]+k]);
					glColor3d((myDipole->getU()+1)/2,(myDipole->getV()+1)/2,(myDipole->getW()+1)/2);
					//glColor3f(i/10.0,j/10.0,k/10.0);
					//glColor3f(0,1,0);
						glVertex3d((double)i/mySize[0]+ .5/mySize[0],
								   (double)j/mySize[1]+ .5/mySize[1],
								   (double)k/mySize[2]+ .5/mySize[2]);
						glVertex3d((double)i/mySize[0]+ .5/mySize[0]+(myDipole->getU())*.5/mySize[0],
								   (double)j/mySize[1]+ .5/mySize[1]+(myDipole->getV())*.5/mySize[1],
								   (double)k/mySize[2]+ .5/mySize[2]+(myDipole->getW())*.5/mySize[2]);
					
				}
			}
		}
		glEnd();

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

/*this function iterates the system*/
void iterate()
{
	for(int m = 0; m<1; m++)
	{
		int i,j,k;
		for(i=0; i<mySize[0]; i++)
		{
			for(j=0; j<mySize[1]; j++)
			{
				for(k=0; k<mySize[2]; k++)
				{
					tempDipole = currentLat->getDipole(i,j,k);
					
					energyBefore = getEnergy(i,j,k);
					energyBest = 1000000;
					
					int numTimes = 0;
					
					int a,b;
					if(counter >1500)
					{
						a=10; b=1;
					}
					else if(counter>1000)
					{
						a = 5; b = 2;
					}
					else if(counter>500)
					{
						a = 2; b = 60;
					}
					else if(counter>=0)
					{
						a = 1; b = 90;
						//cout<<a<<b<<endl;
					}
					
					
					
					while (numTimes< 10)
					{
						//if(rand()%10 == 3)
						//	srand(time(NULL));
						
						phiTemp = (PI*rand()/RAND_MAX-PI/2);
						thetaTemp = (PI*rand()/(double)RAND_MAX-PI/2);
						psiTemp = (PI*rand()/(double)RAND_MAX-PI/2);
						
						if(counter%50 == 0)
							angleOut<<phiTemp<<" "<<thetaTemp<<" "<<psiTemp<<endl;
						
						
						createRot(phiTemp,thetaTemp,psiTemp);
						ut = A1->dot(tempDipole);
						vt = A2->dot(tempDipole);
						wt = A3->dot(tempDipole);
						
						energyAfter = getEnergy(i,j,k,ut,vt,wt);
						numTimes++;
						
						if(energyAfter<energyBest)
						{
							energyBest = energyAfter;
							ubest = ut; vbest = vt; wbest = wt;
						}
					}
					
					if ( energyBest < energyBefore)
					{
						dipole* tempDipole2 = writeLat->getDipole(i,j,k);			
						tempDipole2->setDipole(ubest,vbest,wbest);
					}
									
																	
					//if(tempVec->magnitude() > maxVecMag)
					//	maxVecMag = tempVec->magnitude();
					//colorFunc(tempVec->magnitude(),
					//		  myColor[i*mySize[1]*mySize[2]+j*mySize[2]+k]);
					
				}
			}
		}
		
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
		
		for(i=0; i<mySize[0]; i++)
		{
			for(j=0; j<mySize[1]; j++)
			{
				for(k=0; k<mySize[2]; k++)
				{
					totalEnergy += getEnergy(i,j,k);
				}
			}
		}
		//if(counter>=500)
		//	exit(0);
		outputEnergy<<(++counter)<<" "<<totalEnergy<<endl;
		totalEnergy = 0;
		cout<<counter<<endl;
		if(rand()%20 == 7)
			srand(time(NULL));
		 
	}	
}

/*Idle Function used for animation*/
void my_idle()
{
	iterate();	
	glutPostRedisplay();  //REDISPLAY
}


/*This is my timer function used for animation*/
void my_timer(int v)
{
	iterate();
	glutPostRedisplay();  //REDISPLAY
	glutTimerFunc(1000/v,my_timer,v);   //RECURSIVE CALL TO THE TIMER  (called 30 times per second)
}

//My Setup Function
void init(void)
{
	srand(time(NULL));
	/*set clearing (background) color*/
	glClearColor(0.0, 0.0, 0.0, 0.0);

	/*initialize viewing values*/
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-.1,.1,-.1,.1,.1,10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	for(int i = 0; i < mySize[0]*mySize[1]*mySize[2]; i++)
	{
		myColor[i] = new float[3];
	}

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
	
	glutTimerFunc(500, my_timer, 60);   //starts my timer for animation!!!!!!!
	//glutIdleFunc(my_idle); 
	
	//Enter OpenGL loop
	glutMainLoop();
}