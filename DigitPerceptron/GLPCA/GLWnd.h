#pragma once
#include "afxwin.h"
#include "DPoint.h"
#include "DRect.h"

#define ShiftMask		(1<<0)
#define LockMask		(1<<1)
#define ControlMask		(1<<2)
#define Mod1Mask		(1<<3)

#define DOUBLECLICK_TIME		500 // miliseconds

typedef struct tagSELECTIONMANAGEMENT
{
	CPoint selected; // tracking recently selected item
	clock_t tmbuffer;
	tagSELECTIONMANAGEMENT()
	{
		selected.x=selected.y=0;
		tmbuffer=0;
	}
}SELECTIONMANAGEMENT;


class CGLWnd : public CWnd, public CDRect
{
public:
	CGLWnd(void);
	virtual ~CGLWnd(void);

	BOOL InitializeOpenGL();
	BOOL SetupPixelFormat();
	BOOL MakeActive();
	void GetCurrent(HDC& hDC, HGLRC& hRC);
	BOOL SetCurrent(HDC hDC, HGLRC hRC);
	int checkDoubleClicking();

// Attributes
protected:
	HGLRC m_hRC;	//Rendering Context
	CDC* m_pDC;		//Device Context
	clock_t m_prevtmBuffer;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
};

inline void CGLWnd::GetCurrent(HDC& hDC, HGLRC& hRC)
{
	hDC = wglGetCurrentDC();
	hRC = wglGetCurrentContext();
}

inline BOOL CGLWnd::SetCurrent(HDC hDC, HGLRC hRC)
{
	return wglMakeCurrent(hDC, hRC);
}

inline int CGLWnd::checkDoubleClicking()
{
	long gap = DOUBLECLICK_TIME;
	clock_t tmBuffer;
	tmBuffer = clock();
	if (m_prevtmBuffer != 0)
		gap = tmBuffer - m_prevtmBuffer;
	m_prevtmBuffer = tmBuffer;
	if (gap < DOUBLECLICK_TIME)	// less than defined milliseconds
		return 1;
	return -1;
}

class CActiveOpenGLContext
{
public:
	CActiveOpenGLContext(CWnd *wnd)
	{
		m_pParent = wnd;
		m_hDC = NULL;
		m_hRC = NULL;
		m_hDC = wglGetCurrentDC();
		m_hRC = wglGetCurrentContext();
		((CGLWnd*)m_pParent)->MakeActive();
	}
	virtual ~CActiveOpenGLContext(void)
	{
		// Restore last rendering and device contexts
		if (m_hDC != NULL && m_hRC != NULL)
			((CGLWnd*)m_pParent)->SetCurrent(m_hDC, m_hRC);
	}

private:
	CWnd *m_pParent;
	HDC m_hDC;
	HGLRC m_hRC;
};
