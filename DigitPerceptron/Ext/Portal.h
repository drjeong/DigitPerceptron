//================================================================//
// CPortal.h  : Defines CPortal class                             //
//                                                                //
//================================================================//

#ifndef __PORTAL_H
#define __PORTAL_H

//#include "typedef.h"
//#include "glut.h"

#include "freeglut.h"
#include "DRect.h"
#include "DPoint.h"
#include "GLWnd.h"

// -------------------------------------------------------------------------------------------------
#define MOUSE_ZOOM_IN                           1  // mouse zoom in and out in main window
#define MOUSE_ZOOM_OUT							2
#define MOUSE_PANNING                           4  // panning main window
#define	MOUSE_NODEPOS_MOVE						5
#define MOUSE_RANGE_SELECTION					6

class CPortal
{
public:
	// temporary saved initial pad coordinate
	double m_dInitPadXPos, m_dInitPadYPos;	// pad coordinate
	double m_dInitPadWidth, m_dInitPadHeight;
    double m_dXPos,m_dYPos;			// screen coordinate (left, bottom)
	double m_dWidth, m_dHeight;
	double m_dPadXPos, m_dPadYPos;	// pad coordinate
	double m_dPadWidth, m_dPadHeight;

public:
	double m_dMinimumPadHeight;

    int m_nObjStack;
    int m_nObjId;       // the object that subportal showing

    double c_z_x, c_z_y, c_z_w, c_z_h;  // current pad coordinate of overview 

//    int		m_nWindowId;				
    double	m_dMouseX, m_dMouseY, m_dMouseX0, m_dMouseY0;
    bool	m_bMouseActivated;			// Enabled when user tries to move mouse in some location
    int		m_nEnabledMouseButtonId;		// GLUT_LEFT_BUTTON(0), GLUT_MIDDLE_BUTTON(1), or GLUT_RIGHT_BUTTON(2)
    int		m_nDefinedMouseAction;		// Allocated when user tries to apply some action to the mouse 
	
public:
    CPortal();
    CPortal(double x, double y, double w, double h, double z_x, double z_y, double z_w, double z_h);
	virtual ~CPortal();
	
	void InitSelectionBuffer();
	void setReInit();
	bool PortalMotion(long time); // ture: update, false: noupdate

	void PortalMotion_ZoomIn(double increasestep);
	void PortalMotion_ZoomOut(double increasestep);
	void PortalMotion_Panning(CDPoint glpoint);

	CDRect GetPadPos(CDRect rt);
	CDPoint GetPadPos(CDPoint pt);

    void movePortal(double t);
    void setPosition(double x, double y, double w, double h, double z_x, double z_y, double z_w, double z_h);

	void pad2scr(double *sx,double *sy,double px,double py);
	void scr2pad(double *px,double *py,double sx,double sy);
	void scr2PadOrigin(double *px,double *py,double sx,double sy);

	void setPositionInfo(CPortal *p);

//	inline void GetXY(double *px,double *py,int mx,int my)
//    {
//		GLint viewport[4];
//		GLdouble mvmatrix[16], projmatrix[16];
//		GLint realy;  /*  OpenGL y coordinate position  */
//        double pz;
//
//        glGetDoublev(GL_MODELVIEW_MATRIX,mvmatrix);
//        glGetDoublev(GL_PROJECTION_MATRIX,projmatrix);
//        glGetIntegerv(GL_VIEWPORT,viewport);
//
//		realy = viewport[3] - (GLint) my - 1;
//		
//        gluUnProject(mx,(GLdouble) realy,0.0,mvmatrix,projmatrix,viewport,px,py,&pz);
//    }
//
//	inline void GetXY(double *px,double *py,double *pz, double mx, double my, double mz)
//    {
//		GLint vp[4];
//        double M[16],p[16];
//
//        glGetDoublev(GL_MODELVIEW_MATRIX,M);
//        glGetDoublev(GL_PROJECTION_MATRIX,p);
//        glGetIntegerv(GL_VIEWPORT,vp);
//        gluUnProject(mx,CPortal::m_nWindowHeight-my,mz,M,p,vp,px,py,pz);
//    }


};

#endif
