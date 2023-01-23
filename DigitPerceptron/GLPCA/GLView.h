#pragma once

#include "glut.h"

#include "DRect.h"
#include "DPoint.h"
#include "GLDraw.h"
#include "ftGLFontWrapper.h"

/* glutGetModifiers return mask. */
#define GL_ACTIVE_SHIFT               1
#define GL_ACTIVE_CTRL                2
#define GL_ACTIVE_ALT                 4

#define GL_SELECT_BUFFER_SIZE 4096

// used in m_nWindowType
#define GLWIN_PCASPACE						209393
#define GLWIN_PCOORD						209394
#define GLWIN_VCOORD						209395
#define GLWIN_SLIDERCTRLSPACE				209396
#define GLWIN_CORRELATION					209397

// CGLView

class CGLScreenMapping
{
public:
	void Reset() {
		m_rtGLRegion.SetRectEmpty(); m_rtScreenRegion.SetRectEmpty();
	}
	void SetGLRegion(CDRect rect) { m_rtGLRegion = rect; }
	void SetScreenRegion(CRect rect) { m_rtScreenRegion = rect; }

	CDRect *GetGLRegion() { return &m_rtGLRegion; }
	CRect *GetScreenRegion() { return &m_rtScreenRegion; }

	BOOL IsRectEmpty() { return (m_rtGLRegion.IsRectEmpty() && m_rtScreenRegion.IsRectEmpty()); }

private:
	CDRect m_rtGLRegion;
	CRect m_rtScreenRegion;
};

class CGLView : public CDRect, public CGLDraw
{
public:
	CGLView();
	virtual ~CGLView();
	
	typedef enum {
		CLASSICLAYOUT, SEPARATELAYOUT, SIDEBYSIDELAYOUT
	} PCP_LAYOUT; // Parallel Coordinate Plot line drawing
	typedef enum { 
		STRAIGHTLINES, SPLINECURVES
	} PCP_LINEDRAWING; // Parallel Coordinate Plot line drawing
	typedef enum {
		CLASSICALPCP, BINNEDHISTOGRAM, MVBINNEDHISTOGRAM, ANGULARHISTOGRAM
	} PCP_STYLE; // Parallel Coordinate Plot line drawing
	typedef enum {
		LOWCOMPLEXITY, HIGHCOMPLEXITY, MEDIUMCOMPLEXITY, MANUALCOMPLEXITY
	} VISUALCOMPLEXITY; // Visual complexity

	void ShowWindow(int nCmdShow) { m_nCmdShow = nCmdShow; };
	BOOL IsWindowVisible() { return (m_nCmdShow == SW_SHOW) ? TRUE : FALSE; };
	CWnd *GetParent() { return m_pParent; };
	void SetParent(CWnd *parent) { m_pParent = parent; };

public: // for object GL selection
	unsigned int __glModifierMask;
	CWnd *m_pParent;				// GL Parent Window
	unsigned int m_nWindowType;		// Window Type allocated when window is created.
	GLuint* m_selectBuffer;			// used to determine selected (or highlited) items with GL_SELECT
	CftGLFontWrapper m_ftglFontText;

public:
	unsigned int glGetModifiers();

private:
	int m_nCmdShow;	// Window Visible
};
