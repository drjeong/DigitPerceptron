#pragma once
#include "pch.h"
// GLPCA.h : header file
//

#include "GLWnd.h"
#include "freeglut.h"
#include <time.h>
#include "Portal.h"
#include "ClutterManager.h"
#include "ftGLFontWrapper.h"
#include "GLPCASpace.h"
#include "GLDraw.h"

/////////////////////////////////////////////////////////////////////////////
// CGLPCA window

class CGLPCA : public CGLWnd, public CGLDraw
{
	DECLARE_DYNAMIC(CGLPCA)
	// Construction
public:
	CGLPCA();

	BOOL IsReady();

	CGLPCASpace *GetGLPCASpace(){return &m_glPCASpaceWin;};

	void Redraw();
	void InitViews();
	void Reset();
	void CaptureScreen(char *filename, GLint format);
	BOOL IsMouseOnBorders(CDPoint glPoint);	// check if the mouse cursor is inside of any borders


	// Attributes
private: 
	vector<CDRect> m_rtHorzBorders;

	typedef struct{
		CDRect region; // border region
		CGLView *pleft;
		CGLView *pright;
	}VERTICALBORDER;
	vector<VERTICALBORDER> m_rtVertBorders; // exist in the 1st row

	int m_nSelectedHorzBorder_forBorderMovement;
	int m_nSelectedVertzBorder_forBorderMovement;
	CDPoint m_ptCursorMovement_forBorderMovement;

	// height of each visualization view	
	double m_dRow1VizHeightRatio, m_dRow2VizHeightRatio, m_dRow3VizHeightRatio;
	int m_nPrevVisibleVizRows;

	CGLPCASpace m_glPCASpaceWin;

public:

	// For Range Selection
	POINTLIST m_ptlist;
	
	// mouse cursor activation on borders (used for moving the borders)
	UINT m_nMouseCursor;

	clock_t	m_timebuffer;
	DRECTnIDX_LIST m_rtGLPCADataItemPos;
	
	// Implementation
public:
	virtual ~CGLPCA();
	BOOL LButtonUp(CPoint point, CDPoint glpoint, BOOL bDisable=FALSE);
	void PreparePickScene(CRect region, GLuint* selectBuffer);
	void PreparePickScene(int pickX, int pickY, GLuint* selectBuffer);

	BOOL SpecialKeyPressed(UINT vkey);
	BOOL SpecialKeyReleased();

private: 

	// Generated message map functions
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseLeave();
};
