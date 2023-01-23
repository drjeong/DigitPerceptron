//================================================================//
// CPortal.cpp  : Draw CPortal                                      //
//                                                                //
//                                      author: Dong Hyun Jeong    //
//================================================================//

#include "pch.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "portal.h"


CPortal::CPortal() 
{
	m_nEnabledMouseButtonId = -1;
	m_nDefinedMouseAction = NULL;
    m_dMinimumPadHeight = 0.03;
    m_bMouseActivated = false;
}

CPortal::CPortal(double x, double y, double w, double h, double z_x, double z_y, double z_w, double z_h)
{
    setPosition(x,y,w,h,z_x,z_y,z_w,z_h);
    m_dMinimumPadHeight = 0.03;
}

CPortal::~CPortal()
{
	
}

void CPortal::setReInit()
{
    m_dXPos = m_dInitPadXPos;
    m_dYPos = m_dInitPadYPos;
    m_dWidth = m_dInitPadWidth;
    m_dHeight = m_dInitPadHeight;
}

void CPortal::setPosition(double x, double y, double w, double h, double z_x, double z_y, double z_w, double z_h)
{
    m_dXPos = x;
    m_dYPos = y;
    m_dWidth = w;
    m_dHeight = h;
    m_dInitPadXPos = m_dPadXPos = z_x;
    m_dInitPadYPos = m_dPadYPos = z_y;
    m_dInitPadWidth = m_dPadWidth = z_w;
    m_dInitPadHeight = m_dPadHeight = z_h;
}

void CPortal::pad2scr(double *sx,double *sy,double px,double py)
{
	*sx = m_dXPos + (px-m_dPadXPos) * m_dWidth/m_dPadWidth;
	*sy = m_dYPos + (py-m_dPadYPos) * m_dHeight/m_dPadHeight;
}

void CPortal::scr2pad(double *px,double *py,double sx,double sy)
{
	*px = m_dPadXPos + (sx-m_dXPos) * m_dPadWidth/m_dWidth;
	*py = m_dPadYPos + (sy-m_dYPos) * m_dPadHeight/m_dHeight;
}

void CPortal::scr2PadOrigin(double *px,double *py,double sx,double sy)
{
	*px = m_dInitPadXPos + (sx-m_dXPos) * m_dInitPadWidth/m_dWidth;
	*py = m_dInitPadYPos + (sy-m_dYPos) * m_dInitPadHeight/m_dHeight;
}

void CPortal::setPositionInfo(CPortal *p)
{
	m_dPadXPos = p->m_dPadXPos;
	m_dPadYPos = p->m_dPadYPos;
	m_dPadHeight = p->m_dPadHeight;
	m_dPadWidth = p->m_dPadWidth;
    m_dMinimumPadHeight = p->m_dMinimumPadHeight;
}

CDRect CPortal::GetPadPos(CDRect rt)
{
	double psx,psy,pex,pey;
	double sx,sy,ex,ey;

	scr2PadOrigin(&psx, &psy, rt.left, rt.bottom);
	scr2PadOrigin(&pex,&pey,rt.right, rt.top);
	
	pad2scr(&sx,&sy,psx, psy);
	pad2scr(&ex,&ey,pex, pey);
	rt.left = sx;
	rt.right = ex;
	rt.top = ey;
	rt.bottom = sy;

	return rt;
}

CDPoint CPortal::GetPadPos(CDPoint pt)
{
	double psx,psy;
	double sx,sy;

	scr2PadOrigin(&psx, &psy, pt.x, pt.y);
	pad2scr(&sx,&sy,psx, psy);

	pt.x = sx;
	pt.y = sy;

	return pt;
}

void CPortal::PortalMotion_Panning(CDPoint glpoint)
{
	double delx = glpoint.x - m_dMouseX;
	double dely = glpoint.y - m_dMouseY;
	CPortal::m_dPadXPos -= delx*CPortal::m_dPadWidth;
	CPortal::m_dPadYPos -= dely*CPortal::m_dPadHeight;
	m_dMouseX = glpoint.x;
	m_dMouseY = glpoint.y;
}
	
void CPortal::PortalMotion_ZoomIn(double increasestep)
{
	//zoom in
	double px, py;	scr2pad(&px, &py, m_dMouseX, m_dMouseY);		// pad coordinate

	m_dPadWidth = (0.99 - increasestep)*m_dPadWidth;
	m_dPadHeight = (0.99 - increasestep)*m_dPadHeight;

	m_dPadXPos = px - m_dPadWidth*(m_dMouseX + 0.5);
	m_dPadYPos = py - m_dPadHeight*(m_dMouseY + 0.5);

	// 		printf("pw: %.2f ph: %.2f px: %.2f py: %.2f\n", m_dPadWidth, m_dPadHeight, m_dPadXPos, m_dPadYPos);
}

void CPortal::PortalMotion_ZoomOut(double increasestep)
{
	double px, py;	scr2pad(&px, &py, m_dMouseX, m_dMouseY);		// pad coordinate
	m_dPadWidth = (1.01 + increasestep)*m_dPadWidth;
	m_dPadHeight = (1.01 + increasestep)*m_dPadHeight;

	if (m_dPadXPos < m_dInitPadXPos) m_dPadXPos = m_dInitPadXPos;
	if (m_dPadYPos < m_dInitPadYPos) m_dPadYPos = m_dInitPadYPos;
	if (m_dPadWidth > m_dInitPadWidth) m_dPadWidth = m_dInitPadWidth;
	if (m_dPadHeight > m_dInitPadHeight) m_dPadHeight = m_dInitPadHeight;

	if (((float)m_dPadWidth == (float)m_dInitPadWidth) && ((float)m_dPadHeight == (float)m_dInitPadHeight))
	{
		if (m_dPadXPos == m_dInitPadXPos)
			m_dPadYPos -= 0.01;
		if (m_dPadYPos == m_dInitPadYPos)
			m_dPadXPos -= 0.01;
		if ((m_dPadYPos == m_dInitPadYPos) && (m_dPadXPos != m_dInitPadXPos))
			m_dPadXPos = m_dInitPadXPos;
		if ((m_dPadXPos == m_dInitPadXPos) && (m_dPadYPos != m_dInitPadYPos))
			m_dPadYPos = m_dInitPadYPos;
		if ((m_dPadXPos != m_dInitPadXPos) && (m_dPadYPos != m_dInitPadYPos))
		{
			m_dPadXPos = m_dInitPadXPos;
			m_dPadYPos = m_dInitPadYPos;
		}
	}
	else
	{
		m_dPadXPos = px - m_dPadWidth*(m_dMouseX + 0.5);
		m_dPadYPos = py - m_dPadHeight*(m_dMouseY + 0.5);
	}
	// 		printf("pw: %.2f ph: %.2f px: %.2f py: %.2f\n", m_dPadWidth, m_dPadHeight, m_dPadXPos, m_dPadYPos);
}

bool CPortal::PortalMotion(long time)
{
	double increasestep = (time/100.0)*0.05; 

	if (m_nDefinedMouseAction == MOUSE_ZOOM_IN) 
	{	// left button, zoom in
		PortalMotion_ZoomIn(increasestep);
		return true;
	}
	else
	if(m_nDefinedMouseAction == MOUSE_ZOOM_OUT) 
	{
		PortalMotion_ZoomOut(increasestep);
		return true;
	}
	return false;
}


