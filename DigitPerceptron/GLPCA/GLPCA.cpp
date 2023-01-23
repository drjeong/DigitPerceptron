// GLPCA.cpp : implementation file
//
#include "pch.h"
#include "GLPCA.h"
#include "DigitPerceptronDlg.h"

#include <iostream>

#include "ColorSpace.h"
#include "glut.h"
#include "DRect.h"
#include "ConvexHull.h"
#include "DataFunctions.h"
#include "StringWrapper.h"
#include <PCA/mathfunctions.h>
#include "gl2ps.h"

#define ZOOMING_TIME 10
#define GLPCASPACE_MARGINGAP 5
#define VIZBETWEENMARGIN 5
#define MINUMUM_WIDTH 50

#define CURSOR_IDC_ARROW 0x0000
#define CURSOR_IDC_SIZENS 0x0002
#define CURSOR_IDC_SIZEWE 0x0004
#define CURSOR_BORDER_MOVE 0x0008

BOOL g_bVectorDrawing2File = FALSE;
int currentTime = 0, previousTime = 0;
int frameCount = 0; //  The number of frames

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGLPCA

CGLPCA::CGLPCA()
{
	m_timebuffer = 0;	
	m_nMouseCursor = CURSOR_IDC_ARROW;
	m_dRow1VizHeightRatio = 0.4;
	m_dRow2VizHeightRatio = 0.3;
	m_dRow3VizHeightRatio = 0.3;
	m_nPrevVisibleVizRows = 0;
}

CGLPCA::~CGLPCA()
{
	Reset();
}

IMPLEMENT_DYNAMIC(CGLPCA, CGLWnd)
BEGIN_MESSAGE_MAP(CGLPCA, CGLWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_DESTROY()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEHWHEEL()
	ON_WM_CONTEXTMENU()	// OnContextMenu
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGLPCA message handlers

void CGLPCA::InitViews()
{
	CRect rect;
	GetWindowRect(rect);

	this->SetRect(CDPoint(0,0), rect.Height(), rect.Width());
	if (GetGLPCASpace())
		GetGLPCASpace()->ShowWindow(SW_SHOW);

	// set default size as the whole screen space
	GetGLPCASpace()->SetRect(*this);

	// set parent to GLViews
	if (GetGLPCASpace()) GetGLPCASpace()->SetParent(this);
}

void CGLPCA::Redraw()
{
	if (GetGLPCASpace()) GetGLPCASpace()->ResetClutterManager();
	Invalidate(FALSE);
}

void CGLPCA::Reset()
{	// reset all positional information
	if (GetGLPCASpace()) GetGLPCASpace()->Reset();
}

time_t start_time=0;
int CGLPCA::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	InitializeOpenGL();

//	SetTimer(1, 100, 0);

	return 0;
}

void CGLPCA::OnDestroy()
{
	__super::OnDestroy();
}

void CGLPCA::OnPaint() 
{
	//cout << "CGLPCA::OnPaint() " << endl;
	CPaintDC dc(this); // device context for painting
	
	CActiveOpenGLContext active(this);

	glViewport((int)this->left, (int)this->bottom, (int)this->Width(), (int)this->Height());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(this->left, this->right, this->bottom, this->top, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw horizontal borders
	for (auto it = m_rtHorzBorders.begin(); it != m_rtHorzBorders.end(); ++it)
	{
		glColor4f(0.5f, 0.0f, 0.0f, 0.3f);
		drawObject(*it, GL_POLYGON);
	}

	// draw vertical borders
	for (auto it = m_rtVertBorders.begin(); it != m_rtVertBorders.end(); ++it)
	{
		glColor4f(0.5f, 0.0f, 0.0f, 0.3f);
		drawObject((*it).region, GL_POLYGON);
	}

	if (GetGLPCASpace()->IsWindowVisible())
		GetGLPCASpace()->OnPaint();
	
	// draw outline
	glColor4f(0.5f, 0.5f, 0.5f, 0.3f);
	drawObject(*this, GL_LINE_STRIP);


	// Tell OpenGL to flush its pipeline
	::glFinish();

	// Swap the buffers in case of double buffering
	PIXELFORMATDESCRIPTOR pfd;
	int nGLPixelIndex = GetPixelFormat(dc.GetSafeHdc());
	DescribePixelFormat(dc.GetSafeHdc(), nGLPixelIndex, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	if (pfd.dwFlags & PFD_DOUBLEBUFFER)
		SwapBuffers(dc.GetSafeHdc());
	
	// Do not call CGLWnd::OnPaint() for painting messages
}

void CGLPCA::OnSize(UINT nType, int cx, int cy) 
{
	CActiveOpenGLContext active(this);
	__super::OnSize(nType, cx, cy);
	
	if (cy == 0) cy = 1;

	//CGLView::SetWindowSize(CRect(0, 0, cx, cy));
	this->SetRect(CDPoint(0,0), cx, cy);

//	GetGLPCASpace()->OnSize(nType, rect);

	Redraw();
}

void CGLPCA::OnTimer(UINT_PTR nIDEvent) 
{
	CActiveOpenGLContext active(this);
	//Invalidate(FALSE);

#ifdef _DEBUG
	//  Increase frame count
	frameCount++;

	//  Get the number of milliseconds since glutInit called 
	//  (or first call to glutGet(GLUT ELAPSED TIME)).
	currentTime = glutGet(GLUT_ELAPSED_TIME);

	//  Calculate time passed
	int timeInterval = currentTime - previousTime;
	if(timeInterval > 1000)
	{
		//  calculate the number of frames per second
		float fps = frameCount / (timeInterval / 1000.0f);

		std::cout << fps << "fps" << endl;

		//  Set time
		previousTime = currentTime;

		//  Reset frame count
		frameCount = 0;
	}
#endif

	CGLWnd::OnTimer(nIDEvent);
}

BOOL CGLPCA::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (IsReady() == FALSE) return FALSE;
	CActiveOpenGLContext active(this);

	ScreenToClient(&pt); // Get Mouse Click position and 
	CDPoint glpoint = CGLDraw::GetGLPos(pt.x, pt.y);
	ClientToScreen(&pt); // convert it to the Screen Co-ordinate

	if (GetGLPCASpace())
		if (GetGLPCASpace()->IsWindowVisible())
			if (GetGLPCASpace()->OnMouseWheel(zDelta, pt, glpoint)) 
				Invalidate(FALSE);

	return __super::OnMouseWheel(nFlags, zDelta, pt);
}

void CGLPCA::OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (IsReady() == FALSE) return;
	CActiveOpenGLContext active(this); 

	ScreenToClient(&pt); // Get Mouse Click position and 
	CDPoint glpoint = CGLDraw::GetGLPos(pt.x, pt.y);
	ClientToScreen(&pt); // convert it to the Screen Co-ordinate

	if (GetGLPCASpace())
		if (GetGLPCASpace()->IsWindowVisible())
			if (GetGLPCASpace()->OnMouseHWheel(zDelta, pt, glpoint)) 
				Invalidate(FALSE);

	__super::OnMouseHWheel(nFlags, zDelta, pt);
}

void CGLPCA::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (IsReady()==FALSE) return;
	CActiveOpenGLContext active(this);

	CDPoint glpoint = CGLDraw::GetGLPos(point.x, point.y);

	if (GetGLPCASpace())
		if (GetGLPCASpace()->IsWindowVisible())
			GetGLPCASpace()->OnLButtonDown(point, glpoint);
		
	__super::OnLButtonDown(nFlags, point);
}

void CGLPCA::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (IsReady()==FALSE) return;
	CActiveOpenGLContext active(this);

	CDPoint glpoint = CGLDraw::GetGLPos(point.x, point.y);
	if (GetGLPCASpace())
		if (GetGLPCASpace()->IsWindowVisible())	
			if (GetGLPCASpace()->OnRButtonDown(glpoint)) Invalidate(FALSE);

	__super::OnRButtonDown(nFlags, point);
}

void CGLPCA::OnMButtonDown(UINT nFlags, CPoint point)
{
	if (IsReady() == FALSE) return;
	CActiveOpenGLContext active(this);

	CDPoint glpoint = CGLDraw::GetGLPos(point.x, point.y);

	if (GetGLPCASpace())
		if (GetGLPCASpace()->IsWindowVisible())
			if (GetGLPCASpace()->OnMButtonDown(point, glpoint)) Invalidate(FALSE);

	__super::OnMButtonDown(nFlags, point);
}


void CGLPCA::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (IsReady()==FALSE) return;
	CActiveOpenGLContext active(this);

	// Get point in GL space
	CDPoint glpoint = CGLDraw::GetGLPos(point.x, point.y);

	if (GetGLPCASpace())
		if (GetGLPCASpace()->IsWindowVisible())
			if (GetGLPCASpace()->OnMouseMove(point, glpoint)) Redraw();

	__super::OnMouseMove(nFlags, point);
}

void CGLPCA::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (IsReady()==FALSE) return;
	CActiveOpenGLContext active(this);

	if (m_nMouseCursor != CURSOR_IDC_ARROW)
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		m_nMouseCursor = CURSOR_IDC_ARROW;
		//Invalidate(FALSE);
	}

	CDPoint glpoint = CGLDraw::GetGLPos(point.x, point.y);

	if (LButtonUp(point, glpoint))
	{
		LButtonUp(point, glpoint, TRUE);
		Invalidate(FALSE);
	}
	
	__super::OnLButtonUp(nFlags, point);
}

BOOL CGLPCA::LButtonUp(CPoint point, CDPoint glpoint, BOOL bDisable)
{
	if (bDisable) // Disabled Button Actions
	{
		if (GetGLPCASpace())
			if (GetGLPCASpace()->IsWindowVisible())
				GetGLPCASpace()->LButtonUp(glpoint);
	}
	else 
	{
		if (GetGLPCASpace())
			if (GetGLPCASpace()->IsWindowVisible())
				if (GetGLPCASpace()->OnLButtonUp(glpoint))
					return TRUE;
	}

	return FALSE;
}


void CGLPCA::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if (IsReady()==FALSE) return;
	CActiveOpenGLContext active(this);

	CDPoint glpoint = CGLDraw::GetGLPos(point.x, point.y);

	if (GetGLPCASpace())
		if (GetGLPCASpace()->IsWindowVisible())	
			GetGLPCASpace()->OnRButtonUp(glpoint);

//	Invalidate(FALSE);	// Currently Nothing is connected to RButtonUp

	__super::OnRButtonUp(nFlags, point);
}


void CGLPCA::OnMButtonUp(UINT nFlags, CPoint point)
{
	if (IsReady() == FALSE) return;
	CActiveOpenGLContext active(this);

	CDPoint glpoint = CGLDraw::GetGLPos(point.x, point.y);

	if (GetGLPCASpace())
		if (GetGLPCASpace()->IsWindowVisible())
			GetGLPCASpace()->OnMButtonUp(glpoint);

	__super::OnMButtonUp(nFlags, point);
}


BOOL CGLPCA::IsReady()
{
	if (((CDigitPerceptronDlg*)AfxGetMainWnd())->GetDataManager()->IsDataLoaded()==FALSE ||
		((CDigitPerceptronDlg*)AfxGetMainWnd())->GetDataManager()->GetPCA()->IsPCACalculated()==FALSE ||
		this->IsWindowVisible()==FALSE) return FALSE;
	
	return TRUE;
}

void CGLPCA::PreparePickScene(CRect region, GLuint* selectBuffer)
{
	glSelectBuffer(GL_SELECT_BUFFER_SIZE, selectBuffer);
	glRenderMode(GL_SELECT);
	glInitNames();

	glViewport(0, 0, (int)this->Width(), (int)this->Height());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	
	/*  create width x height pixel picking region near the center of the region */
	CPoint center = region.CenterPoint();
	gluPickMatrix(center.x, this->Height() - center.y,
		region.Width(), region.Height(), viewport);
	glOrtho(0, this->Width(), 0, this->Height(), -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void CGLPCA::PreparePickScene(int pickX, int pickY, GLuint* selectBuffer) 
{
	glSelectBuffer(GL_SELECT_BUFFER_SIZE, selectBuffer);
	glRenderMode(GL_SELECT);
	glInitNames();

	glViewport(0, 0, (int)this->Width(), (int)this->Height());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	/*  create 2 x 2 pixel picking region near cursor location      */
	gluPickMatrix(pickX, this->Height() - pickY, 2.0, 2.0, viewport);
	glOrtho(0, this->Width(), 0, this->Height(), -1, 1);	

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

BOOL CGLPCA::IsMouseOnBorders(CDPoint glPoint)
{
	if (m_nMouseCursor == CURSOR_IDC_ARROW)
	{
		// check if the mouse cursor is on any horizontal borders 
		for (int i = 0; i < m_rtHorzBorders.size(); i++)
		{
			if (m_rtHorzBorders[i].IsInside(glPoint.x, glPoint.y))
			{
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS)); // Double-pointed arrow pointing north and south
				m_nMouseCursor |= CURSOR_IDC_SIZENS;
				m_nSelectedHorzBorder_forBorderMovement = i;
				return TRUE;
			}
		}

		// check if the mouse cursor is on any vertical borders
		for (int i=0; i< m_rtVertBorders.size(); i++)
		{
			if (m_rtVertBorders[i].region.IsInside(glPoint.x, glPoint.y))
			{
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));	 // Double-pointed arrow pointing west and east 
				m_nMouseCursor |= CURSOR_IDC_SIZEWE;
				m_nSelectedVertzBorder_forBorderMovement = i;
				return TRUE;
			}
		}
	}


	//// default mouse cursor shape if the mouse cursor is in not inside of any borders
	//if (m_nMouseCursor != CURSOR_IDC_ARROW)
	//{
	//	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	//	m_nMouseCursor = CURSOR_IDC_ARROW;
	//	Invalidate(FALSE);
	//}
	//
	return FALSE;
}

void CGLPCA::CaptureScreen(char *filename, GLint format)
{
	g_bVectorDrawing2File = TRUE;

	gl2psLineWidth(1.0);
	gl2psEnable(GL2PS_BLEND);

	FILE *fp = fopen(filename, "wb");
	int state = GL2PS_OVERFLOW, buffsize = 0;
	printf("Writing '%s'... \n", filename);

	GLint viewport[4];
	viewport[0] = 0;
	viewport[1] = 0;
	viewport[2] = (int)this->Width();
	viewport[3] = (int)this->Height();
	int opt = GL2PS_DRAW_BACKGROUND | GL2PS_NO_PS3_SHADING | GL2PS_TIGHT_BOUNDING_BOX; //GL2PS_DRAW_BACKGROUND | GL2PS_USE_CURRENT_VIEWPORT
	while (state == GL2PS_OVERFLOW) {
		buffsize += 2048 * 2048;
		gl2psBeginPage(filename, "gl2psPCA", viewport, format, GL2PS_SIMPLE_SORT,
			opt,
			GL_RGBA, 0, NULL, 32, 32, 32, buffsize, fp, filename);
		OnPaint();
		state = gl2psEndPage();
	}
	printf("Writing Done!\n");
	fclose(fp);

	g_bVectorDrawing2File = FALSE;
}


void CGLPCA::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (IsReady() == FALSE) return;
	CActiveOpenGLContext active(this);

	ScreenToClient(&point); // Get Mouse Click position and 
	CDPoint glpoint = CGLDraw::GetGLPos(point.x, point.y);
	ClientToScreen(&point); // convert it to the Screen Co-ordinate

	if (GetGLPCASpace())
		if (GetGLPCASpace()->IsWindowVisible())
			GetGLPCASpace()->OnContextMenu(point, glpoint);

}

BOOL CGLPCA::SpecialKeyReleased()
{
	if (m_nMouseCursor != CURSOR_IDC_ARROW)
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		m_nMouseCursor = CURSOR_IDC_ARROW;
		//Invalidate(FALSE);
	}
	return TRUE;
}

BOOL CGLPCA::SpecialKeyPressed(UINT vkey)
{
	//if (vkey == VK_SHIFT) m_bBorderControl = TRUE;

	return TRUE;
}


void CGLPCA::OnMouseLeave()
{
	// TODO: Add your message handler code here and/or call default
	cout << "TEST" << endl;
}
