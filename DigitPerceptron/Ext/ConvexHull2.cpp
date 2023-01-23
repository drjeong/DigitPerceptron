// ConvexHull2.cpp : implementation file
//

#include "pch.h"
#include <math.h>
#include <iostream>
#include "glut.h"
#include "ConvexHull2.h"

#define M_PI2 3.1415926535897932385 

/////////////////////////////////////////////////////////////////////////////
// CConvexHull2
CConvexHull2::CConvexHull2()
{
	m_firstPoint=NULL;
}

CConvexHull2::~CConvexHull2()
{
	resetmemory();
}

/////////////////////////////////////////////////////////////////////////////
// CConvexHull2 message handlers
void CConvexHull2::resetmemory()
{
	m_HullPtList_Input.clear();
	m_HullPtList_Output.clear();

	while (m_firstPoint)
	{
		CPTLISTNODE *tmp = m_firstPoint;
		m_firstPoint = m_firstPoint->next;
		delete tmp;
	}
	m_firstPoint=NULL;
}

bool CConvexHull2::isConvexHull(int nParentNode, int nSNode, int nSize)
{
	// since the nSNode is changing whenever a cluster is expaned and shrinked, it is not necessary to be checked.
	if (m_nParentNode==nParentNode && m_nSize==nSize) 
		return true;
	return false;
}

void CConvexHull2::setConvexHull(int nParentNode, int nSNode, int nSize)
{
	m_nParentNode = nParentNode; // Parent node id
	m_nSNode = nSNode;	// Starting sequence id in the selected cluster 
	m_nSize = nSize; // # of ndoes in the selected cluster
}

double CConvexHull2::findAngle(CDPoint pt1, CDPoint pt2)
{
    double deltaX=pt2.x-pt1.x;
    double deltaY=pt2.y-pt1.y;
    
    if (deltaX==0 && deltaY==0)
        return 0;
    
    double angle=atan(deltaY/deltaX)*(180.0/3.141592);

    //TAKE INTO ACCOUNT QUADRANTS, VALUE: 0?360?    
	if (deltaX>=0 && deltaY>=0)
        angle=90+angle;
    else if (deltaX>=0 && deltaY<0)
        angle=90+angle;
    else if (deltaX<0 && deltaY>0)
        angle=270+angle;
    else if (deltaX<0 && deltaY<=0)
        angle=270+angle;
    
    return angle;
}

void CConvexHull2::AddPointList(CDPoint pt)
{
	m_HullPtList_Input.push_back(pt);
}

void CConvexHull2::removePointList()
{
	m_HullPtList_Input.clear();
	m_HullPtList_Output.clear();
}

void CConvexHull2::draw_ellipse_p(double xc, double yc, double r, int slice)
{
	double th, dth = 2*M_PI2 / slice;

	glBegin(GL_TRIANGLE_FAN);
	for(th=0; th<2*M_PI2; th+=dth) {
		glVertex2d(xc+cos(th)*r, yc+sin(th)*r);
	}
	glEnd();

	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINE_STRIP);
	for(th=0; th<2*M_PI2; th+=dth) {
		glVertex2d(xc+cos(th)*r, yc+sin(th)*r);
	}
	glEnd();
}

void CConvexHull2::DisplayConvexBoundary(CftGLFontWrapper* pftglFontValue)
{
	glBegin(GL_LINE_LOOP);
	for (POINTLIST::iterator it=m_HullPtList_Output.begin(); it!=m_HullPtList_Output.end(); it++)
	{
		CDPoint pt = *it;
		glVertex3d(pt.x, pt.y, 0.7f);
	}
	glEnd();

// 	glColor3f(0.0f, 0.0f, 0.0f);
// 	int idx=0;
// 	for (POINTLIST::iterator it=m_HullPtList_Output.begin(); it!=m_HullPtList_Output.end(); it++)
// 	{
// 		CDPoint pt = *it;
// 		wxString string = wxString::Format(wxT("%d"), idx++);
// 		pftglFontValue->FontDrawText(string,pt.x, pt.y, 0.8f);
// 	}
}

void CConvexHull2::DisplayConvexHull()
{
	glBegin(GL_POLYGON);
	for (POINTLIST::iterator it=m_HullPtList_Output.begin(); it!=m_HullPtList_Output.end(); it++)
	{
		CDPoint pt = *it;
		glVertex3d(pt.x, pt.y, 0.6f);
	}
	glEnd();
}

void CConvexHull2::CreateConvexHull()
{
	CreateConvexHull_Jarvis();
	//CreateConvexHull_Graham();
}

void CConvexHull2::CreateConvexHullnDraw()
{
	CreateConvexHull();
	DisplayConvexHull();
}

//////////////////////////////////////////////////////////////////////////////
// Graham
void CConvexHull2::addPoint(CPTLISTNODE Point)
{
	CPTLISTNODE *tempPoint,*tempPointA,*tempPointB, *curPoint;

	//ALLOCATE A NEW POINT STRUCTURE AND INITIALIZE INTERNAL VARIABLES
    tempPoint = new CPTLISTNODE;
	tempPoint->pt = Point.pt;
	tempPoint->angle=Point.angle;  
	tempPoint->next=NULL;
	tempPoint->prev=NULL;

	if (m_firstPoint==NULL) //TEST IF LIST IS EMPTY
	{
		m_firstPoint=tempPoint;
		return;
	}

	if (m_firstPoint->next==NULL && tempPoint->angle >= m_firstPoint->angle) 
		//TEST IF ONLY ONE NODE IN LIST AND CURRENT NODE HAS GREATER ANGLE
	{
		m_firstPoint->next=tempPoint;
		tempPoint->prev=m_firstPoint;
		return;
	}

	curPoint=m_firstPoint;

	while (tempPoint->angle >= curPoint->angle && curPoint->next!=NULL)
		//CONTINUE THROUGH LIST UNTIL A NODE IS FOUND WITH A GREATER ANGLE THAN CURRENT NODE
		curPoint=curPoint->next;

	if (curPoint==m_firstPoint) //TEST IF NODE IS FIRSTPOINT.  IF SO, ADD AT FRONT OF LIST.
	{
		m_firstPoint->prev=tempPoint;
		tempPoint->next=m_firstPoint;
		m_firstPoint=tempPoint;
		return;
	}
	else if (curPoint->next==NULL && tempPoint->angle >= curPoint->angle) 
		//TEST IF WHILE LOOP REACHED FINAL NODE IN LIST.  IF SO, ADD AT END OF THE LIST.
	{
		curPoint->next=tempPoint;
		tempPoint->prev=curPoint;
		return;
	}
	else //OTHERWISE, INTERMEDIATE NODE HAS BEEN FOUND.  INSERT INTO LIST.  
	{
		tempPointA=curPoint->prev;
		tempPointB=curPoint->prev->next;
		tempPoint->next=tempPointB;
		tempPoint->prev=tempPointA;
		tempPoint->prev->next=tempPoint;
		tempPoint->next->prev=tempPoint;
	}

	return;   
}

void CConvexHull2::CreateConvexHull_Graham()
{
	int minPoint = 0;
	int NumPoints = m_HullPtList_Input.size();
	m_firstPoint = NULL;

	for (int j=1;j<NumPoints;j++) //FOR ALL POINTS IN THE SET, FIND MIN POINT
		if (m_HullPtList_Input[j].y<m_HullPtList_Input[minPoint].y)
			minPoint=j;

	for (int i=0;i<NumPoints;i++) //SORT RANDOM POINTS
	{
		double angle=findAngle(CDPoint(m_HullPtList_Input[minPoint].x,
			m_HullPtList_Input[minPoint].y),
			CDPoint(m_HullPtList_Input[i].x,
			m_HullPtList_Input[i].y));
		CPTLISTNODE ptListNode;
		ptListNode.pt = m_HullPtList_Input[i];
		ptListNode.angle = angle;
		addPoint(ptListNode);
	}

	CPTLISTNODE *tempPtr = m_firstPoint;
	do  //FIND LAST NODE IN LINKED LIST
	{
		tempPtr=tempPtr->next;
	} while (tempPtr->next!=NULL);

	tempPtr=m_firstPoint->next;
	for (int i=1;i<NumPoints;i++) //DRAW LINES FROM MIN POINT TO ALL OTHERS
	{
		m_HullPtList_Output.push_back(tempPtr->pt);
		tempPtr=tempPtr->next;
	}

	while (m_firstPoint)
	{
		CPTLISTNODE *tmp = m_firstPoint;
		m_firstPoint = m_firstPoint->next;
		delete tmp;
	}
	m_firstPoint=NULL;
}

//////////////////////////////////////////////////////////////////////////////
// Jarvis
bool CConvexHull2::notUsed(int y)
{
	//FOR ALL POINTS IN THE SET, CHECK IF INDEX HAS ALREADY BEEN ADDED
	for (size_t i=0;i<m_UsedPoints.size();i++) 
		if (y==m_UsedPoints[i])
			return false;
	return true;
}

void CConvexHull2::CreateConvexHull_Jarvis()
{ // this source code is not perfectly working. It needs to be revised.
// jarvis algorithm (gift-wrapping)
	int minPoint = 0;
	int maxPoint = 0;
	int currPoint = 0;
	int NumPoints = m_HullPtList_Input.size();
	int maxAngle=0, minAngle=0;
	int i;
	
    for (int k=1;k<NumPoints;k++)
        if (m_HullPtList_Input[k].y>m_HullPtList_Input[maxPoint].y)
            maxPoint=k;

    for (int j=1;j<NumPoints;j++) //FOR ALL POINTS IN THE SET, FIND MIN POINT
        if (m_HullPtList_Input[j].y<m_HullPtList_Input[minPoint].y)
            minPoint=j;
	
	m_UsedPoints.push_back(minPoint);//ADD MIN POINT TO LIST
	currPoint=minPoint;

	//BUILD LEFT-HAND SIDE OF CONVEX HULL
	while (currPoint != maxPoint)
	{
        maxAngle=currPoint;
		for (int y=0;y<NumPoints;y++)
		{
            if (findAngle(m_HullPtList_Input[currPoint],m_HullPtList_Input[maxAngle]) < 
				findAngle(m_HullPtList_Input[currPoint],m_HullPtList_Input[y]) 
				&& (notUsed(y) || y==maxPoint) 
				&& findAngle(m_HullPtList_Input[currPoint],m_HullPtList_Input[y])<=270)
            {
                maxAngle=y;
            }	
		}
		currPoint=maxAngle;
		m_UsedPoints.push_back(currPoint); //ADD NEW POINT TO FINAL PERIMETER LIST
	}

	currPoint=minPoint;
    //BUILD RIGHT-HAND SIDE OF CONVEX HULL
    while(currPoint!=maxPoint) 
    {
        minAngle=maxPoint;
        
        for (int y=0;y<NumPoints;y++)
        {
            if (findAngle(m_HullPtList_Input[currPoint],m_HullPtList_Input[minAngle]) >
				findAngle(m_HullPtList_Input[currPoint],m_HullPtList_Input[y]) 
				&& (notUsed(y) || y==maxPoint) 
				&& findAngle(m_HullPtList_Input[currPoint],m_HullPtList_Input[y])>=90)
            {
                minAngle=y;
            }
        }

        currPoint=minAngle;
		m_UsedPoints.push_back(currPoint);
    }

// 	// merge results of left & right-hand side of convex hull
// 	for (int i=0; i<m_UsedPoints.size(); i++)
// 		m_HullPtList_Output.push_back(m_HullPtList_Input[m_UsedPoints[i]]);

	int k=0;
	while (m_UsedPoints[k]!=maxPoint) //RENDER LEFT-HAND SIDE OF CONVEX HULL
	{
		m_HullPtList_Output.push_back(m_HullPtList_Input[m_UsedPoints[k]]);
		k++;
	}

	//while (k<m_UsedPoints.size()) //RENDER RIGHT-HAND SIDE OF CONVEX HULL
	for (i=m_UsedPoints.size()-1; i>k; i--)
	{
		m_HullPtList_Output.push_back(m_HullPtList_Input[m_UsedPoints[i]]);
	}
// 	k++;
// 	m_HullPtList_Output.push_back(m_HullPtList_Input[m_UsedPoints[0]]);

}
