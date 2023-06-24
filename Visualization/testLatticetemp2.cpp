/*
 *  testLattice.cpp
 *  
 *
 *  Created by Zac Oler on 6/4/07.
 *  Copyright 2007 ZWING INC. All rights reserved.
 *
 */

//Inclue Lattice Stuff
#include "StreamSurface.h"

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
float magX = 0.5;
float magY = 0;
float magZ = 0;
vec* magneticField = new vec(magX,magY,magZ);

//Temp Variables
dipole* myDipole;
vec* tempVec;
dipole* tempDipole;
float totalEnergy;
double maxVecMag = 0;

//Display and Iterate Options
float dipole_alpha =1.0;
int framespersecond = 10;
int itersperframe = 1;
int rendervolume = TRUE;
int renderdipoles = TRUE;
int gotimer = FALSE;
int dipolecoloring = TRUE;

//Variable for Temperature
float temperature =  0.06;
float dtheta = sqrt(temperature);
float realtemperature=temperature;
float realdtheta=dtheta;
vec R1, R2, R3;

//fstream variables
ofstream outputData("Data.txt", ios::out);

//Texture Variables and Other Variables for VolumeRenderer
GLubyte image[sizex][sizey][sizez][4];
static GLuint texName;
PFNGLTEXIMAGE3DPROC glTexImage3D;
int r=0,g=255,b=0;
int pickvrcolor = 1;
float mv[16];

//GLUI VARIABLES
int mainwindow_id;
GLUI* glui;
GLUI_Rotation* rotation_var;
GLUI_Panel* motion_panel;
GLUI_Rollout* displayoptions_panel;
GLUI_Rollout* vroptionspanel;
GLUI_Rollout* pickacolorpanel;
GLUI_Rollout* update_panel;
GLUI_Rollout* timer_panel;
GLUI_Rollout* magfield_panel;
GLUI_Rollout* temp_panel;
GLUI_Translation* xytrans_var;
GLUI_Translation* ztrans_var;
GLUI_Spinner* dipaspin;
GLUI_Spinner* fpsspin;
GLUI_Spinner* ipfspin;
GLUI_Spinner* radiusspin;
GLUI_Spinner* tempspin;
GLUI_Spinner* xspin;
GLUI_Spinner* yspin;
GLUI_Spinner* zspin;
GLUI_Spinner* rspin;
GLUI_Spinner* gspin;
GLUI_Spinner* bspin;
GLUI_EditText* angletext;
GLUI_EditText* realangletext;
GLUI_EditText* realtemptext;
GLUI_EditText* energytext;
GLUI_EditText* countertext;
GLUI_RadioGroup* vrcolorradio;


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

//Function to Draw the Dipoles
//Only Draws Ten Dipoles (It uses the getInterpolatedVector function which gives a normalized vector)
void drawDipoles()
{
	int i,j,k;
	glBegin(GL_LINES);
	for(i=0; i<10; i+=1)
	{
		for(j=0; j<10; j+=1)
		{
			for(k=0; k<10; k+=1)
			{
				vec myVec = getInterpolatedVector(point((float)i/10*sizex,(float)j/10*sizey,(float)k/10*sizez));
				if(dipolecoloring)
					glColor4d((myVec.getU()+1)/2,(myVec.getV()+1)/2,(myVec.getW()+1)/2,dipole_alpha);
				else
					glColor4d(1,1,1,dipole_alpha);
				glVertex3d((double)i*0.1+ .05,
						   (double)j*0.1+ .05,
						   (double)k*0.1+ .05);
				glVertex3d((double)i*0.1+ .05+(myVec.getU())*.05,
						   (double)j*0.1+ .05+(myVec.getV())*.05,
						   (double)k*0.1+ .05+(myVec.getW())*.05);
				
			}
		}
	}
	glEnd();
	
}


//My Display Function
void display(void)
{
	glui->sync_live(); //Sync data with glui
	
	/*clear all pixels*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	
	//Push on Matrix
	glPushMatrix();
		//Set Eye
		gluLookAt(.5,.5,3,.5,.5,0,0,1,0);
		//Set Zoom and Translate
		glTranslatef(0,0,ztrans_var->get_z());
		glTranslatef(-1*xytrans_var->get_x(),-1*xytrans_var->get_y(),0);		
		//Set Rotation about center of cube
		glTranslatef(.5,.5,.5);
		rotation_var->get_float_array_val( mv );
		glMultMatrixf(mv);
		glTranslatef(-.5,-.5,-.5);		
		
		//Draw Colored Axis
		glLineWidth(3.0);
		glBegin(GL_LINES);
			glColor3f(1.0,0,0);
			glVertex3f(0,0,0);
			glVertex3f(1,0,0);
			glColor3f(0,1.0,0);
			glVertex3f(0,0,0);
			glVertex3f(0,1,0);
			glColor3f(0,0,1.0);
			glVertex3f(0,0,0);
			glVertex3f(0,0,1);
		glEnd();
		//DRAW THE OUTER CUBE
		glLineWidth(1.0);
		glColor3f(1.0,1.0,1.0);
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
		
		
		//Render Dipoles
		glLineWidth(1.5);		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		if(renderdipoles)
			drawDipoles();
		
		//Render Volume
		if(rendervolume)
		{
			double mv2[16];
			glGetDoublev(GL_MODELVIEW_MATRIX, mv2);
			
			glEnable(GL_TEXTURE_3D_EXT);
			renderVolume(.5,mv2);
			
			//This fixs errors i found
			glDisable(GL_TEXTURE_3D_EXT);
			glDisable(GL_TEXTURE_3D);
			glDisable(GL_LIGHTING);
		}
		
		
	//Pop the Matrix OFF
	glPopMatrix();
	
	glutSwapBuffers();  //SWAP BUFFERS
}

/*this function iterates the system*/
void iterate()
{
	dtheta = sqrt((double)temperature); //Calculate Angle
	
	//Part 0 of for loop rotates the dipoles radomnly (aka simulation of temperature)
	//Part 1 of for loop moves the vectors to minimize energy
	for(int part = 0; part<2; part++)
	{
		//Global vars for part 0)
		float mov = 0, finalavg = 0, avgmin = 1000, avgmax = -1000;
		float avgstore[sizex*sizey*sizez];
		
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
						float radiusOfDist = radiusspin->get_float_val();
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
	int i,j,k;
	
	if(!pickvrcolor)
	{
		//Get Color of VolumeRendered based on alignment with X-dirrection
		for(i=0; i<sizex; i++)
		{
			for(j=0; j<sizey; j++)
			{
				for(k=0; k<sizez; k++)
				{
					tempDipole = currentLat->getDipole(i,j,k);
					u_component += tempDipole->getU();
				}
			}
		}
		u_component/=sizex*sizey*sizez;
		u_component+=1; u_component/=2;
		r = 0;
		g = (int)((1-u_component)*150);
		b = (int)(u_component*150);
	}
	
	
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
				int a = (int)(3.0*(energyat+getMaxEnergy(i,j,k)));
				image[k][j][i][0] = (GLubyte)r;
				image[k][j][i][1] = (GLubyte)g;
				image[k][j][i][2] = (GLubyte)b;
				image[k][j][i][3] = (GLubyte)a;
			}
		}
	}
	
	
	if(counter == 1)
	{
		outputData<<"#Iteration Energy MinEnergy Delta"<<endl;
	}
	outputData<<counter<<" "<<totalEnergy<<" "<<minEnergy<<" "<<totalEnergy-minEnergy<<" "
	          <<realtemperature<<" "<<magX<<" "<<magY<<" "<<magZ<<endl;
}

//This is my timer function used for animation
void my_timer(int v)
{
	glui->sync_live();
	magneticField->setVec(magX,magY,magZ);
	if(v == 1)
		gotimer = !gotimer;
	for(int i = 0; i < itersperframe; i++)
		iterate();
	if(rendervolume)
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, sizex, sizey, sizez, 0 , GL_RGBA, GL_UNSIGNED_BYTE, image);
	glutPostRedisplay();  //REDISPLAY
	if(gotimer)
		glutTimerFunc(1000/framespersecond,my_timer,0);   
}

//Called when Iterate or Quit Buttons are pushed
void my_button(int buttonid)
{
	glui->sync_live();
	magneticField->setVec(magX,magY,magZ);
	switch(buttonid)
	{
		case 1:
			exit(0);
			break;	
					
		case 2:
			iterate();
			if(rendervolume)
				glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, sizex, sizey, sizez, 0 , GL_RGBA, GL_UNSIGNED_BYTE, image);
			glutPostRedisplay();
			break;
			
		case 3:
			for(int i = 0; i < 10; i++)
				iterate();
			if(rendervolume)
				glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, sizex, sizey, sizez, 0 , GL_RGBA, GL_UNSIGNED_BYTE, image);
			glutPostRedisplay();
			break;
		default:
			return;
	}
}

//My Setup Function
void init(void)
{
	/*set clearing (background) color*/
	glClearColor(0.1, 0.1, 0.1, 0.0);

	/*initialize viewing values*/
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,1,.01,10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	//Setup Depth Testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);
	
	//Setup 3D Textures
	glTexImage3D = (PFNGLTEXIMAGE3DPROC) glutGetProcAddress("glTexImage3D");
	
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_3D, texName);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//Setup Blending Fuctions
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	
	scaleValue = .02;
	
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, sizex, sizey, sizez, 0 , GL_RGBA, GL_UNSIGNED_BYTE, image);
	
}

//My MAIN FUNCTION YEAH!!!
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA | GLUT_DEPTH); //Double Buffered
	glutInitWindowPosition(100,100);			 //Set Initial Postion
	glutInitWindowSize(WIDTH,HEIGHT);			 //Set Width and Height
	mainwindow_id = glutCreateWindow("Pretty Picture");			 //Name at top of Window
	init();										 //Call init fuction
	glutDisplayFunc(display);					 //Set Display Function
	
	glutInitWindowPosition(105+WIDTH,150);
	glui = GLUI_Master.create_glui("Controls");
	
	//Motion Rollout Panel
	motion_panel = glui->add_panel( "Movement" );
	rotation_var = glui->add_rotation_to_panel(motion_panel, "Rotate");
	glui->add_column_to_panel(motion_panel,false);
	ztrans_var = glui->add_translation_to_panel(motion_panel,"Zoom",GLUI_TRANSLATION_Z);
	ztrans_var->set_speed(.02);
	glui->add_column_to_panel(motion_panel,false);
	xytrans_var = glui->add_translation_to_panel(motion_panel,"Translate",GLUI_TRANSLATION_XY);
	xytrans_var->set_speed(.02);
	
	//Display Options Panel
	displayoptions_panel = glui->add_rollout("Display Options");
	glui->add_checkbox_to_panel(displayoptions_panel,"Dipoles",&renderdipoles);
	dipaspin = glui->add_spinner_to_panel(displayoptions_panel,"Alpha of Dipoles:",GLUI_SPINNER_FLOAT,&dipole_alpha);
	//dipaspin->set_speed(.05);
	dipaspin->set_float_limits(.05,1.00);
	glui->add_checkbox_to_panel(displayoptions_panel,"ColorDipoles",&dipolecoloring);
	glui->add_checkbox_to_panel(displayoptions_panel,"Volume",&rendervolume);
	vroptionspanel = glui->add_rollout_to_panel(displayoptions_panel,"Volume Options");
	vroptionspanel->close();
	vrcolorradio = glui->add_radiogroup_to_panel(vroptionspanel,&pickvrcolor);
	glui->add_radiobutton_to_group(vrcolorradio,"Alignment with Xaxis");
	glui->add_radiobutton_to_group(vrcolorradio,"Pick a color");
	pickacolorpanel = glui->add_rollout_to_panel(vroptionspanel,"Specify Color");
	pickacolorpanel->close();
	rspin = glui->add_spinner_to_panel(pickacolorpanel,"r:",GLUI_SPINNER_INT,&r);
	rspin->set_int_limits(0,255);
	rspin->set_speed(2);
	gspin = glui->add_spinner_to_panel(pickacolorpanel,"g:",GLUI_SPINNER_INT,&g);
	gspin->set_int_limits(0,255);
	gspin->set_speed(2);
	bspin = glui->add_spinner_to_panel(pickacolorpanel,"b:",GLUI_SPINNER_INT,&b);
	bspin->set_int_limits(0,255);
	bspin->set_speed(2);
	
	//Timer Rollout Panel
	timer_panel = glui->add_rollout("Timer Options");
	glui->add_button_to_panel(timer_panel,"Start/Stop",1,my_timer);
	fpsspin = glui->add_spinner_to_panel(timer_panel,"FPS:",GLUI_SPINNER_INT,&framespersecond);
	fpsspin->set_int_limits(1,60);
	fpsspin->set_speed(.3);
	ipfspin = glui->add_spinner_to_panel(timer_panel,"IPF:",GLUI_SPINNER_INT,&itersperframe);
	ipfspin->set_int_limits(1,10);
	ipfspin->set_speed(.1);
	
	//Update Options Panel
	update_panel = glui->add_rollout("Update Options");
	glui->add_button_to_panel(update_panel, "Iterate", 2 , my_button);
	glui->add_button_to_panel(update_panel, "Iterate 10x", 3 , my_button);
	
	//Add Column
	glui->add_column(false);
	
	//Magnetic Field Rollout Panel
	magfield_panel = glui->add_rollout("MagneticField");
	xspin = glui->add_spinner_to_panel(magfield_panel,"U:",GLUI_SPINNER_FLOAT,&magX);
	xspin->set_float_limits(-1.0,7.0);
	xspin->set_speed(.25);
	yspin = glui->add_spinner_to_panel(magfield_panel,"V:",GLUI_SPINNER_FLOAT,&magY);
	yspin->set_float_limits(-1.0,1.0);
	zspin = glui->add_spinner_to_panel(magfield_panel,"W:",GLUI_SPINNER_FLOAT,&magZ);
	zspin->set_float_limits(-1.0,1.0);
	
	//Temperature Control Panel
	temp_panel = glui->add_rollout("Temperature");
	tempspin = glui->add_spinner_to_panel(temp_panel,"temp:",GLUI_SPINNER_FLOAT,&temperature);
	tempspin->set_float_limits(0,.4);
	tempspin->set_speed(.1);
	realtemptext = glui->add_edittext_to_panel(temp_panel,"realtemp:",GLUI_EDITTEXT_FLOAT,&realtemperature);
	realtemptext->disable();
	angletext = glui->add_edittext_to_panel(temp_panel,"angle:",GLUI_EDITTEXT_FLOAT,&dtheta);
	angletext->disable();
	realangletext = glui->add_edittext_to_panel(temp_panel,"realangle:",GLUI_EDITTEXT_FLOAT,&realdtheta);
	realangletext->disable();
	radiusspin = glui->add_spinner_to_panel(temp_panel,"radius:",GLUI_SPINNER_FLOAT);
	radiusspin->set_float_limits(0.0001,.1);
	radiusspin->set_speed(.25);
	radiusspin->set_float_val(.025);
	
	//Energy and Quit
	energytext = glui->add_edittext("TotalEnergy:",GLUI_EDITTEXT_FLOAT,&totalEnergy);
	energytext->set_w(200);
	energytext->disable();
	countertext = glui->add_edittext("Iter:",GLUI_EDITTEXT_INT,&counter);
	countertext->disable();
	glui->add_button( "Quit", 1 , my_button);
	
	glui->set_main_gfx_window( mainwindow_id );
	GLUI_Master.set_glutIdleFunc( NULL );
	
	//Enter OpenGL loop
	glutMainLoop();
}