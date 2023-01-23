#if !defined(AFX_CONVEXHULL_H__3B860674_9215_4806_AE46_4754D41B7E67__INCLUDED_)
#define AFX_CONVEXHULL_H__3B860674_9215_4806_AE46_4754D41B7E67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConvexHull.h : header file
//

#include <vector>
#include "DataManager.h"

using namespace std;
/////////////////////////////////////////////////////////////////////////////
// CConvexHull window

typedef vector<CDPoint> DPTLIST;

#define NOTSET 9999999

typedef struct tagMINMAXDPT{
	double minval;
	double maxval;
	tagMINMAXDPT()
	{
		minval=maxval=NOTSET;
	}
}STMINMAXDPT;

class CConvexHull
{
// Construction
public:
	CConvexHull();

// Attributes
public:
	 DPTLIST *m_pHullPList;
	 STMINMAXDPT *m_pHullMinMax;
	 int m_nPT;
	 int m_nSelectedItem;
	 bool m_bConvexHullCreated;

// Operations
public:
	void AddPointList(MatrixXd &mat);
	void AddPointList(MatrixXd &chulldata, MatrixXd &inputdata, int row);

	void Reset();
	void CreateConvexHull();
	void CreateConvexHull(DPTLIST input, DPTLIST *poutput);
	void CreateConvexHullWithGraham(DPTLIST input, DPTLIST *poutput);

	double findAngle(CDPoint pt1, CDPoint pt2);
	bool notUsed(DPTLIST &input, DPTLIST &ResultList, int index);

// Implementation
public:
	virtual ~CConvexHull();

	// Generated message map functions
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONVEXHULL_H__3B860674_9215_4806_AE46_4754D41B7E67__INCLUDED_)
