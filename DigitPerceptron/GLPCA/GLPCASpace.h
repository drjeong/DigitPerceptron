#pragma once

// GLPCASpace.h : header file
//

#include "GLWnd.h"
#include "freeglut.h"
#include <time.h>
#include "Portal.h"
#include "GLView.h"
#include "ClutterManager.h"
#include "ftGLFontWrapper.h"

/////////////////////////////////////////////////////////////////////////////
// CGLPCASpace window

class CGLPCASpace : public CPortal, public CGLView
{
// Construction
public:
	CGLPCASpace();

	void DrawInstances();
	void DrawBoxPlot();
	void DrawBasic_HorizontalBoxPlot(CDRect region);
	void DrawBasic_VerticalBoxPlot(CDRect region);
	void DrawGrouped_HorizontalBoxPlot(CDRect region);
	void DrawGrouped_VerticalBoxPlot(CDRect region);
	void DrawBiplot();
	void DrawGLScene();
	void DrawRangeSelectionBoundary();

	BOOL IsReadyToInteract(CDPoint glpoint);
	BOOL IsReadyToRender();

	int FindHighlightedObject(double mx, double my);
	int SelectObject(double mx, double my);
	int DetermineSelectedObject();

	void Reset();
	void ResetClutterManager();
	void UpdateDataRange();

	// For Range Selection
	CDPoint GetGLScreenCoordinate(CDPoint pt);
	void FindSelectedInstances();

// Attributes
public:
	CDataItemClutterManager m_ClutterManager_DataInstances;
	CTextClutterManager m_ClutterManager_TextInfo;
	//CGLFrameBufferTextureObject m_FrameBufferTexObj;
	CString m_HighlitedObjectText;

	// For Range Selection for multiple items selection
	CGLScreenMapping m_UserSelectedRegion;

	C2DMinMax m_PC1DataRange, m_PC2DataRange;
	
	clock_t	m_timebuffer;

	UINT m_nShowBoxplot;
	UINT m_nShowBiplot;
	//CGLBoxPlot::BOXPLOTBODY_STYLE m_nBoxplotBodyStyle;
	//CGLBoxPlot::BOXPLOT_STYLE m_nBoxplotStyle;
	//CGLBoxPlot::WHISKER_STYLE m_nWhiskerStyle;
	CGLView::VISUALCOMPLEXITY m_nVisualComplexity;

private:
	CDRect m_glRenderingRegion;

// Operations
public:

// Implementation
public:
	virtual ~CGLPCASpace();

	// Generated message map functions
public:
	void OnPaint();
	BOOL OnMouseWheel(short zDelta, CPoint scpoint, CDPoint glpoint);
	BOOL OnMouseHWheel(short zDelta, CPoint scpoint, CDPoint glpoint);
	BOOL OnLButtonDown(CPoint scpoint, CDPoint glpoint);
	BOOL OnRButtonDown(CDPoint glpoint);
	BOOL OnMButtonDown(CPoint scpoint, CDPoint glpoint);
	BOOL OnMouseMove(CPoint scpoint, CDPoint glpoint);
	BOOL OnLButtonUp(CDPoint glpoint);
	void OnRButtonUp(CDPoint glpoint);
	BOOL OnMButtonUp(CDPoint glpoint);
	BOOL LButtonUp(CDPoint glpoint);
	BOOL OnContextMenu(CPoint point, CDPoint glpoint);
};
