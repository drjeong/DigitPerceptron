// ConvexHull.cpp : implementation file
//

#include "pch.h"
#include "ConvexHull.h"
#include "graham.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConvexHull
int index=0;

CConvexHull::CConvexHull()
{
	m_pHullPList = NULL;
	m_nPT = 0;
	m_bConvexHullCreated = false;
	m_pHullMinMax = NULL;
	m_nSelectedItem = -1;
}

CConvexHull::~CConvexHull()
{
	Reset();
}

/////////////////////////////////////////////////////////////////////////////
// CConvexHull message handlers
void CConvexHull::Reset()
{
	if (m_pHullPList) delete [] m_pHullPList;
	if (m_pHullMinMax) delete [] m_pHullMinMax;

	m_pHullPList = NULL;
	m_nPT = 0;
	m_bConvexHullCreated = false;
	m_pHullMinMax = NULL;
	m_nSelectedItem = -1;
	index=0;
}

int __cdecl compare_ypos(const void *id1, const void *id2 )
{ // Ascending Order
	CDPoint p1 = *((CDPoint*)id1);
	CDPoint p2 = *((CDPoint*)id2);

	if ( p1.y > p2.y ) return -1;
	if ( p1.y == p2.y ) return 0;
	if ( p1.y < p2.y ) return 1;
	return 0;
}

double CConvexHull::findAngle(CDPoint pt1, CDPoint pt2)
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

bool CConvexHull::notUsed(DPTLIST &input, DPTLIST &ResultList, int index)
{
	DPTLIST::iterator it;
	for (it=ResultList.begin(); it!= ResultList.end(); it++)
	{
		CDPoint pt = *it;

		if ((input[index].x == pt.x) &&  (input[index].y == pt.y))
			return true;
	}

	return false;
}


void CConvexHull::AddPointList(MatrixXd &chulldata, MatrixXd &inputdata, int row)
{
	m_bConvexHullCreated=false;
	m_nSelectedItem = row;
	m_nPT = 1;//chulldata.rows();
	if (m_pHullPList==NULL)
		m_pHullPList = new DPTLIST[m_nPT];
	if (m_pHullMinMax==NULL)
		m_pHullMinMax = new STMINMAXDPT[chulldata.cols()];

	CDPoint chpt = CDPoint(chulldata(row,0), chulldata(row,1));// dimension 1,2 - X,Y Axis
	m_pHullPList[0].push_back(chpt);

	for (int j=0; j<chulldata.cols(); j++)
	{
		if (m_pHullMinMax[j].minval==NOTSET&&m_pHullMinMax[j].maxval==NOTSET)
		{// printf(".");
			m_pHullMinMax[j].minval = chulldata(row,j);
			m_pHullMinMax[j].maxval = chulldata(row,j);
		}

		if (m_pHullMinMax[j].minval > chulldata(row,j)) m_pHullMinMax[j].minval = chulldata(row,j);
		if (m_pHullMinMax[j].maxval < chulldata(row,j)) m_pHullMinMax[j].maxval = chulldata(row,j);
	}
	m_bConvexHullCreated=true;
}

void CConvexHull::AddPointList(MatrixXd &mat)
{
	m_nPT = mat.rows();
	if (m_pHullPList==NULL)
		m_pHullPList = new DPTLIST[m_nPT];

	for (int i=0; i<mat.rows(); i++)
	{
		CDPoint pt = CDPoint(mat(i,0), mat(i,1));// dimension 1,2 - X,Y Axis
		m_pHullPList[i].push_back(pt);
	}

//	CreateConvexHull();
}

void CConvexHull::CreateConvexHull()
{
	return;
	CreateConvexHull(m_pHullPList[0], &m_pHullPList[0]);

	DPTLIST::iterator it;
	for (it=m_pHullPList[0].begin();it!=m_pHullPList[0].end();it++)
	{
		CDPoint pt = *it;
	}
}

void CConvexHull::CreateConvexHullWithGraham(DPTLIST input, DPTLIST *poutput)
{
	CGrahamScan GrahamScan(input.size());
	for (int i=0; i<input.size(); i++)
		GrahamScan.AddItem(input[i].x, input[i].y);
	DPTLIST output;
	GrahamScan.CreateHull(output);
	*poutput = output;

//	DPTLIST::iterator it;
//	for (it=output.begin(); it!=output.end(); it++)
//	{
//		CDPoint pt = *it;
//		printf("(%f,%f) ", pt.x, pt.y);
//	}
}

void CConvexHull::CreateConvexHull(DPTLIST input, DPTLIST *poutput)
{
// jarvis algorithm (gift-wrapping)
	int minPoint = 0;
	int maxPoint = 0;
	int currPoint = 0;
	int NumPoints = (int)input.size();
	int maxAngle=0, minAngle=0;
	
//	qsort(&input[0], input.size(), sizeof(CDPoint), compare_ypos);

    for (int k=1;k<NumPoints;k++)
        if (input[k].y>input[maxPoint].y)
            maxPoint=k;

    for (int j=1;j<NumPoints;j++) //FOR ALL POINTS IN THE SET, FIND MIN POINT
        if (input[j].y<input[minPoint].y)
            minPoint=j;

	bool *used = new bool [input.size()];
	for (int i=0; i<input.size(); i++)
		used[i] = false; 

	DPTLIST LeftList, RightList;
	LeftList.push_back(input[minPoint]);
	used[minPoint] = true;

	currPoint = minPoint;
	//BUILD LEFT-HAND SIDE OF CONVEX HULL
	while (currPoint != maxPoint)
	{
        maxAngle=currPoint;
		for (int y=0;y<NumPoints;y++)
		{
            if (findAngle(input[currPoint],input[maxAngle])<findAngle(input[currPoint],input[y]) 
				&& (used[y]==false || y==maxPoint) 
				&& findAngle(input[currPoint],input[y])<=270)
            {
                maxAngle=y;
            }	
		}
		currPoint=maxAngle;
		if (used[currPoint]==false)
			LeftList.push_back(input[currPoint]);
		used[currPoint] = true;
	}

	currPoint=minPoint;
    //BUILD RIGHT-HAND SIDE OF CONVEX HULL
    while(currPoint!=maxPoint) 
    {
        minAngle=maxPoint;
        
        for (int y=0;y<NumPoints;y++)
        {
            if (findAngle(input[currPoint],input[minAngle])>findAngle(input[currPoint],input[y]) 
				&& (used[y]==false || y==maxPoint) 
				&& findAngle(input[currPoint],input[y])>=90)
            {
                minAngle=y;
            }
        }

        currPoint=minAngle;
		if (used[currPoint]==false)
			RightList.push_back(input[currPoint]);
		used[currPoint] = true;
    }

	// copy convex hull to output buffer
	poutput->clear();
	DPTLIST::iterator it;
	if (LeftList.size()>0)
		for (it=LeftList.begin(); it!=LeftList.end(); it++)
			poutput->push_back(*it);
	if (RightList.size()>0)
		for (it=RightList.end()-1; it!=RightList.begin(); it--)
			poutput->push_back(*it);
	delete [] used;
}
