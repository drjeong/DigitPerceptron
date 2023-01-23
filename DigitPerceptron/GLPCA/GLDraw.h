#pragma once

#include "glut.h"
#include "DRect.h"
#include "DPoint.h"
#include <list>

// CGLDraw

class CGLDraw
{

public:
	CGLDraw();
	virtual ~CGLDraw();

public:
	CGLColor getHilightColor(CGLColor &color);
	void drawObject(CDRect rt);
	void drawObject(GLfloat zpos, CDRect rt);
	void drawObject(CDRect rt, GLenum mode);
	void drawObject(CDPoint pt, double width, double height);
	void drawObject(CDPoint pt, double width, double height, GLenum mode);
	void drawObject(CDPoint lt, CDPoint lb, CDPoint rt, CDPoint rb, GLenum mode);
	void drawObject(list<CDPoint> &pts, GLenum mode);
	void drawObject(vector<CDPoint> &pts, GLenum mode);
	void drawTriangleObject(vector<CDPoint> &pts);
	
	void drawHorzStandardBoxPlot(CDRect region, double dnQ2, double dnQ1_SemiIQR, double dnQ3_SemiIQR);
	void drawHorzNotchBoxPlot(CDRect region, double dnQ2,
		double dnQ1_SemiIQR, double dnQ3_SemiIQR, double dnLowerLimit, double dnUpperLimit);
	void drawVertStandardBoxPlot(CDRect region, double dnQ2, double dnQ1_SemiIQR, double dnQ3_SemiIQR);
	void drawVertNotchBoxPlot(CDRect region, double dnQ2,
		double dnQ1_SemiIQR, double dnQ3_SemiIQR, double dnLowerLimit, double dnUpperLimit);

	/*void drawHorizontalBoxplot(CDRect region, vector<double>data, CGLColor color, double tbmargin);*/

	void drawCircle(CDPoint pt, double radius);	
	void drawCircleOutline(CDPoint pt, double radius);
	void drawLine(CDPoint pt1, CDPoint pt2);

	void drawObject(unsigned int gllist, CDRect rt, float zpos);

	double GetRelativeTargetValue(double tmax, double tmin, double smax, double smin, double val);

public:
	void GetXY(double *px,double *py,int mx,int my);
	CDPoint GetGLPos(int mx,int my);

};

inline void CGLDraw::GetXY(double *px,double *py,int mx,int my)
{
	GLint viewport[4];
	GLdouble mvmatrix[16], projmatrix[16];
	GLint realy;  /*  OpenGL y coordinate position  */
	double pz;

	glGetDoublev(GL_MODELVIEW_MATRIX,mvmatrix);
	glGetDoublev(GL_PROJECTION_MATRIX,projmatrix);
	glGetIntegerv(GL_VIEWPORT,viewport);

	realy = viewport[3] - (GLint) my - 1; //0 - x, y - 1, length - 2, width - 3

	gluUnProject(mx,(GLdouble) realy,0.0,mvmatrix,projmatrix,viewport,px,py,&pz);
}

inline CDPoint CGLDraw::GetGLPos(int mx,int my)
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;

	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetIntegerv( GL_VIEWPORT, viewport );

	winX = (float)mx;
	winY = (float)viewport[3] - (float)my;
	//printf("GetXY: %d %f %f\n", viewport[3], winX, winY);
	glReadPixels( mx, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );

	gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
	return CDPoint(posX, posY);
}
