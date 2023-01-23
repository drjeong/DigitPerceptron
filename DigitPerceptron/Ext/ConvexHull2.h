#if !defined(AFX_CONVEXHULL2_H__3B860674_9215_4806_AE46_4754D41B7E67__INCLUDED_)
#define AFX_CONVEXHULL2_H__3B860674_9215_4806_AE46_4754D41B7E67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConvexHull.h : header file
//

#include "DPoint.h"
#include "DRect.h"
#include "ftGLFontWrapper.h"

/////////////////////////////////////////////////////////////////////////////
// CConvexHull2 window

#define NOTSET 9999999

class CPTLISTNODE
{
public:
	CPTLISTNODE(){next=NULL; prev=NULL;angle=0;};
	CDPoint pt; // POSITION
	CPTLISTNODE *next; //POINTER TO NEXT NODE IN THE LIST
	CPTLISTNODE *prev; //POINTER TO PREVIOUS NODE IN THE LIST
	double angle; //INTERMEDIATE ANGLE VALUE STORAGE
};

class CConvexHull2
{
// Construction
public:
	CConvexHull2();

// Attributes
public:

// Operations
public:
	void AddPointList(CDPoint pt);
	void AddPointOutList(CDPoint pt){m_HullPtList_Output.push_back(pt);}

	void removePointList();
	void setColor(float r, float g, float b){m_red=r;m_green=g;m_blue=b;};

	size_t GetTotInputPoints(){return m_HullPtList_Input.size();};
	size_t GetTotOutputPoints(){return m_HullPtList_Output.size();};

	float getRed(){return m_red;};
	float getGreen(){return m_green;};
	float getBlue(){return m_blue;};

	POINTLIST *GetHullOutputList(){return &m_HullPtList_Output;}

	void setConvexHull(int nParentNode, int nSNode, int nSize);
	bool isConvexHull(int nParentNode, int nSNode, int nSize);
	bool notUsed(int y);

	void DisplayConvexHull();
	void DisplayConvexBoundary(CftGLFontWrapper* pftglFontValue);

	void resetmemory();
	void CreateConvexHull();
	void CreateConvexHullnDraw();
	void CreateConvexHull_Jarvis();
	void CreateConvexHull_Graham();
	void addPoint(CPTLISTNODE Point);

	void draw_ellipse_p(double xc, double yc, double r, int slice);

	double findAngle(CDPoint pt1, CDPoint pt2);

	// Implementation
public:
	virtual ~CConvexHull2();

private:
	int m_nParentNode;
	int m_nSNode;
	int m_nSize;

	POINTLIST m_HullPtList_Input;
	POINTLIST m_HullPtList_Output;
	
	vector<int> m_UsedPoints;
	CPTLISTNODE *m_firstPoint; //GLOBAL POINTER TO MIN POINT IN DOUBLELY LINKED LIST

	float m_red, m_green, m_blue;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONVEXHULL2_H__3B860674_9215_4806_AE46_4754D41B7E67__INCLUDED_)
