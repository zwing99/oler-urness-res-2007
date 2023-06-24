#include "VolumeRenderer.h"
#include <math.h>
#include <iostream>
#include <OpenGL/glext.h>
using namespace std;

//#define MAX(x, y) ((x) > (y) ? (x) : (y))
//#define MIN(x, y) ((x) < (y) ? (x) : (y))
//#define ABS(x) (((x) > 0) ? (x) : (-x))

/* just so I can remember how to use this
******** AFFINE(i,x,I,o,O)
**
** given intervals [i,I], [o,O] and a value x which may or may not be
** inside [i,I], return the value y such that y stands in the same
** relationship to [o,O] that x does with [i,I].  Or:
**
**    y - o	    x - i     
**   -------   =   -------
**    O - o	    I - i
**
** It is the callers responsibility to make sure I-i and O-o are 
** both greater than zero -glk
*/
#define AFFINE(i,x,I,o,O) ( ((double)(O)-(o))*((double)(x)-(i)) \
			    / ((double)(I)-(i)) + (o))

//--------------------------------------------------------------------------



//========================================================================
//------------------- Render some volumes --------------------------------
//========================================================================


//standard 3d view alligned sliceing
void renderVolume(float sampleRate, GLdouble mv[16])
{
		render3DVolumeEXTVA(sampleRate, mv, 0);
		return;
}

//------------------------------------------------------------------------
//-------------- View Aligned slices -------------------------------------
//------------------------------------------------------------------------

void render3DVolumeEXTVA(float sampleFrequency, 
					 GLdouble mv[16],
					 int v)
{
     float vo[8][3] = { //volume vertex edge coords
      {0,	 0,	   0},
      {sizef[0], 0,	   0},
      {0,	 sizef[1], 0},
      {sizef[0], sizef[1], 0},
      {0,	 0,	   sizef[2]},
      {sizef[0], 0,	   sizef[2]},
      {0,	 sizef[1], sizef[2]},
      {sizef[0], sizef[1], sizef[2]}};

   float tx[8][3] = { //volume texture coords/ edge to edge (bad for bricks)
      {0, 0, 0},
      {1, 0, 0},
      {0, 1, 0},
      {1, 1, 0},
      {0, 0, 1},
      {1, 0, 1},
      {0, 1, 1},
      {1, 1, 1}};

   float axis[3] = {0,0,1};

   render3DVA(sampleFrequency, mv, v, vo, tx, axis);
}


//------------------------------------------------------------------------
//------ Gineric render allong a vector ----------------------------------
//------------------------------------------------------------------------
void render3DVA(float sampleFrequency,
		GLdouble mv[16],
		int v,
		float vo[8][3],	  //volume vertex coords model-space coords
		float tx[8][3],	  //texture vertex coords tex-space coords
		float axis[3])	 //axis to slice along world-space coords
{

   float rv[8][3];     //the rotated volume (may include a scale)
   float maxval = -10; //(tmp)
   float minval = 10;
   int minvert = 0;
   GLdouble mvinv[16];

   inverseMatrix(mvinv, mv); //invert model view matrix

   for(int i=0; i<8; ++i){   //translate model to world coords (view space)
		 translateV3(rv[i], mv, vo[i]);
		 if(maxval < max(maxval, rv[i][2])){
			 maxval = max(maxval, rv[i][2]);
		 }
		 if(minval > min(minval, rv[i][2])){
			 minval = min(minval, rv[i][2]);
			 minvert = i;  //determine the starting point for slicing
		 }
   }
	 // fix this function so that the eye is the universal reference point for all volumes!
	 //  we will get artifacts at brick boundaries otherwise!
	

   //find the slice plane point 'sp' (initial) and the slice plane normal 'sn'
   //sp is the sliceing starting point, simply the vertex farthest from the eye
   float sp[3] = {vo[minvert][0], vo[minvert][1], vo[minvert][2]}; 
   //float vpn[3] = {0,0,1};  //view plane normal, points to eye (temp variable)
   float vpn[3];
   vpn[0] = axis[0]; vpn[1] = axis[1]; vpn[2] = axis[2];
   float sn[3];		    //slice plane normal
   translateV3(sn, mvinv, vpn); //move vpn to sn (model space);
   //note: sn & sp are defined in Model Space, ie the space where the volume
   // is alligned with the (x,y,z) axies
   float normsn = (float)sqrt((float)sn[0]*sn[0] + sn[1]*sn[1] + sn[2]*sn[2]); //normalize
   sn[0]/=normsn;
   sn[1]/=normsn;
   sn[2]/=normsn;
   
   //now find the distance we need to slice (|max_vertex - min_vertex|)
   float maxd[3] = {0, 0, maxval}; //(tmp) only use z-coord (view space)
   float mind[3] = {0, 0, minval}; //(tmp) ditto	    (view space)
   float maxv[3], minv[3];	   //(tmp)
   translateV3(maxv, mvinv, maxd); //translate back to model space
   translateV3(minv, mvinv, mind); //ditto
   maxv[0] -= minv[0]; //subtract
   maxv[1] -= minv[1];
   maxv[2] -= minv[2];

   //now take the norm of this vector... we have the distance to be sampled
   float dist = (float)sqrt(maxv[0]*maxv[0] + maxv[1]*maxv[1] + maxv[2]*maxv[2]);

   //draw a red bounding box
   //if(m_bbb) renderBBoxBrackets(sizef[0], sizef[1], sizef[2]);
	sizef[0] = 1; sizef[1] = 1; sizef[2]=1;
	size[0] = 100; size[1] = 100; size[2]=100;
   //renderBBox(sizef[0], sizef[1], sizef[2]);


   glDisable(GL_LIGHTING); //light makes it look bad!

   glDisable(GL_CULL_FACE);
   glPolygonMode(GL_FRONT, GL_FILL);
   glPolygonMode(GL_BACK, GL_FILL);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_TEXTURE_3D);


   glColor4f(1, 1, 1, 1);
   float dis = sizef[0] / (size[0] * sampleFrequency); 
   float del[3] = {sn[0]*dis, sn[1]*dis, sn[2]*dis};

   int samples = (int)((dist) / dis);//(total distance to be sampled)/(sample spacing)

   //samples /= 40;
   //del[0] *= 40;
   //del[1] *= 40;
   //del[2] *= 40;

   float poly[6][3];   // for edge intersections
   float tcoord[6][3]; // for texture intersections
   float tpoly[6][3];  // for transformed edge intersections
   int edges;	       // total number of edge intersections

   //highly un-optimized!!!!!!!!!
   for(int i = 0 ; i < samples; ++i){ //for each slice
      //increment the slice plane point by the slice distance
      sp[0] += del[0];
      sp[1] += del[1];
      sp[2] += del[2];

      edges = 0; 
      //now check each edge of the volume for intersection with.. 
      //the plane defined by sp & sn
      //front bottom edge
      edges += intersect(vo[0], vo[1], tx[0], tx[1], rv[0], rv[1], sp, sn, 
			 poly[edges], tcoord[edges], tpoly[edges]);
      //front left edge
      edges += intersect(vo[0], vo[2], tx[0], tx[2], rv[0], rv[2], sp, sn,
			 poly[edges], tcoord[edges], tpoly[edges]);
      //front right edge
      edges += intersect(vo[1], vo[3], tx[1], tx[3], rv[1], rv[3], sp, sn, 
			 poly[edges], tcoord[edges], tpoly[edges]);
      //left bottom edge
      edges += intersect(vo[4], vo[0], tx[4], tx[0], rv[4], rv[0], sp, sn,
			 poly[edges], tcoord[edges], tpoly[edges]);
      //right bottom edge
      edges += intersect(vo[1], vo[5], tx[1], tx[5], rv[1], rv[5], sp, sn,
			 poly[edges], tcoord[edges], tpoly[edges]);
      //front top edge
      edges += intersect(vo[2], vo[3], tx[2], tx[3], rv[2], rv[3], sp, sn, 
			 poly[edges], tcoord[edges], tpoly[edges]);
      //back bottom edge
      edges += intersect(vo[4], vo[5], tx[4], tx[5], rv[4], rv[5], sp, sn,
			 poly[edges], tcoord[edges], tpoly[edges]);
      //back left edge
      edges += intersect(vo[4], vo[6], tx[4], tx[6], rv[4], rv[6], sp, sn,
			 poly[edges], tcoord[edges], tpoly[edges]);
      //back right edge
      edges += intersect(vo[5], vo[7], tx[5], tx[7], rv[5], rv[7], sp, sn,
			 poly[edges], tcoord[edges], tpoly[edges]);
      //back top edge
      edges += intersect(vo[6], vo[7], tx[6], tx[7], rv[6], rv[7], sp, sn,
			 poly[edges], tcoord[edges], tpoly[edges]);
      //left top edge
      edges += intersect(vo[2], vo[6], tx[2], tx[6], rv[2], rv[6], sp, sn,
			 poly[edges], tcoord[edges], tpoly[edges]);
      //right top edge
      edges += intersect(vo[3], vo[7], tx[3], tx[7], rv[3], rv[7], sp, sn,
			 poly[edges], tcoord[edges], tpoly[edges]);

      if(edges == 3){ //no sort required for triangles
				//cerr << "doing a triangle" <<endl;
				glBegin(GL_POLYGON);
				{

					//printf("texture %f %f %f\n",tcoord[0][0],tcoord[0][1],tcoord[0][2]);
					//printf("poly %f %f %f\n",poly[0],tcoord[1],tcoord[2]);
					//glColor3f(1.0,1.0,1.0);
					glTexCoord3fv(tcoord[0]);
					glVertex3fv(poly[0]);
					glTexCoord3fv(tcoord[1]);
					glVertex3fv(poly[1]);
					glTexCoord3fv(tcoord[2]);
					glVertex3fv(poly[2]);
				}
				glEnd();
      }
      else { //compute convex hull and sort, a little piece of magic from:
				// B.M.E. Moret & H.D. Shapiro "P to NP" pp. 453
				
				float dx, dy, tt ,theta, cen[2];  //tt= TempTheta
				cen[0] = cen[1] = 0.0;
				int next;
				//rather than swap 3 arrays, only one?
				int order[6] ={0,1,2,3,4,5};
				
				// order[6] could be an extreemly inefficient way to do this
				for( int j=0; j<edges; ++j){ //find the center of the points
					cen[0] += tpoly[j][0];
					cen[1] += tpoly[j][1];
				} //by averaging
				
				cen[0]/= edges;
				cen[1]/= edges;	 
				
				for(int j=0; j<edges; ++j){ //for each vertex
					theta = -10;	       //find one with largest angle from center.. 
					next = j;
					for ( int k= j; k<edges; ++k){ 
						//... and check angle made between other edges
						dx = tpoly[order[k]][0] - cen[0];
						dy = tpoly[order[k]][1] - cen[1];
						if( (dx == 0) && (dy == 0)){ //same as center? 
							next = k;
							cout << "what teh " << endl;
							break; //out of this for-loop
						}
						tt = dy/(fabs(dx) + fabs(dy)); //else compute theta [0-4]
						if( dx < 0.0 ) tt = (float)(2.0 - tt); //check quadrants 2&3
						else if( dy < 0.0 ) tt = (float)(4.0 + tt); //quadrant 4
						if( theta <= tt ){  //grab the max theta
							next = k;
							theta = tt;
						}
					} //end for(k) angle checking
					// i am using 'tt' as a temp
					// swap polygon vertex ( is this better than another branch?)
					// I am not sure wich is worse: swapping 3 vectors for every edge
					// or: using an array to index into another array??? hmmm....
					//   should have payed more attention in class
					int tmp = order[j];
					order[j] = order[next];
					order[next] = tmp;
					
				} //end for(j) edge /angle sort
				
				glBegin(GL_POLYGON); //draw slice and texture map it
				//glBegin(GL_LINE_LOOP);
				{
					for(int j=0; j< edges; ++j){
						glTexCoord3fv(tcoord[order[j]]);
						glVertex3fv(poly[order[j]]);
					}
				}
				glEnd();
				//glFlush();
      }//end else compute convex hull
   }// end for(i) each slice

   
   glDisable(GL_BLEND);
   //glDisable(GL_TEXTURE_3D_EXT);
   glEnable(GL_LIGHTING);
}



//------------------------------------------------------------------------

void renderBBox()
{
   //glDisable(GL_LIGHTING);
//   glDisable(GL_TEXTURE_3D_EXT);
   glDisable(GL_TEXTURE_2D);
   //glDisable(GL_TEXTURE_3D_EXT);
   glDisable(GL_LIGHTING);

   glPolygonMode(GL_FRONT, GL_LINE);
   glPolygonMode(GL_BACK, GL_LINE);
   glLineWidth(3);

   //glBindTexture(GL_TEXTURE_2D, 0);
   glColor3f(.35, 0, .1);
   glBegin(GL_QUADS);
   {
      glVertex3f(0, 0, 0);
      glVertex3f(1, 0, 0);
      glVertex3f(1, 1, 0);
      glVertex3f(0, 1, 0);

      glVertex3f(0, 0, 1);
      glVertex3f(1, 0, 1);
      glVertex3f(1, 1, 1);
      glVertex3f(0, 1, 1);

      glVertex3f(0, 0, 0);
      glVertex3f(0, 0, 1);
      glVertex3f(0, 1, 1);
      glVertex3f(0, 1, 0);

      glVertex3f(1, 0, 0);
      glVertex3f(1, 0, 1);
      glVertex3f(1, 1, 1);
      glVertex3f(1, 1, 0);
   }
   glEnd();

   glLineWidth(1);

   glPolygonMode(GL_FRONT, GL_FILL);
   glPolygonMode(GL_BACK, GL_FILL);
   glEnable(GL_LIGHTING);
}

//------------------------------------------------------------------------

void renderBBox(float xb, float yb, float zb)
{
   //glDisable(GL_LIGHTING);
   //glDisable(GL_TEXTURE_3D_EXT);
   //glDisable(GL_TEXTURE_2D);
   glDisable(GL_TEXTURE_3D_EXT);
   glDisable(GL_LIGHTING);

   glPolygonMode(GL_FRONT, GL_LINE);
   glPolygonMode(GL_BACK, GL_LINE);
   glLineWidth(3);

   //glBindTexture(GL_TEXTURE_2D, 0);
   glColor3f(1, 1, 1);

   glBegin(GL_QUADS);
   {
      glVertex3f(0, 0, 0);
      glVertex3f(xb, 0, 0);
      glVertex3f(xb, yb, 0);
      glVertex3f(0, yb, 0);
      
      glVertex3f(0, 0, zb);
      glVertex3f(xb, 0, zb);
      glVertex3f(xb, yb, zb);
      glVertex3f(0, yb, zb);

      glVertex3f(0, 0, 0);
      glVertex3f(0, 0, zb);
      glVertex3f(0, yb, zb);
      glVertex3f(0, yb, 0);

      glVertex3f(xb, 0, 0);
      glVertex3f(xb, 0, zb);
      glVertex3f(xb, yb, zb);
      glVertex3f(xb, yb, 0);
   }
   glEnd();

   glLineWidth(1);

   glPolygonMode(GL_FRONT, GL_FILL);
   glPolygonMode(GL_BACK, GL_FILL);
   glEnable(GL_LIGHTING);
   glEnable(GL_TEXTURE_3D_EXT);
}

//--------------------------------------------------------------------------


//==========================================================================
//---------- Inlines -------------------------------------------------------
//==========================================================================


inline void translateV3(GLfloat out[3], 
					GLdouble mat[16], 
					GLfloat in[3])
{
   out[0] = mat[0]*in[0] + mat[4]*in[1] + mat[8]* in[2];// + mat[12];
   out[1] = mat[1]*in[0] + mat[5]*in[1] + mat[9]* in[2];// + mat[13];
   out[2] = mat[2]*in[0] + mat[6]*in[1] + mat[10]*in[2];// + mat[14];
}
//--------------------------------------------------------------------------
inline GLfloat dotV3(GLfloat one[4], GLfloat two[4])
{
   return one[0]*two[0] + one[1]*two[1] + one[2]*two[2];
}
//--------------------------------------------------------------------------
inline void identityMatrix(GLdouble m[16])
{
   m[0]=1; m[4]=0; m[8]= 0; m[12]=0;
   m[1]=0; m[5]=1; m[9]= 0; m[13]=0;
   m[2]=0; m[6]=0; m[10]=1; m[14]=0;
   m[3]=0; m[7]=0; m[11]=0; m[15]=1;
}
//--------------------------------------------------------------------------   
inline int intersect(
		     const float p0[3], const float p1[3], //line end points
		     const float t0[3], const float t1[3], //texture points
		     const float v0[3], const float v1[3], //view coord points
		     const float sp[3], const float sn[3], //plane point & norm
		     float pnew[3], float tnew[3], float vnew[3]) //new values 
{
   //t = (sn.(sp - p0))/(sn.(p1 - p0))
   float t = ((sn[0]*(sp[0] - p0[0]) + sn[1]*(sp[1] - p0[1]) 
	       + sn[2]*(sp[2] - p0[2])) / 
	      (sn[0]*(p1[0] - p0[0]) + sn[1]*(p1[1] - p0[1])
	       + sn[2]*(p1[2] - p0[2])));
   //note if the denominator is zero t is a NAN so we should have no problems?
   
   if( (t>=0) && (t<=1) ){
      //compute line intersection
      pnew[0] = p0[0] + t*(p1[0] - p0[0]); 
      pnew[1] = p0[1] + t*(p1[1] - p0[1]); 
      pnew[2] = p0[2] + t*(p1[2] - p0[2]);
      //compute texture interseciton
      tnew[0] = t0[0] + t*(t1[0] - t0[0]); 
      tnew[1] = t0[1] + t*(t1[1] - t0[1]); 
      tnew[2] = t0[2] + t*(t1[2] - t0[2]);
      //compute view coordinate intersections
      vnew[0] = v0[0] + t*(v1[0] - v0[0]); 
      vnew[1] = v0[1] + t*(v1[1] - v0[1]); 
      vnew[2] = v0[2] + t*(v1[2] - v0[2]);
      return 1;
   }
   return 0;
} 
//--------------------------------------------------------------------------
void inverseMatrix( GLdouble invm[16], const GLdouble m[16] )
{
   GLfloat det = 
      m[0]*m[5]*m[10]-
      m[0]*m[6]*m[9]-
      m[1]*m[4]*m[10]+
      m[1]*m[6]*m[8]+
      m[2]*m[4]*m[9]-
      m[2]*m[5]*m[8];
   
   invm[0] = (m[5]*m[10]-m[6]*m[9])/det;
   invm[1] = (-m[1]*m[10]+m[2]*m[9])/det;
   invm[2] = (m[1]*m[6]-m[2]*m[5])/det;
   invm[3] = 0.0;
   
   invm[4] = (-m[4]*m[10]+m[6]*m[8])/det;
   invm[5] = (m[0]*m[10]-m[2]*m[8])/det;
   invm[6] = (-m[0]*m[6]+m[2]*m[4])/det;
   invm[7] = 0.0;
   
   invm[8] = (m[4]*m[9]-m[5]*m[8])/det;
   invm[9] = (-m[0]*m[9]+m[1]*m[8])/det;
   invm[10] = (m[0]*m[5]-m[1]*m[4])/det;
   invm[11] = 0.0;
   
   invm[12] = (-m[4]*m[9]*m[14]+m[4]*m[13]*m[10]+
	       m[5]*m[8]*m[14]-m[5]*m[12]*m[10]-
	       m[6]*m[8]*m[13]+m[6]*m[12]*m[9])/det;
   invm[13] = (m[0]*m[9]*m[14]-m[0]*m[13]*m[10]-
	       m[1]*m[8]*m[14]+m[1]*m[12]*m[10]+
	       m[2]*m[8]*m[13]-m[2]*m[12]*m[9])/det;
   invm[14] = (-m[0]*m[5]*m[14]+m[0]*m[13]*m[6]+
	       m[1]*m[4]*m[14]-m[1]*m[12]*m[6]-
	       m[2]*m[4]*m[13]+m[2]*m[12]*m[5])/det;
   invm[15] = 1.0;
}

//--------------------------------------------------------------------------
inline void drawLine(float x0, float y0, float z0, 
				     float x1, float y1, float z1)
{
   glBegin(GL_LINES);{
      glVertex3f(x0, y0, z0);
      glVertex3f(x1, y1, z1);
   } glEnd();	
}

