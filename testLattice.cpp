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
#include "Point.h"
#include "StreamSurface.h"

//Define Size
#define WIDTH 600
#define HEIGHT 600
//Define PI
#define PI 3.14159265

//START ASSUMING LEFT BUTTON IS DOWN
int left_button_down = FALSE;
int right_button_down = FALSE;

//VARIALBES FOR ROTATION
int previous_mouseX;
int previous_mouseY;
double view_rotX;
double view_rotY;
double view_rotZ;

//Zoom Variable
float zoom;
float xmov =0;
float ymov =0;

//Lattice Variables
lattice* myLat1 = new lattice(10,10,10);
int* mySize = myLat1->getSize();
lattice* myLat2 = new lattice(10,10,10);
int latNum = 1;
lattice* currentLat = myLat1;
lattice* writeLat = myLat2;

float** myColor = new float*[mySize[0]*mySize[1]*mySize[2]];

//Temporary Dipole Storage
dipole* myDipole;

//Magnetic Field
vec* magneticField = new vec(0,0,0);

//Temp Variables
double ut,vt,wt;
int xt,yt,zt;
vec* tempVec;
dipole* tempDipole;
double totalEnergy;
double maxVecMag = 0;

//StreamSurfaceVariable
StreamSurface* mySurface;

float eyepos[] =  {.5,.5,2};

double scaleValue;

GLfloat white_color[] = {1.0,1.0,1.0,1.0};
GLfloat surface_color[] = {1,0,0,1};//{122.0/255,20.0/255,201.0/255,1.0};

//fstream variables
ofstream outputEnergy("TotalEnergy.txt", ios::out);
ofstream outputFieldU("u.dat",ios::out | ios::binary);
ofstream outputFieldV("v.dat",ios::out | ios::binary);
ofstream outputFieldW("w.dat",ios::out | ios::binary);
ofstream outputEnergyField("energy.dat",ios::out | ios::binary);
long counter = 0;

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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*set pen color to white and make width 1*/
	glLineWidth(1.0);
	glColor3f(1.0,1.0,1.0);
	
	//Push on Matrix
	glPushMatrix();
		//Set Eye and Zoom
		//gluLookAt(0,0,1,0,0,0,0,1,0);
		gluLookAt(eyepos[0],eyepos[1],eyepos[2],
		      eyepos[0]+sin(view_rotY/180.0*M_PI),eyepos[1]-sin(view_rotX/180.0*M_PI),eyepos[2]-cos(view_rotY/180.0*M_PI),
		      0,1,0);
		//Set Up Rotating with Mouse
		//glRotatef(view_rotX,1,0,0);
		//glRotatef(view_rotY,0,1,0);
		//glRotatef(view_rotZ,0,0,1);
		//glTranslatef(.5,5,3);
		//glTranslatef(-.5-xmov,-.5-ymov,-2+zoom);
		
		
		
		
		
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
		
		
		
		
		glPushMatrix();
			glScalef(1.0/mySize[0],1.0/mySize[1],1.0/mySize[2]);
			glColor3f(1,0,0);
			for(int n = 0; n < mySurface->getNumOfStreams(); n++)
				mySurface->drawStream(n);
			
			glEnable(GL_LIGHTING);
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, surface_color);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, surface_color);
			//glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white_color);
			//glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0);			
			
			for(int n=0; n < mySurface->getNumOfSurfaces(); n++)
				mySurface->drawSurface(n);
			glDisable(GL_LIGHTING);
		glPopMatrix();
		
		glEnable(GL_LIGHTING);
		glPushMatrix();
		glTranslatef(.5,.5,.5);
		glutSolidSphere(.25,40,40);
		glPopMatrix();
		glDisable(GL_LIGHTING);
		
		//Draw THE DIPOLES
		/*
		int i,j,k;
		glBegin(GL_LINES);
		for(i=0; i<mySize[0]; i+=1)
		{
			for(j=0; j<mySize[1]; j+=1)
			{
				for(k=0; k<mySize[2]; k+=1)
				{
					myDipole = currentLat->getDipole(i,j,k);
					//glColor3fv(myColor[i*mySize[1]*mySize[2]+j*mySize[2]+k]);
					glColor4d((myDipole->getU()+1)/2,(myDipole->getV()+1)/2,(myDipole->getW()+1)/2,.2);
					//glColor3f(i/10.0,j/10.0,k/10.0);
					//glColor3f(0,1,0);
						glVertex3d((double)i/mySize[0]+ .5/mySize[0],
								   (double)j/mySize[1]+ .5/mySize[1],
								   (double)k/mySize[2]+ .5/mySize[2]);
						glVertex3d((double)i/mySize[0]+ .5/mySize[0]+(myDipole->getU())*.5/mySize[0],
								   (double)j/mySize[1]+ .5/mySize[0]+(myDipole->getV())*.5/mySize[1],
								   (double)k/mySize[2]+ .5/mySize[0]+(myDipole->getW())*.5/mySize[2]);
					
				}
			}
		}
		glEnd();
		*/
	//Pop the Matrix OFF
	glPopMatrix();
	
	glutSwapBuffers();  //SWAP BUFFERS
}

//Function for SpecialKeys (aka the ZOOM)
void my_specialkey(int key, int x, int y)
{
	if(key==GLUT_KEY_UP)   //Pushing up key changes the zoom  (zoom in)
	{
		eyepos[0]+=sin(view_rotY/180.0*M_PI)*.05;
		eyepos[2]-=cos(view_rotY/180.0*M_PI)*.05;
		eyepos[1]-=sin(view_rotX/180.0*M_PI)*.05;
	}
	if(key==GLUT_KEY_DOWN) //Pushing down key changes the zoom (zoom out)
	{
		eyepos[0]-=sin(view_rotY/180.0*M_PI)*.05;
		eyepos[2]+=cos(view_rotY/180.0*M_PI)*.05;
		eyepos[1]+=sin(view_rotX/180.0*M_PI)*.05;
	}
	if(key==GLUT_KEY_LEFT)
	{
		eyepos[0]-=cos(view_rotY/180.0*M_PI)*.05;
		eyepos[2]-=sin(view_rotY/180.0*M_PI)*.05;
		eyepos[1]+=sin(view_rotX/180.0*M_PI)*.05;
	}
	if(key==GLUT_KEY_RIGHT)
	{
		eyepos[0]+=cos(view_rotY/180.0*M_PI)*.05;
		eyepos[2]+=sin(view_rotY/180.0*M_PI)*.05;
		eyepos[1]-=sin(view_rotX/180.0*M_PI)*.05;
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
	if (state == GLUT_DOWN && button == GLUT_RIGHT_BUTTON)
	{
		right_button_down = TRUE;
		previous_mouseX = x;
		previous_mouseY = y;
	}
	else if (state == GLUT_UP)
	{
		left_button_down = FALSE;
		right_button_down = FALSE;
	}
	glutPostRedisplay();
}
void mouse_dragged(int x, int y)
{
	double thetaY, thetaX;
	double xchange,ychange;
	if (left_button_down)
	{
		thetaY = 360.0 * (x-previous_mouseX)/WIDTH;
		thetaX = 360.0 * (y-previous_mouseY)/HEIGHT; 
		previous_mouseX = x;
		previous_mouseY = y;
		view_rotX += thetaX;
		view_rotY += thetaY;    
	}
	if (right_button_down)
	{
		xchange = (x-previous_mouseX);
		ychange = (previous_mouseY-y);
		previous_mouseX = x;
		previous_mouseY = y;
		xmov += (0.001)* xchange;
		ymov += (0.001)* ychange;    
	}
	glutPostRedisplay();
}

/*this function iterates the system*/
void iterate()
{
	for(int m = 0; m<2; m++)
	{
		int i,j,k;
		for(i=0; i<mySize[0]; i++)
		{
			for(j=0; j<mySize[1]; j++)
			{
				for(k=0; k<mySize[2]; k++)
				{
					dipole* dipxyz = currentLat->getDipole(i,j,k);
					
					tempVec = new vec();					
					
					if(magneticField->magnitude() != 0)
					{
							tempVec->add(magneticField);
					}						
					
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
					
					
					tempVec->scale((1.0/(tempVec->magnitude())));
					tempVec->subtract(dipxyz);
					tempVec->scale(scaleValue);
						
					
					//if(tempVec->magnitude() > maxVecMag)
					//	maxVecMag = tempVec->magnitude();
					//colorFunc(tempVec->magnitude(),
					//		  myColor[i*mySize[1]*mySize[2]+j*mySize[2]+k]);
					
					tempVec->add(dipxyz);
					
					tempDipole = writeLat->getDipole(i,j,k);			
					
					tempDipole->setDipole(tempVec->getU(),
										  tempVec->getV(),
										  tempVec->getW());
					
					delete tempVec;								
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
		
		/*for(i=0; i<mySize[0]; i++)
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
		totalEnergy = 0;*/
		 
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
			int i,j,k;
			for(k=0; k<mySize[2]; k++)
			{
				for(j=0; j<mySize[1]; j++)
				{
					for(i=0; i<mySize[0]; i++)
					{
						dipole* dipxyz = currentLat->getDipole(i,j,k);
						float u = ((float)dipxyz->getU());
						float v = ((float)dipxyz->getV());
						float w = ((float)dipxyz->getW());
						float energyat = ((float)getEnergy(i,j,k));
						outputFieldU.write((char*)&u,sizeof(float));
						outputFieldV.write((char*)&v,sizeof(float));
						outputFieldW.write((char*)&w,sizeof(float));
						outputEnergyField.write((char*)&energyat,sizeof(float));
					}
				}
			}
			outputFieldU.close();
			outputFieldV.close();
			outputFieldW.close();							
			exit(0);
			break;	
					
		case 'i':
			iterate();
			mySurface->updateCurrentStream();
			glutPostRedisplay();
			break;
		
		case 's':
			mySurface->calculateNextSurface();
			mySurface->calculateNextStream();
			glutPostRedisplay();
			break;
		default:
			return;            
	}     
}

//My Setup Function
void init(void)
{
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	//glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
	//glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	
	GLfloat light_position[] = {100,100,100,0.0};
	GLfloat white_light[] = {.5,.5,.5,1.0};
	GLfloat ambient_light[] = {0.1,0.1,0.1, 1.0};
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
	//glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
	
	
	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_light);
	
	glEnable(GL_LIGHT0);
	
	/*set clearing (background) color*/
	glClearColor(0.0, 0.0, 0.0, 0.0);

	/*initialize viewing values*/
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glFrustum(-.1,.1,-.1,.1,.01,1.25);
	gluPerspective(45,1,.01,10);
	
	
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_DEPTH_TEST);
	
	//glShadeModel(GL_SMOOTH);
	
	

	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	
	//for(int i = 0; i < mySize[0]*mySize[1]*mySize[2]; i++)
	//{
	//	myColor[i] = new float[3];
	//}
	
	Point p0(5,5,5);
	scaleValue = .9;
	for(int i = 0; i < 4; i++)
		iterate();
	scaleValue = .01;
	mySurface = new StreamSurface(p0,currentLat);
	mySurface->calculateNextStream();
	for(int i = 0; i < 50; i++)
		iterate();
	mySurface->calculateNextStream();
	mySurface->calculateNextSurface();
	mySurface->calculateNextStream();
	
	
}

//My MAIN FUNCTION YEAH!!!
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA | GLUT_DEPTH); //Double Buffered
	glutInitWindowPosition(100,100);			 //Set Initial Postion
	glutInitWindowSize(WIDTH,HEIGHT);			 //Set Width and Height
	glutCreateWindow("Pretty Picture");			 //Name at top of Window
	init();										 //Call init fuction
	glutDisplayFunc(display);					 //Set Display Function
	glutMouseFunc(mouse);						 //Set Mouse Functions
	glutMotionFunc(mouse_dragged);
	glutKeyboardFunc(Key);						 //Set KeyBoard Functions
	glutSpecialFunc(my_specialkey);
	
	//glutTimerFunc(4000, my_timer, 30);   //starts my timer for animation!!!!!!!
	//glutIdleFunc(my_idle); 
	
	//Enter OpenGL loop
	glutMainLoop();
}